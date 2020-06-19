// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUERY__H__
#define __ALSA_GOBJECT_ALSASEQ_QUERY__H__

#include <glib.h>
#include <glib-object.h>

#include <seq/system-info.h>
#include <seq/client-info.h>
#include <seq/port-info.h>
#include <seq/client-pool.h>
#include <seq/subscribe-data.h>
#include <seq/queue-info.h>
#include <seq/queue-status.h>

G_BEGIN_DECLS

void alsaseq_get_seq_sysname(gchar **sysname, GError **error);

void alsaseq_get_seq_devnode(gchar **devnode, GError **error);

void alsaseq_get_system_info(ALSASeqSystemInfo **system_info, GError **error);

void alsaseq_get_client_id_list(guint **entries, gsize *entry_count,
                                GError **error);

void alsaseq_get_client_info(guint8 client_id, ALSASeqClientInfo **client_info,
			     GError **error);

void alsaseq_get_port_id_list(guint8 client_id, guint8 **entries,
                              gsize *entry_count, GError **error);

void alsaseq_get_port_info(guint8 client_id, guint8 port_id,
                           ALSASeqPortInfo **port_info, GError **error);

void alsaseq_get_client_pool(guint8 client_id, ALSASeqClientPool **client_pool,
                             GError **error);

void alsaseq_get_subscription_list(const ALSASeqAddr *addr,
                                   ALSASeqQuerySubscribeType query_type,
                                   GList **entries, GError **error);

void alsaseq_get_queue_id_list(guint **entries, gsize *entry_count,
                               GError **error);

void alsaseq_get_queue_info_by_id(guint8 queue_id, ALSASeqQueueInfo **queue_info,
                                  GError **error);
void alsaseq_get_queue_info_by_name(const gchar *name,
                                    ALSASeqQueueInfo **queue_info,
                                    GError **error);

void alsaseq_get_queue_status(guint8 queue_id,
                              ALSASeqQueueStatus *const *queue_status,
                              GError **error);

G_END_DECLS

#endif
