// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-result.h"

ALSASeqEventDataResult *seq_event_data_result_copy(const ALSASeqEventDataResult *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataResult, alsaseq_event_data_result, seq_event_data_result_copy, g_free)
