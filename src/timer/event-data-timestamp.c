// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-timestamp.h"
#include "privates.h"

/**
 * SECTION: event-data-timestamp
 * @Title: ALSATimerEventDataTimestamp
 * @Short_description: A boxed object to represent event of timer with
 *                     timestamp
 *
 * A #ALSATimerEventDataTimestamp is a boxed object to represent event of timer
 * with timestamp.
 *
 * The object wraps 'struct snd_timer_tread' in UAPI of Linux sound subsystem.
 */
ALSATimerEventDataTimestamp *timer_event_data_timestamp_copy(const ALSATimerEventDataTimestamp *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSATimerEventDataTimestamp, alsatimer_event_data_timestamp, timer_event_data_timestamp_copy, g_free)
