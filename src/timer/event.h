// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_EVENT__H__
#define __ALSA_GOBJECT_ALSATIMER_EVENT__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_EVENT    (alsatimer_event_get_type())

typedef union {
    struct snd_timer_read tick;
    struct snd_timer_tread timestamp;
} ALSATimerEvent;

GType alsatimer_event_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
