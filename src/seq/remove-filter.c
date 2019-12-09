// SPDX-License-Identifier: LGPL-3.0-or-later
#include "remove-filter.h"

ALSASeqRemoveFilter *seq_remove_filter_copy(const ALSASeqRemoveFilter *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqRemoveFilter, alsaseq_remove_filter, seq_remove_filter_copy, g_free)
