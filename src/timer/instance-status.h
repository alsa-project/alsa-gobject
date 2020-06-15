// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_INSTANCE_STATUS__H__
#define __ALSA_GOBJECT_ALSATIMER_INSTANCE_STATUS__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_INSTANCE_STATUS  (alsatimer_instance_status_get_type())

#define ALSATIMER_INSTANCE_STATUS(obj)                          \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSATIMER_TYPE_INSTANCE_STATUS, \
                                ALSATimerInstanceStatus))
#define ALSATIMER_IS_INSTANCE_STATUS(obj)                       \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSATIMER_TYPE_INSTANCE_STATUS))

#define ALSATIMER_INSTANCE_STATUS_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSATIMER_TYPE_INSTANCE_STATUS,    \
                             ALSATimerInstanceStatusClass))
#define ALSATIMER_IS_INSTANCE_STATUS_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSATIMER_TYPE_INSTANCE_STATUS))
#define ALSATIMER_INSTANCE_STATUS_GET_CLASS(obj)                \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSATIMER_TYPE_INSTANCE_STATUS,  \
                               ALSATimerInstanceStatusClass))

typedef struct _ALSATimerInstanceStatus         ALSATimerInstanceStatus;
typedef struct _ALSATimerInstanceStatusClass    ALSATimerInstanceStatusClass;
typedef struct _ALSATimerInstanceStatusPrivate  ALSATimerInstanceStatusPrivate;

struct _ALSATimerInstanceStatus {
    GObject parent_instance;

    ALSATimerInstanceStatusPrivate *priv;
};

struct _ALSATimerInstanceStatusClass {
    GObjectClass parent_class;
};

GType alsatimer_instance_status_get_type() G_GNUC_CONST;

ALSATimerInstanceStatus *alsatimer_instance_status_new();

void alsatimer_instance_status_get_tstamp(ALSATimerInstanceStatus *self,
                                          const gint64 *tstamp[2]);

G_END_DECLS

#endif
