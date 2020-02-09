// SPDX-License-Identifier: LGPL-3.0-or-later
#include "device-id.h"

ALSATimerDeviceId *timer_device_id_copy(const ALSATimerDeviceId *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSATimerDeviceId, alsatimer_device_id, timer_device_id_copy, g_free)
