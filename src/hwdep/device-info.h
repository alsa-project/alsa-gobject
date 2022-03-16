// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_DEVICE_INFO__H__
#define __ALSA_GOBJECT_ALSAHWDEP_DEVICE_INFO__H__

#include <alsahwdep.h>

G_BEGIN_DECLS

#define ALSAHWDEP_TYPE_DEVICE_INFO      (alsahwdep_device_info_get_type())

#define ALSAHWDEP_DEVICE_INFO(obj)                          \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSAHWDEP_TYPE_DEVICE_INFO, \
                                ALSAHwdepDeviceInfo))
#define ALSAHWDEP_IS_DEVICE_INFO(obj)                       \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSAHWDEP_TYPE_DEVICE_INFO))

#define ALSAHWDEP_DEVICE_INFO_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSAHWDEP_TYPE_DEVICE_INFO,    \
                             ALSAHwdepDeviceInfoClass))
#define ALSAHWDEP_IS_DEVICE_INFO_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSAHWDEP_TYPE_DEVICE_INFO))
#define ALSAHWDEP_DEVICE_INFO_GET_CLASS(obj)                \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSAHWDEP_TYPE_DEVICE_INFO,  \
                               ALSAHwdepDeviceInfoClass))

typedef struct _ALSAHwdepDeviceInfo         ALSAHwdepDeviceInfo;
typedef struct _ALSAHwdepDeviceInfoClass    ALSAHwdepDeviceInfoClass;
typedef struct _ALSAHwdepDeviceInfoPrivate  ALSAHwdepDeviceInfoPrivate;

struct _ALSAHwdepDeviceInfo {
    GObject parent_instance;

    ALSAHwdepDeviceInfoPrivate *priv;
};

struct _ALSAHwdepDeviceInfoClass {
    GObjectClass parent_class;
};

GType alsahwdep_device_info_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
