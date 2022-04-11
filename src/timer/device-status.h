// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_STATUS_H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_STATUS_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_STATUS    (alsatimer_device_status_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSATimerDeviceStatus, alsatimer_device_status, ALSATIMER, DEVICE_STATUS,
                         GObject);

struct _ALSATimerDeviceStatusClass {
    GObjectClass parent_class;
};

ALSATimerDeviceStatus *alsatimer_device_status_new();

G_END_DECLS

#endif
