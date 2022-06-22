// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSATimerTickTimeEvent:
 * A boxed object to express event of timer with tick count.
 *
 * A [struct@TickTimeEvent] includes tick count at which the event is queued.
 *
 * The object wraps `struct snd_timer_read` in UAPI of Linux sound subsystem.
 */
ALSATimerTickTimeEvent *timer_tick_time_event_copy(const ALSATimerTickTimeEvent *self)
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

G_DEFINE_BOXED_TYPE(ALSATimerTickTimeEvent, alsatimer_tick_time_event, timer_tick_time_event_copy, g_free)

/**
 * alsatimer_tick_time_event_get_resolution:
 * @self: A [struct@TickTimeEvent].
 * @resolution: (out): The resolution of tick event.
 *
 * Get the resolution of tick event.
 */
void alsatimer_tick_time_event_get_resolution(const ALSATimerTickTimeEvent *self,
                                              guint *resolution)
{
    *resolution = self->resolution;
}

/**
 * alsatimer_tick_time_event_get_count:
 * @self: A [struct@TickTimeEvent].
 * @count: (out): The tick count since the last event.
 *
 * Get the tick count since the last event.
 */
void alsatimer_tick_time_event_get_ticks(const ALSATimerTickTimeEvent *self, guint *count)
{
    *count = self->ticks;
}
