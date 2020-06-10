// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TICK__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TICK__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT_DATA_TICK   (alsatimer_event_data_tick_get_type())

typedef struct snd_timer_read ALSATimerEventDataTick;

GType alsatimer_event_data_tick_get_type() G_GNUC_CONST;

void alsatimer_event_data_tick_get_resolution(ALSATimerEventDataTick *self,
                                              guint *resolution);

void alsatimer_event_data_tick_get_ticks(ALSATimerEventDataTick *self,
                                         guint *ticks);

G_END_DECLS

#endif
