// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqSubscribeData:
 * A GObject-derived object to express data for subscription between ports.
 *
 * A [class@SubscribeData] is a GObject-derived object to express data for subscription between
 * a pair of ports. The call of [func@get_subscription_list] returns the list of data. The call of
 * [method@UserClient.operate_subscription] requires the instance of object.
 *
 * The object wraps `struct snd_seq_port_subscribe` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_seq_port_subscribe data;
} ALSASeqSubscribeDataPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqSubscribeData, alsaseq_subscribe_data, G_TYPE_OBJECT)

enum seq_subscribe_data_prop_type {
    SEQ_SUBSCRIBE_DATA_PROP_SENDER = 1,
    SEQ_SUBSCRIBE_DATA_PROP_DEST,
    SEQ_SUBSCRIBE_DATA_PROP_IS_EXCLUSIVE,
    SEQ_SUBSCRIBE_DATA_PROP_HAS_TSTAMP,
    SEQ_SUBSCRIBE_DATA_PROP_TSTAMP_MODE,
    SEQ_SUBSCRIBE_DATA_PROP_QUEUE_ID,
    SEQ_SUBSCRIBE_DATA_PROP_COUNT,
};
static GParamSpec *seq_subscribe_data_props[SEQ_SUBSCRIBE_DATA_PROP_COUNT] = { NULL, };

static void seq_subscribe_data_set_property(GObject *obj, guint id,
                                            const GValue *val, GParamSpec *spec)
{
    ALSASeqSubscribeData *self = ALSASEQ_SUBSCRIBE_DATA(obj);
    ALSASeqSubscribeDataPrivate *priv =
                            alsaseq_subscribe_data_get_instance_private(self);

    switch (id) {
    case SEQ_SUBSCRIBE_DATA_PROP_SENDER:
    {
        ALSASeqAddr *addr = g_value_get_boxed(val);
        priv->data.sender = *addr;
        break;
    }
    case SEQ_SUBSCRIBE_DATA_PROP_DEST:
    {
        ALSASeqAddr *addr = g_value_get_boxed(val);
        priv->data.dest = *addr;
        break;
    }
    case SEQ_SUBSCRIBE_DATA_PROP_IS_EXCLUSIVE:
        priv->data.flags &= ~SNDRV_SEQ_PORT_SUBS_EXCLUSIVE;
        if (g_value_get_boolean(val))
            priv->data.flags |= SNDRV_SEQ_PORT_SUBS_EXCLUSIVE;
        break;
    case SEQ_SUBSCRIBE_DATA_PROP_HAS_TSTAMP:
        priv->data.flags &= ~SNDRV_SEQ_PORT_SUBS_TIMESTAMP;
        if (g_value_get_boolean(val))
            priv->data.flags |= SNDRV_SEQ_PORT_SUBS_TIMESTAMP;
        break;
    case SEQ_SUBSCRIBE_DATA_PROP_TSTAMP_MODE:
        priv->data.flags &= ~SNDRV_SEQ_PORT_SUBS_TIME_REAL;
        if (g_value_get_enum(val) == ALSASEQ_EVENT_TSTAMP_MODE_REAL)
            priv->data.flags |= SNDRV_SEQ_PORT_SUBS_TIME_REAL;
        break;
    case SEQ_SUBSCRIBE_DATA_PROP_QUEUE_ID:
        priv->data.queue = g_value_get_uchar(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_subscribe_data_get_property(GObject *obj, guint id, GValue *val,
                                            GParamSpec *spec)
{
    ALSASeqSubscribeData *self = ALSASEQ_SUBSCRIBE_DATA(obj);
    ALSASeqSubscribeDataPrivate *priv =
                            alsaseq_subscribe_data_get_instance_private(self);

    switch (id) {
    case SEQ_SUBSCRIBE_DATA_PROP_SENDER:
        g_value_set_static_boxed(val, &priv->data.sender);
        break;
    case SEQ_SUBSCRIBE_DATA_PROP_DEST:
        g_value_set_static_boxed(val, &priv->data.dest);
        break;
    case SEQ_SUBSCRIBE_DATA_PROP_IS_EXCLUSIVE:
        g_value_set_boolean(val, (priv->data.flags & SNDRV_SEQ_PORT_SUBS_EXCLUSIVE) > 0);
        break;
    case SEQ_SUBSCRIBE_DATA_PROP_HAS_TSTAMP:
        g_value_set_boolean(val, (priv->data.flags & SNDRV_SEQ_PORT_SUBS_TIMESTAMP) > 0);
        break;
    case SEQ_SUBSCRIBE_DATA_PROP_TSTAMP_MODE:
    {
        ALSASeqEventTstampMode mode;

        if (priv->data.flags & SNDRV_SEQ_PORT_SUBS_TIME_REAL)
            mode = ALSASEQ_EVENT_TSTAMP_MODE_REAL;
        else
            mode = ALSASEQ_EVENT_TSTAMP_MODE_TICK;

        g_value_set_enum(val, mode);
        break;
    }
    case SEQ_SUBSCRIBE_DATA_PROP_QUEUE_ID:
        g_value_set_uchar(val, priv->data.queue);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_subscribe_data_class_init(ALSASeqSubscribeDataClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_subscribe_data_set_property;
    gobject_class->get_property = seq_subscribe_data_get_property;

    /**
     * ALSASeqSubscribeData:sender:
     *
     * The address of sender.
     *
     * Since: 0.3
     */
    seq_subscribe_data_props[SEQ_SUBSCRIBE_DATA_PROP_SENDER] =
        g_param_spec_boxed("sender", "sender",
                           "The address of sender.",
                           ALSASEQ_TYPE_ADDR,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqSubscribeData:dest:
     *
     * The address of destination.
     *
     * Since: 0.3
     */
    seq_subscribe_data_props[SEQ_SUBSCRIBE_DATA_PROP_DEST] =
        g_param_spec_boxed("dest", "dest",
                           "The address of destination.",
                           ALSASEQ_TYPE_ADDR,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqSubscribeData:is-exclusive:
     *
     * Whether the subscription can be changed by originator only,
     *
     * Since: 0.3
     */
    seq_subscribe_data_props[SEQ_SUBSCRIBE_DATA_PROP_IS_EXCLUSIVE] =
        g_param_spec_boolean("is-exclusive", "is-exclusive",
                             "Whether the subscription can be changed by originator only",
                             FALSE,
                             G_PARAM_READWRITE);

    /**
     * ALSASeqSubscribeData:has-tstamp:
     *
     * Any event for the subscription has time stamp,
     *
     * Since: 0.3
     */
    seq_subscribe_data_props[SEQ_SUBSCRIBE_DATA_PROP_HAS_TSTAMP] =
        g_param_spec_boolean("has-tstamp", "has-tstamp",
                             "Any event for the subscription has time stamp",
                             FALSE,
                             G_PARAM_READWRITE);

    /**
     * ALSASeqSubscribeData:tstamp-mode:
     *
     * The type of time stamp. This is effective when the has-tstamp property enabled.
     *
     * Since: 0.3
     */
    seq_subscribe_data_props[SEQ_SUBSCRIBE_DATA_PROP_TSTAMP_MODE] =
        g_param_spec_enum("tstamp-mode", "tstamp-mode",
                          "The type of time stamp. This is effective when the has-tstamp property"
                          "enabled.",
                          ALSASEQ_TYPE_EVENT_TSTAMP_MODE,
                          ALSASEQ_EVENT_TSTAMP_MODE_TICK,
                          G_PARAM_READWRITE);

    /**
     * ALSASeqSubscribeData:queue-id:
     *
     * The numeric ID of queue to deliver. One of ALSASeqSpecificQueueId is available as well as
     * any numeric value.
     *
     * Since: 0.3
     */
    seq_subscribe_data_props[SEQ_SUBSCRIBE_DATA_PROP_QUEUE_ID] =
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numeric ID of queue to deliver. One of ALSASeqSpecificQueueId is "
                           "available as well as any numeric value.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_SUBSCRIBE_DATA_PROP_COUNT,
                                      seq_subscribe_data_props);
}

static void alsaseq_subscribe_data_init(ALSASeqSubscribeData *self)
{
    return;
}

/**
 * alsaseq_subscribe_data_new:
 *
 * Allocates and returns the instance of [class@SubscribeData].
 *
 * Returns: A [class@SubscribeData].
 */
ALSASeqSubscribeData *alsaseq_subscribe_data_new()
{
    return g_object_new(ALSASEQ_TYPE_SUBSCRIBE_DATA, NULL);
}

void seq_subscribe_data_refer_private(ALSASeqSubscribeData *self,
                                      struct snd_seq_port_subscribe **data)
{
    ALSASeqSubscribeDataPrivate *priv =
                            alsaseq_subscribe_data_get_instance_private(self);

    *data = &priv->data;
}
