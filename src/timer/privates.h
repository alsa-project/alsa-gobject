// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_PRIVATES_H__
#define __ALSA_GOBJECT_ALSATIMER_PRIVATES_H__

#include "alsatimer.h"

G_BEGIN_DECLS

void timer_device_info_refer_private(ALSATimerDeviceInfo *self,
                                     struct snd_timer_ginfo **info);

void timer_device_status_refer_private(ALSATimerDeviceStatus *self,
                                       struct snd_timer_gstatus **status);

void timer_device_params_refer_private(ALSATimerDeviceParams *self,
                                       struct snd_timer_gparams **params);

void timer_instance_info_refer_private(ALSATimerInstanceInfo *self,
                                       struct snd_timer_info **info);

void timer_instance_params_refer_private(ALSATimerInstanceParams *self,
                                         struct snd_timer_params **params);

void timer_instance_status_refer_private(ALSATimerInstanceStatus *self,
                                         struct snd_timer_status **status);

G_END_DECLS

#endif
