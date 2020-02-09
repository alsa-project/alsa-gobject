// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_USER_INSTANCE__H__
#define __ALSA_GOBJECT_ALSATIMER_USER_INSTANCE__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/device-id.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_USER_INSTANCE    (alsatimer_user_instance_get_type())

#define ALSATIMER_USER_INSTANCE(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSATIMER_TYPE_USER_INSTANCE,   \
                                ALSATimerUserInstance))
#define ALSATIMER_IS_USER_INSTANCE(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSATIMER_TYPE_USER_INSTANCE))

#define ALSATIMER_USER_INSTANCE_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSATIMER_TYPE_USER_INSTANCE,      \
                             ALSATimerUserInstanceClass))
#define ALSATIMER_IS_USER_INSTANCE_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSATIMER_TYPE_USER_INSTANCE))
#define ALSATIMER_USER_INSTANCE_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSATIMER_TYPE_USER_INSTANCE,    \
                               ALSATimerUserInstanceClass))

typedef struct _ALSATimerUserInstance           ALSATimerUserInstance;
typedef struct _ALSATimerUserInstanceClass      ALSATimerUserInstanceClass;
typedef struct _ALSATimerUserInstancePrivate    ALSATimerUserInstancePrivate;

struct _ALSATimerUserInstance {
    GObject parent_instance;

    ALSATimerUserInstancePrivate *priv;
};

struct _ALSATimerUserInstanceClass {
    GObjectClass parent_class;
};

GType alsatimer_user_instance_get_type() G_GNUC_CONST;

ALSATimerUserInstance *alsatimer_user_instance_new();

void alsatimer_user_instance_open(ALSATimerUserInstance *self, GError **error);

void alsatimer_user_instance_attach(ALSATimerUserInstance *self,
                                    ALSATimerDeviceId *device_id,
                                    GError **error);

G_END_DECLS

#endif
