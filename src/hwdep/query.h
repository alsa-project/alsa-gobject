// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_QUERY_H__
#define __ALSA_GOBJECT_ALSAHWDEP_QUERY_H__

#include <alsahwdep.h>

G_BEGIN_DECLS

void alsahwdep_get_device_id_list(guint card_id, guint **entries,
                                  gsize *entry_count, GError **error);

void alsahwdep_get_hwdep_sysname(guint card_id, guint device_id,
                                 char **sysname, GError **error);

void alsahwdep_get_hwdep_devnode(guint card_id, guint device_id,
                                 char **devnode, GError **error);

void alsahwdep_get_device_info(guint card_id, guint device_id,
                               ALSAHwdepDeviceInfo **device_info,
                               GError **error);

G_END_DECLS

#endif
