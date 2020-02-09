// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_INSTANCE_INFO__H__
#define __ALSA_GOBJECT_ALSATIMER_INSTANCE_INFO__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/alsatimer-enums.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_INSTANCE_INFO    (alsatimer_instance_info_get_type())

#define ALSATIMER_INSTANCE_INFO(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSATIMER_TYPE_INSTANCE_INFO,   \
                                ALSATimerInstanceInfo))
#define ALSATIMER_IS_INSTANCE_INFO(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSATIMER_TYPE_INSTANCE_INFO))

#define ALSATIMER_INSTANCE_INFO_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSATIMER_TYPE_INSTANCE_INFO,      \
                             ALSATimerInstanceInfoClass))
#define ALSATIMER_IS_INSTANCE_INFO_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSATIMER_TYPE_INSTANCE_INFO))
#define ALSATIMER_INSTANCE_INFO_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSATIMER_TYPE_INSTANCE_INFO,    \
                               ALSATimerInstanceInfoClass))

typedef struct _ALSATimerInstanceInfo           ALSATimerInstanceInfo;
typedef struct _ALSATimerInstanceInfoClass      ALSATimerInstanceInfoClass;
typedef struct _ALSATimerInstanceInfoPrivate    ALSATimerInstanceInfoPrivate;

struct _ALSATimerInstanceInfo {
    GObject parent_instance;

    ALSATimerInstanceInfoPrivate *priv;
};

struct _ALSATimerInstanceInfoClass {
    GObjectClass parent_class;
};

GType alsatimer_instance_info_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
