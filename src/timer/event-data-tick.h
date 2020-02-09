// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TICK__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TICK__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/event-data.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT_DATA_TICK   (alsatimer_event_data_tick_get_type())

#define ALSATIMER_EVENT_DATA_TICK(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                              \
                                ALSATIMER_TYPE_EVENT_DATA_TICK,     \
                                ALSATimerEventDataTick))
#define ALSATIMER_IS_EVENT_DATA_TICK(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                              \
                                ALSATIMER_TYPE_EVENT_DATA_TICK))

#define ALSATIMER_EVENT_DATA_TICK_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                               \
                             ALSATIMER_TYPE_EVENT_DATA_TICK,        \
                             ALSATimerEventDataTickClass))
#define ALSATIMER_IS_EVENT_DATA_TICK_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                               \
                             ALSATIMER_TYPE_EVENT_DATA_TICK))
#define ALSATIMER_EVENT_DATA_TICK_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                               \
                               ALSATIMER_TYPE_EVENT_DATA_TICK,      \
                               ALSATimerEventDataTickkClass))

typedef struct _ALSATimerEventDataTick          ALSATimerEventDataTick;
typedef struct _ALSATimerEventDataTickClass     ALSATimerEventDataTickClass;
typedef struct _ALSATimerEventDataTickPrivate   ALSATimerEventDataTickPrivate;

struct _ALSATimerEventDataTick {
    ALSATimerEventData parent_instance;

    ALSATimerEventDataTickPrivate *priv;
};

struct _ALSATimerEventDataTickClass {
    ALSATimerEventDataClass parent_class;
};

GType alsatimer_event_data_tick_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
