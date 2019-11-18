// SPDX-License-Identifier: LGPL-3.0-or-later
#include "stream-pair.h"
#include "query.h"
#include "privates.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct _ALSARawmidiStreamPairPrivate {
    int fd;
    char *devnode;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSARawmidiStreamPair, alsarawmidi_stream_pair, G_TYPE_OBJECT)

enum rawmidi_stream_pair_prop_type {
    RAWMIDI_STREAM_PAIR_PROP_DEVNODE = 1,
    RAWMIDI_STREAM_PAIR_PROP_COUNT,
};
static GParamSpec *rawmidi_stream_pair_props[RAWMIDI_STREAM_PAIR_PROP_COUNT] = { NULL, };

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

    rawmidi_stream_pair_props[RAWMIDI_STREAM_PAIR_PROP_DEVNODE] =
        g_param_spec_string("devnode", "devnode",
                            "The full path of rawmidi character device.",
                            "",
                            G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      RAWMIDI_STREAM_PAIR_PROP_COUNT,
                                      rawmidi_stream_pair_props);
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
 * Allocate and return an instance of ALSARawmidiStreamPair class.
 */
ALSARawmidiStreamPair *alsarawmidi_stream_pair_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_STREAM_PAIR, NULL);
}

/**
 * alsarawmidi_stream_pair_open:
 * @self: A #ALSARawmidiStreamPair.
 * @card_id: The numerical ID of sound card.
 * @device_id: The numerical ID of rawmidi device for the sound card.
 * @subdevice_id: The numerical ID of subdevice for the rawmidi device.
 * @access_modes: Access flags for stream direction.
 * @open_flag: The flag of open(2) system call. O_RDWR, O_WRONLY and O_RDONLY
 *             are forced to fulfil internally according to the access_modes.
 * @error: A #GError.
 *
 * Open file descriptor for a pair of streams to attach input/output substreams
 * corresponding to the given subdevice.
 */
void alsarawmidi_stream_pair_open(ALSARawmidiStreamPair *self, guint card_id,
                                  guint device_id, guint subdevice_id,
                                  ALSARawmidiStreamPairInfoFlag access_modes,
                                  gint open_flag, GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    char *devnode;

    g_return_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self));
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    // The flag is used to attach substreams for each direction.
    if (access_modes & ~(ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT |
                         ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT)) {
        generate_error(error, EINVAL);
        return;
    }

    if ((access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT) &&
        (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT)) {
        open_flag = O_RDWR;
    } else if (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT) {
        open_flag = O_WRONLY;
    } else if (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT) {
        open_flag = O_RDONLY;
    } else {
        generate_error(error, EINVAL);
        return;
    }

    alsarawmidi_get_rawmidi_devnode(card_id, device_id, &devnode, error);
    if (*error != NULL)
        return;

    rawmidi_select_subdevice(card_id, subdevice_id, error);
    if (*error != NULL) {
        g_free(devnode);
        return;
    }

    priv->fd = open(devnode, open_flag);
    if (priv->fd < 0) {
        generate_error(error, errno);
        g_free(devnode);
        return;
    }

    priv->devnode = devnode;
}

/**
 * alsarawmidi_stream_pair_get_substream_info:
 * @self: A #ALSARawmidiStreamPair.
 * @direction: The direction of substream attached to the stream pair.
 * @substream_info: (out): The information for requested substream.
 * @error: A #GError.
 *
 * Get information of substream attached to the stream pair.
 */
void alsarawmidi_stream_pair_get_substream_info(ALSARawmidiStreamPair *self,
                                ALSARawmidiStreamDirection direction,
                                ALSARawmidiSubstreamInfo **substream_info,
                                GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    struct snd_rawmidi_info *info;

    g_return_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self));
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    *substream_info = g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_INFO, NULL);

    rawmidi_substream_info_refer_private(*substream_info, &info);
    info->stream = direction;
    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_INFO, info) < 0) {
        generate_error(error, errno);
        g_object_unref(*substream_info);
        return;
    }
}

/**
 * alsarawmidi_stream_pair_set_substream_params:
 * @self: A #ALSARawmidiStreamPair.
 * @direction: The direction of substream attached to the stream pair.
 * @substream_params: The parameters of substream.
 * @error: A #GError.
 *
 * Set parameters of substream for given direction, which is attached to the
 * pair of streams.
 */
void alsarawmidi_stream_pair_set_substream_params(ALSARawmidiStreamPair *self,
                                ALSARawmidiStreamDirection direction,
                                ALSARawmidiSubstreamParams *substream_params,
                                GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    struct snd_rawmidi_params *params;

    g_return_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self));
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    rawmidi_substream_params_refer_private(substream_params, &params);

    params->stream = direction;
    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_PARAMS, params) < 0)
        generate_error(error, errno);
}

/**
 * alsarawmidi_stream_pair_get_substream_status:
 * @self: A #ALSARawmidiStreamPair.
 * @direction: The direction of substream attached to the stream pair.
 * @substream_status: (inout): The status of substream.
 * @error: A #GError.
 *
 * Retrieve status of substream for given direction, which is attached to the
 * pair of streams.
 */
void alsarawmidi_stream_pair_get_substream_status(ALSARawmidiStreamPair *self,
                            ALSARawmidiStreamDirection direction,
                            ALSARawmidiSubstreamStatus *const *substream_status,
                            GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    struct snd_rawmidi_status *status;

    g_return_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self));
    g_return_if_fail(substream_status != NULL);
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    rawmidi_substream_status_refer_private(*substream_status, &status);

    status->stream = direction;
    if (ioctl(priv->fd, SNDRV_RAWMIDI_IOCTL_STATUS, status) < 0)
        generate_error(error, errno);
}

/**
 * alsarawmidi_stream_pair_read_from_substream:
 * @self: A #ALSARawmidiStreamPair.
 * @buf: (array length=buf_size)(inout): The buffer to copy data.
 * @buf_size: The size of buffer.
 * @error: A #GError.
 *
 * Copy data from intermediate buffer to given buffer for substream attached to
 * the pair of streams. In a case that the instance is opened without
 * O_NONBLOCK flag and the intermediate buffer has no data, call of the API
 * is blocked till any data is available.
 */
void alsarawmidi_stream_pair_read_from_substream(ALSARawmidiStreamPair *self,
                                        guint8 *const *buf, gsize *buf_size,
                                        GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    int len;

    g_return_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self));
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    len = read(priv->fd, *buf, *buf_size);
    if (len < 0) {
        generate_error(error, errno);
        return;
    }

    *buf_size = len;
}

/**
 * alsarawmidi_stream_pair_write_to_substream:
 * @self: A #ALSARawmidiStreamPair.
 * @buf: (array length=buf_size): The buffer to copy data.
 * @buf_size: The size of buffer.
 * @error: A #GError.
 *
 * Copy data from given buffer to intermediate buffer for substream attached to
 * the pair of streams. In a case that the instance is opened without
 * O_NONBLOCK flag and the intermediate buffer is full, call of the API is
 * blocked till the buffer has space for the data.
 */
void alsarawmidi_stream_pair_write_to_substream(ALSARawmidiStreamPair *self,
                                        const guint8 *buf, gsize buf_size,
                                        GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    int len;

    g_return_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self));
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    len = write(priv->fd, buf, buf_size);
    if (len < 0) {
        generate_error(error, errno);
        return;
    }
}
