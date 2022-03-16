// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * SECTION: queue-timer
 * @Title: ALSASeqQueueTimer
 * @Short_description: A GObject-derived object to represent timer for queue
 *
 * A #ALSASeqQueueTimer is a GObject-derived object to represent the information
 * of timer which drives the queue.
 *
 * The object wraps 'struct snd_seq_queue_timer' in UAPI of Linux sound subsystem.
 */
struct _ALSASeqQueueTimerPrivate {
    struct snd_seq_queue_timer timer;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqQueueTimer, alsaseq_queue_timer, G_TYPE_OBJECT)

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
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numerical ID of queue, except for one of "
                           "ALSASeqSpecificClientId.",
                           0, G_MAXUINT8,
                           0,
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

/**
 * alsaseq_queue_timer_new:
 *
 * Allocate and return the instance of #ALSASeqQueueTimer.
 *
 * Returns: the instance of #ALSASeqQueueTimer.
 */
ALSASeqQueueTimer *alsaseq_queue_timer_new()
{
    return g_object_new(ALSASEQ_TYPE_QUEUE_TIMER, NULL);
}

/**
 * alsaseq_queue_timer_get_alsa_data:
 * @self: A #ALSASeqQueueTimer.
 * @data: (out)(transfer none): A #ALSASeqQueueTimerDataAlsa.
 *
 * Refer to the data of timer for queue in the case that the device in ALSATimer
 * drives the timer.
 */
void alsaseq_queue_timer_get_alsa_data(ALSASeqQueueTimer *self,
                                       const ALSASeqQueueTimerDataAlsa **data)
{
    ALSASeqQueueTimerPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_QUEUE_TIMER(self));
    priv = alsaseq_queue_timer_get_instance_private(self);

    g_return_if_fail(data != NULL);

    *data = (const ALSASeqQueueTimerDataAlsa *)&priv->timer.u.alsa;
}

/**
 * alsaseq_queue_timer_set_alsa_data:
 * @self: A #ALSASeqQueueTimer.
 * @data: A #ALSASeqQueueTimerDataAlsa.
 *
 * Set the data of timer for queue in the case that the device in ALSATimer
 * drives the timer.
 */
void alsaseq_queue_timer_set_alsa_data(ALSASeqQueueTimer *self,
                                       const ALSASeqQueueTimerDataAlsa *data)
{
    ALSASeqQueueTimerPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_QUEUE_TIMER(self));
    priv = alsaseq_queue_timer_get_instance_private(self);

    g_return_if_fail(data != NULL);

    priv->timer.type = SNDRV_SEQ_TIMER_ALSA;
    priv->timer.u.alsa.id = data->device_id;
    priv->timer.u.alsa.resolution = data->resolution;
}

void seq_queue_timer_refer_private(ALSASeqQueueTimer *self,
                                   struct snd_seq_queue_timer **timer)
{
    ALSASeqQueueTimerPrivate *priv =
                                alsaseq_queue_timer_get_instance_private(self);

    *timer = &priv->timer;
}
