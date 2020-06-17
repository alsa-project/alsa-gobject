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

/**
 * alsatimer_event_data_tick_get_resolution:
 * @self: A #ALSATimerEventDataTick.
 * @resolution: (out): The resolution of tick event.
 *
 * Get the resolution of tick event.
 */
void alsatimer_event_data_tick_get_resolution(const ALSATimerEventDataTick *self,
                                              guint *resolution)
{
    *resolution = self->resolution;
}

/**
 * alsatimer_event_data_tick_get_ticks:
 * @self: A #ALSATimerEventDataTick.
 * @ticks: (out): The tick count since the last event.
 *
 * Get the tick count since the last event.
 */
void alsatimer_event_data_tick_get_ticks(const ALSATimerEventDataTick *self,
                                         guint *ticks)
{
    *ticks = self->ticks;
}
