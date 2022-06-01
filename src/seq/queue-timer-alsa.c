// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqQueueTimerAlsa:
 * An object to express queue timer specific to instance in ALSA Timer.
 *
 * A [class@GObject.Object] derived object class for queue timer specific to any instance in ALSA
 * Timer.
 */
typedef struct {
    struct snd_seq_queue_timer data;
} ALSASeqQueueTimerAlsaPrivate;

static void queue_timer_common_iface_init(ALSASeqQueueTimerCommonInterface *iface);

G_DEFINE_TYPE_WITH_CODE(ALSASeqQueueTimerAlsa, alsaseq_queue_timer_alsa, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(ALSASeqQueueTimerAlsa)
                        G_IMPLEMENT_INTERFACE(ALSASEQ_TYPE_QUEUE_TIMER_COMMON,
                                              queue_timer_common_iface_init))

enum queue_timer_alsa_prop_type {
    QUEUE_TIMER_ALSA_PROP_DEVICE_ID = QUEUE_TIMER_COMMON_PROP_COUNT,
    QUEUE_TIMER_ALSA_PROP_RESOLUTION_TICKS,
    QUEUE_TIMER_ALSA_PROP_COUNT,
};

static void seq_queue_timer_alsa_set_property(GObject *obj, guint id, const GValue *val,
                                              GParamSpec *spec)
{
    ALSASeqQueueTimerAlsa *self = ALSASEQ_QUEUE_TIMER_ALSA(obj);
    ALSASeqQueueTimerAlsaPrivate *priv = alsaseq_queue_timer_alsa_get_instance_private(self);
    struct snd_seq_queue_timer *data = &priv->data;

    switch (id) {
    case QUEUE_TIMER_ALSA_PROP_DEVICE_ID:
    {
        const struct snd_timer_id *src = (const struct snd_timer_id *)g_value_get_boxed(val);
        data->u.alsa.id = *src;
        break;
    }
    case QUEUE_TIMER_ALSA_PROP_RESOLUTION_TICKS:
        data->u.alsa.resolution = g_value_get_uint(val);
        break;
    default:
        queue_timer_common_set_property(data, obj, id, val, spec);
        break;
    }
}

static void seq_queue_timer_alsa_get_property(GObject *obj, guint id, GValue *val,
                                              GParamSpec *spec)
{
    ALSASeqQueueTimerAlsa *self = ALSASEQ_QUEUE_TIMER_ALSA(obj);
    ALSASeqQueueTimerAlsaPrivate *priv = alsaseq_queue_timer_alsa_get_instance_private(self);
    const struct snd_seq_queue_timer *data = &priv->data;

    switch (id) {
    case QUEUE_TIMER_ALSA_PROP_DEVICE_ID:
        g_value_set_static_boxed(val, &data->u.alsa.id);
        break;
    case QUEUE_TIMER_ALSA_PROP_RESOLUTION_TICKS:
        g_value_set_uint(val, data->u.alsa.resolution);
        break;
    default:
        queue_timer_common_get_property(data, obj, id, val, spec);
        break;
    }
}

static void alsaseq_queue_timer_alsa_class_init(ALSASeqQueueTimerAlsaClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_queue_timer_alsa_set_property;
    gobject_class->get_property = seq_queue_timer_alsa_get_property;

    queue_timer_common_class_override_properties(gobject_class);

    /**
     * ALSASeqQueueTimerAlsa:device-id:
     *
     * The identifier of associated timer instance in ALSA Timer.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, QUEUE_TIMER_ALSA_PROP_DEVICE_ID,
        g_param_spec_boxed("device-id", "device-id",
                           "The identifier of associated timer instance in ALSA Timer",
                           ALSATIMER_TYPE_DEVICE_ID,
                           G_PARAM_READWRITE));

    /**
     * ALSASeqQueueTimerAlsa:resolution-ticks:
     *
     * The number of ticks as resolution of timer.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, QUEUE_TIMER_ALSA_PROP_RESOLUTION_TICKS,
        g_param_spec_uint("resolution-ticks", "resolution-ticks",
                          "The number of ticks as resolution of timer.",
                          0, G_MAXUINT, 0,
                          G_PARAM_READWRITE));
}

static void alsaseq_queue_timer_alsa_init(ALSASeqQueueTimerAlsa *self)
{
    return;
}

static void queue_timer_common_iface_init(ALSASeqQueueTimerCommonInterface *iface)
{
    return;
}

/**
 * alsaseq_queue_timer_alsa_new:
 *
 * Allocate and return an instance of [class@QueueTimerAlsa].
 *
 * Returns: An instance of [class@QueueTimerAlsa].
 */
ALSASeqQueueTimerAlsa *alsaseq_queue_timer_alsa_new()
{
    return g_object_new(ALSASEQ_TYPE_QUEUE_TIMER_ALSA, NULL);
}

void seq_queue_timer_alsa_refer_private(ALSASeqQueueTimerAlsa *self,
                                        struct snd_seq_queue_timer **timer)
{
    ALSASeqQueueTimerAlsaPrivate *priv = alsaseq_queue_timer_alsa_get_instance_private(self);
    *timer = &priv->data;
}
