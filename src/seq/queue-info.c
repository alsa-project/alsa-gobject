// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqQueueInfo:
 * A GObject-derived object to express information of queue.
 *
 * A [class@QueueInfo] is a GObject-derived object to express information of queue. The call of
 * [func@get_queue_info_by_id] and [func@get_queue_info_by_name] returns the instance of object.
 * The call of [method@UserClient.create_queue] and [method@UserClient.update_queue] requires the
 * instance of object.
 *
 * The object wraps `struct snd_seq_queue_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_seq_queue_info info;
} ALSASeqQueueInfoPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqQueueInfo, alsaseq_queue_info, G_TYPE_OBJECT)

enum seq_queue_info_prop_type {
    SEQ_QUEUE_INFO_PROP_QUEUE_ID = 1,
    SEQ_QUEUE_INFO_PROP_CLIENT_ID,
    SEQ_QUEUE_INFO_PROP_LOCKED,
    SEQ_QUEUE_INFO_PROP_NAME,
    SEQ_QUEUE_INFO_PROP_COUNT,
};
static GParamSpec *seq_queue_info_props[SEQ_QUEUE_INFO_PROP_COUNT] = { NULL, };

static void seq_queue_info_set_property(GObject *obj, guint id,
                                         const GValue *val, GParamSpec *spec)
{
    ALSASeqQueueInfo *self = ALSASEQ_QUEUE_INFO(obj);
    ALSASeqQueueInfoPrivate *priv =
                                alsaseq_queue_info_get_instance_private(self);

    switch (id) {
    case SEQ_QUEUE_INFO_PROP_QUEUE_ID:
        priv->info.queue = (int)g_value_get_uchar(val);
        break;
    case SEQ_QUEUE_INFO_PROP_CLIENT_ID:
        priv->info.owner = (int)g_value_get_uchar(val);
        break;
    case SEQ_QUEUE_INFO_PROP_LOCKED:
        priv->info.locked = g_value_get_boolean(val);
        break;
    case SEQ_QUEUE_INFO_PROP_NAME:
        g_strlcpy(priv->info.name, g_value_get_string(val), sizeof(priv->info.name));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_queue_info_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqQueueInfo *self = ALSASEQ_QUEUE_INFO(obj);
    ALSASeqQueueInfoPrivate *priv =
                                alsaseq_queue_info_get_instance_private(self);

    switch (id) {
    case SEQ_QUEUE_INFO_PROP_QUEUE_ID:
        g_value_set_uchar(val, (guint8)priv->info.queue);
        break;
    case SEQ_QUEUE_INFO_PROP_CLIENT_ID:
        g_value_set_uchar(val, (guint8)priv->info.owner);
        break;
    case SEQ_QUEUE_INFO_PROP_LOCKED:
        g_value_set_boolean(val, priv->info.locked);
        break;
    case SEQ_QUEUE_INFO_PROP_NAME:
        g_value_set_static_string(val, priv->info.name);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_queue_info_class_init(ALSASeqQueueInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_queue_info_set_property;
    gobject_class->get_property = seq_queue_info_get_property;

    /**
     * ALSASeqQueueInfo:queue-id:
     *
     * The numeric ID of queue. An entry of ALSASeqSpecificQueueId is available as well.
     */
    seq_queue_info_props[SEQ_QUEUE_INFO_PROP_QUEUE_ID] =
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numeric ID of queue. An entry of ALSASeqSpecificQueueId is "
                           "available as well",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqQueueInfo:client-id:
     *
     * The numeric ID of client which owns the queue, including one of ALSASeqSpecificClientId.
     */
    seq_queue_info_props[SEQ_QUEUE_INFO_PROP_CLIENT_ID] =
        g_param_spec_uchar("client-id", "client-id",
                           "The numeric ID of client which owns the queue, including one of "
                           "ALSASeqSpecificClientId.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqQueueInfo:locked:
     *
     * Whether to be locked by the other queues or not.
     */
    seq_queue_info_props[SEQ_QUEUE_INFO_PROP_LOCKED] =
        g_param_spec_boolean("locked", "locked",
                             "Whether to be locked by the other queues or not.",
                             FALSE,
                             G_PARAM_READWRITE);

    /**
     * ALSASeqQueueInfo:name:
     *
     * The name of queue.
     */
    seq_queue_info_props[SEQ_QUEUE_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of queue.",
                            "",
                            G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_QUEUE_INFO_PROP_COUNT,
                                      seq_queue_info_props);
}

static void alsaseq_queue_info_init(ALSASeqQueueInfo *self)
{
    return;
}

/**
 * alsaseq_queue_info_new:
 *
 * Allocate and return an instance of [class@QueueInfo].
 *
 * Returns: An instance of [class@QueueInfo].
 */
ALSASeqQueueInfo *alsaseq_queue_info_new()
{
    return g_object_new(ALSASEQ_TYPE_QUEUE_INFO, NULL);
}

void seq_queue_info_refer_private(ALSASeqQueueInfo *self,
                                  struct snd_seq_queue_info **info)
{
    ALSASeqQueueInfoPrivate *priv =
                                alsaseq_queue_info_get_instance_private(self);

    *info = &priv->info;
}
