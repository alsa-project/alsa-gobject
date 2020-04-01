// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-queue.h"

ALSASeqEventDataQueue *seq_event_data_queue_copy(const ALSASeqEventDataQueue *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataQueue, alsaseq_event_data_queue, seq_event_data_queue_copy, g_free)
