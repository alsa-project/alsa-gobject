// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TIMESTAMP__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT_DATA_TIMESTAMP__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP (alsatimer_event_data_timestamp_get_type())

typedef struct snd_timer_tread ALSATimerEventDataTimestamp;

GType alsatimer_event_data_timestamp_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
