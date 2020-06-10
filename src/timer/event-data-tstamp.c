// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-tstamp.h"
#include "privates.h"

/**
 * SECTION: event-data-tstamp
 * @Title: ALSATimerEventDataTstamp
 * @Short_description: A boxed object to represent event of timer with
 *                     tstamp
 *
 * A #ALSATimerEventDataTstamp is a boxed object to represent event of timer
 * with tstamp.
 *
 * The object wraps 'struct snd_timer_tread' in UAPI of Linux sound subsystem.
 */
ALSATimerEventDataTstamp *timer_event_data_tstamp_copy(const ALSATimerEventDataTstamp *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSATimerEventDataTstamp, alsatimer_event_data_tstamp, timer_event_data_tstamp_copy, g_free)

/**
 * alsatimer_event_data_tstamp_get_event:
 * @self: A #ALSATimerEventDataTstamp.
 * @event: (out): The type of tstamp event, one of ALSATimerEventType.
 *
 * Get the kind of event for the timestamp event.
 */
void alsatimer_event_data_tstamp_get_event(ALSATimerEventDataTstamp *self,
                                           ALSATimerEventType *event)
{
    *event = (ALSATimerEventType)self->event;
}

/**
 * alsatimer_event_data_tstamp_get_tstamp:
 * @self: A #ALSATimerEventDataTstamp.
 * @tv_sec: (out): The seconds part of timestamp.
 * @tv_nsec: (out): The nanoseconds part of timestamp.
 *
 * Get the seconds and nanoseconds part for the timestamp event.
 */
void alsatimer_event_data_tstamp_get_tstamp(ALSATimerEventDataTstamp *self,
                                            guint *tv_sec, guint *tv_nsec)
{
    *tv_sec = (guint)self->tstamp.tv_sec;
    *tv_nsec = (guint)self->tstamp.tv_nsec;
}

/**
 * alsatimer_event_data_tstamp_get_val:
 * @self: A #ALSATimerEventDataTstamp.
 * @val: (out): The value depending on the type of timestamp event.
 *
 * Get the value depending on the type of timestamp event.
 */
void alsatimer_event_data_tstamp_get_val(ALSATimerEventDataTstamp *self,
                                         guint *val)
{
    *val = self->val;
}
