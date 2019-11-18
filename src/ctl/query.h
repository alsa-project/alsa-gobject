// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_QUERY__H__
#define __ALSA_GOBJECT_ALSACTL_QUERY__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

void alsactl_get_card_id_list(guint **entries, gsize *entry_count,
                              GError **error);

void alsactl_get_card_sysname(guint card_id, char **sysname, GError **error);

G_END_DECLS

#endif
