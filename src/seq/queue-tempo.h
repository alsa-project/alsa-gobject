// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TEMPO_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TEMPO_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_TEMPO    (alsaseq_queue_tempo_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqQueueTempo, alsaseq_queue_tempo, ALSASEQ, QUEUE_TEMPO, GObject);

struct _ALSASeqQueueTempoClass {
    GObjectClass parent_class;
};

ALSASeqQueueTempo *alsaseq_queue_tempo_new();

void alsaseq_queue_tempo_get_skew(ALSASeqQueueTempo *self, const guint32 *skew[2]);
void alsaseq_queue_tempo_set_skew(ALSASeqQueueTempo *self, const guint32 skew[2]);

G_END_DECLS

#endif
