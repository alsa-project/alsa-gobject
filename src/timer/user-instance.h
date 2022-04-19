// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_USER_INSTANCE_H__
#define __ALSA_GOBJECT_ALSATIMER_USER_INSTANCE_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_USER_INSTANCE    (alsatimer_user_instance_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSATimerUserInstance, alsatimer_user_instance, ALSATIMER, USER_INSTANCE,
                         GObject);

#define ALSATIMER_USER_INSTANCE_ERROR   alsatimer_user_instance_error_quark()

GQuark alsatimer_user_instance_error_quark();

struct _ALSATimerUserInstanceClass {
    GObjectClass parent_class;

    /**
     * ALSATimerUserInstanceClass::handle_event:
     * @self: A [class@UserInstance].
     * @event: (transfer none): An object derived from [struct@Event].
     *
     * Class closure for the [signal@UserInstance::handle-event] signal.
     */
    void (*handle_event)(ALSATimerUserInstance *self,
                         const ALSATimerEvent *event);

    /**
     * ALSATimerUserInstanceClass::handle_disconnection:
     * @self: A [class@UserInstance].
     *
     * Class closure for the [signal@UserInstance::handle-disconnection] signal.
     */
    void (*handle_disconnection)(ALSATimerUserInstance *self);
};

ALSATimerUserInstance *alsatimer_user_instance_new();

void alsatimer_user_instance_open(ALSATimerUserInstance *self, gint open_flag,
                                  GError **error);

void alsatimer_user_instance_get_protocol_version(ALSATimerUserInstance *self,
                                        const guint16 *proto_ver_triplet[3],
                                        GError **error);

void alsatimer_user_instance_choose_event_data_type(ALSATimerUserInstance *self,
                                        ALSATimerEventDataType event_data_type,
                                        GError **error);

void alsatimer_user_instance_attach(ALSATimerUserInstance *self,
                                    ALSATimerDeviceId *device_id,
                                    GError **error);

void alsatimer_user_instance_attach_as_slave(ALSATimerUserInstance *self,
                                        ALSATimerSlaveClass slave_class,
                                        int slave_id,
                                        GError **error);

void alsatimer_user_instance_get_info(ALSATimerUserInstance *self,
                                      ALSATimerInstanceInfo **instance_info,
                                      GError **error);

void alsatimer_user_instance_set_params(ALSATimerUserInstance *self,
                                ALSATimerInstanceParams *const *instance_params,
                                GError **error);

void alsatimer_user_instance_get_status(ALSATimerUserInstance *self,
                                ALSATimerInstanceStatus *const *instance_status,
                                GError **error);

void alsatimer_user_instance_create_source(ALSATimerUserInstance *self,
                                         GSource **gsrc, GError **error);

void alsatimer_user_instance_start(ALSATimerUserInstance *self, GError **error);

void alsatimer_user_instance_stop(ALSATimerUserInstance *self, GError **error);

void alsatimer_user_instance_pause(ALSATimerUserInstance *self, GError **error);

void alsatimer_user_instance_continue(ALSATimerUserInstance *self,
                                      GError **error);

G_END_DECLS

#endif
