// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_INFO__H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_INFO__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/device-id.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_INFO   (alsatimer_device_info_get_type())

#define ALSATIMER_DEVICE_INFO(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSATIMER_TYPE_DEVICE_INFO,     \
                                ALSATimerDeviceInfo))
#define ALSATIMER_IS_DEVICE_INFO(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSATIMER_TYPE_DEVICE_INFO))

#define ALSATIMER_DEVICE_INFO_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSATIMER_TYPE_DEVICE_INFO,        \
                             ALSATimerDeviceInfoClass))
#define ALSATIMER_IS_DEVICE_INFO_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSATIMER_TYPE_DEVICE_INFO))
#define ALSATIMER_DEVICE_INFO_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSATIMER_TYPE_DEVICE_INFO,      \
                               ALSATimerDeviceInfoClass))

typedef struct _ALSATimerDeviceInfo         ALSATimerDeviceInfo;
typedef struct _ALSATimerDeviceInfoClass    ALSATimerDeviceInfoClass;
typedef struct _ALSATimerDeviceInfoPrivate  ALSATimerDeviceInfoPrivate;

struct _ALSATimerDeviceInfo {
    GObject parent_instance;

    ALSATimerDeviceInfoPrivate *priv;
};

struct _ALSATimerDeviceInfoClass {
    GObjectClass parent_class;
};

GType alsatimer_device_info_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
