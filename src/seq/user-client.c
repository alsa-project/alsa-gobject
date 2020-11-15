// SPDX-License-Identifier: LGPL-3.0-or-later
#include "user-client.h"
#include "query.h"
#include "privates.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

/**
 * SECTION: user-client
 * @Title: ALSASeqUserClient
 * @Short_description: A GObject-derived object to represent user client
 *
 * A #ALSASeqUserClient is a GObject-derived object to represent user client.
 * Any port can be added to the client as destination or source for any event.
 *
 * When the call of alsaseq_user_client_open(), the object maintain file
 * descriptor till object destruction. The call of
 * alsaseq_user_client_create_source() returns the instance of GSource. Once
 * attached to the GSource, GMainContext/GMainLoop is available as event
 * dispatcher. The #handle-event GObject signal is emitted in the event
 * dispatcher to notify the event. The call of
 * alsaseq_user_client_schedule_event() schedules event with given parameters.
 */
struct _ALSASeqUserClientPrivate {
    int fd;
    int client_id;
    guint16 proto_ver_triplet[3];
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqUserClient, alsaseq_user_client, G_TYPE_OBJECT)

typedef struct {
    GSource src;
    ALSASeqUserClient *self;
    gpointer tag;
    void *buf;
    size_t buf_len;
    ALSASeqEventCntr *ev_cntr;
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

    G_OBJECT_CLASS(alsaseq_user_client_parent_class)->finalize(obj);
}

static void alsaseq_user_client_class_init(ALSASeqUserClientClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = seq_user_client_finalize;
    gobject_class->get_property = seq_user_client_get_property;

    seq_user_client_props[SEQ_USER_CLIENT_PROP_CLIENT_ID] =
        g_param_spec_uchar("client-id", "client-id",
                           "The numerical ID of the client.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_USER_CLIENT_PROP_COUNT,
                                      seq_user_client_props);

    /**
     * ALSASeqUserClient::handle-event:
     * @self: A #ALSASeqUserClient.
     * @ev_cntr: (transfer none): The instance of ALSASeqEventCntr which
     *             points to the batch of events.
     *
     * When event occurs, this signal is emit with the instance of object which
     * points to a batch of events. The instance should not be passed directly
     * to alsaseq_user_client_schedule_event() again because its memory
     * alignment is different for events with blob data.
     */
    seq_user_client_sigs[SEQ_USER_CLIENT_SIG_TYPE_HANDLE_EVENT] =
        g_signal_new("handle-event",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSASeqUserClientClass, handle_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
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
 * Allocate and return an instance of ALSASeqUserClient class.
 */
ALSASeqUserClient *alsaseq_user_client_new()
{
    return g_object_new(ALSASEQ_TYPE_USER_CLIENT, NULL);
}

/**
 * alsaseq_user_client_open:
 * @self: A #ALSASeqUserClient.
 * @open_flag: The flag of open(2) system call. O_RDWR is forced to fulfil internally.
 * @error: A #GError.
 *
 * Open ALSA sequencer character device.
 *
 * The call of function executes open(2) system call, then executes ioctl(2)
 * system call with SNDRV_SEQ_IOCTL_CLIENT_ID command for ALSA sequencer
 * character device.
 */
void alsaseq_user_client_open(ALSASeqUserClient *self, gint open_flag,
                              GError **error)
{
    ALSASeqUserClientPrivate *priv;
    char *devnode;
    int proto_ver;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    open_flag |= O_RDWR;
    priv->fd = open(devnode, open_flag);
    g_free(devnode);
    if (priv->fd < 0) {
        generate_error(error, errno);
        return;
    }

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CLIENT_ID, &priv->client_id) < 0) {
        generate_error(error, errno);
        close(priv->fd);
        priv->fd = -1;
    }

    // Remember the version of protocol currently used.
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_PVERSION, &proto_ver) < 0) {
        generate_error(error, errno);
        close(priv->fd);
        priv->fd = -1;
        return;
    }

    priv->proto_ver_triplet[0] = SNDRV_PROTOCOL_MAJOR(proto_ver);
    priv->proto_ver_triplet[1] = SNDRV_PROTOCOL_MINOR(proto_ver);
    priv->proto_ver_triplet[2] = SNDRV_PROTOCOL_MICRO(proto_ver);
}

/**
 * alsaseq_user_client_get_protocol_version:
 * @self: A #ALSASeqUserClient.
 * @proto_ver_triplet: (array fixed-size=3)(out)(transfer none): The version of
 *                     protocol currently used.
 * @error: A #GError.
 *
 * Get the version of sequencer protocol currently used. The version is
 * represented as the array with three elements; major, minor, and micro version
 * in the order. The length of major version is 16 bit, the length of minor
 * and micro version is 8 bit each.
 */
void alsaseq_user_client_get_protocol_version(ALSASeqUserClient *self,
                                        const guint16 *proto_ver_triplet[3],
                                        GError **error)
{
    ALSASeqUserClientPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    if (priv->fd < 0) {
        generate_error(error, ENXIO);
        return;
    }

    *proto_ver_triplet = (const guint16 *)priv->proto_ver_triplet;
}

/**
 * alsaseq_user_client_set_info:
 * @self: A #ALSASeqUserClient.
 * @client_info: A #ALSASeqClientInfo.
 * @error: A #GError.
 *
 * Get client information.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SET_CLIENT_INFO command for ALSA sequencer character device.
 */
void alsaseq_user_client_set_info(ALSASeqUserClient *self,
                                  ALSASeqClientInfo *client_info,
                                  GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_CLIENT_INFO(client_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_client_info_refer_private(client_info, &info);
    info->client = priv->client_id;
    info->type = USER_CLIENT;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_CLIENT_INFO, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_get_info:
 * @self: A #ALSASeqUserClient.
 * @client_info: (inout): A #ALSASeqClientInfo.
 * @error: A #GError.
 *
 * Set client information.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_GET_CLIENT_INFO command for ALSA sequencer character device.
 */
void alsaseq_user_client_get_info(ALSASeqUserClient *self,
                                  ALSASeqClientInfo *const *client_info,
                                  GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(client_info != NULL);
    g_return_if_fail(ALSASEQ_IS_CLIENT_INFO(*client_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_client_info_refer_private(*client_info, &info);
    info->client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_create_port:
 * @self: A #ALSASeqUserClient.
 * @port_info: (inout): A #ALSASeqPortInfo.
 * @error: A #GError.
 *
 * Create a port into the client.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_CREATE_PORT command for ALSA sequencer character device.
 */
void alsaseq_user_client_create_port(ALSASeqUserClient *self,
                                     ALSASeqPortInfo *const *port_info,
                                     GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_PORT_INFO(*port_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_port_info_refer_private(*port_info, &info);

    info->addr.client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CREATE_PORT, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_create_port_at:
 * @self: A #ALSASeqUserClient.
 * @port_info: (inout): A #ALSASeqPortInfo.
 * @port_id: The numerical ID of port to create.
 * @error: A #GError.
 *
 * Create a port into the client with the given numerical port ID.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_CREATE_PORT command for ALSA sequencer character device.
 */
void alsaseq_user_client_create_port_at(ALSASeqUserClient *self,
                                        ALSASeqPortInfo *const *port_info,
                                        guint8 port_id, GError **error)
{
    struct snd_seq_port_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_PORT_INFO(*port_info));

    seq_port_info_refer_private(*port_info, &info);

    info->addr.port = port_id;
    info->flags |= SNDRV_SEQ_PORT_FLG_GIVEN_PORT;

    alsaseq_user_client_create_port(self, port_info, error);
}

/**
 * alsaseq_user_client_update_port:
 * @self: A #ALSASeqUserClient.
 * @port_info: A #ALSASeqPortInfo.
 * @port_id: The numerical ID of port.
 * @error: A #GError.
 *
 * Update port information.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SET_PORT_INFO command for ALSA sequencer character device.
 */
void alsaseq_user_client_update_port(ALSASeqUserClient *self,
                                     ALSASeqPortInfo *port_info,
                                     guint8 port_id, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_PORT_INFO(port_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_port_info_refer_private(port_info, &info);

    info->addr.client = priv->client_id;
    info->addr.port = port_id;

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_PORT_INFO, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_delete_port:
 * @self: A #ALSASeqUserClient.
 * @port_id: The numerical ID of port.
 * @error: A #GError.
 *
 * Delete a port from the client.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_DELETE_PORT command for ALSA sequencer character device.
 */
void alsaseq_user_client_delete_port(ALSASeqUserClient *self,
                                     guint8 port_id, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_info info = {0};

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    info.addr.client = priv->client_id;
    info.addr.port = port_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_DELETE_PORT, &info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_set_pool:
 * @self: A #ALSASeqUserClient.
 * @client_pool: A #ALSASeqClientPool to be configured for the client.
 * @error: A #GError.
 *
 * Configure memory pool in the client.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SET_CLIENT_POOL command for ALSA sequencer character device.
 */
void alsaseq_user_client_set_pool(ALSASeqUserClient *self,
                                  ALSASeqClientPool *client_pool,
                                  GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_pool *pool;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_CLIENT_POOL(client_pool));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_client_pool_refer_private(client_pool, &pool);
    pool->client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_CLIENT_POOL, pool) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_get_pool:
 * @self: A #ALSASeqUserClient.
 * @client_pool: (inout): A #ALSASeqClientPool to be configured for the client.
 * @error: A #GError.
 *
 * Get information of memory pool in the client.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_GET_CLIENT_POOL command for ALSA sequencer character device.
 */
void alsaseq_user_client_get_pool(ALSASeqUserClient *self,
                                  ALSASeqClientPool *const *client_pool,
                                  GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_client_pool *pool;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(*client_pool != NULL);
    g_return_if_fail(ALSASEQ_IS_CLIENT_POOL(*client_pool));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_client_pool_refer_private(*client_pool, &pool);
    pool->client = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_CLIENT_POOL, pool) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_schedule_event:
 * @self: A #ALSASeqUserClient.
 * @ev_cntr: An instance of #ALSASeqEventCntr pointing to events.
 * @count: The number of events in the ev_cntr to write.
 * @error: A #GError.
 *
 * Deliver the event immediately, or schedule it into memory pool of the client.
 *
 * The call of function executes write(2) system call for ALSA sequencer
 * character device.
 */
void alsaseq_user_client_schedule_event(ALSASeqUserClient *self,
                                        ALSASeqEventCntr *ev_cntr,
                                        gsize count, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    gsize total;
    const guint8 *buf;
    gsize length;
    ssize_t len;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(ev_cntr));
    priv = alsaseq_user_client_get_instance_private(self);

    alsaseq_event_cntr_count_events(ev_cntr, &total);
    if (count > total) {
        generate_error(error, ENOBUFS);
        return;
    }

    seq_event_cntr_get_buf(ev_cntr, count, &buf, &length);
    if (buf == NULL || length == 0) {
        generate_error(error, ENODATA);
        return;
    }

    len = write(priv->fd, buf, length);
    if (len < 0)
        generate_error(error, errno);
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

    seq_event_cntr_set_buf(src->ev_cntr, src->buf, len);

    g_signal_emit(self,
                seq_user_client_sigs[SEQ_USER_CLIENT_SIG_TYPE_HANDLE_EVENT],
                0, src->ev_cntr);

    // Just be sure to continue to process this source.
    return G_SOURCE_CONTINUE;
}

static void seq_user_client_finalize_src(GSource *gsrc)
{
    UserClientSource *src = (UserClientSource *)gsrc;

    g_object_unref(src->ev_cntr);

    g_free(src->buf);
    g_object_unref(src->self);
}

/**
 * alsaseq_user_client_create_source:
 * @self: A #ALSASeqUserClient.
 * @gsrc: (out): A #GSource to handle events from ALSA seq character device.
 * @error: A #GError.
 *
 * Allocate GSource structure to handle events from ALSA seq character device.
 * In each iteration of GMainContext, the read(2) system call is exected to
 * dispatch sequencer event for 'handle-event' signal, according to the result
 * of poll(2) system call.
 */
void alsaseq_user_client_create_source(ALSASeqUserClient *self,
                                       GSource **gsrc, GError **error)
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

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    if (priv->fd < 0) {
        generate_error(error, ENXIO);
        return;
    }

    buf = g_malloc0(page_size);

    *gsrc = g_source_new(&funcs, sizeof(*src));
    src = (UserClientSource *)(*gsrc);

    src->ev_cntr = g_object_new(ALSASEQ_TYPE_EVENT_CNTR, NULL);

    g_source_set_name(*gsrc, "ALSASeqUserClient");
    g_source_set_priority(*gsrc, G_PRIORITY_HIGH_IDLE);
    g_source_set_can_recurse(*gsrc, TRUE);

    src->self = g_object_ref(self);
    src->tag = g_source_add_unix_fd(*gsrc, priv->fd, G_IO_IN);
    src->buf = buf;
    src->buf_len = page_size;
}

/**
 * alsaseq_user_client_operate_subscription:
 * @self: A #ALSASeqUserClient.
 * @subs_data: A #ALSASeqSubscribeData.
 * @establish: Whether to establish subscription between two ports, or break it.
 * @error: A #GError.
 *
 * Operate subscription between two ports pointed by the data.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT and SNDRV_SEQ_IOCTL_UNSUBSCRIBE_PORT commands
 * for ALSA sequencer character device.
 */
void alsaseq_user_client_operate_subscription(ALSASeqUserClient *self,
                                         ALSASeqSubscribeData *subs_data,
                                         gboolean establish,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_subscribe *data;
    long request;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_SUBSCRIBE_DATA(subs_data));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_subscribe_data_refer_private(subs_data, &data);

    if (establish)
        request = SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT;
    else
        request = SNDRV_SEQ_IOCTL_UNSUBSCRIBE_PORT;

    if (ioctl(priv->fd, request, data) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_create_queue:
 * @self: A #ALSASeqUserClient.
 * @queue_info: (inout): The information of queue to add.
 * @error: A #GError.
 *
 * Create a new queue owned by the client. The content of information is updated
 * if success.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_CREATE_QUEUE command for ALSA sequencer character device.
 */
void alsaseq_user_client_create_queue(ALSASeqUserClient *self,
                                      ALSASeqQueueInfo *const *queue_info,
                                      GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_QUEUE_INFO(*queue_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_queue_info_refer_private(*queue_info, &info);

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CREATE_QUEUE, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_delete_queue:
 * @self: A #ALSASeqUserClient.
 * @queue_id: The numerical ID of queue, except for one of ALSASeqSpecificQueueId.
 * @error: A #GError.
 *
 * Delete the queue owned by the client.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_DELETE_QUEUE command for ALSA sequencer character device.
 */
void alsaseq_user_client_delete_queue(ALSASeqUserClient *self,
                                      guint8 queue_id, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_info info = {0};

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    info.queue = (int)queue_id;
    info.owner = priv->client_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_DELETE_QUEUE, &info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_update_queue:
 * @self: A #ALSASeqUserClient.
 * @queue_info: The information of queue to add.
 * @error: A #GError.
 *
 * Update owned queue according to the information.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SET_QUEUE_INFO command for ALSA sequencer character device.
 */
void alsaseq_user_client_update_queue(ALSASeqUserClient *self,
                                      ALSASeqQueueInfo *queue_info,
                                      GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_QUEUE_INFO(queue_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_queue_info_refer_private(queue_info, &info);

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_INFO, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_get_queue_usage:
 * @self: A #ALSASeqUserClient.
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @use: (out): Whether the client uses the queue or not.
 * @error: A #GError.
 *
 * Get usage of the queue by the client.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_GET_QUEUE_CLIENT command for ALSA sequencer character device.
 */
void alsaseq_user_client_get_queue_usage(ALSASeqUserClient *self,
                                         guint8 queue_id, gboolean *use,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_client data = {0};

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    data.queue = (int)queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_QUEUE_CLIENT, &data) < 0) {
        generate_error(error, errno);
        return;
    }

    *use = data.used;
}

/**
 * alsaseq_user_client_set_queue_usage:
 * @self: A #ALSASeqUserClient.
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @use: Whether to use the queue or not.
 * @error: A #GError.
 *
 * Start the queue to use or not.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SET_QUEUE_CLIENT command for ALSA sequencer character device.
 */
void alsaseq_user_client_set_queue_usage(ALSASeqUserClient *self,
                                         guint8 queue_id, gboolean use,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_client data = {0};

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    data.queue = (int)queue_id;
    data.client = priv->client_id;
    data.used = use;

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_CLIENT, &data) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_set_queue_tempo:
 * @self: A #ALSASeqUserClient.
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @queue_tempo: The data of tempo for queue.
 * @error: A #GError.
 *
 * Set the data of tempo to the queue.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SET_QUEUE_TEMPO command for ALSA sequencer character device.
 */
void alsaseq_user_client_set_queue_tempo(ALSASeqUserClient *self,
                                guint8 queue_id, ALSASeqQueueTempo *queue_tempo,
                                GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_tempo *tempo;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_QUEUE_TEMPO(queue_tempo));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_queue_tempo_refer_private(queue_tempo, &tempo);
    tempo->queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_TEMPO, tempo) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_get_queue_tempo:
 * @self: A #ALSASeqUserClient.
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @queue_tempo: (out): The data of tempo for queue.
 * @error: A #GError.
 *
 * Get the data of tempo for the queue.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_GET_QUEUE_TEMPO command for ALSA sequencer character device.
 */
void alsaseq_user_client_get_queue_tempo(ALSASeqUserClient *self,
                                guint8 queue_id, ALSASeqQueueTempo **queue_tempo,
                                GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_tempo *tempo;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(queue_tempo != NULL);
    priv = alsaseq_user_client_get_instance_private(self);

    *queue_tempo = g_object_new(ALSASEQ_TYPE_QUEUE_TEMPO, NULL);
    seq_queue_tempo_refer_private(*queue_tempo, &tempo);

    tempo->queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_QUEUE_TEMPO, tempo) < 0) {
        generate_error(error, errno);
        g_object_unref(*queue_tempo);
    }
}

/**
 * alsaseq_user_client_set_queue_timer:
 * @self: A #ALSASeqUserClient.
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @queue_timer: The data of timer for queue.
 * @error: A #GError.
 *
 * Set the data of timer for the queue.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_SET_QUEUE_TIMER command for ALSA sequencer character device.
 */
void alsaseq_user_client_set_queue_timer(ALSASeqUserClient *self,
                                         guint8 queue_id,
                                         ALSASeqQueueTimer *queue_timer,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_timer *timer;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_QUEUE_TIMER(queue_timer));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_queue_timer_refer_private(queue_timer, &timer);

    switch (timer->type) {
    case SNDRV_SEQ_TIMER_ALSA:
        break;
    case SNDRV_SEQ_TIMER_MIDI_CLOCK:
    case SNDRV_SEQ_TIMER_MIDI_TICK:
    default:
        generate_error(error, EINVAL);
        return;
    }

    timer->queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_TIMER, timer) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_get_queue_timer:
 * @self: A #ALSASeqUserClient.
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @queue_timer: (out): The data of timer for queue.
 * @error: A #GError.
 *
 * Get the data of timer for the queue.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_GET_QUEUE_TIMER command for ALSA sequencer character device.
 */
void alsaseq_user_client_get_queue_timer(ALSASeqUserClient *self,
                                         guint8 queue_id,
                                         ALSASeqQueueTimer **queue_timer,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_timer *timer;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(queue_timer != NULL);
    priv = alsaseq_user_client_get_instance_private(self);

    *queue_timer = g_object_new(ALSASEQ_TYPE_QUEUE_TIMER, NULL);
    seq_queue_timer_refer_private(*queue_timer, &timer);

    timer->queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_QUEUE_TIMER, timer) < 0) {
        generate_error(error, errno);
        return;
    }

    switch (timer->type) {
    case SNDRV_SEQ_TIMER_ALSA:
        break;
    case SNDRV_SEQ_TIMER_MIDI_CLOCK:
    case SNDRV_SEQ_TIMER_MIDI_TICK:
    default:
        // Not available.
        g_object_unref(*queue_timer);
        *queue_timer = NULL;
        generate_error(error, ENXIO);
        return;
    }
}

/**
 * alsaseq_user_client_remove_events:
 * @self: A #ALSASeqUserClient.
 * @filter: A #ALSASeqRemoveFilter.
 * @error: A #GError.
 *
 * Remove queued events according to the filter.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_SEQ_IOCTL_REMOVE_EVENTS command for ALSA sequencer character device.
 */
void alsaseq_user_client_remove_events(ALSASeqUserClient *self,
                                       ALSASeqRemoveFilter *filter,
                                       GError **error)
{
    ALSASeqUserClientPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_REMOVE_EVENTS, filter) < 0) {
        generate_error(error, errno);
        return;
    }
}
