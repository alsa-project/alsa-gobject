// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_ID__H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_ID__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_ID    (alsatimer_device_id_get_type())

typedef struct snd_timer_id ALSATimerDeviceId;

GType alsatimer_device_id_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
