// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * SECTION: instance-status
 * @Title: ALSATimerInstanceStatus
 * @Short_description: A GObject-derived object to represent status of user
 *                     instance
 *
 * A #ALSATimerInstanceStatus is a GObject-derived object to represent status
 * of user instance attached to any timer device or the other instance as slave.
 * The call of alsatimer_user_instance_get_status() returns the instance of
 * object.
 *
 * The object wraps 'struct snd_timer_status' in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_timer_status status;
    gint64 tstamp[2];
} ALSATimerInstanceStatusPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerInstanceStatus, alsatimer_instance_status, G_TYPE_OBJECT)

enum timer_instance_status_props {
    TIMER_INSTANCE_STATUS_PROP_INTERVAL = 1,
    TIMER_INSTANCE_STATUS_PROP_LOST,
    TIMER_INSTANCE_STATUS_PROP_OVERRUN,
    TIMER_INSTANCE_STATUS_PROP_QUEUE_SIZE,
    TIMER_INSTANCE_STATUS_PROP_COUNT,
};
static GParamSpec *timer_instance_status_props[TIMER_INSTANCE_STATUS_PROP_COUNT] = { NULL, };

static void timer_instance_status_get_property(GObject *obj, guint id,
                                             GValue *val, GParamSpec *spec)
{
    ALSATimerInstanceStatus *self = ALSATIMER_INSTANCE_STATUS(obj);
    ALSATimerInstanceStatusPrivate *priv =
                           alsatimer_instance_status_get_instance_private(self);

    switch (id) {
    case TIMER_INSTANCE_STATUS_PROP_INTERVAL:
        g_value_set_uint(val, priv->status.resolution);
        break;
    case TIMER_INSTANCE_STATUS_PROP_LOST:
        g_value_set_uint(val, priv->status.lost);
        break;
    case TIMER_INSTANCE_STATUS_PROP_OVERRUN:
        g_value_set_uint(val, priv->status.overrun);
        break;
    case TIMER_INSTANCE_STATUS_PROP_QUEUE_SIZE:
        g_value_set_uint(val, priv->status.queue);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_instance_status_class_init(ALSATimerInstanceStatusClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = timer_instance_status_get_property;

    timer_instance_status_props[TIMER_INSTANCE_STATUS_PROP_INTERVAL] =
        g_param_spec_uint("interval", "interval",
                          "The current interval in nano second.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    timer_instance_status_props[TIMER_INSTANCE_STATUS_PROP_LOST] =
        g_param_spec_uint("lost", "lost",
                          "The count of losts master ticks.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    timer_instance_status_props[TIMER_INSTANCE_STATUS_PROP_OVERRUN] =
        g_param_spec_uint("overrun", "overrun",
                          "The count of overrun in read queue.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    timer_instance_status_props[TIMER_INSTANCE_STATUS_PROP_QUEUE_SIZE] =
        g_param_spec_uint("queue-size", "queue-size",
                          "The current size of queue.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      TIMER_INSTANCE_STATUS_PROP_COUNT,
                                      timer_instance_status_props);
}

static void alsatimer_instance_status_init(ALSATimerInstanceStatus *self)
{
    return;
}

/**
 * alsatimer_instance_status_get_tstamp:
 * @self: A #ALSATimerInstanceStatus.
 * @tstamp: (array fixed-size=2)(out)(transfer none): The array with two
 *          elements for the seconds and nanoseconds parts of timestamp
 *          when the instance queues the latest event.
 *
 * Get timestamp for the latest event.
 */
void alsatimer_instance_status_get_tstamp(ALSATimerInstanceStatus *self,
                                          const gint64 *tstamp[2])
{
    ALSATimerInstanceStatusPrivate *priv;

    g_return_if_fail(ALSATIMER_IS_INSTANCE_STATUS(self));
    priv = alsatimer_instance_status_get_instance_private(self);

    g_return_if_fail(tstamp != NULL);

    priv->tstamp[0] = (gint64)priv->status.tstamp.tv_sec;
    priv->tstamp[1] = (gint64)priv->status.tstamp.tv_nsec;

    *tstamp = (const gint64 *)&priv->tstamp;
}

/**
 * alsatimer_instance_status_new:
 *
 * Allocate and return an instance of #ALSATimerInstanceStatus.
 *
 * Returns: A #ALSATimerInstanceStatus.
 */
ALSATimerInstanceStatus *alsatimer_instance_status_new()
{
    return g_object_new(ALSATIMER_TYPE_INSTANCE_STATUS, NULL);
}

void timer_instance_status_refer_private(ALSATimerInstanceStatus *self,
                                         struct snd_timer_status **status)
{
    ALSATimerInstanceStatusPrivate *priv =
                        alsatimer_instance_status_get_instance_private(self);

    *status = &priv->status;
}
