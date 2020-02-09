// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT_DATA__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT_DATA__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/alsatimer-enums.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT_DATA   (alsatimer_event_data_get_type())

#define ALSATIMER_EVENT_DATA(obj)                           \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSATIMER_TYPE_EVENT_DATA,  \
                                ALSATimerEventData))
#define ALSATIMER_IS_EVENT_DATA(obj)                        \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSATIMER_TYPE_EVENT_DATA))

#define ALSATIMER_EVENT_DATA_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSATIMER_TYPE_EVENT_DATA,     \
                             ALSATimerEventDataClass))
#define ALSATIMER_IS_EVENT_DATA_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSATIMER_TYPE_EVENT_DATA))
#define ALSATIMER_EVENT_DATA_GET_CLASS(obj)                 \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSATIMER_TYPE_EVENT_DATA,   \
                               ALSATimerEventDataClass))

typedef struct _ALSATimerEventData          ALSATimerEventData;
typedef struct _ALSATimerEventDataClass     ALSATimerEventDataClass;
typedef struct _ALSATimerEventDataPrivate   ALSATimerEventDataPrivate;

struct _ALSATimerEventData {
    GObject parent_instance;

    ALSATimerEventDataPrivate *priv;
};

struct _ALSATimerEventDataClass {
    GObjectClass parent_class;
};

GType alsatimer_event_data_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
