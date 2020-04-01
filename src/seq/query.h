// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUERY__H__
#define __ALSA_GOBJECT_ALSASEQ_QUERY__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

void alsaseq_get_seq_sysname(gchar **sysname, GError **error);

G_END_DECLS

#endif
