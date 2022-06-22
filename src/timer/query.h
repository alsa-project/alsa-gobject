// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_QUERY_H__
#define __ALSA_GOBJECT_ALSATIMER_QUERY_H__

#include <alsatimer.h>

G_BEGIN_DECLS

gboolean alsatimer_get_sysname(char **sysname, GError **error);

gboolean alsatimer_get_devnode(char **devnode, GError **error);

gboolean alsatimer_get_device_id_list(GList **entries, GError **error);

gboolean alsatimer_get_device_info(ALSATimerDeviceId *device_id, ALSATimerDeviceInfo **device_info,
                                   GError **error);

gboolean alsatimer_get_device_status(ALSATimerDeviceId *device_id,
                                     ALSATimerDeviceStatus *const *device_status, GError **error);

gboolean alsatimer_set_device_params(ALSATimerDeviceId *device_id,
                                     const ALSATimerDeviceParams *device_params, GError **error);

gboolean alsatimer_get_real_time_clock_id(int *clock_id, GError **error);

G_END_DECLS

#endif
