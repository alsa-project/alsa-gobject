// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_CLIENT_INFO__H__
#define __ALSA_GOBJECT_ALSASEQ_CLIENT_INFO__H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_CLIENT_INFO     (alsaseq_client_info_get_type())

#define ALSASEQ_CLIENT_INFO(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_CLIENT_INFO,   \
                                ALSASeqClientInfo))
#define ALSASEQ_IS_CLIENT_INFO(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_CLIENT_INFO))

#define ALSASEQ_CLIENT_INFO_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_CLIENT_INFO,      \
                             ALSASeqClientInfoClass))
#define ALSASEQ_IS_CLIENT_INFO_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_CLIENT_INFO))
#define ALSASEQ_CLIENT_INFO_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_CLIENT_INFO,    \
                               ALSASeqClientInfoClass))

typedef struct _ALSASeqClientInfo           ALSASeqClientInfo;
typedef struct _ALSASeqClientInfoClass      ALSASeqClientInfoClass;
typedef struct _ALSASeqClientInfoPrivate    ALSASeqClientInfoPrivate;

struct _ALSASeqClientInfo {
    GObject parent_instance;

    ALSASeqClientInfoPrivate *priv;
};

struct _ALSASeqClientInfoClass {
    GObjectClass parent_class;
};

GType alsaseq_client_info_get_type() G_GNUC_CONST;

ALSASeqClientInfo *alsaseq_client_info_new();

void alsaseq_client_info_set_event_filter(ALSASeqClientInfo *self,
                                          const ALSASeqEventType *event_types,
                                          gsize event_type_count,
                                          GError **error);

void alsaseq_client_info_get_event_filter(ALSASeqClientInfo *self,
                                          ALSASeqEventType **event_types,
                                          gsize *event_type_count,
                                          GError **error);

G_END_DECLS

#endif
