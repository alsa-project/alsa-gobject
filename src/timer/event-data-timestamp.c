// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-timestamp.h"
#include "privates.h"

/**
 * SECTION: event-data-timestamp
 * @Title: ALSATimerEventDataTimestamp
 * @Short_description: A GObject-derived object to represent event of timer with
 *                     timestamp
 *
 * A #ALSATimerEventDataTimestamp is a GObject-derived object to represent
 * event of timer with timestamp. The instance of object is passed to handler for
 * 'handle-event' GObject signal in ALSATimerUserInstance when it's available.
 * The source of timestamp is decided by 'timer_tstamp_monotonic' option of
 * 'snd-timer' kernel module. The object inherits properties of
 * #ALSATimerEventData.
 *
 * The object wraps 'struct snd_timer_read' in UAPI of Linux sound subsystem.
 */
struct _ALSATimerEventDataTimestampPrivate {
    struct snd_timer_tread event;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerEventDataTimestamp, alsatimer_event_data_timestamp, ALSATIMER_TYPE_EVENT_DATA)

enum timer_event_data_timestamp_prop_type{
    TIMER_EVENT_DATA_TIMESTAMP_PROP_EVENT = 1,
    TIMER_EVENT_DATA_TIMESTAMP_PROP_VALUE,
    TIMER_EVENT_DATA_TIMESTAMP_PROP_COUNT,
};
static GParamSpec *timer_event_data_timestamp_props[TIMER_EVENT_DATA_TIMESTAMP_PROP_COUNT] = { NULL, };

static void timer_event_data_timestamp_get_property(GObject *obj, guint id, GValue *val, GParamSpec *spec)
{
    ALSATimerEventDataTimestamp *self = ALSATIMER_EVENT_DATA_TIMESTAMP(obj);
    ALSATimerEventDataTimestampPrivate *priv = alsatimer_event_data_timestamp_get_instance_private(self);

    switch (id) {
    case TIMER_EVENT_DATA_TIMESTAMP_PROP_EVENT:
        g_value_set_enum(val, priv->event.event);
        break;
    case TIMER_EVENT_DATA_TIMESTAMP_PROP_VALUE:
        g_value_set_uint(val, priv->event.val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_event_data_timestamp_class_init(ALSATimerEventDataTimestampClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = timer_event_data_timestamp_get_property;

    timer_event_data_timestamp_props[TIMER_EVENT_DATA_TIMESTAMP_PROP_EVENT] =
        g_param_spec_enum("event", "event",
                          "The type of event.",
                          ALSATIMER_TYPE_EVENT_TYPE,
                          ALSATIMER_EVENT_TYPE_RESOLUTION,
                          G_PARAM_READABLE);

    timer_event_data_timestamp_props[TIMER_EVENT_DATA_TIMESTAMP_PROP_VALUE] =
        g_param_spec_uint("value", "value",
                          "The value specific to the event.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class, TIMER_EVENT_DATA_TIMESTAMP_PROP_COUNT,
                                      timer_event_data_timestamp_props);
}

static void alsatimer_event_data_timestamp_init(ALSATimerEventDataTimestamp *self)
{
    return;
}

/**
 * alsatimer_event_data_timestamp_get_timestamp:
 * @self: A #ALSATimerEventDataTimestamp.
 * @tv_sec: (out): The part of timestamp for the field of second.
 * @tv_nsec: (out): The part of timestamp for the field of nano second.
 *
 * Return a pair of fields for timestamp of event.
 */
void alsatimer_event_data_timestamp_get_timestamp(ALSATimerEventDataTimestamp *self,
                                                guint *tv_sec, guint *tv_nsec)
{
    ALSATimerEventDataTimestampPrivate *priv;

    g_return_if_fail(ALSATIMER_IS_EVENT_DATA_TIMESTAMP(self));
    g_return_if_fail(tv_sec != NULL);
    g_return_if_fail(tv_nsec != NULL);
    priv = alsatimer_event_data_timestamp_get_instance_private(self);

    *tv_sec = priv->event.tstamp.tv_sec;
    *tv_nsec = priv->event.tstamp.tv_nsec;
}

void timer_event_data_timestamp_set_data(ALSATimerEventDataTimestamp *self,
                                         struct snd_timer_tread *data)
{
    ALSATimerEventDataTimestampPrivate *priv =
                    alsatimer_event_data_timestamp_get_instance_private(self);

    priv->event = *data;
}
