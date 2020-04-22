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

struct _ALSASeqUserClientPrivate {
    int fd;
    int client_id;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqUserClient, alsaseq_user_client, G_TYPE_OBJECT)

typedef struct {
    GSource src;
    ALSASeqUserClient *self;
    gpointer tag;
    void *buf;
    size_t buf_len;
    ALSASeqEventFixed *ev_fixed;
    ALSASeqEventVariable *ev_var;
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
     * @event: (transfer none): An object derived from #ALSASeqEvent.
     *
     * When event occurs, this signal is emit with an object for the event.
     */
    seq_user_client_sigs[SEQ_USER_CLIENT_SIG_TYPE_HANDLE_EVENT] =
        g_signal_new("handle-event",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSASeqUserClientClass, handle_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, ALSASEQ_TYPE_EVENT);
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
 */
void alsaseq_user_client_open(ALSASeqUserClient *self, gint open_flag,
                              GError **error)
{
    ALSASeqUserClientPrivate *priv;
    char *devnode;

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
}

/**
 * alsaseq_user_client_set_info:
 * @self: A #ALSASeqUserClient.
 * @client_info: A #ALSASeqClientInfo.
 * @error: A #GError.
 *
 * Get client information.
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
 * @port_info: A #ALSASeqPortInfo.
 * @port_id: (nullable): The numerical ID of port if specified.
 * @error: A #GError.
 *
 * Create a port into the client.
 */
void alsaseq_user_client_create_port(ALSASeqUserClient *self,
                                     ALSASeqPortInfo *port_info,
                                     guint8 *port_id, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_port_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_PORT_INFO(port_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_port_info_refer_private(port_info, &info);

    info->addr.client = priv->client_id;
    if (port_id != NULL) {
        info->addr.port = *port_id;
        info->flags |= SNDRV_SEQ_PORT_FLG_GIVEN_PORT;
    }
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CREATE_PORT, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_update_port:
 * @self: A #ALSASeqUserClient.
 * @port_info: A #ALSASeqPortInfo.
 * @port_id: The numerical ID of port.
 * @error: A #GError.
 *
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
 * @port_info: A #ALSASeqPortInfo.
 * @port_id: The numerical ID of port.
 * @error: A #GError.
 *
 * Delete a port from the client.
 */
void alsaseq_user_client_delete_port(ALSASeqUserClient *self,
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
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_DELETE_PORT, info) < 0)
        generate_error(error, errno);
}

/**
 * alsaseq_user_client_set_pool:
 * @self: A #ALSASeqUserClient.
 * @client_pool: A #ALSASeqClientPool to be configured for the client.
 * @error: A #GError.
 *
 * Configure memory pool in the client.
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
 * @event: An instance of #ALSASeqEvent to schedule.
 * @error: A #GError.
 *
 * Deliver the event immediately, or schedule it into memory pool of the client.
 */
void alsaseq_user_client_schedule_event(ALSASeqUserClient *self,
                                        ALSASeqEvent *event, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_event *ev;
    ssize_t len;
    guint8 *ptr;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_EVENT(event));
    priv = alsaseq_user_client_get_instance_private(self);
    seq_event_refer_private(event, &ev);

    switch (ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_FIXED:
        ptr = NULL;
        len = sizeof(*ev);
        break;
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
        if (ev->data.ext.len == 0 || ev->data.ext.ptr == NULL) {
            generate_error(error, EINVAL);
            return;
        }

        len = sizeof(*ev) + ev->data.ext.len;
        ptr = g_malloc0(len);
        if (ptr == NULL) {
            generate_error(error, ENOMEM);
            return;
        }

        memcpy(ptr, ev, sizeof(*ev));
        memcpy(ptr + sizeof(*ev), ev->data.ext.ptr, ev->data.ext.len);

        ev = (struct snd_seq_event *)ptr;
        break;
    case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
        // Unsupported since it handles raw pointer which is difficult to
        // be exposed by interfaces capable for g-i.
    default:
        break;
    }

    len = write(priv->fd, ev, len);
    if (len < 0)
        generate_error(error, errno);

    if (ptr != NULL)
        g_free(ptr);
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
    guint8 *buf;
    struct snd_seq_event *event;

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

    buf = src->buf;
    while (len >= sizeof(*event)) {
        struct snd_seq_event *data_ptr;
        ALSASeqEvent *ev;

        event = (struct snd_seq_event *)buf;

        buf += sizeof(*event);
        len -= sizeof(*event);

        // For variable length of event.
        switch (event->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
        case SNDRV_SEQ_EVENT_LENGTH_FIXED:
            ev = ALSASEQ_EVENT(src->ev_fixed);
            break;
        case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
        {
            unsigned int length = event->data.ext.len;
            unsigned int offset = (length + sizeof(*event) - 1) / sizeof(*event);

            // MEMO: Although ALSA Sequencer core never truncate the event,
            // let's skip it for safe.
            if (length == 0 || len < offset)
                break;

            event->data.ext.ptr = buf;
            buf += offset;
            len -= offset;

            ev = ALSASEQ_EVENT(src->ev_var);
            break;
        }
        case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
            // Unsupported since it handles raw pointer which is difficult to
            // be exposed by interfaces capable for g-i.
        default:
            continue;
        }

        // Copy event cell (= 28 bytes).
        seq_event_refer_private(ev, &data_ptr);
        *data_ptr = *event;

        g_signal_emit(self,
                    seq_user_client_sigs[SEQ_USER_CLIENT_SIG_TYPE_HANDLE_EVENT],
                    0, ev);
    }

    // Just be sure to continue to process this source.
    return G_SOURCE_CONTINUE;
}

static void seq_user_client_finalize_src(GSource *gsrc)
{
    UserClientSource *src = (UserClientSource *)gsrc;

    g_object_unref(src->ev_fixed);
    g_object_unref(src->ev_var);

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

    buf = g_try_malloc0(page_size);
    if (buf == NULL) {
        generate_error(error, ENOMEM);
        return;
    }

    *gsrc = g_source_new(&funcs, sizeof(*src));
    src = (UserClientSource *)(*gsrc);

    src->ev_fixed = g_object_new(ALSASEQ_TYPE_EVENT_FIXED, NULL);
    src->ev_var = g_object_new(ALSASEQ_TYPE_EVENT_VARIABLE, NULL);

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
 * @queue_info: The information of queue to add.
 * @error: A #GError.
 *
 * Create a new queue owned by the client. The content of information is updated
 * if success.
 */
void alsaseq_user_client_create_queue(ALSASeqUserClient *self,
                                ALSASeqQueueInfo *queue_info, GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_info *info;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_QUEUE_INFO(queue_info));
    priv = alsaseq_user_client_get_instance_private(self);

    seq_queue_info_refer_private(queue_info, &info);

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
 */
void alsaseq_user_client_delete_queue(ALSASeqUserClient *self,
                                      guint queue_id, GError **error)
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

    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_CREATE_QUEUE, info) < 0)
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
 */
void alsaseq_user_client_get_queue_usage(ALSASeqUserClient *self,
                                         guint queue_id, gboolean *use,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_client data = {0};

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    priv = alsaseq_user_client_get_instance_private(self);

    data.queue = (int)queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_SET_QUEUE_CLIENT, &data) < 0) {
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
 */
void alsaseq_user_client_set_queue_usage(ALSASeqUserClient *self,
                                         guint queue_id, gboolean use,
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
 *
 * Set the data of tempo to the queue.
 */
void alsaseq_user_client_set_queue_tempo(ALSASeqUserClient *self,
                                guint queue_id, ALSASeqQueueTempo *queue_tempo,
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
 */
void alsaseq_user_client_get_queue_tempo(ALSASeqUserClient *self,
                                guint queue_id, ALSASeqQueueTempo **queue_tempo,
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
 */
void alsaseq_user_client_set_queue_timer(ALSASeqUserClient *self,
                                         guint queue_id,
                                         ALSASeqQueueTimer *queue_timer,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_timer *timer;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(ALSASEQ_IS_QUEUE_TIMER(queue_timer));
    priv = alsaseq_user_client_get_instance_private(self);

    if (ALSASEQ_IS_QUEUE_TIMER_ALSA(queue_timer)) {
        generate_error(error, EINVAL);
        return;
    }

    seq_queue_timer_refer_private(queue_timer, &timer);
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
 */
void alsaseq_user_client_get_queue_timer(ALSASeqUserClient *self,
                                         guint queue_id,
                                         ALSASeqQueueTimer **queue_timer,
                                         GError **error)
{
    ALSASeqUserClientPrivate *priv;
    struct snd_seq_queue_timer timer, *timer_ptr;

    g_return_if_fail(ALSASEQ_IS_USER_CLIENT(self));
    g_return_if_fail(queue_timer != NULL);
    priv = alsaseq_user_client_get_instance_private(self);

    timer.queue = queue_id;
    if (ioctl(priv->fd, SNDRV_SEQ_IOCTL_GET_QUEUE_TIMER, &timer) < 0) {
        generate_error(error, errno);
        return;
    }

    switch (timer.type) {
    case SNDRV_SEQ_TIMER_ALSA:
        *queue_timer = ALSASEQ_QUEUE_TIMER(alsaseq_queue_timer_alsa_new());
        break;
    case SNDRV_SEQ_TIMER_MIDI_CLOCK:
    case SNDRV_SEQ_TIMER_MIDI_TICK:
    default:
        // Not available.
        generate_error(error, ENXIO);
        return;
    }

    seq_queue_timer_refer_private(*queue_timer, &timer_ptr);
    *timer_ptr = timer;
}

/**
 * alsaseq_user_client_remove_events:
 * @self: A #ALSASeqUserClient.
 * @filter: A #ALSASeqRemoveFilter.
 * @error: A #GError.
 *
 * Remove queued events according to the filter.
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
