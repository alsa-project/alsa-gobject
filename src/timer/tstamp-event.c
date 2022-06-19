// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSATimerTstampEvent:
 * A boxed object to represent event of timer with tstamp.
 *
 * A [struct@TstampEvent] is a boxed object to represent event of timer with tstamp.
 *
 * The object wraps `struct snd_timer_tread` in UAPI of Linux sound subsystem.
 */
ALSATimerTstampEvent *timer_tstamp_event_copy(const ALSATimerTstampEvent *self)
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

G_DEFINE_BOXED_TYPE(ALSATimerTstampEvent, alsatimer_tstamp_event, timer_tstamp_event_copy, g_free)

/**
 * alsatimer_tstamp_event_get_event:
 * @self: A [struct@TstampEvent].
 * @event: (out): The type of tstamp event, one of [enum@TstampEventType].
 *
 * Get the kind of event for the timestamp event.
 */
void alsatimer_tstamp_event_get_event(const ALSATimerTstampEvent *self,
                                      ALSATimerTstampEventType *event)
{
    *event = (ALSATimerTstampEventType)self->event;
}

/**
 * alsatimer_tstamp_event_get_tstamp:
 * @self: A [struct@TstampEvent].
 * @tstamp: (array fixed-size=2) (inout): The array with two elements for the seconds and
 *          nanoseconds part of timestamp when the instance queues the timestamp event.
 *
 * Get the seconds and nanoseconds part for the timestamp event.
 */
void alsatimer_tstamp_event_get_tstamp(const ALSATimerTstampEvent *self, gint64 *const tstamp[2])
{
    (*tstamp)[0] = (gint64)self->tstamp.tv_sec;
    (*tstamp)[1] = (gint64)self->tstamp.tv_nsec;
}

/**
 * alsatimer_tstamp_event_get_val:
 * @self: A [struct@TstampEvent].
 * @val: (out): The value depending on the type of timestamp event.
 *
 * Get the value depending on the type of timestamp event.
 */
void alsatimer_tstamp_event_get_val(const ALSATimerTstampEvent *self, guint *val)
{
    *val = self->val;
}