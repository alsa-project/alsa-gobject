// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_PRIVATES__H__
#define __ALSA_GOBJECT_ALSATIMER_PRIVATES__H__

#include "device-info.h"
#include "device-status.h"
#include "device-params.h"
#include "instance-info.h"
#include "instance-params.h"

#include <sound/asound.h>

G_BEGIN_DECLS

GQuark alsatimer_error_quark(void);

#define generate_error(err, errno)                              \
    g_set_error(err, alsatimer_error_quark(), errno,            \
                __FILE__ ":%d: %s", __LINE__, strerror(errno))

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

G_END_DECLS

#endif
