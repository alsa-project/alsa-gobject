// SPDX-License-Identifier: LGPL-3.0-or-later
#include "queue-timer-alsa.h"
#include "privates.h"

/**
 * SECTION: queue-timer-alsa
 * @Title: ALSASeqQueueTimerAlsa
 * @Short_description: A GObject-derived object to represent timer instance for
 *                     queue
 *
 * A #ALSASeqQueueTimerAlsa is a GObject-derived object to represent timer
 * instance for queue. The instance belongs to ALSA timer functionality and is
 * pointed by the identifier as property of the object. The object inherits
 * properties and methods from #ALSASeqQueueTimer.
 */
G_DEFINE_TYPE(ALSASeqQueueTimerAlsa, alsaseq_queue_timer_alsa, ALSASEQ_TYPE_QUEUE_TIMER)

enum seq_queue_timer_alsa_prop_type {
    SEQ_QUEUE_TIMER_ALSA_PROP_TIMER_DEVICE_ID = 1,
    SEQ_QUEUE_TIMER_ALSA_PROP_RESOLUTION,
    SEQ_QUEUE_TIMER_ALSA_PROP_COUNT,
};
static GParamSpec *seq_queue_timer_alsa_props[SEQ_QUEUE_TIMER_ALSA_PROP_COUNT] = { NULL, };

static void seq_queue_timer_alsa_set_property(GObject *obj, guint id,
                                         const GValue *val, GParamSpec *spec)
{
    ALSASeqQueueTimer *parent = ALSASEQ_QUEUE_TIMER(obj);
    struct snd_seq_queue_timer *timer;

    seq_queue_timer_refer_private(parent, &timer);

    switch (id) {
    case SEQ_QUEUE_TIMER_ALSA_PROP_TIMER_DEVICE_ID:
    {
        ALSATimerDeviceId *device_id = g_value_get_boxed(val);
        timer->u.alsa.id = *device_id;
        break;
    }
    case SEQ_QUEUE_TIMER_ALSA_PROP_RESOLUTION:
        timer->u.alsa.resolution = g_value_get_uint(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_queue_timer_alsa_get_property(GObject *obj, guint id,
                                              GValue *val, GParamSpec *spec)
{
    ALSASeqQueueTimer *parent = ALSASEQ_QUEUE_TIMER(obj);
    struct snd_seq_queue_timer *timer;

    seq_queue_timer_refer_private(parent, &timer);

    switch (id) {
    case SEQ_QUEUE_TIMER_ALSA_PROP_TIMER_DEVICE_ID:
    {
        g_value_set_static_boxed(val, &timer->u.alsa.id);
        break;
    }
    case SEQ_QUEUE_TIMER_ALSA_PROP_RESOLUTION:
        g_value_set_uint(val, timer->u.alsa.resolution);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_queue_timer_alsa_class_init(ALSASeqQueueTimerAlsaClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_queue_timer_alsa_set_property;
    gobject_class->get_property = seq_queue_timer_alsa_get_property;

    seq_queue_timer_alsa_props[SEQ_QUEUE_TIMER_ALSA_PROP_TIMER_DEVICE_ID] =
        g_param_spec_boxed("timer-device-id", "timer-device-id",
                           "The device id for used timer.",
                           ALSATIMER_TYPE_DEVICE_ID,
                           G_PARAM_READWRITE);

    seq_queue_timer_alsa_props[SEQ_QUEUE_TIMER_ALSA_PROP_RESOLUTION] =
        g_param_spec_uint("resolution", "resolution",
                          "The resolution of timer in Hz.",
                          10, 6250,
                          1000,
                          G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_QUEUE_TIMER_ALSA_PROP_COUNT,
                                      seq_queue_timer_alsa_props);
}

static void alsaseq_queue_timer_alsa_init(ALSASeqQueueTimerAlsa *self)
{
    return;
}

/**
 * alsaseq_queue_timer_alsa_new:
 *
 * Allocate and return an instance of ALSASeqQueueTimerAlsa.
 *
 * Returns: A #ALSASeqQueueTimerAlsa.
 */
ALSASeqQueueTimerAlsa *alsaseq_queue_timer_alsa_new()
{
    return g_object_new(ALSASEQ_TYPE_QUEUE_TIMER_ALSA, NULL);
}
