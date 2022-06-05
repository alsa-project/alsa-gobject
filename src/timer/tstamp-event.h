// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_TSTAMP_EVENT_H__
#define __ALSA_GOBJECT_ALSATIMER_TSTAMP_EVENT_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_TSTAMP_EVENT     (alsatimer_tstamp_event_get_type())

typedef struct snd_timer_tread ALSATimerTstampEvent;

GType alsatimer_tstamp_event_get_type() G_GNUC_CONST;

void alsatimer_tstamp_event_get_event(const ALSATimerTstampEvent *self,
                                      ALSATimerTstampEventType *event);

void alsatimer_tstamp_event_get_tstamp(const ALSATimerTstampEvent *self, gint64 *const tstamp[2]);

void alsatimer_tstamp_event_get_val(const ALSATimerTstampEvent *self, guint *val);

G_END_DECLS

#endif
