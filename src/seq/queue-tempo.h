// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TEMPO_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TEMPO_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_TEMPO    (alsaseq_queue_tempo_get_type())

#define ALSASEQ_QUEUE_TEMPO(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_QUEUE_TEMPO,   \
                                ALSASeqQueueTempo))
#define ALSASEQ_IS_QUEUE_TEMPO(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_QUEUE_TEMPO))

#define ALSASEQ_QUEUE_TEMPO_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_QUEUE_TEMPO,      \
                             ALSASeqQueueTempoClass))
#define ALSASEQ_IS_QUEUE_TEMPO_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_QUEUE_TEMPO))
#define ALSASEQ_QUEUE_TEMPO_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_QUEUE_TEMPO,    \
                               ALSASeqQueueTempoClass))

typedef struct _ALSASeqQueueTempo           ALSASeqQueueTempo;
typedef struct _ALSASeqQueueTempoClass      ALSASeqQueueTempoClass;
typedef struct _ALSASeqQueueTempoPrivate    ALSASeqQueueTempoPrivate;

struct _ALSASeqQueueTempo {
    GObject parent_instance;

    ALSASeqQueueTempoPrivate *priv;
};

struct _ALSASeqQueueTempoClass {
    GObjectClass parent_class;
};

GType alsaseq_queue_tempo_get_type() G_GNUC_CONST;

ALSASeqQueueTempo *alsaseq_queue_tempo_new();

void alsaseq_queue_tempo_get_skew(ALSASeqQueueTempo *self, const guint32 *skew[2]);
void alsaseq_queue_tempo_set_skew(ALSASeqQueueTempo *self, const guint32 skew[2]);

G_END_DECLS

#endif
