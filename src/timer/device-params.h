// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_PARAMS__H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_PARAMS__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_PARAMS    (alsatimer_device_params_get_type())

#define ALSATIMER_DEVICE_PARAMS(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSATIMER_TYPE_DEVICE_PARAMS,   \
                                ALSATimerDeviceParams))
#define ALSATIMER_IS_DEVICE_PARAMS(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSATIMER_TYPE_DEVICE_PARAMS))

#define ALSATIMER_DEVICE_PARAMS_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSATIMER_TYPE_DEVICE_PARAMS,      \
                             ALSATimerDeviceParamsClass))
#define ALSATIMER_IS_DEVICE_PARAMS_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSATIMER_TYPE_DEVICE_PARAMS))
#define ALSATIMER_DEVICE_PARAMS_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSATIMER_TYPE_DEVICE_PARAMS,    \
                               ALSATimerDeviceParamsClass))

typedef struct _ALSATimerDeviceParams           ALSATimerDeviceParams;
typedef struct _ALSATimerDeviceParamsClass      ALSATimerDeviceParamsClass;

struct _ALSATimerDeviceParams {
    GObject parent_instance;
};

struct _ALSATimerDeviceParamsClass {
    GObjectClass parent_class;
};

GType alsatimer_device_params_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
