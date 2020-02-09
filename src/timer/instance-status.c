// SPDX-License-Identifier: LGPL-3.0-or-later
#include "instance-status.h"

#include <sound/asound.h>

struct _ALSATimerInstanceStatusPrivate {
    struct snd_timer_status status;
};
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
 * alsatimer_instance_status_get_timestamp:
 * @self: A #ALSATimerInstanceStatus.
 * @tv_sec: (out): The second part of timestamp.
 * @tv_nsec: (out): The nano second part of timerstamp.
 *
 * Get timestamp for the latest update.
 */
void alsatimer_instance_status_get_timestamp(ALSATimerInstanceStatus *self,
                                             guint *tv_sec, guint *tv_nsec)
{
    ALSATimerInstanceStatusPrivate *priv;

    g_return_if_fail(ALSATIMER_IS_INSTANCE_STATUS(self));
    g_return_if_fail(tv_sec != NULL);
    g_return_if_fail(tv_nsec != NULL);
    priv = alsatimer_instance_status_get_instance_private(self);

    *tv_sec = (guint)priv->status.tstamp.tv_sec;
    *tv_nsec = (guint)priv->status.tstamp.tv_nsec;

}
