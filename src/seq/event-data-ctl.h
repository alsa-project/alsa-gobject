// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_CTL__H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_CTL__H__

#include <glib.h>
#include <glib-object.h>

#include <seq/alsaseq-enums.h>

#include <sound/asequencer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_DATA_CTL (alsaseq_event_data_ctl_get_type())

typedef struct snd_seq_ev_ctrl ALSASeqEventDataCtl;

GType alsaseq_event_data_ctl_get_type() G_GNUC_CONST;

guint8 alsaseq_event_data_ctl_get_channel(ALSASeqEventDataCtl *self);
void alsaseq_event_data_ctl_set_channel(ALSASeqEventDataCtl *self,
                                        guint8 channel);

guint alsaseq_event_data_ctl_get_param(ALSASeqEventDataCtl *self);
void alsaseq_event_data_ctl_set_param(ALSASeqEventDataCtl *self, guint param);

gint alsaseq_event_data_ctl_get_value(ALSASeqEventDataCtl *self);
void alsaseq_event_data_ctl_set_value(ALSASeqEventDataCtl *self, gint value);

G_END_DECLS

#endif
