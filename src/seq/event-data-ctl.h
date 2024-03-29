// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_CTL_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_CTL_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_DATA_CTL (alsaseq_event_data_ctl_get_type())

typedef struct snd_seq_ev_ctrl ALSASeqEventDataCtl;

GType alsaseq_event_data_ctl_get_type() G_GNUC_CONST;

void alsaseq_event_data_ctl_get_channel(const ALSASeqEventDataCtl *self,
                                        guint8 *channel);
void alsaseq_event_data_ctl_set_channel(ALSASeqEventDataCtl *self,
                                        guint8 channel);

void alsaseq_event_data_ctl_get_param(const ALSASeqEventDataCtl *self,
                                      guint *param);
void alsaseq_event_data_ctl_set_param(ALSASeqEventDataCtl *self, guint param);

void alsaseq_event_data_ctl_get_value(const ALSASeqEventDataCtl *self,
                                      gint *value);
void alsaseq_event_data_ctl_set_value(ALSASeqEventDataCtl *self, gint value);

G_END_DECLS

#endif
