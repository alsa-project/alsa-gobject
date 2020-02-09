// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_INSTANCE_PARAMS__H__
#define __ALSA_GOBJECT_ALSATIMER_INSTANCE_PARAMS__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_INSTANCE_PARAMS  (alsatimer_instance_params_get_type())

#define ALSATIMER_INSTANCE_PARAMS(obj)                          \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSATIMER_TYPE_INSTANCE_PARAMS, \
                                ALSATimerInstanceParams))
#define ALSATIMER_IS_INSTANCE_PARAMS(obj)                       \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSATIMER_TYPE_INSTANCE_PARAMS))

#define ALSATIMER_INSTANCE_PARAMS_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSATIMER_TYPE_INSTANCE_PARAMS,    \
                             ALSATimerInstanceParamsClass))
#define ALSATIMER_IS_INSTANCE_PARAMS_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSATIMER_TYPE_INSTANCE_PARAMS))
#define ALSATIMER_INSTANCE_PARAMS_GET_CLASS(obj)                \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSATIMER_TYPE_INSTANCE_PARAMS,  \
                               ALSATimerInstanceParamsClass))

typedef struct _ALSATimerInstanceParams         ALSATimerInstanceParams;
typedef struct _ALSATimerInstanceParamsClass    ALSATimerInstanceParamsClass;

struct _ALSATimerInstanceParams {
    GObject parent_instance;
};

struct _ALSATimerInstanceParamsClass {
    GObjectClass parent_class;
};

GType alsatimer_instance_params_get_type() G_GNUC_CONST;

ALSATimerInstanceParams *alsatimer_instance_params_new();

G_END_DECLS

#endif
