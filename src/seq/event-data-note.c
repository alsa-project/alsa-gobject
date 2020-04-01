// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-note.h"

ALSASeqEventDataNote *seq_event_data_note_copy(const ALSASeqEventDataNote *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataNote, alsaseq_event_data_note, seq_event_data_note_copy, g_free)
