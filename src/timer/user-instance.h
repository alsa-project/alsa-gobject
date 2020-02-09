// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_USER_INSTANCE__H__
#define __ALSA_GOBJECT_ALSATIMER_USER_INSTANCE__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/device-id.h>
#include <timer/instance-info.h>
#include <timer/instance-params.h>
#include <timer/instance-status.h>
#include <timer/event-data.h>

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

    /**
     * ALSATimerUserInstanceClass::handle_event:
     * @self: A #ALSATimerUserInstance.
     * @event_data: (transfer none): An object derived from #ALSATimerEventData.
     *
     * When event occurs, this signal is emit.
     */
    void (*handle_event)(ALSATimerUserInstance *self,
                         const ALSATimerEventData *event_data);
};

GType alsatimer_user_instance_get_type() G_GNUC_CONST;

ALSATimerUserInstance *alsatimer_user_instance_new();

void alsatimer_user_instance_open(ALSATimerUserInstance *self, GError **error);

void alsatimer_user_instance_attach(ALSATimerUserInstance *self,
                                    ALSATimerDeviceId *device_id,
                                    ALSATimerEventDataType event_data_type,
                                    GError **error);

void alsatimer_user_instance_attach_as_slave(ALSATimerUserInstance *self,
                                        ALSATimerSlaveClass slave_class,
                                        int slave_id,
                                        ALSATimerEventDataType event_data_type,
                                        GError **error);

void alsatimer_user_instance_get_info(ALSATimerUserInstance *self,
                                      ALSATimerInstanceInfo **instance_info,
                                      GError **error);

void alsatimer_user_instance_set_params(ALSATimerUserInstance *self,
                                ALSATimerInstanceParams *const *instance_params,
                                GError **error);

void alsatimer_user_instance_get_status(ALSATimerUserInstance *self,
                                    ALSATimerInstanceStatus **instance_status,
                                    GError **error);

void alsatimer_user_instance_create_source(ALSATimerUserInstance *self,
                                         GSource **gsrc, GError **error);

G_END_DECLS

#endif
