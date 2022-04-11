// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_PARAMS_H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_PARAMS_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_PARAMS    (alsatimer_device_params_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSATimerDeviceParams, alsatimer_device_params, ALSATIMER, DEVICE_PARAMS,
                         GObject);

struct _ALSATimerDeviceParamsClass {
    GObjectClass parent_class;
};

ALSATimerDeviceParams *alsatimer_device_params_new();

G_END_DECLS

#endif
