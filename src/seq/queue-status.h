// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_STATUS__H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_STATUS__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_STATUS       (alsaseq_queue_status_get_type())

#define ALSASEQ_QUEUE_STATUS(obj)                           \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_QUEUE_STATUS,  \
                                ALSASeqQueueStatus))
#define ALSASEQ_IS_QUEUE_STATUS(obj)                        \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_QUEUE_STATUS))

#define ALSASEQ_QUEUE_STATUS_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_QUEUE_STATUS,     \
                             ALSASeqQueueStatusClass))
#define ALSASEQ_IS_QUEUE_STATUS_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_QUEUE_STATUS))
#define ALSASEQ_QUEUE_STATUS_GET_CLASS(obj)                 \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_QUEUE_STATUS,   \
                               ALSASeqQueueStatusClass))

typedef struct _ALSASeqQueueStatus          ALSASeqQueueStatus;
typedef struct _ALSASeqQueueStatusClass     ALSASeqQueueStatusClass;

struct _ALSASeqQueueStatus {
    GObject parent_instance;
};

struct _ALSASeqQueueStatusClass {
    GObjectClass parent_class;
};

GType alsaseq_queue_status_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
