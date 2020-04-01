// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-ctl.h"

ALSASeqEventDataCtl *seq_event_data_ctl_copy(const ALSASeqEventDataCtl *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataCtl, alsaseq_event_data_ctl, seq_event_data_ctl_copy, g_free)
