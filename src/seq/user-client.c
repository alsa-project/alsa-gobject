// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

/**
 * ALSASeqUserClient:
 * A GObject-derived object to represent user client.
 *
 * A [class@UserClient] is a GObject-derived object to represent user client. Any port can be added
 * to the client as destination or source for any event.
 *
 * When the call of [method@UserClient.open] the object maintain file descriptor till object
 * destruction. The call of [method@UserClient.create_source] returns the instance of
 * [struct@GLib.Source]. Once attached to the [struct@GLib.Source],
 * [struct@GLib.MainContext] / [struct@GLib.MainLoop] is available as event dispatcher. The
 * [signal@UserClient::handle-event] signal is emitted in the event dispatcher to notify the
 * event. The call of [method@UserClient.schedule_event] schedules event with given parameters.
 */
typedef struct {
    int fd;
    const char *devnode;
    int client_id;
    guint16 proto_ver_triplet[3];
} ALSASeqUserClientPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqUserClient, alsaseq_user_client, G_TYPE_OBJECT)

/**
 * alsaseq_user_client_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] which has code of [enum@UserClientError]
 * enumerations.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(alsaseq-user-client-error-quark, alsaseq_user_client_error)

static const char *const err_msgs[] = {
        [ALSASEQ_USER_CLIENT_ERROR_PORT_PERMISSION] = "The operation fails due to access permission of port",
        [ALSASEQ_USER_CLIENT_ERROR_QUEUE_PERMISSION] = "The operation fails due to access permission of queue",
};

#define generate_local_error(exception, code) \
        g_set_error_literal(exception, ALSASEQ_USER_CLIENT_ERROR, code, err_msgs[code])

#define generate_syscall_error(exception, errno, fmt, arg) \
        g_set_error(exception, ALSASEQ_USER_CLIENT_ERROR, ALSASEQ_USER_CLIENT_ERROR_FAILED, \
                    fmt" %d(%s)", arg, errno, strerror(errno))

typedef struct {
    GSource src;
    ALSASeqUserClient *self;
    gpointer tag;
    void *buf;
    size_t buf_len;
} UserClientSource;

enum seq_user_client_prop_type {
    SEQ_USER_CLIENT_PROP_CLIENT_ID = 1,
    SEQ_USER_CLIENT_PROP_COUNT,
};
static GParamSpec *seq_user_client_props[SEQ_USER_CLIENT_PROP_COUNT] = { NULL, };

enum seq_user_client_sig_type {
    SEQ_USER_CLIENT_SIG_TYPE_HANDLE_EVENT = 0,
    SEQ_USER_CLIENT_SIG_TYPE_COUNT,
};
static guint seq_user_client_sigs[SEQ_USER_CLIENT_SIG_TYPE_COUNT] = { 0 };

static void seq_user_client_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqUserClient *self = ALSASEQ_USER_CLIENT(obj);
    ALSASeqUserClientPrivate *priv =
                                alsaseq_user_client_get_instance_private(self);

    switch (id) {
    case SEQ_USER_CLIENT_PROP_CLIENT_ID:
        g_value_set_uchar(val, (guint8)priv->client_id);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_user_client_finalize(GObject *obj)
{
    ALSASeqUserClient *self = ALSASEQ_USER_CLIENT(obj);
    ALSASeqUserClientPrivate *priv =
                                alsaseq_user_client_get_instance_private(self);

    if (priv->fd >= 0)
        close(priv->fd);
    g_free((gpointer)priv->devnode);

    G_OBJECT_CLASS(alsaseq_user_client_parent_class)->finalize(obj);
}

static void alsaseq_user_client_class_init(ALSASeqUserClientClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = seq_user_client_finalize;
    gobject_class->get_property = seq_user_client_get_property;

    seq_user_client_props[SEQ_USER_CLIENT_PROP_CLIENT_ID] =
        g_param_spec_uchar("client-id", "client-id",
                           "The numeric ID of the client.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_USER_CLIENT_PROP_COUNT,
                                      seq_user_client_props);

    /**
     * ALSASeqUserClient::handle-event:
     * @self: A [class@UserClient].
     * @ev_cntr: (transfer none): The instance of [struct@EventCntr] which includes batch of events.
     *
     * When event occurs, this signal is emit with the instance of object which includes batch of
     * of events.
     */
    seq_user_client_sigs[SEQ_USER_CLIENT_SIG_TYPE_HANDLE_EVENT] =
        g_signal_new("handle-event",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSASeqUserClientClass, handle_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOXED,
                     G_TYPE_NONE, 1, ALSASEQ_TYPE_EVENT_CNTR);
}

static void alsaseq_user_client_init(ALSASeqUserClient *self)
{
    ALSASeqUserClientPrivate *priv =
                                alsaseq_user_client_get_instance_private(self);
    priv->fd = -1;
}

/**
 * alsaseq_user_client_new:
 *
 * Allocate and return an instance of [class@UserClient].
 *
 * Returns: An instance of [class@UserClient].
 */
ALSASeqUserClient *alsaseq_user_client_new()
{
    return g_object_new(ALSASEQ_TYPE_USER_CLIENT, NULL);
}

/**
 * alsaseq_user_client_open:
 * @self: A [class@UserClient].
 * @open_flag: The flag of `open(2)` system call. `O_RDWR` is forced to fulfil internally.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSASeq.UserClientError`.
 *
 * Open ALSA sequencer character device.
 *
 * The call of function executes `open(2)` system call, then executes `ioctl(2)` system call with
 * `SNDRV_SEQ_IOCTL_CLIENT_ID` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_open(ALSASeqUserClient *self, gint open_flag, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    char *devnode;
    int proto_ver;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!alsaseq_get_seq_devnode(&devnode, error))
        return FALSE;

    open_flag |= O_RDWR;
    priv->fd = open(devnode, open_flag);
    if (priv->fd < 0) {
        GFileError code = g_file_error_from_errno(errno);

        if (code != G_FILE_ERROR_FAILED)
            generate_file_error(error, code, "open(%s)", devnode);
        else
            generate_syscall_error(error, errno, "open(%s)", devnode);

        g_free(devnode);
        return FALSE;
    }
    priv->devnode = devnode;

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CLIENT_ID, &priv->client_id) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "CLIENT_ID");
        close(priv->fd);
        priv->fd = -1;
        return FALSE;
    }

    // Remember the version of protocol currently used.
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_PVERSION, &proto_ver) < 0) {
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
 * alsaseq_user_client_get_protocol_version:
 * @self: A [class@UserClient].
 * @proto_ver_triplet: (array fixed-size=3)(out)(transfer none): The version of protocol currently
 *                     used.
 * @error: A [struct@GLib.Error].
 *
 * Get the version of sequencer protocol currently used. The version is represented as the array
 * with three elements; major, minor, and micro version in the order. The length of major version
 * is 16 bit, the length of minor and micro version is 8 bit each.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_get_protocol_version(ALSASeqUserClient *self,
                                                  const guint16 *proto_ver_triplet[3],
                                                  GError **error)
{
    ALSASeqUserClientPrivate *priv;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(proto_ver_triplet != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *proto_ver_triplet = (const guint16 *)priv->proto_ver_triplet;

    return TRUE;
}

/**
 * alsaseq_user_client_set_info:
 * @self: A [class@UserClient].
 * @client_info: A [class@ClientInfo].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Get client information.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SET_CLIENT_INFO`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_set_info(ALSASeqUserClient *self, ALSASeqClientInfo *client_info,
                                      GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_info *info;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_CLIENT_INFO(client_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_client_info_refer_private(client_info, &info);
    info->client = priv->client_id;
    info->type = USER_CLIENT;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_CLIENT_INFO, info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "SET_CLIENT_INFO");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_get_info:
 * @self: A [class@UserClient].
 * @client_info: (inout): A [class@ClientInfo].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Set client information.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_GET_CLIENT_INFO`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_get_info(ALSASeqUserClient *self,
                                      ALSASeqClientInfo *const *client_info, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_info *info;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(client_info != NULL, FALSE);
    g_return_val_if_fail(ALSASEQ_IS_CLIENT_INFO(*client_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_client_info_refer_private(*client_info, &info);
    info->client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "GET_CLIENT_INFO");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_create_port:
 * @self: A [class@UserClient].
 * @port_info: (inout): A [class@PortInfo].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Create a port into the client.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_CREATE_PORT` command
 * for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_create_port(ALSASeqUserClient *self,
                                         ALSASeqPortInfo *const *port_info, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_info *info;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_PORT_INFO(*port_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_port_info_refer_private(*port_info, &info);

    info->addr.client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CREATE_PORT, info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "CREATE_PORT");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_create_port_at:
 * @self: A [class@UserClient].
 * @port_info: (inout): A [class@PortInfo].
 * @port_id: The numeric ID of port to create.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Create a port into the client with the given numeric port ID.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_CREATE_PORT` command
 * for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_create_port_at(ALSASeqUserClient *self,
                                            ALSASeqPortInfo *const *port_info,
                                            guint8 port_id, GError **error)
{
    struct snd_seq_port_info *info;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    g_return_val_if_fail(ALSASEQ_IS_PORT_INFO(*port_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_port_info_refer_private(*port_info, &info);

    info->addr.port = port_id;
    info->flags |= SNDRV_SEQ_PORT_FLG_GIVEN_PORT;

    return alsaseq_user_client_create_port(self, port_info, error);
}

/**
 * alsaseq_user_client_update_port:
 * @self: A [class@UserClient].
 * @port_info: A [class@PortInfo].
 * @port_id: The numeric ID of port.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Update port information.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SET_PORT_INFO` command
 * for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_update_port(ALSASeqUserClient *self, ALSASeqPortInfo *port_info,
                                         guint8 port_id, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_info *info;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_PORT_INFO(port_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_port_info_refer_private(port_info, &info);

    info->addr.client = priv->client_id;
    info->addr.port = port_id;

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_PORT_INFO, info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "SET_PORT_INFO");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_delete_port:
 * @self: A [class@UserClient].
 * @port_id: The numeric ID of port.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Delete a port from the client.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_DELETE_PORT` command
 * for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_delete_port(ALSASeqUserClient *self, guint8 port_id, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_info info = {0};

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    info.addr.client = priv->client_id;
    info.addr.port = port_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_DELETE_PORT, &info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "DELETE_PORT");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_set_pool:
 * @self: A [class@UserClient].
 * @client_pool: A [class@ClientPool] to be configured for the client.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Configure memory pool in the client.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SET_CLIENT_POOL`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_set_pool(ALSASeqUserClient *self, ALSASeqClientPool *client_pool,
                                      GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_pool *pool;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_CLIENT_POOL(client_pool), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_client_pool_refer_private(client_pool, &pool);
    pool->client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_CLIENT_POOL, pool) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "SET_CLIENT_POOL");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_get_pool:
 * @self: A [class@UserClient].
 * @client_pool: (inout): A [class@ClientPool] to be configured for the client.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Get information of memory pool in the client.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_GET_CLIENT_POOL`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_get_pool(ALSASeqUserClient *self,
                                      ALSASeqClientPool *const *client_pool, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_pool *pool;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_CLIENT_POOL(*client_pool), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_client_pool_refer_private(*client_pool, &pool);
    pool->client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_CLIENT_POOL, pool) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "GET_CLIENT_POOL");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_schedule_event:
 * @self: A [class@UserClient].
 * @event: An instance of [struct@Event].
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSASeq.UserClientError`.
 *
 * Deliver the event immediately, or schedule it into memory pool of the client.
 *
 * The call of function executes `write(2)` system call for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_schedule_event(ALSASeqUserClient *self, const ALSASeqEvent *event,
                                            GError **error)
{
    ALSASeqUserClientPrivate *priv;
    size_t length;
    guint8 *buf;
    ssize_t result;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    priv = alsaseq_user_client_get_instance_private(self);

    if (!seq_event_is_deliverable(event)) {
        g_set_error_literal(error, ALSASEQ_USER_CLIENT_ERROR,
                            ALSASEQ_USER_CLIENT_ERROR_EVENT_UNDELIVERABLE,
                            "The operation failes due to undeliverable event");
        return FALSE;
    }

    length = seq_event_calculate_flattened_length(event, FALSE);
    if (length == sizeof(*event)) {
        buf = (guint8 *)event;
    } else {
        buf = g_malloc0(length);

        seq_event_copy_flattened(event, buf, length);
    }

    result = write(priv->fd, buf, length);
    if (length != sizeof(*event))
        g_free(buf);
    if (result < 0) {
        GFileError code = g_file_error_from_errno(errno);

        if (code != G_FILE_ERROR_FAILED)
            generate_file_error(error, errno, "write(%s)", priv->devnode);
        else
            generate_syscall_error(error, errno, "write(%s)", priv->devnode);

        return FALSE;
    }

    g_return_val_if_fail(result == length, FALSE);

    return TRUE;
}

/**
 * alsaseq_user_client_schedule_events:
 * @self: A [class@UserClient].
 * @events: (element-type ALSASeq.Event) (transfer none): The list of [struct@Event].
 * @count: (out): The number of events to be scheduled.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSASeq.UserClientError`.
 *
 * Deliver the events immediately, or schedule it into memory pool of the client.
 *
 * The call of function executes `write(2)` system call for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_schedule_events(ALSASeqUserClient *self, const GList *events,
                                             gsize *count, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    ALSASeqEventCntr cntr = { 0 };
    const GList *entry;
    gsize index;
    gsize pos;
    ssize_t result;
    gsize scheduled;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    index = 0;
    for (entry = events; entry != NULL; entry = g_list_next(entry)) {
        const struct snd_seq_event *ev = (const struct snd_seq_event *)entry->data;

        g_return_val_if_fail(ev != NULL, FALSE);
        if (!seq_event_is_deliverable(ev)) {
            g_set_error(error, ALSASEQ_USER_CLIENT_ERROR,
                        ALSASEQ_USER_CLIENT_ERROR_EVENT_UNDELIVERABLE,
                        "The operation failes due to undeliverable event: index %lu",
                        index);
            return FALSE;
        }
        ++index;
    }

    seq_event_cntr_serialize(&cntr, events, FALSE);

    // Nothing to do.
    if (cntr.length == 0)
        return TRUE;

    result = write(priv->fd, cntr.buf, cntr.length);
    g_free(cntr.buf);
    if (result < 0) {
        GFileError code = g_file_error_from_errno(errno);

        if (code != G_FILE_ERROR_FAILED)
            generate_file_error(error, errno, "write(%s)", priv->devnode);
        else
            generate_syscall_error(error, errno, "write(%s)", priv->devnode);

        return FALSE;
    }

    // Compute the count of scheduled events.
    pos = 0;
    scheduled = 0;
    for (entry = events; entry != NULL; entry = g_list_next(entry)) {
        const struct snd_seq_event *ev = (const struct snd_seq_event *)entry->data;

        ++scheduled;

        pos += seq_event_calculate_flattened_length(ev, FALSE);
        if (pos >= result)
            break;
    }

    g_return_val_if_fail(result == pos, FALSE);

    *count = scheduled;

    return TRUE;
}

static gboolean seq_user_client_check_src(GSource *gsrc)
{
    UserClientSource *src = (UserClientSource *)gsrc;
    GIOCondition condition;

    // Don't go to dispatch if nothing available. As an exception, return TRUE
    // for POLLERR to call .dispatch for internal destruction.
    condition = g_source_query_unix_fd(gsrc, src->tag);
    return !!(condition & (G_IO_IN | G_IO_ERR));
}

static gboolean seq_user_client_dispatch_src(GSource *gsrc, GSourceFunc cb,
                                             gpointer user_data)
{
    UserClientSource *src = (UserClientSource *)gsrc;
    ALSASeqUserClient *self = src->self;
    ALSASeqUserClientPrivate *priv;
    GIOCondition condition;
    ALSASeqEventCntr ev_cntr = { 0 };
    int len;

    priv = alsaseq_user_client_get_instance_private(self);
    if (priv->fd < 0)
        return G_SOURCE_REMOVE;

    condition = g_source_query_unix_fd(gsrc, src->tag);
    if (condition & G_IO_ERR)
        return G_SOURCE_REMOVE;

    len = read(priv->fd, src->buf, src->buf_len);
    if (len < 0) {
        if (errno == EAGAIN)
            return G_SOURCE_CONTINUE;

        return G_SOURCE_REMOVE;
    }

    // NOTE: The buffer is flatten layout.
    ev_cntr.buf = src->buf;
    ev_cntr.length = len;
    ev_cntr.aligned = TRUE;

    g_signal_emit(self, seq_user_client_sigs[SEQ_USER_CLIENT_SIG_TYPE_HANDLE_EVENT], 0, &ev_cntr);

    // Just be sure to continue to process this source.
    return G_SOURCE_CONTINUE;
}

static void seq_user_client_finalize_src(GSource *gsrc)
{
    UserClientSource *src = (UserClientSource *)gsrc;

    g_free(src->buf);
    g_object_unref(src->self);
}

/**
 * alsaseq_user_client_create_source:
 * @self: A [class@UserClient].
 * @gsrc: (out): A #GSource to handle events from ALSA seq character device.
 * @error: A [struct@GLib.Error].
 *
 * Allocate [struct@GLib.Source] structure to handle events from ALSA seq character device. In each
 * iteration of [struct@GLib.MainContext], the `read(2)` system call is exected to dispatch
 * sequencer event for [signal@UserClient::handle-event] signal, according to the result of
 * `poll(2)` system call.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_create_source(ALSASeqUserClient *self, GSource **gsrc, GError **error)
{
    static GSourceFuncs funcs = {
            .check          = seq_user_client_check_src,
            .dispatch       = seq_user_client_dispatch_src,
            .finalize       = seq_user_client_finalize_src,
    };
    ALSASeqUserClientPrivate *priv;
    UserClientSource *src;
    void *buf;
    long page_size = sysconf(_SC_PAGESIZE);

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(gsrc != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    buf = g_malloc0(page_size);

    *gsrc = g_source_new(&funcs, sizeof(*src));
    src = (UserClientSource *)(*gsrc);

    g_source_set_name(*gsrc, "ALSASeqUserClient");
    g_source_set_priority(*gsrc, G_PRIORITY_HIGH_IDLE);
    g_source_set_can_recurse(*gsrc, TRUE);

    src->self = g_object_ref(self);
    src->tag = g_source_add_unix_fd(*gsrc, priv->fd, G_IO_IN);
    src->buf = buf;
    src->buf_len = page_size;

    return TRUE;
}

/**
 * alsaseq_user_client_operate_subscription:
 * @self: A [class@UserClient].
 * @subs_data: A [class@SubscribeData].
 * @establish: Whether to establish subscription between two ports, or break it.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Operate subscription between two ports pointed by the data.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT` and
 * `SNDRV_SEQ_IOCTL_UNSUBSCRIBE_PORT` commands for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_operate_subscription(ALSASeqUserClient *self,
                                                  ALSASeqSubscribeData *subs_data,
                                                  gboolean establish, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_subscribe *data;
    long request;
    const char *label;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_SUBSCRIBE_DATA(subs_data), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_subscribe_data_refer_private(subs_data, &data);

    if (establish) {
        request = SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT;
        label = "SUBSCRIBE_PORT";
    } else {
        request = SNDRV_SEQ_IOCTL_UNSUBSCRIBE_PORT;
        label = "UNSUBSCRIBE_PORT";
    }

    if (ioctl(priv->fd, request, data) < 0) {
        if (errno == EPERM)
            generate_local_error(error, ALSASEQ_USER_CLIENT_ERROR_PORT_PERMISSION);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", label);
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_create_queue:
 * @self: A [class@UserClient].
 * @queue_info: (inout): The information of queue to add.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Create a new queue owned by the client. The content of information is updated if success.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_CREATE_QUEUE` command
 * for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_create_queue(ALSASeqUserClient *self,
                                          ALSASeqQueueInfo *const *queue_info, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_info *info;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_QUEUE_INFO(*queue_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_queue_info_refer_private(*queue_info, &info);

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CREATE_QUEUE, info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "CREATE_QUEUE");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_delete_queue:
 * @self: A [class@UserClient].
 * @queue_id: The numeric ID of queue, except for one of [enum@SpecificQueueId].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Delete the queue owned by the client.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_DELETE_QUEUE` command
 * for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_delete_queue(ALSASeqUserClient *self, guint8 queue_id, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_info info = {0};

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    info.queue = (int)queue_id;
    info.owner = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_DELETE_QUEUE, &info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "DELETE_QUEUE");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_update_queue:
 * @self: A [class@UserClient].
 * @queue_info: The information of queue to add.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Update owned queue according to the information.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SET_QUEUE_INFO`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_update_queue(ALSASeqUserClient *self, ALSASeqQueueInfo *queue_info,
                                          GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_info *info;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_QUEUE_INFO(queue_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_queue_info_refer_private(queue_info, &info);

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_INFO, info) < 0) {
        if (errno == EPERM)
            generate_local_error(error, ALSASEQ_USER_CLIENT_ERROR_QUEUE_PERMISSION);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "SET_QUEUE_INFO");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_get_queue_usage:
 * @self: A [class@UserClient].
 * @queue_id: The numeric ID of queue, except for entries in [enum@SpecificQueueId].
 * @use: (out): Whether the client uses the queue or not.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Get usage of the queue by the client.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_GET_QUEUE_CLIENT`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_get_queue_usage(ALSASeqUserClient *self, guint8 queue_id,
                                             gboolean *use, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_client data = {0};

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(use != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    data.queue = (int)queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_QUEUE_CLIENT, &data) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "GET_QUEUE_CLIENT");
        return FALSE;
    }

    *use = data.used;
    return TRUE;
}

/**
 * alsaseq_user_client_set_queue_usage:
 * @self: A [class@UserClient].
 * @queue_id: The numeric ID of queue, except for entries in [enum@SpecificQueueId].
 * @use: Whether to use the queue or not.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Start the queue to use or not.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SET_QUEUE_CLIENT`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_set_queue_usage(ALSASeqUserClient *self, guint8 queue_id,
                                             gboolean use, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_client data = {0};

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    data.queue = (int)queue_id;
    data.client = priv->client_id;
    data.used = use;

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_CLIENT, &data) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "SET_QUEUE_CLIENT");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_set_queue_tempo:
 * @self: A [class@UserClient].
 * @queue_id: The numeric ID of queue, except for entries in [enum@SpecificQueueId].
 * @queue_tempo: The data of tempo for queue.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Set the data of tempo to the queue.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SET_QUEUE_TEMPO`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_set_queue_tempo(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTempo *queue_tempo, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_tempo *tempo;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_QUEUE_TEMPO(queue_tempo), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    seq_queue_tempo_refer_private(queue_tempo, &tempo);
    tempo->queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_TEMPO, tempo) < 0) {
        if (errno == EPERM)
            generate_local_error(error, ALSASEQ_USER_CLIENT_ERROR_QUEUE_PERMISSION);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "SET_QUEUE_TEMPO");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_get_queue_tempo:
 * @self: A [class@UserClient].
 * @queue_id: The numeric ID of queue, except for entries in [enum@SpecificQueueId].
 * @queue_tempo: (out): The data of tempo for queue.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Get the data of tempo for the queue.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_GET_QUEUE_TEMPO`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_get_queue_tempo(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTempo **queue_tempo, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_tempo *tempo;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(queue_tempo != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *queue_tempo = g_object_new(ALSASEQ_TYPE_QUEUE_TEMPO, NULL);
    seq_queue_tempo_refer_private(*queue_tempo, &tempo);

    tempo->queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_QUEUE_TEMPO, tempo) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "GET_QUEUE_TEMPO");
        g_object_unref(*queue_tempo);
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_set_queue_timer:
 * @self: A [class@UserClient].
 * @queue_id: The numeric ID of queue, except for entries in [enum@SpecificQueueId].
 * @queue_timer: The data of timer for queue, which implements [iface@QueueTimerCommon].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Set the data of timer for the queue.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_SET_QUEUE_TIMER`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_set_queue_timer(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTimerCommon *queue_timer, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    ALSASeqQueueTimerType timer_type;
    struct snd_seq_queue_timer *data;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(ALSASEQ_IS_QUEUE_TIMER_COMMON(queue_timer), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    g_object_get(queue_timer, TIMER_TYPE_PROP_NAME, &timer_type, NULL);

    switch ((int)timer_type) {
    case SNDRV_SEQ_TIMER_ALSA:
        seq_queue_timer_alsa_refer_private(ALSASEQ_QUEUE_TIMER_ALSA(queue_timer), &data);
        break;
    case SNDRV_SEQ_TIMER_MIDI_CLOCK:
    case SNDRV_SEQ_TIMER_MIDI_TICK:
    default:
        // Not supported.
        g_return_val_if_reached(FALSE);
    }

    data->queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_TIMER, data) < 0) {
        if (errno == EPERM)
            generate_local_error(error, ALSASEQ_USER_CLIENT_ERROR_QUEUE_PERMISSION);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "SET_QUEUE_TIMER");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_user_client_get_queue_timer:
 * @self: A [class@UserClient].
 * @queue_id: The numeric ID of queue, except for entries in [enum@SpecificQueueId].
 * @queue_timer: (out): The data of timer for queue, which implements [iface@QueueTimerCommon].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Get the data of timer for the queue.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_GET_QUEUE_TIMER`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_get_queue_timer(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTimerCommon **queue_timer, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_timer *dst, data = {0};

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(queue_timer != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    data.queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_QUEUE_TIMER, &data) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "GET_QUEUE_TIMER");
        return FALSE;
    }

    switch (data.type) {
    case SNDRV_SEQ_TIMER_ALSA:
        *queue_timer = g_object_new(ALSASEQ_TYPE_QUEUE_TIMER_ALSA, NULL);
        seq_queue_timer_alsa_refer_private(ALSASEQ_QUEUE_TIMER_ALSA(*queue_timer), &dst);
        break;
    case SNDRV_SEQ_TIMER_MIDI_CLOCK:
    case SNDRV_SEQ_TIMER_MIDI_TICK:
    default:
        // Not supported.
        g_return_val_if_reached(FALSE);
    }

    *dst = data;

    return TRUE;
}

/**
 * alsaseq_user_client_remove_events:
 * @self: A [class@UserClient].
 * @filter: A [struct@RemoveFilter].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSASeq.UserClientError`.
 *
 * Remove queued events according to the filter.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_SEQ_IOCTL_REMOVE_EVENTS`
 * command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_user_client_remove_events(ALSASeqUserClient *self, ALSASeqRemoveFilter *filter,
                                           GError **error)
{
    ALSASeqUserClientPrivate *priv;

    g_return_val_if_fail(ALSASEQ_IS_USER_CLIENT(self), FALSE);
    priv = alsaseq_user_client_get_instance_private(self);

    g_return_val_if_fail(filter != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_REMOVE_EVENTS, filter) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "REMOVE_EVENTS");
        return FALSE;
    }

    return TRUE;
}
