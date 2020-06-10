// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event.h"

ALSATimerEvent *timer_event_copy(const ALSATimerEvent *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSATimerEvent, alsatimer_event, timer_event_copy, g_free)
