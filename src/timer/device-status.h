// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_STATUS__H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_STATUS__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_STATUS    (alsatimer_device_status_get_type())

#define ALSATIMER_DEVICE_STATUS(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSATIMER_TYPE_DEVICE_STATUS,   \
                                ALSATimerDeviceStatus))
#define ALSATIMER_IS_DEVICE_STATUS(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSATIMER_TYPE_DEVICE_STATUS))

#define ALSATIMER_DEVICE_STATUS_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSATIMER_TYPE_DEVICE_STATUS,      \
                             ALSATimerDeviceStatusClass))
#define ALSATIMER_IS_DEVICE_STATUS_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSATIMER_TYPE_DEVICE_STATUS))
#define ALSATIMER_DEVICE_STATUS_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSATIMER_TYPE_DEVICE_STATUS,    \
                               ALSATimerDeviceStatusClass))

typedef struct _ALSATimerDeviceStatus           ALSATimerDeviceStatus;
typedef struct _ALSATimerDeviceStatusClass      ALSATimerDeviceStatusClass;

struct _ALSATimerDeviceStatus {
    GObject parent_instance;
};

struct _ALSATimerDeviceStatusClass {
    GObjectClass parent_class;
};

GType alsatimer_device_status_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
