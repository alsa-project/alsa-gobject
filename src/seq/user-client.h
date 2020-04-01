// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_USER_CLIENT__H__
#define __ALSA_GOBJECT_ALSASEQ_USER_CLIENT__H__

#include <glib.h>
#include <glib-object.h>

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

struct _ALSASeqUserClient {
    GObject parent_instance;
};

struct _ALSASeqUserClientClass {
    GObjectClass parent_class;
};

GType alsaseq_user_client_get_type() G_GNUC_CONST;

ALSASeqUserClient *alsaseq_user_client_new();

G_END_DECLS

#endif
