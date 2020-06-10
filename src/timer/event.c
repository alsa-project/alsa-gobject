// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event.h"

/**
 * SECTION: event
 * @Title: ALSATimerEvent
 * @Short_description: A boxed object to represent event of timer
 *
 * A #ALSATimerEvent is a boxed object to represent event of timer. The
 * instance of object uses single storage for two types of event data;
 * #ALSATimerEventDataTick for and #ALSATimerEventDataTstamp. Applications can
 * decide to use one of the two by passing one of #ALSATimerEventType to the
 * call of alsatimer_user_instance_attach().
 */
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
