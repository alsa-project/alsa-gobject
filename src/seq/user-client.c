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

enum seq_user_client_prop_type {
    SEQ_USER_CLIENT_PROP_CLIENT_ID = 1,
    SEQ_USER_CLIENT_PROP_COUNT,
};
static GParamSpec *seq_user_client_props[SEQ_USER_CLIENT_PROP_COUNT] = { NULL, };

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
