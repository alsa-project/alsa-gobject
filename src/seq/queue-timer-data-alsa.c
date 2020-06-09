// SPDX-License-Identifier: LGPL-3.0-or-later
#include "queue-timer-data-alsa.h"

ALSASeqQueueTimerDataAlsa *seq_queue_timer_data_alsa_copy(const ALSASeqQueueTimerDataAlsa *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqQueueTimerDataAlsa, alsaseq_queue_timer_data_alsa, seq_queue_timer_data_alsa_copy, g_free)
