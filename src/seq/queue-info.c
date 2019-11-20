// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

struct _ALSASeqQueueInfoPrivate {
    struct snd_seq_queue_info info;
};
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
        priv->info.queue = g_value_get_int(val);
        break;
    case SEQ_QUEUE_INFO_PROP_CLIENT_ID:
        priv->info.owner = g_value_get_int(val);
        break;
    case SEQ_QUEUE_INFO_PROP_LOCKED:
        priv->info.locked = g_value_get_boolean(val);
        break;
    case SEQ_QUEUE_INFO_PROP_NAME:
        strncpy(priv->info.name, g_value_get_string(val), sizeof(priv->info.name));
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
        g_value_set_int(val, priv->info.queue);
        break;
    case SEQ_QUEUE_INFO_PROP_CLIENT_ID:
        g_value_set_int(val, priv->info.owner);
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

    seq_queue_info_props[SEQ_QUEUE_INFO_PROP_QUEUE_ID] =
        g_param_spec_int("queue-id", "queue-id",
                         "The numerical ID of queue, except for one of "
                         "ALSASeqSpecificClientId.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READWRITE);

    seq_queue_info_props[SEQ_QUEUE_INFO_PROP_CLIENT_ID] =
        g_param_spec_int("client-id", "client-id",
                         "The numerical ID of client which owns the queue, "
                         "except for one of ALSASeqSpecificClientId.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READWRITE);

    seq_queue_info_props[SEQ_QUEUE_INFO_PROP_LOCKED] =
        g_param_spec_boolean("locked", "locked",
                             "Whether to be locked by the other queues or not.",
                             FALSE,
                             G_PARAM_READWRITE);

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
 * Allocate and return an instance of ALSASeqQueueInfo.
 *
 * Returns: A #ALSASeqQueueInfo.
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