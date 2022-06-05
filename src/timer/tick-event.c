// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSATimerTickEvent:
 * A boxed object to represent event of timer with tick count.
 *
 * A [struct@TickEvent] is a boxed object to represent event of timer with tick count.
 *
 * The object wraps `struct snd_timer_read` in UAPI of Linux sound subsystem.
 */
ALSATimerTickEvent *timer_tick_event_copy(const ALSATimerTickEvent *self)
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

G_DEFINE_BOXED_TYPE(ALSATimerTickEvent, alsatimer_tick_event, timer_tick_event_copy, g_free)

/**
 * alsatimer_tick_event_get_resolution:
 * @self: A [struct@TickEvent].
 * @resolution: (out): The resolution of tick event.
 *
 * Get the resolution of tick event.
 */
void alsatimer_tick_event_get_resolution(const ALSATimerTickEvent *self,
                                              guint *resolution)
{
    *resolution = self->resolution;
}

/**
 * alsatimer_tick_event_get_ticks:
 * @self: A [struct@TickEvent].
 * @ticks: (out): The tick count since the last event.
 *
 * Get the tick count since the last event.
 */
void alsatimer_tick_event_get_ticks(const ALSATimerTickEvent *self,
                                         guint *ticks)
{
    *ticks = self->ticks;
}
