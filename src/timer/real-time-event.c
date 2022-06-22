// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSATimerRealTimeEvent:
 * A boxed object to express event of timer with real time.
 *
 * A [struct@RealTimeEvent] includes real time at which the event is queued.
 *
 * The object wraps `struct snd_timer_tread` in UAPI of Linux sound subsystem.
 */
ALSATimerRealTimeEvent *timer_real_time_event_copy(const ALSATimerRealTimeEvent *self)
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

G_DEFINE_BOXED_TYPE(ALSATimerRealTimeEvent, alsatimer_real_time_event, timer_real_time_event_copy, g_free)

/**
 * alsatimer_real_time_event_get_event:
 * @self: A [struct@RealTimeEvent].
 * @event: (out): The type of real time event, one of [enum@RealTimeEventType].
 *
 * Get the kind of event for the real time event.
 */
void alsatimer_real_time_event_get_event(const ALSATimerRealTimeEvent *self,
                                         ALSATimerRealTimeEventType *event)
{
    *event = (ALSATimerRealTimeEventType)self->event;
}

/**
 * alsatimer_real_time_event_get_time:
 * @self: A [struct@RealTimeEvent].
 * @real_time: (array fixed-size=2) (inout): The array with two elements for the seconds and
 *             nanoseconds part of timestamp when the real time event occurs.
 *
 * Get the seconds and nanoseconds part for the real time event.
 */
void alsatimer_real_time_event_get_time(const ALSATimerRealTimeEvent *self,
                                        gint64 *const real_time[2])
{
    (*real_time)[0] = (gint64)self->tstamp.tv_sec;
    (*real_time)[1] = (gint64)self->tstamp.tv_nsec;
}

/**
 * alsatimer_real_time_event_get_val:
 * @self: A [struct@RealTimeEvent].
 * @val: (out): The value depending on the type of timestamp event.
 *
 * Get the value depending on the type of real time event.
 */
void alsatimer_real_time_event_get_val(const ALSATimerRealTimeEvent *self, guint *val)
{
    *val = self->val;
}
