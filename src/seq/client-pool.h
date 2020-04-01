// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_CLIENT_POOL__H__
#define __ALSA_GOBJECT_ALSASEQ_CLIENT_POOL__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_CLIENT_POOL        (alsaseq_client_pool_get_type())

#define ALSASEQ_CLIENT_POOL(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_CLIENT_POOL,   \
                                ALSASeqClientPool))
#define ALSASEQ_IS_CLIENT_POOL(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_CLIENT_POOL))

#define ALSASEQ_CLIENT_POOL_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_CLIENT_POOL,      \
                             ALSASeqClientPoolClass))
#define ALSASEQ_IS_CLIENT_POOL_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_CLIENT_POOL))
#define ALSASEQ_CLIENT_POOL_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_CLIENT_POOL,    \
                               ALSASeqClientPoolClass))

typedef struct _ALSASeqClientPool           ALSASeqClientPool;
typedef struct _ALSASeqClientPoolClass      ALSASeqClientPoolClass;
typedef struct _ALSASeqClientPoolPrivate    ALSASeqClientPoolPrivate;

struct _ALSASeqClientPool {
    GObject parent_instance;

    ALSASeqClientPoolPrivate *priv;
};

struct _ALSASeqClientPoolClass {
    GObjectClass parent_class;
};

GType alsaseq_client_pool_get_type() G_GNUC_CONST;

ALSASeqClientPool *alsaseq_client_pool_new();

G_END_DECLS

#endif
