// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_INFO__H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_INFO__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_INFO     (alsaseq_queue_info_get_type())

#define ALSASEQ_QUEUE_INFO(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_QUEUE_INFO,    \
                                ALSASeqQueueInfo))
#define ALSASEQ_IS_QUEUE_INFO(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_QUEUE_INFO))

#define ALSASEQ_QUEUE_INFO_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_QUEUE_INFO,       \
                             ALSASeqQueueInfoClass))
#define ALSASEQ_IS_QUEUE_INFO_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_QUEUE_INFO))
#define ALSASEQ_QUEUE_INFO_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_QUEUE_INFO,     \
                               ALSASeqQueueInfoClass))

typedef struct _ALSASeqQueueInfo            ALSASeqQueueInfo;
typedef struct _ALSASeqQueueInfoClass       ALSASeqQueueInfoClass;
typedef struct _ALSASeqQueueInfoPrivate     ALSASeqQueueInfoPrivate;

struct _ALSASeqQueueInfo {
    GObject parent_instance;

    ALSASeqQueueInfoPrivate *priv;
};

struct _ALSASeqQueueInfoClass {
    GObjectClass parent_class;
};

GType alsaseq_queue_info_get_type() G_GNUC_CONST;

ALSASeqQueueInfo *alsaseq_queue_info_new();

G_END_DECLS

#endif
