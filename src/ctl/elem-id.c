// SPDX-License-Identifier: LGPL-3.0-or-later
#include "elem-id.h"

ALSACtlElemId *ctl_elem_id_copy(const ALSACtlElemId *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSACtlElemId, alsactl_elem_id, ctl_elem_id_copy, g_free);
