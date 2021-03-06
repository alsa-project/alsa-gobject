// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/event-data-tick.h>
#include <timer/event-data-tstamp.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT    (alsatimer_event_get_type())

typedef union {
    ALSATimerEventDataTick tick;
    ALSATimerEventDataTstamp tstamp;
} ALSATimerEvent;

GType alsatimer_event_get_type() G_GNUC_CONST;

ALSATimerEvent *alsatimer_event_new();

void alsatimer_event_get_tick_data(ALSATimerEvent *self,
                                   const ALSATimerEventDataTick **tick);

void alsatimer_event_get_tstamp_data(ALSATimerEvent *self,
                                     const ALSATimerEventDataTstamp **tstamp);

G_END_DECLS

#endif
