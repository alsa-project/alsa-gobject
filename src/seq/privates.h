// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_PRIVATES_H__
#define __ALSA_GOBJECT_ALSASEQ_PRIVATES_H__

#include "alsaseq.h"

G_BEGIN_DECLS

void seq_system_info_refer_private(ALSASeqSystemInfo *self,
                                   struct snd_seq_system_info **info);

void seq_client_info_refer_private(ALSASeqClientInfo *self,
                                   struct snd_seq_client_info **info);

void seq_port_info_refer_private(ALSASeqPortInfo *self,
                                 struct snd_seq_port_info **info);

void seq_client_pool_refer_private(ALSASeqClientPool *self,
                                   struct snd_seq_client_pool **pool);

void seq_subscribe_data_refer_private(ALSASeqSubscribeData *self,
                                      struct snd_seq_port_subscribe **data);

void seq_queue_info_refer_private(ALSASeqQueueInfo *self,
                                  struct snd_seq_queue_info **info);

void seq_queue_status_refer_private(ALSASeqQueueStatus *self,
                                    struct snd_seq_queue_status **status);

void seq_queue_tempo_refer_private(ALSASeqQueueTempo *self,
                                   struct snd_seq_queue_tempo **tempo);

void seq_queue_timer_alsa_refer_private(ALSASeqQueueTimerAlsa *self,
                                        struct snd_seq_queue_timer **timer);

void seq_event_cntr_set_buf(ALSASeqEventCntr *self, guint8 *buf,
                              gsize length);

void seq_event_cntr_get_buf(ALSASeqEventCntr *self, gsize count,
                              const guint8 **buf, gsize *length);

void seq_event_copy_flattened(const ALSASeqEvent *self, guint8 *buf, gsize length);
gsize seq_event_calculate_flattened_length(const ALSASeqEvent *self, gboolean aligned);
gboolean seq_event_is_deliverable(const ALSASeqEvent *self);

#define QUEUE_ID_PROP_NAME          "queue-id"
#define TIMER_TYPE_PROP_NAME        "timer-type"

enum queue_timer_common_prop_type {
    QUEUE_TIMER_COMMON_PROP_QUEUE_ID = 1,
    QUEUE_TIMER_COMMON_PROP_TIMER_TYPE,
    QUEUE_TIMER_COMMON_PROP_COUNT,
};

void queue_timer_common_class_override_properties(GObjectClass *gobject_class);

void queue_timer_common_set_property(struct snd_seq_queue_timer *data, GObject *obj, guint id,
                                     const GValue *val, GParamSpec *spec);

void queue_timer_common_get_property(const struct snd_seq_queue_timer *data, GObject *obj, guint id,
                                     GValue *val, GParamSpec *spec);

G_END_DECLS

#endif
