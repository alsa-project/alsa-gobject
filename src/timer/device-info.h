// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_DEVICE_INFO_H__
#define __ALSA_GOBJECT_ALSATIMER_DEVICE_INFO_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_DEVICE_INFO   (alsatimer_device_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSATimerDeviceInfo, alsatimer_device_info, ALSATIMER, DEVICE_INFO,
                         GObject);

struct _ALSATimerDeviceInfoClass {
    GObjectClass parent_class;
};

G_END_DECLS

#endif
