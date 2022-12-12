// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/**
 * ALSARawmidiStreamPair:
 * GObject-derived object to express a pair of Rawmidi stream.
 *
 * A [class@StreamPair] is a GObject-derived object to express a pair of Rawmidi stream to which
 * substreams are attached. The substream is pointed by the combination of the numeric identifier
 * of device, subdevice, and direction. When the call of [method@StreamPair.open] with the
 * combination, corresponding substreams are attached to the object. Then the object maintains file
 * descriptor till object destruction. The call of [method@StreamPair.create_source] returns the
 * instance of [struct@GLib.Source]. Once attached to the [struct@GLib.Source],
 * [struct@GLib.MainContext] / [struct@GLib.MainLoop] is available as event dispatcher. The
 * [signal@StreamPair::handle-messages] signal is emitted in the event dispatcher to notify the
 * intermediate buffer of capture substream has available messages. The call of
 * [method@StreamPair.read_from_substream] fills the given buffer with the available messages. The
 * call of [method@StreamPair.write_to_substream] write messages in the given buffer into the
 * intermediate buffer of playback substream. The call of [method@StreamPair.get_substream_status]
 * is available to check the space in the intermediate buffer according to direction argument.
 */
typedef struct {
    int fd;
    char *devnode;
    guint16 proto_ver_triplet[3];
} ALSARawmidiStreamPairPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSARawmidiStreamPair, alsarawmidi_stream_pair, G_TYPE_OBJECT)

/**
 * alsarawmidi_stream_pair_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] with code in `ALSARawmidi.StreamPairError`.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(alsarawmidi-stream-pair-error-quark, alsarawmidi_stream_pair_error)

static const char *const err_msgs[] = {
        [ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED] = "The card is in disconnect state",
        [ALSARAWMIDI_STREAM_PAIR_ERROR_UNREADABLE] = "The instance is not for read operation",
};

#define generate_local_error(error, code) \
        g_set_error_literal(error, ALSARAWMIDI_STREAM_PAIR_ERROR, code, err_msgs[code])

#define generate_syscall_error(error, errno, format, arg)           \
        g_set_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR,           \
                    ALSARAWMIDI_STREAM_PAIR_ERROR_FAILED,           \
                    format " %d(%s)", arg, errno, strerror(errno))

typedef struct {
    GSource src;
    ALSARawmidiStreamPair *self;
    gpointer tag;
} RawmidiStreamPairSource;

enum rawmidi_stream_pair_prop_type {
    RAWMIDI_STREAM_PAIR_PROP_DEVNODE = 1,
    RAWMIDI_STREAM_PAIR_PROP_COUNT,
};
static GParamSpec *rawmidi_stream_pair_props[RAWMIDI_STREAM_PAIR_PROP_COUNT] = { NULL, };

enum rawmidi_stream_pair_sig_type {
    RAWMIDI_STREAM_PAIR_SIG_HANDLE_MESSAGES = 0,
    RAWMIDI_STREAM_PAIR_SIG_DISCONNECTION,
    RAWMIDI_STREAM_PAIR_SIG_COUNT,
};
static guint rawmidi_stream_pair_sigs[RAWMIDI_STREAM_PAIR_SIG_COUNT] = { 0 };

static void rawmidi_stream_pair_get_property(GObject *obj, guint id,
                                             GValue *val, GParamSpec *spec)
{
    ALSARawmidiStreamPair *self = ALSARAWMIDI_STREAM_PAIR(obj);
    ALSARawmidiStreamPairPrivate *priv =
                            alsarawmidi_stream_pair_get_instance_private(self);
    switch (id) {
    case RAWMIDI_STREAM_PAIR_PROP_DEVNODE:
        g_value_set_string(val, priv->devnode);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void rawmidi_stream_pair_finalize(GObject *obj)
{
    ALSARawmidiStreamPair *self = ALSARAWMIDI_STREAM_PAIR(obj);
    ALSARawmidiStreamPairPrivate *priv =
                            alsarawmidi_stream_pair_get_instance_private(self);

    if (priv->fd >= 0) {
        close(priv->fd);
        g_free(priv->devnode);
    }

    G_OBJECT_CLASS(alsarawmidi_stream_pair_parent_class)->finalize(obj);
}

static void alsarawmidi_stream_pair_class_init(ALSARawmidiStreamPairClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = rawmidi_stream_pair_finalize;
    gobject_class->get_property = rawmidi_stream_pair_get_property;

    /**
     * ALSARawmidiStreamPair:devnode:
     *
     * The full path to special file of rawmidi character device.
     */
    rawmidi_stream_pair_props[RAWMIDI_STREAM_PAIR_PROP_DEVNODE] =
        g_param_spec_string("devnode", "devnode",
                            "The full path to special file of rawmidi character device.",
                            "",
                            G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      RAWMIDI_STREAM_PAIR_PROP_COUNT,
                                      rawmidi_stream_pair_props);

    /**
     * ALSARawmidiStreamPair::handle-messages:
     * @self: A [class@StreamPair].
     *
     * When any input message is available, this event is emit.
     */
    rawmidi_stream_pair_sigs[RAWMIDI_STREAM_PAIR_SIG_HANDLE_MESSAGES] =
        g_signal_new("handle-messages",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSARawmidiStreamPairClass, handle_messages),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0, NULL);

    /**
     * ALSARawmidiStreamPair::handle-disconnection:
     * @self: A [class@StreamPair].
     *
     * When the sound card is not available anymore due to unbinding driver or hot unplugging,
     * this signal is emit. The owner of this object should call [method@GObject.Object.unref] as
     * quickly as possible to release ALSA rawmidi character device.
     */
    rawmidi_stream_pair_sigs[RAWMIDI_STREAM_PAIR_SIG_DISCONNECTION] =
        g_signal_new("handle-disconnection",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSARawmidiStreamPairClass, handle_disconnection),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0, G_TYPE_NONE, 0);
}

static void alsarawmidi_stream_pair_init(ALSARawmidiStreamPair *self)
{
    ALSARawmidiStreamPairPrivate *priv =
                            alsarawmidi_stream_pair_get_instance_private(self);

    priv->fd = -1;
}

/**
 * alsarawmidi_stream_pair_new:
 *
 * Allocate and return an instance of [class@StreamPair].
 *
 * Returns: An instance of [class@StreamPair].
 */
ALSARawmidiStreamPair *alsarawmidi_stream_pair_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_STREAM_PAIR, NULL);
}

/**
 * alsarawmidi_stream_pair_open:
 * @self: A [class@StreamPair].
 * @card_id: The numeric identifier of sound card.
 * @device_id: The numeric identifier of rawmidi device for the sound card.
 * @subdevice_id: The numeric identifier of subdevice for the rawmidi device.
 * @access_modes: Access flags for stream direction.
 * @open_flag: The flag of `open(2)` system call. `O_RDWR`, `O_WRONLY` and `O_RDONLY` are forced
 *             to fulfil internally according to the access_modes.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError`
 *         and `ALSARawmidi.StreamPairError`.
 *
 * Open file descriptor for a pair of streams to attach input/output substreams corresponding to
 * the given subdevice.
 *
 * The call of function executes `open(2)` system call for ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_open(ALSARawmidiStreamPair *self, guint card_id, guint device_id,
                                      guint subdevice_id, ALSARawmidiStreamPairInfoFlag access_modes,
                                      gint open_flag, GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    char *devnode;
    int ctl_fd;
    int proto_ver;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    // The flag is used to attach substreams for each direction.
    g_return_val_if_fail((access_modes & ~(ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT |
                         ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT)) == 0, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    open_flag &= ~(O_RDWR | O_WRONLY | O_RDONLY);
    if ((access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT) &&
        (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT))
        open_flag |= O_RDWR;
    else if (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT)
        open_flag |= O_WRONLY;
    else if (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT)
        open_flag |= O_RDONLY;
    else
        g_return_val_if_reached(FALSE);

    if (!alsarawmidi_get_rawmidi_devnode(card_id, device_id, &devnode, error))
        return FALSE;

    if (!rawmidi_select_subdevice(card_id, subdevice_id, &ctl_fd, error)) {
        g_free(devnode);
        return FALSE;
    }

    priv->fd = open(devnode, open_flag);
    close(ctl_fd);
    if (priv->fd < 0) {
        if (errno == ENODEV) {
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        } else {
            GFileError code = g_file_error_from_errno(errno);

            if (code != G_FILE_ERROR_FAILED)
                generate_file_error(error, code, "open(%s)", devnode);
            else
                generate_syscall_error(error, errno, "open(%s)", devnode);
        }
        g_free(devnode);
        return FALSE;
    }
    priv->devnode = devnode;

    // Remember the version of protocol currently used.
    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_PVERSION, &proto_ver) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "PVERSION");
        close(priv->fd);
        priv->fd = -1;
        return FALSE;
    }

    priv->proto_ver_triplet[0] = SNDRV_PROTOCOL_MAJOR(proto_ver);
    priv->proto_ver_triplet[1] = SNDRV_PROTOCOL_MINOR(proto_ver);
    priv->proto_ver_triplet[2] = SNDRV_PROTOCOL_MICRO(proto_ver);

    return TRUE;
}

/**
 * alsarawmidi_stream_pair_get_protocol_version:
 * @self: A [class@StreamPair].
 * @proto_ver_triplet: (array fixed-size=3)(out)(transfer none): The version of protocol currently
 *                     used.
 * @error: A [struct@GLib.Error].
 *
 * Get the version of rawmidi protocol currently used. The version is expressed as the array with
 * three elements; major, minor, and micro version in the order. The length of major version is
 * 16 bit, the length of minor and micro version is 8 bit each.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_get_protocol_version(ALSARawmidiStreamPair *self,
                                        const guint16 *proto_ver_triplet[3],
                                        GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(proto_ver_triplet != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *proto_ver_triplet = (const guint16 *)priv->proto_ver_triplet;

    return TRUE;
}

/**
 * alsarawmidi_stream_pair_get_substream_info:
 * @self: A [class@StreamPair].
 * @direction: The direction of substream attached to the stream pair.
 * @substream_info: (out): The information for requested substream.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSARawmidi.StreamPairError`.
 *
 * Get information of substream attached to the stream pair.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_RAWMIDI_IOCTL_INFO` command
 * for ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_get_substream_info(ALSARawmidiStreamPair *self,
                                ALSARawmidiStreamDirection direction,
                                ALSARawmidiSubstreamInfo **substream_info,
                                GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    struct snd_rawmidi_info *info;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    g_return_val_if_fail(substream_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *substream_info = g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_INFO, NULL);

    rawmidi_substream_info_refer_private(*substream_info, &info);
    info->stream = direction;
    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_INFO, info) < 0) {
        g_object_unref(*substream_info);

        if (errno == ENODEV)
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "INFO");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsarawmidi_stream_pair_set_substream_params:
 * @self: A [class@StreamPair].
 * @direction: The direction of substream attached to the stream pair.
 * @substream_params: The parameters of substream.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSARawmidi.StreamPairError`.
 *
 * Set parameters of substream for given direction, which is attached to the pair of streams.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_RAWMIDI_IOCTL_PARAMS` command
 * for ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_set_substream_params(ALSARawmidiStreamPair *self,
                                ALSARawmidiStreamDirection direction,
                                ALSARawmidiSubstreamParams *substream_params,
                                GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    struct snd_rawmidi_params *params;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    g_return_val_if_fail(substream_params != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    rawmidi_substream_params_refer_private(substream_params, &params);

    params->stream = direction;
    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_PARAMS, params) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "PARAMS");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsarawmidi_stream_pair_get_substream_status:
 * @self: A [class@StreamPair].
 * @direction: The direction of substream attached to the stream pair.
 * @substream_status: (inout): The status of substream.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSARawmidi.StreamPairError`.
 *
 * Retrieve status of substream for given direction, which is attached to the pair of streams.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_RAWMIDI_IOCTL_STATUS` command
 * for ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_get_substream_status(ALSARawmidiStreamPair *self,
                            ALSARawmidiStreamDirection direction,
                            ALSARawmidiSubstreamStatus *const *substream_status,
                            GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    struct snd_rawmidi_status *status;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    g_return_val_if_fail(substream_status != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    rawmidi_substream_status_refer_private(*substream_status, &status);

    status->stream = direction;
    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_STATUS, status) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "STATUS");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsarawmidi_stream_pair_read_from_substream:
 * @self: A [class@StreamPair].
 * @buf: (array length=buf_size)(inout): The buffer to copy data.
 * @buf_size: The size of buffer.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSARawmidi.StreamPairError`.
 *
 * Copy data from intermediate buffer to given buffer for substream attached to the pair of
 * streams. In a case that the instance is opened without `O_NONBLOCK` flag and the intermediate
 * buffer has no data, call of the API is blocked till any data is available.
 *
 * The call of function executes `read(2)` system for ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_read_from_substream(ALSARawmidiStreamPair *self,
                                        guint8 *const *buf, gsize *buf_size,
                                        GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    int len;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    g_return_val_if_fail(buf != NULL, FALSE);
    g_return_val_if_fail(buf_size != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    len = read(priv->fd, *buf, *buf_size);
    if (len < 0) {
        if (errno == ENODEV) {
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        } else {
            GFileError code = g_file_error_from_errno(errno);

            if (code != G_FILE_ERROR_FAILED)
                generate_file_error(error, code, "read(%s)", priv->devnode);
            else
                generate_syscall_error(error, errno, "read(%s)", priv->devnode);
        }

        return FALSE;
    }

    *buf_size = len;
    return TRUE;
}

/**
 * alsarawmidi_stream_pair_write_to_substream:
 * @self: A [class@StreamPair].
 * @buf: (array length=buf_size): The buffer to copy data.
 * @buf_size: The size of buffer.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSARawmidi.StreamPairError`.
 *
 * Copy data from given buffer to intermediate buffer for substream attached to the pair of
 * streams. In a case that the instance is opened without `O_NONBLOCK` flag and the intermediate
 * buffer is full, call of the API is blocked till the buffer has space for the data.
 *
 * The call of function executes `write(2) system for ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_write_to_substream(ALSARawmidiStreamPair *self,
                                        const guint8 *buf, gsize buf_size,
                                        GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    int len;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    g_return_val_if_fail(buf != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    len = write(priv->fd, buf, buf_size);
    if (len < 0) {
        if (errno == ENODEV) {
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        } else {
            GFileError code = g_file_error_from_errno(errno);

            if (code != G_FILE_ERROR_FAILED)
                generate_file_error(error, code, "write(%s)", priv->devnode);
            else
                generate_syscall_error(error, errno, "write(%s)", priv->devnode);
        }

        return FALSE;
    }

    return TRUE;
}

/**
 * alsarawmidi_stream_pair_drain:
 * @self: A [class@StreamPair].
 * @direction: The direction of substream attached to the stream pair.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSARawmidi.StreamPairError`.
 *
 * Drain queued data in intermediate buffer for substream attached to the pair of streams. In a
 * case that the instance is opened without `O_NONBLOCK` and the call is for output substream and
 * any data is in the intermediate buffer, the call is blocked till no data is in the intermediate
 * buffer.
 *
 * The call of function executes `ioctl(2)` system with `SNDRV_RAWMIDI_IOCTL_DRAIN` command for
 * ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_drain_substream(ALSARawmidiStreamPair *self,
                                        ALSARawmidiStreamDirection direction,
                                        GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_DRAIN, &direction) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "DRAIN");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsarawmidi_stream_pair_drop:
 * @self: A [class@StreamPair].
 * @direction: The direction of substream attached to the stream pair.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSARawmidi.StreamPairError`.
 *
 * Drop queued data in intermediate buffer immediately for substream attached to the pair of
 * streams. In implementation of ALSA rawmidi core, the direction should be for output substream.
 *
 * The call of function executes `ioctl(2)` system with `SNDRV_RAWMIDI_IOCTL_DROP` command for
 * ALSA rawmidi character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_drop_substream(ALSARawmidiStreamPair *self,
                                        ALSARawmidiStreamDirection direction,
                                        GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_DROP, &direction) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "DROP");
        return FALSE;
    }

    return TRUE;
}

static gboolean rawmidi_stream_pair_check_src(GSource *gsrc)
{
    RawmidiStreamPairSource *src = (RawmidiStreamPairSource *)gsrc;
    GIOCondition condition;

    // Don't go to dispatch if nothing available. As an exception, return TRUE
    // for POLLERR to call .dispatch for internal destruction.
    condition = g_source_query_unix_fd(gsrc, src->tag);
    return !!(condition & (G_IO_IN | G_IO_ERR));
}

static gboolean rawmidi_stream_pair_dispatch_src(GSource *gsrc, GSourceFunc cb,
                                                 gpointer user_data)
{
    RawmidiStreamPairSource *src = (RawmidiStreamPairSource *)gsrc;
    ALSARawmidiStreamPair *self = src->self;
    ALSARawmidiStreamPairPrivate *priv;
    GIOCondition condition;

    priv = alsarawmidi_stream_pair_get_instance_private(self);
    if (priv->fd < 0)
        return G_SOURCE_REMOVE;

    condition = g_source_query_unix_fd(gsrc, src->tag);
    if (condition & G_IO_ERR) {
        g_signal_emit(self,
            rawmidi_stream_pair_sigs[RAWMIDI_STREAM_PAIR_SIG_DISCONNECTION],
            0, NULL);
        return G_SOURCE_REMOVE;
    }

    if (condition & G_IO_IN) {
        g_signal_emit(self,
                rawmidi_stream_pair_sigs[RAWMIDI_STREAM_PAIR_SIG_HANDLE_MESSAGES],
                0);
    }

    // Just be sure to continue to process this source.
    return G_SOURCE_CONTINUE;
}

static void rawmidi_stream_pair_finalize_src(GSource *gsrc)
{
    RawmidiStreamPairSource *src = (RawmidiStreamPairSource *)gsrc;

    g_object_unref(src->self);
}

/**
 * alsarawmidi_stream_pair_create_source:
 * @self: A [class@StreamPair].
 * @gsrc: (out): A [struct@GLib.Source] to handle events from ALSA rawmidi character device.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSARawmidi.StreamPairError`.
 *
 * Allocate [struct@GLib.Source] structure to handle events from ALSA rawmidi character device for
 * input substream. In each iteration of [struct@GLib.MainContext], the `read(2)` system call is
 * executed to dispatch control event for [signal@StreamPair::handle-messages] signal, according to
 * the result of `poll(2)` system call.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsarawmidi_stream_pair_create_source(ALSARawmidiStreamPair *self, GSource **gsrc,
                                               GError **error)
{
    static GSourceFuncs funcs = {
            .check          = rawmidi_stream_pair_check_src,
            .dispatch       = rawmidi_stream_pair_dispatch_src,
            .finalize       = rawmidi_stream_pair_finalize_src,
    };
    ALSARawmidiStreamPairPrivate *priv;
    RawmidiStreamPairSource *src;
    int access_modes;

    g_return_val_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self), FALSE);
    priv = alsarawmidi_stream_pair_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(gsrc != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    access_modes = fcntl(priv->fd, F_GETFL);
    if (access_modes < 0) {
        GFileError code = g_file_error_from_errno(errno);

        if (code != G_FILE_ERROR_FAILED)
            generate_file_error(error, code, "fcntl(%s)", "F_GETFL");
        else
            generate_syscall_error(error, errno, "fcntl(%s)", "F_GETFL");

        return FALSE;
    }

    if (!(access_modes & O_RDWR) && !(access_modes & O_WRONLY)) {
        generate_local_error(error, ALSARAWMIDI_STREAM_PAIR_ERROR_UNREADABLE);
        return FALSE;
    }

    *gsrc = g_source_new(&funcs, sizeof(RawmidiStreamPairSource));
    src = (RawmidiStreamPairSource *)(*gsrc);

    g_source_set_name(*gsrc, "ALSARawmidiStreamPair");
    g_source_set_priority(*gsrc, G_PRIORITY_HIGH_IDLE);
    g_source_set_can_recurse(*gsrc, TRUE);

    src->self = g_object_ref(self);
    src->tag = g_source_add_unix_fd(*gsrc, priv->fd, G_IO_IN);

    return TRUE;
}
