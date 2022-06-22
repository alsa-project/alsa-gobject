// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_TICK_TIME_EVENT_H__
#define __ALSA_GOBJECT_ALSATIMER_TICK_TIME_EVENT_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_TICK_TIME_EVENT   (alsatimer_tick_time_event_get_type())

typedef struct snd_timer_read ALSATimerTickTimeEvent;

GType alsatimer_tick_time_event_get_type() G_GNUC_CONST;

void alsatimer_tick_time_event_get_resolution(const ALSATimerTickTimeEvent *self,
                                              guint *resolution);

void alsatimer_tick_time_event_get_count(const ALSATimerTickTimeEvent *self, guint *count);

G_END_DECLS

#endif
