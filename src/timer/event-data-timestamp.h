// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TIMESTAMP__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TIMESTAMP__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/event-data.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP     (alsatimer_event_data_timestamp_get_type())

#define ALSATIMER_EVENT_DATA_TIMESTAMP(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP,    \
                                ALSATimerEventDataTimestamp))
#define ALSATIMER_IS_EVENT_DATA_TIMESTAMP(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP))

#define ALSATIMER_EVENT_DATA_TIMESTAMP_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                                   \
                             ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP,       \
                             ALSATimerEventDataTimestampClass))
#define ALSATIMER_IS_EVENT_DATA_TIMESTAMP_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP))
#define ALSATIMER_EVENT_DATA_TIMESTAMP_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP,     \
                               ALSATimerEventDataTimestampkClass))

typedef struct _ALSATimerEventDataTimestamp         ALSATimerEventDataTimestamp;
typedef struct _ALSATimerEventDataTimestampClass    ALSATimerEventDataTimestampClass;
typedef struct _ALSATimerEventDataTimestampPrivate  ALSATimerEventDataTimestampPrivate;

struct _ALSATimerEventDataTimestamp {
    ALSATimerEventData parent_instance;

    ALSATimerEventDataTimestampPrivate *priv;
};

struct _ALSATimerEventDataTimestampClass {
    ALSATimerEventDataClass parent_class;
};

GType alsatimer_event_data_timestamp_get_type() G_GNUC_CONST;

void alsatimer_event_data_timestamp_get_timestamp(ALSATimerEventDataTimestamp *self,
                                                guint *tv_sec, guint *tv_nsec);

G_END_DECLS

#endif
