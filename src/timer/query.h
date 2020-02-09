// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_QUERY__H__
#define __ALSA_GOBJECT_ALSATIMER_QUERY__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/device-id.h>
#include <timer/device-info.h>

#include <timer/alsatimer-enums.h>

G_BEGIN_DECLS

void alsatimer_get_sysname(char **sysname, GError **error);

void alsatimer_get_devnode(char **devnode, GError **error);

void alsatimer_get_device_id_list(GList **entries, GError **error);

void alsatimer_get_device_info(ALSATimerDeviceId *device_id,
                               ALSATimerDeviceInfo **device_info,
                               GError **error);
G_END_DECLS

#endif
