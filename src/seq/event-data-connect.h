// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_CONNECT_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_CONNECT_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_DATA_CONNECT (alsaseq_event_data_connect_get_type())

typedef struct snd_seq_connect ALSASeqEventDataConnect;

GType alsaseq_event_data_connect_get_type() G_GNUC_CONST;

void alsaseq_event_data_connect_get_src(const ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr **src);
void alsaseq_event_data_connect_set_src(ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr *src);

void alsaseq_event_data_connect_get_dst(const ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr **dst);
void alsaseq_event_data_connect_set_dst(ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr *dst);

G_END_DECLS

#endif
