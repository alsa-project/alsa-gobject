// SPDX-License-Identifier: LGPL-3.0-or-later
#include "device-info.h"

#include <sound/asound.h>

G_DEFINE_TYPE(ALSATimerDeviceInfo, alsatimer_device_info, G_TYPE_OBJECT)

static void alsatimer_device_info_class_init(ALSATimerDeviceInfoClass *klass)
{
}

static void alsatimer_device_info_init(ALSATimerDeviceInfo *self)
{
    return;
}
