// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_QUERY__H__
#define __ALSA_GOBJECT_ALSAHWDEP_QUERY__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

void alsahwdep_get_device_id_list(guint card_id, guint **entries,
                                  gsize *entry_count, GError **error);

G_END_DECLS

#endif
