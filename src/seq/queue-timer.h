// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_TIMER     (alsaseq_queue_timer_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqQueueTimer, alsaseq_queue_timer, ALSASEQ, QUEUE_TIMER, GObject);

struct _ALSASeqQueueTimerClass {
    GObjectClass parent_class;
};

ALSASeqQueueTimer *alsaseq_queue_timer_new();

void alsaseq_queue_timer_get_alsa_data(ALSASeqQueueTimer *self,
                                       const ALSASeqQueueTimerDataAlsa **data);
void alsaseq_queue_timer_set_alsa_data(ALSASeqQueueTimer *self,
                                       const ALSASeqQueueTimerDataAlsa *data);

G_END_DECLS

#endif
