// SPDX-License-Identifier: LGPL-3.0-or-later
#include "tstamp.h"

ALSASeqTstamp *seq_tstamp_copy(const ALSASeqTstamp *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqTstamp, alsaseq_tstamp, seq_tstamp_copy, g_free)
