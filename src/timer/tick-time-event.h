// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_TICK_EVENT_H__
#define __ALSA_GOBJECT_ALSATIMER_TICK_EVENT_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_TICK_EVENT   (alsatimer_tick_event_get_type())

typedef struct snd_timer_read ALSATimerTickEvent;

GType alsatimer_tick_event_get_type() G_GNUC_CONST;

void alsatimer_tick_event_get_resolution(const ALSATimerTickEvent *self,
                                              guint *resolution);

void alsatimer_tick_event_get_ticks(const ALSATimerTickEvent *self,
                                         guint *ticks);

G_END_DECLS

#endif
