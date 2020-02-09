// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_ID__H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_ID__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/alsatimer-enums.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_ID    (alsatimer_device_id_get_type())

typedef struct snd_timer_id ALSATimerDeviceId;

GType alsatimer_device_id_get_type() G_GNUC_CONST;

ALSATimerDeviceId *alsatimer_device_id_new(ALSATimerClass class,
                                           gint card_id, gint device_id,
                                           gint subdevice_id);

void alsatimer_device_id_get_class(const ALSATimerDeviceId *self,
                                   ALSATimerClass *class);

void alsatimer_device_id_get_card_id(const ALSATimerDeviceId *self,
                                     gint *card_id);

void alsatimer_device_id_get_device_id(const ALSATimerDeviceId *self,
                                       gint *device_id);

void alsatimer_device_id_get_subdevice_id(const ALSATimerDeviceId *self,
                                          gint *subdevice_id);

G_END_DECLS

#endif
