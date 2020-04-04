// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_USER_CLIENT__H__
#define __ALSA_GOBJECT_ALSASEQ_USER_CLIENT__H__

#include <glib.h>
#include <glib-object.h>

#include <seq/client-info.h>
#include <seq/port-info.h>
#include <seq/client-pool.h>
#include <seq/event.h>
#include <seq/event-fixed.h>
#include <seq/event-variable.h>
#include <seq/subscribe-data.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_USER_CLIENT     (alsaseq_user_client_get_type())

#define ALSASEQ_USER_CLIENT(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_USER_CLIENT,   \
                                ALSASeqUserClient))
#define ALSASEQ_IS_USER_CLIENT(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_USER_CLIENT))

#define ALSASEQ_USER_CLIENT_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_USER_CLIENT,      \
                             ALSASeqUserClientClass))
#define ALSASEQ_IS_USER_CLIENT_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_USER_CLIENT))
#define ALSASEQ_USER_CLIENT_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_USER_CLIENT,    \
                               ALSASeqUserClientClass))

typedef struct _ALSASeqUserClient           ALSASeqUserClient;
typedef struct _ALSASeqUserClientClass      ALSASeqUserClientClass;
typedef struct _ALSASeqUserClientPrivate    ALSASeqUserClientPrivate;

struct _ALSASeqUserClient {
    GObject parent_instance;

    ALSASeqUserClientPrivate *priv;
};

struct _ALSASeqUserClientClass {
    GObjectClass parent_class;

    /**
     * ALSASeqUserClientClass::handle_event:
     * @self: A #ALSASeqUserClient.
     * @event: (transfer none): A #ALSASeqEvent or derived objects.
     *
     * When event occurs, this signal is emit with an object for the event.
     */
    void (*handle_event)(ALSASeqUserClient *self, const ALSASeqEvent *event);
};

GType alsaseq_user_client_get_type() G_GNUC_CONST;

ALSASeqUserClient *alsaseq_user_client_new();

void alsaseq_user_client_open(ALSASeqUserClient *self, gint open_flag,
                              GError **error);

void alsaseq_user_client_set_info(ALSASeqUserClient *self,
                                  ALSASeqClientInfo *client_info,
                                  GError **error);
void alsaseq_user_client_get_info(ALSASeqUserClient *self,
                                  ALSASeqClientInfo *const *client_info,
                                  GError **error);

void alsaseq_user_client_create_port(ALSASeqUserClient *self,
                                     ALSASeqPortInfo *port_info,
                                     guint8 *port_id, GError **error);

void alsaseq_user_client_update_port(ALSASeqUserClient *self,
                                     ALSASeqPortInfo *port_info,
                                     guint8 port_id, GError **error);

void alsaseq_user_client_delete_port(ALSASeqUserClient *self,
                                     ALSASeqPortInfo *port_info,
                                     guint8 port_id, GError **error);

void alsaseq_user_client_set_pool(ALSASeqUserClient *self,
                                  ALSASeqClientPool *client_pool,
                                  GError **error);

void alsaseq_user_client_get_pool(ALSASeqUserClient *self,
                                  ALSASeqClientPool *const *client_pool,
                                  GError **error);

void alsaseq_user_client_schedule_event(ALSASeqUserClient *self,
                                        ALSASeqEvent *event, GError **error);

void alsaseq_user_client_create_source(ALSASeqUserClient *self,
                                       GSource **gsrc, GError **error);

void alsaseq_user_client_operate_subscription(ALSASeqUserClient *self,
                                              ALSASeqSubscribeData *subs_data,
                                              gboolean establish,
                                              GError **error);

G_END_DECLS

#endif
