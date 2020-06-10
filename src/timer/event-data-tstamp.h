// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TSTAMP__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TSTAMP__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT_DATA_TSTAMP    (alsatimer_event_data_tstamp_get_type())

typedef struct snd_timer_tread ALSATimerEventDataTstamp;

GType alsatimer_event_data_tstamp_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
