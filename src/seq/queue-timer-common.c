// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqQueueTimerCommon:
 * An interface to express common features of queue timer.
 *
 * A [iface@QueueTimerCommon] should be implemented by any type of queue timer.
 *
 * The object wraps `struct snd_seq_queue_timer` in UAPI of Linux sound subsystem.
 *
 * Since: 0.3.
 */

static void alsaseq_queue_timer_common_default_init(ALSASeqQueueTimerCommonInterface *iface);

G_DEFINE_INTERFACE(ALSASeqQueueTimerCommon, alsaseq_queue_timer_common, G_TYPE_OBJECT)

static void alsaseq_queue_timer_common_default_init(ALSASeqQueueTimerCommonInterface *iface)
{
    /**
     * ALSASeqQueueTimerCommon:queue-id:
     *
     * The numeric identifier of queue. An entry of [enum@SpecificClientId] is available as well.
     *
     * Since: 0.3.
     */
    g_object_interface_install_property(iface,
        g_param_spec_uchar(QUEUE_ID_PROP_NAME, QUEUE_ID_PROP_NAME,
                           "The numeric identifier of queue including ALSASeqSpecificClientId.",
                           0, G_MAXUINT8, 0,
                           G_PARAM_READABLE));

    /**
     * ALSASeqQueueTimerCommon:timer-type:
     *
     * The type of timer for the queue, one of [enum@QueueTimerType].
     *
     * Since: 0.3.
     */
    g_object_interface_install_property(iface,
        g_param_spec_enum(TIMER_TYPE_PROP_NAME, TIMER_TYPE_PROP_NAME,
                          "The type of timer for the queue, one of ALSASeqQueueTimerType.",
                          ALSASEQ_TYPE_QUEUE_TIMER_TYPE, ALSASEQ_QUEUE_TIMER_TYPE_ALSA,
                          G_PARAM_READABLE));
}

void queue_timer_common_class_override_properties(GObjectClass *gobject_class)
{
    g_object_class_override_property(gobject_class, QUEUE_TIMER_COMMON_PROP_QUEUE_ID,
                                      QUEUE_ID_PROP_NAME);

    g_object_class_override_property(gobject_class, QUEUE_TIMER_COMMON_PROP_TIMER_TYPE,
                                     TIMER_TYPE_PROP_NAME);
}

void queue_timer_common_set_property(struct snd_seq_queue_timer *data, GObject *obj, guint id,
                                     const GValue *val, GParamSpec *spec)
{
    switch (id) {
    case QUEUE_TIMER_COMMON_PROP_QUEUE_ID:
        data->queue = (int)g_value_get_uchar(val);
        break;
    case QUEUE_TIMER_COMMON_PROP_TIMER_TYPE:
        data->type = (int)g_value_get_enum(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

void queue_timer_common_get_property(const struct snd_seq_queue_timer *data, GObject *obj, guint id,
                                     GValue *val, GParamSpec *spec)
{
    switch (id) {
    case QUEUE_TIMER_COMMON_PROP_QUEUE_ID:
        g_value_set_uchar(val, (guchar)data->queue);
        break;
    case QUEUE_TIMER_COMMON_PROP_TIMER_TYPE:
        g_value_set_enum(val, (ALSASeqQueueTimerType)data->type);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}
