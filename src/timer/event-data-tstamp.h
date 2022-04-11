// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TSTAMP_H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TSTAMP_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT_DATA_TSTAMP    (alsatimer_event_data_tstamp_get_type())

typedef struct snd_timer_tread ALSATimerEventDataTstamp;

GType alsatimer_event_data_tstamp_get_type() G_GNUC_CONST;

void alsatimer_event_data_tstamp_get_event(const ALSATimerEventDataTstamp *self,
                                           ALSATimerEventType *event);

void alsatimer_event_data_tstamp_get_tstamp(const ALSATimerEventDataTstamp *self,
                                            gint64 *const tstamp[2]);

void alsatimer_event_data_tstamp_get_val(const ALSATimerEventDataTstamp *self,
                                         guint *val);

G_END_DECLS

#endif
