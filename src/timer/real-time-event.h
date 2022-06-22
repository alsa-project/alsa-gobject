// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_REAL_TIME_EVENT_H__
#define __ALSA_GOBJECT_ALSATIMER_REAL_TIME_EVENT_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_REAL_TIME_EVENT     (alsatimer_real_time_event_get_type())

typedef struct snd_timer_tread ALSATimerRealTimeEvent;

GType alsatimer_real_time_event_get_type() G_GNUC_CONST;

void alsatimer_real_time_event_get_event(const ALSATimerRealTimeEvent *self,
                                         ALSATimerRealTimeEventType *event);

void alsatimer_real_time_event_get_time(const ALSATimerRealTimeEvent *self,
                                        gint64 *const real_time[2]);

void alsatimer_real_time_event_get_val(const ALSATimerRealTimeEvent *self, guint *val);

G_END_DECLS

#endif
