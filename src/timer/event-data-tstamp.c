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
