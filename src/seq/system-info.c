// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

struct _ALSASeqSystemInfoPrivate {
    struct snd_seq_system_info info;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqSystemInfo, alsaseq_system_info, G_TYPE_OBJECT)

enum seq_system_info_prop_type {
    SEQ_SYSTEM_INFO_PROP_MAXIMUM_QUEUE_COUNT = 1,
    SEQ_SYSTEM_INFO_PROP_MAXIMUM_CLIENT_COUNT,
    SEQ_SYSTEM_INFO_PROP_MAXIMUM_PORT_COUNT,
    SEQ_SYSTEM_INFO_PROP_MAXIMUM_CHANNEL_COUNT,
    SEQ_SYSTEM_INFO_PROP_CURRENT_CLIENT_COUNT,
    SEQ_SYSTEM_INFO_PROP_CURRENT_QUEUE_COUNT,
    SEQ_SYSTEM_INFO_PROP_COUNT,
};
static GParamSpec *seq_system_info_props[SEQ_SYSTEM_INFO_PROP_COUNT] = { NULL, };

static void seq_system_info_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqSystemInfo *self = ALSASEQ_SYSTEM_INFO(obj);
    ALSASeqSystemInfoPrivate *priv =
                                alsaseq_system_info_get_instance_private(self);

    switch (id) {
    case SEQ_SYSTEM_INFO_PROP_MAXIMUM_QUEUE_COUNT:
        g_value_set_int(val, priv->info.queues);
        break;
    case SEQ_SYSTEM_INFO_PROP_MAXIMUM_CLIENT_COUNT:
        g_value_set_int(val, priv->info.clients);
        break;
    case SEQ_SYSTEM_INFO_PROP_MAXIMUM_PORT_COUNT:
        g_value_set_int(val, priv->info.ports);
        break;
    case SEQ_SYSTEM_INFO_PROP_MAXIMUM_CHANNEL_COUNT:
        g_value_set_int(val, priv->info.channels);
        break;
    case SEQ_SYSTEM_INFO_PROP_CURRENT_CLIENT_COUNT:
        g_value_set_int(val, priv->info.cur_clients);
        break;
    case SEQ_SYSTEM_INFO_PROP_CURRENT_QUEUE_COUNT:
        g_value_set_int(val, priv->info.cur_queues);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_system_info_class_init(ALSASeqSystemInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = seq_system_info_get_property;

    seq_system_info_props[SEQ_SYSTEM_INFO_PROP_MAXIMUM_QUEUE_COUNT] =
        g_param_spec_int("maximum-queue-count", "maximum-queue-count",
                         "The maximum number of available queues.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_system_info_props[SEQ_SYSTEM_INFO_PROP_MAXIMUM_CLIENT_COUNT] =
        g_param_spec_int("maximum-client-count", "maximum-client-count",
                         "The maximum number of clients.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_system_info_props[SEQ_SYSTEM_INFO_PROP_MAXIMUM_PORT_COUNT] =
        g_param_spec_int("maximum-port-count", "maximum-port-count",
                         "The maximum number of ports.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_system_info_props[SEQ_SYSTEM_INFO_PROP_MAXIMUM_CHANNEL_COUNT] =
        g_param_spec_int("maximum-channel-count", "maximum-channel-count",
                         "The maximum number of channels.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_system_info_props[SEQ_SYSTEM_INFO_PROP_CURRENT_CLIENT_COUNT] =
        g_param_spec_int("current-client-count", "current-client-count",
                         "The current number of clients.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_system_info_props[SEQ_SYSTEM_INFO_PROP_CURRENT_QUEUE_COUNT] =
        g_param_spec_int("current-queue-count", "current-queue-count",
                         "The current number of queues.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_SYSTEM_INFO_PROP_COUNT,
                                      seq_system_info_props);

}

static void alsaseq_system_info_init(ALSASeqSystemInfo *self)
{
    return;
}

void seq_system_info_refer_private(ALSASeqSystemInfo *self,
                                   struct snd_seq_system_info **info)
{
    ALSASeqSystemInfoPrivate *priv =
                                alsaseq_system_info_get_instance_private(self);

    *info = &priv->info;
}
