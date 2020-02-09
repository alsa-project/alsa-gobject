// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-tick.h"
#include "privates.h"

struct _ALSATimerEventDataTickPrivate {
    struct snd_timer_read event;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerEventDataTick, alsatimer_event_data_tick, ALSATIMER_TYPE_EVENT_DATA)

enum timer_event_data_tick_prop_type{
    TIMER_EVENT_DATA_TICK_PROP_RESOLUTION = 1,
    TIMER_EVENT_DATA_TICK_PROP_TICKS,
    TIMER_EVENT_DATA_TICK_PROP_COUNT,
};
static GParamSpec *timer_event_data_tick_props[TIMER_EVENT_DATA_TICK_PROP_COUNT] = { NULL, };

static void timer_event_data_tick_get_property(GObject *obj, guint id, GValue *val, GParamSpec *spec)
{
    ALSATimerEventDataTick *self = ALSATIMER_EVENT_DATA_TICK(obj);
    ALSATimerEventDataTickPrivate *priv = alsatimer_event_data_tick_get_instance_private(self);

    switch (id) {
    case TIMER_EVENT_DATA_TICK_PROP_RESOLUTION:
        g_value_set_uint(val, priv->event.resolution);
        break;
    case TIMER_EVENT_DATA_TICK_PROP_TICKS:
        g_value_set_uint(val, priv->event.ticks);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_event_data_tick_class_init(ALSATimerEventDataTickClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = timer_event_data_tick_get_property;

    timer_event_data_tick_props[TIMER_EVENT_DATA_TICK_PROP_RESOLUTION] =
        g_param_spec_uint("resolution", "resolution",
                          "The resolution of tick.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    timer_event_data_tick_props[TIMER_EVENT_DATA_TICK_PROP_TICKS] =
        g_param_spec_uint("ticks", "ticks",
                          "The ticks since the latest event.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class, TIMER_EVENT_DATA_TICK_PROP_COUNT,
                                      timer_event_data_tick_props);
}

static void alsatimer_event_data_tick_init(ALSATimerEventDataTick *self)
{
    return;
}

void timer_event_data_tick_set_data(ALSATimerEventDataTick *self,
                                    struct snd_timer_read *data)
{
    ALSATimerEventDataTickPrivate *priv =
                        alsatimer_event_data_tick_get_instance_private(self);

    priv->event = *data;
}
