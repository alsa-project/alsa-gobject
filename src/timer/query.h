// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_QUERY__H__
#define __ALSA_GOBJECT_ALSATIMER_QUERY__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/device-id.h>
#include <timer/device-info.h>
#include <timer/device-status.h>
#include <timer/device-params.h>

#include <timer/alsatimer-enums.h>

G_BEGIN_DECLS

void alsatimer_get_sysname(char **sysname, GError **error);

void alsatimer_get_devnode(char **devnode, GError **error);

void alsatimer_get_device_id_list(GList **entries, GError **error);

void alsatimer_get_device_info(ALSATimerDeviceId *device_id,
                               ALSATimerDeviceInfo **device_info,
                               GError **error);

void alsatimer_get_device_status(ALSATimerDeviceId *device_id,
                                 ALSATimerDeviceStatus **device_status,
                                 GError **error);

void alsatimer_set_device_params(ALSATimerDeviceId *device_id,
                                 const ALSATimerDeviceParams *device_params,
                                 GError **error);

void alsatimer_get_tstamp_source(int *clock_id, GError **error);

G_END_DECLS

#endif
