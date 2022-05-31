// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_QUERY_H__
#define __ALSA_GOBJECT_ALSACTL_QUERY_H__

#include <alsactl.h>

G_BEGIN_DECLS

gboolean alsactl_get_card_id_list(guint **entries, gsize *entry_count,
                              GError **error);

gboolean alsactl_get_card_sysname(guint card_id, char **sysname, GError **error);

gboolean alsactl_get_control_sysname(guint card_id, char **sysname, GError **error);

gboolean alsactl_get_control_devnode(guint card_id, char **devnode, GError **error);

G_END_DECLS

#endif
