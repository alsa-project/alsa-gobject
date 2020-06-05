// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data.h"

/**
 * SECTION: event-data
 * @Title: ALSATimerEventData
 * @Short_description: A GObject-derived abstract object to represent common
 *                     data of event
 *
 * A #ALSATimerEventData is a GObject-derived abstract object to represent
 * common data of event.
 */
struct _ALSATimerEventDataPrivate {
    ALSATimerEventDataType type;
};
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(ALSATimerEventData, alsatimer_event_data, G_TYPE_OBJECT)

enum timer_event_data_prop_type{
    TIMER_EVENT_DATA_PROP_EVENT_DATA_TYPE = 1,
    TIMER_EVENT_DATA_PROP_COUNT,
};
static GParamSpec *timer_event_data_props[TIMER_EVENT_DATA_PROP_COUNT] = { NULL, };

static void timer_event_data_set_property(GObject *obj, guint id,
                                          const GValue *val, GParamSpec *spec)
{
    ALSATimerEventData *self = ALSATIMER_EVENT_DATA(obj);
    ALSATimerEventDataPrivate *priv = alsatimer_event_data_get_instance_private(self);

    switch (id) {
    case TIMER_EVENT_DATA_PROP_EVENT_DATA_TYPE:
	priv->type = g_value_get_enum(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void timer_event_data_get_property(GObject *obj, guint id, GValue *val, GParamSpec *spec)
{
    ALSATimerEventData *self = ALSATIMER_EVENT_DATA(obj);
    ALSATimerEventDataPrivate *priv = alsatimer_event_data_get_instance_private(self);

    switch (id) {
    case TIMER_EVENT_DATA_PROP_EVENT_DATA_TYPE:
        g_value_set_enum(val, priv->type);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_event_data_class_init(ALSATimerEventDataClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = timer_event_data_set_property;
    gobject_class->get_property = timer_event_data_get_property;

    timer_event_data_props[TIMER_EVENT_DATA_PROP_EVENT_DATA_TYPE] =
        g_param_spec_enum("type", "type",
                          "The type of event_data, one of ALSATimerEventDataType.",
                          ALSATIMER_TYPE_EVENT_DATA_TYPE,
                          ALSATIMER_EVENT_DATA_TYPE_TICK,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(gobject_class, TIMER_EVENT_DATA_PROP_COUNT,
                                      timer_event_data_props);
}

static void alsatimer_event_data_init(ALSATimerEventData *self)
{
    return;
}
