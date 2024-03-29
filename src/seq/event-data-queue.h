// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_QUEUE_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_QUEUE_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_DATA_QUEUE (alsaseq_event_data_queue_get_type())

typedef struct snd_seq_ev_queue_control ALSASeqEventDataQueue;

GType alsaseq_event_data_queue_get_type() G_GNUC_CONST;

void alsaseq_event_data_queue_get_queue_id(const ALSASeqEventDataQueue *self,
                                           guint8 *queue_id);
void alsaseq_event_data_queue_set_queue_id(ALSASeqEventDataQueue *self,
                                           guint8 queue_id);

void alsaseq_event_data_queue_get_value_param(const ALSASeqEventDataQueue *self,
                                              gint *value);
void alsaseq_event_data_queue_set_value_param(ALSASeqEventDataQueue *self,
                                              gint value);

void alsaseq_event_data_queue_get_tick_time_param(const ALSASeqEventDataQueue *self,
                                                  guint *tick_time);
void alsaseq_event_data_queue_set_tick_time_param(ALSASeqEventDataQueue *self,
                                                  const guint tick_time);

void alsaseq_event_data_queue_get_real_time_param(const ALSASeqEventDataQueue *self,
                                                  const guint32 *real_time[2]);
void alsaseq_event_data_queue_set_real_time_param(ALSASeqEventDataQueue *self,
                                                  const guint32 real_time[2]);

void alsaseq_event_data_queue_get_position_param(const ALSASeqEventDataQueue *self,
                                                 guint *position);
void alsaseq_event_data_queue_set_position_param(ALSASeqEventDataQueue *self,
                                                 guint position);

void alsaseq_event_data_queue_get_skew_param(const ALSASeqEventDataQueue *self,
                                             const guint *skew[2]);
void alsaseq_event_data_queue_set_skew_param(ALSASeqEventDataQueue *self,
                                             const guint skew[2]);

void alsaseq_event_data_queue_get_quadlet_param(const ALSASeqEventDataQueue *self,
                                                const guint32 *quadlets[2]);
void alsaseq_event_data_queue_set_quadlet_param(ALSASeqEventDataQueue *self,
                                                const guint32 quadlets[2]);

void alsaseq_event_data_queue_get_byte_param(const ALSASeqEventDataQueue *self,
                                             const guint8 *bytes[8]);
void alsaseq_event_data_queue_set_byte_param(ALSASeqEventDataQueue *self,
                                             const guint8 bytes[8]);

G_END_DECLS

#endif
