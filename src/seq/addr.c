// SPDX-License-Identifier: LGPL-3.0-or-later
#include "addr.h"

ALSASeqAddr *seq_addr_copy(const ALSASeqAddr *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqAddr, alsaseq_addr, seq_addr_copy, g_free)
