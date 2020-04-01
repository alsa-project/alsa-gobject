// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_QUEUE_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_QUEUE_H__

#include <glib.h>
#include <glib-object.h>

#include <seq/tstamp.h>

#include <sound/asequencer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_DATA_QUEUE (alsaseq_event_data_queue_get_type())

typedef struct snd_seq_ev_queue_control ALSASeqEventDataQueue;

GType alsaseq_event_data_queue_get_type() G_GNUC_CONST;

guint8 alsaseq_event_data_queue_get_queue_id(ALSASeqEventDataQueue *self);
void alsaseq_event_data_queue_set_queue_id(ALSASeqEventDataQueue *self,
                                           guint8 queue_id);

gint alsaseq_event_data_queue_get_value_param(ALSASeqEventDataQueue *self);
void alsaseq_event_data_queue_set_value_param(ALSASeqEventDataQueue *self,
                                              gint value);

const ALSASeqTstamp *alsaseq_event_data_queue_get_tstamp_param(ALSASeqEventDataQueue *self);
void alsaseq_event_data_queue_set_tstamp_param(ALSASeqEventDataQueue *self,
                                               const ALSASeqTstamp *tstamp);

guint alsaseq_event_data_queue_get_position_param(ALSASeqEventDataQueue *self);
void alsaseq_event_data_queue_set_position_param(ALSASeqEventDataQueue *self,
                                                 guint position);

const guint *alsaseq_event_data_queue_get_skew_param(ALSASeqEventDataQueue *self);
void alsaseq_event_data_queue_set_skew_param(ALSASeqEventDataQueue *self,
                                             const guint skew[2]);

const guint32 *alsaseq_event_data_queue_get_quadlet_param(ALSASeqEventDataQueue *self);
void alsaseq_event_data_queue_set_quadlet_param(ALSASeqEventDataQueue *self,
                                                const guint32 quadlets[2]);

const guint8 *alsaseq_event_data_queue_get_byte_param(ALSASeqEventDataQueue *self);
void alsaseq_event_data_queue_set_byte_param(ALSASeqEventDataQueue *self,
                                             const guint32 bytes[8]);

G_END_DECLS

#endif
