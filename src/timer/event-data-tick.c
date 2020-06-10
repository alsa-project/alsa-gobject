// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-tick.h"
#include "privates.h"

/**
 * SECTION: event-data-tick
 * @Title: ALSATimerEventDataTick
 * @Short_description: A boxed object to represent event of timer with tick
 *                     count
 *
 * A #ALSATimerEventDataTick is a boxed object to represent event of timer with
 * tick count.
 *
 * The object wraps 'struct snd_timer_read' in UAPI of Linux sound subsystem.
 */
ALSATimerEventDataTick *timer_event_data_tick_copy(const ALSATimerEventDataTick *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSATimerEventDataTick, alsatimer_event_data_tick, timer_event_data_tick_copy, g_free)
