// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_USER_CLIENT_H__
#define __ALSA_GOBJECT_ALSASEQ_USER_CLIENT_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_USER_CLIENT     (alsaseq_user_client_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqUserClient, alsaseq_user_client, ALSASEQ, USER_CLIENT, GObject);

#define ALSASEQ_USER_CLIENT_ERROR   alsaseq_user_client_error_quark()

GQuark alsaseq_user_client_error_quark();

struct _ALSASeqUserClientClass {
    GObjectClass parent_class;

    /**
     * ALSASeqUserClientClass::handle_event:
     * @self: A [class@UserClient].
     * @ev_cntr: (transfer none): The instance of [class@EventCntr] which
     *             points to the batch of events.
     *
     * When event occurs, this signal is emit with the instance of object which
     * points to a batch of events. The instance should not be passed directly
     * to alsaseq_user_client_schedule_event() again because its memory
     * alignment is different for events with blob data.
     */
    void (*handle_event)(ALSASeqUserClient *self,
                         const ALSASeqEventCntr *ev_cntr);
};

ALSASeqUserClient *alsaseq_user_client_new();

gboolean alsaseq_user_client_open(ALSASeqUserClient *self, gint open_flag, GError **error);

gboolean alsaseq_user_client_get_protocol_version(ALSASeqUserClient *self,
                                                  const guint16 *proto_ver_triplet[3],
                                                  GError **error);

gboolean alsaseq_user_client_set_info(ALSASeqUserClient *self, ALSASeqClientInfo *client_info,
                                      GError **error);
gboolean alsaseq_user_client_get_info(ALSASeqUserClient *self,
                                      ALSASeqClientInfo *const *client_info, GError **error);

gboolean alsaseq_user_client_create_port(ALSASeqUserClient *self,
                                         ALSASeqPortInfo *const *port_info, GError **error);
gboolean alsaseq_user_client_create_port_at(ALSASeqUserClient *self,
                                            ALSASeqPortInfo *const *port_info,
                                            guint8 port_id, GError **error);

gboolean alsaseq_user_client_update_port(ALSASeqUserClient *self, ALSASeqPortInfo *port_info,
                                         guint8 port_id, GError **error);

gboolean alsaseq_user_client_delete_port(ALSASeqUserClient *self, guint8 port_id, GError **error);

gboolean alsaseq_user_client_set_pool(ALSASeqUserClient *self, ALSASeqClientPool *client_pool,
                                      GError **error);

gboolean alsaseq_user_client_get_pool(ALSASeqUserClient *self,
                                      ALSASeqClientPool *const *client_pool, GError **error);

gboolean alsaseq_user_client_schedule_event(ALSASeqUserClient *self, const ALSASeqEvent *event,
                                            GError **error);
gboolean alsaseq_user_client_schedule_events(ALSASeqUserClient *self, const GList *events,
                                             gsize *count, GError **error);

gboolean alsaseq_user_client_create_source(ALSASeqUserClient *self, GSource **gsrc, GError **error);

gboolean alsaseq_user_client_operate_subscription(ALSASeqUserClient *self,
                                                  ALSASeqSubscribeData *subs_data,
                                                  gboolean establish, GError **error);

gboolean alsaseq_user_client_create_queue(ALSASeqUserClient *self,
                                          ALSASeqQueueInfo *const *queue_info, GError **error);
gboolean alsaseq_user_client_delete_queue(ALSASeqUserClient *self, guint8 queue_id, GError **error);
gboolean alsaseq_user_client_update_queue(ALSASeqUserClient *self, ALSASeqQueueInfo *queue_info,
                                          GError **error);

gboolean alsaseq_user_client_get_queue_usage(ALSASeqUserClient *self, guint8 queue_id,
                                             gboolean *use, GError **error);
gboolean alsaseq_user_client_set_queue_usage(ALSASeqUserClient *self, guint8 queue_id,
                                             gboolean use, GError **error);

gboolean alsaseq_user_client_set_queue_tempo(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTempo *queue_tempo, GError **error);
gboolean alsaseq_user_client_get_queue_tempo(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTempo **queue_tempo, GError **error);

gboolean alsaseq_user_client_set_queue_timer(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTimerCommon *queue_timer, GError **error);
gboolean alsaseq_user_client_get_queue_timer(ALSASeqUserClient *self, guint8 queue_id,
                                             ALSASeqQueueTimerCommon **queue_timer, GError **error);

gboolean alsaseq_user_client_remove_events(ALSASeqUserClient *self, ALSASeqRemoveFilter *filter,
                                           GError **error);

G_END_DECLS

#endif
