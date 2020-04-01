// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-connect.h"

ALSASeqEventDataConnect *seq_event_data_connect_copy(const ALSASeqEventDataConnect *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataConnect, alsaseq_event_data_connect, seq_event_data_connect_copy, g_free)
