// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSATimerEvent:
 * A boxed object to represent event of timer.
 *
 * A [struct@Event] is a boxed object to represent event of timer. The instance of object uses
 * single storage for two types of event data; [struct@EventDataTick] for and
 * [struct@EventDataTstamp]. Applications can decide to use one of the two by passing one of
 * [enum@EventType] to the call of [method@UserInstance.attach].
 */
ALSATimerEvent *timer_event_copy(const ALSATimerEvent *self)
{
#ifdef g_memdup2
    return g_memdup2(self, sizeof(*self));
#else
    // GLib v2.68 deprecated g_memdup() with concern about overflow by narrow conversion from size_t to
    // unsigned int however it's safe in the local case.
    gpointer ptr = g_malloc(sizeof(*self));
    memcpy(ptr, self, sizeof(*self));
    return ptr;
#endif
}

G_DEFINE_BOXED_TYPE(ALSATimerEvent, alsatimer_event, timer_event_copy, g_free)

/**
 * alsatimer_event_new:
 *
 * Allocate and return the instance of [struct@Event].
 *
 * Returns: An instance of [struct@Event].
 */
ALSATimerEvent *alsatimer_event_new()
{
    return g_malloc0(sizeof(ALSATimerEvent));
}

/**
 * alsatimer_event_get_tick_data:
 * @self: A [struct@Event].
 * @tick: (out)(transfer none): The instance of [struct@EventDataTick].
 *
 * Refer to the instance of [struct@EventDataTick].
 */
void alsatimer_event_get_tick_data(ALSATimerEvent *self,
                                   const ALSATimerEventDataTick **tick)
{
    *tick = &self->tick;
}

/**
 * alsatimer_event_get_tstamp_data:
 * @self: A [struct@Event].
 * @tstamp: (out)(transfer none): The instance of [struct@EventDataTstamp].
 *
 * Refer to the instance of [struct@EventDataTstamp].
 */
void alsatimer_event_get_tstamp_data(ALSATimerEvent *self,
                                     const ALSATimerEventDataTstamp **tstamp)
{
    *tstamp = &self->tstamp;
}
