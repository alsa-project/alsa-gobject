// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_ADDR__H__
#define __ALSA_GOBJECT_ALSASEQ_ADDR__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asequencer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_ADDR   (alsaseq_addr_get_type())

typedef struct snd_seq_addr ALSASeqAddr;

GType alsaseq_addr_get_type() G_GNUC_CONST;

ALSASeqAddr *alsaseq_addr_new(guint8 client_id, guint8 port_id);

void alsaseq_addr_get_client_id(const ALSASeqAddr *self, guint8 *client_id);

void alsaseq_addr_get_port_id(const ALSASeqAddr *self, guint8 *port_id);

gboolean alsaseq_addr_equal(const ALSASeqAddr *self, const ALSASeqAddr *target);

G_END_DECLS

#endif
