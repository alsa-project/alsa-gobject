// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event.h"

ALSATimerEvent *timer_event_copy(const ALSATimerEvent *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSATimerEvent, alsatimer_event, timer_event_copy, g_free)

/**
 * alsatimer_event_get_tick_data:
 * @self: A #ALSATimerEvent.
 * @tick: (out)(transfer none): The instance of #ALSATimerEventDataTick.
 *
 * Refer to the instance of #ALSATimerEventDataTick.
 */
void alsatimer_event_get_tick_data(ALSATimerEvent *self,
                                   const ALSATimerEventDataTick **tick)
{
    *tick = &self->tick;
}

/**
 * alsatimer_event_get_tstamp_data:
 * @self: A #ALSATimerEvent.
 * @tstamp: (out)(transfer none): The instance of #ALSATimerEventDataTstamp.
 *
 * Refer to the instance of #ALSATimerEventDataTstamp.
 */
void alsatimer_event_get_tstamp_data(ALSATimerEvent *self,
                                     const ALSATimerEventDataTstamp **tstamp)
{
    *tstamp = &self->tstamp;
}
