// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUERY__H__
#define __ALSA_GOBJECT_ALSASEQ_QUERY__H__

#include <glib.h>
#include <glib-object.h>

#include <seq/system-info.h>
#include <seq/client-info.h>

G_BEGIN_DECLS

void alsaseq_get_seq_sysname(gchar **sysname, GError **error);

void alsaseq_get_seq_devnode(gchar **devnode, GError **error);

void alsaseq_get_system_info(ALSASeqSystemInfo **system_info, GError **error);

void alsaseq_get_client_id_list(guint **entries, gsize *entry_count,
                                GError **error);

void alsaseq_get_client_info(guint client_id, ALSASeqClientInfo **client_info,
			     GError **error);

G_END_DECLS

#endif
