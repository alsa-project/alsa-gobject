// SPDX-License-Identifier: LGPL-3.0-or-later
#include "queue-timer.h"

#include <sound/asequencer.h>

/**
 * SECTION: queue-timer
 * @Title: ALSASeqQueueTimer
 * @Short_description: A GObject-derived abstract object to represent timer for
 *                     queue
 *
 * A #ALSASeqQueueTimer is a GObject-derived abstract object to represent common
 * properties and methods of timer for queue. Applications can use derived
 * object; ALSASeqQueueTimerAlsa.
 *
 * The object wraps 'struct snd_seq_queue_timer' in UAPI of Linux sound subsystem.
 */
struct _ALSASeqQueueTimerPrivate {
    struct snd_seq_queue_timer timer;
};
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(ALSASeqQueueTimer, alsaseq_queue_timer, G_TYPE_OBJECT)

enum seq_queue_timer_prop_type {
    SEQ_QUEUE_TIMER_PROP_QUEUE_ID = 1,
    SEQ_QUEUE_TIMER_PROP_TIMER_TYPE,
    SEQ_QUEUE_TIMER_PROP_COUNT,
};
static GParamSpec *seq_queue_timer_props[SEQ_QUEUE_TIMER_PROP_COUNT] = { NULL, };

static void seq_queue_timer_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqQueueTimer *self = ALSASEQ_QUEUE_TIMER(obj);
    ALSASeqQueueTimerPrivate *priv =
                                alsaseq_queue_timer_get_instance_private(self);

    switch (id) {
    case SEQ_QUEUE_TIMER_PROP_QUEUE_ID:
        g_value_set_int(val, priv->timer.queue);
        break;
    case SEQ_QUEUE_TIMER_PROP_TIMER_TYPE:
        g_value_set_enum(val, (ALSASeqQueueTimerType)priv->timer.type);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_queue_timer_class_init(ALSASeqQueueTimerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = seq_queue_timer_get_property;

    seq_queue_timer_props[SEQ_QUEUE_TIMER_PROP_QUEUE_ID] =
        g_param_spec_int("queue-id", "queue-id",
                         "The numerical ID of queue, except for one of "
                         "ALSASeqSpecificClientId.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READABLE);

    seq_queue_timer_props[SEQ_QUEUE_TIMER_PROP_TIMER_TYPE] =
        g_param_spec_enum("type", "type",
                          "The type of timer for the queue, one of "
                          "ALSASeqQueueTimerType.",
                          ALSASEQ_TYPE_QUEUE_TIMER_TYPE,
                          ALSASEQ_QUEUE_TIMER_TYPE_ALSA,
                          G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_QUEUE_TIMER_PROP_COUNT,
                                      seq_queue_timer_props);
}

static void alsaseq_queue_timer_init(ALSASeqQueueTimer *self)
{
    return;
}

void seq_queue_timer_refer_private(ALSASeqQueueTimer *self,
                                   struct snd_seq_queue_timer **timer)
{
    ALSASeqQueueTimerPrivate *priv =
                                alsaseq_queue_timer_get_instance_private(self);

    *timer = &priv->timer;
}
