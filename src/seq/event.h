// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT      (alsaseq_event_get_type())

typedef struct snd_seq_event ALSASeqEvent;

GType alsaseq_event_get_type() G_GNUC_CONST;

#define ALSASEQ_EVENT_ERROR     (alsaseq_event_error_quark())

GQuark alsaseq_event_error_quark();

ALSASeqEvent *alsaseq_event_new(ALSASeqEventType event_type);

void alsaseq_event_get_event_type(const ALSASeqEvent *self, ALSASeqEventType *event_type);

void alsaseq_event_get_length_mode(const ALSASeqEvent *self, ALSASeqEventLengthMode *length_mode);
void alsaseq_event_get_tstamp_mode(const ALSASeqEvent *self, ALSASeqEventTstampMode *tstamp_mode);

void alsaseq_event_get_time_mode(const ALSASeqEvent *self, ALSASeqEventTimeMode *time_mode);
void alsaseq_event_set_time_mode(ALSASeqEvent *self, ALSASeqEventTimeMode time_mode);

void alsaseq_event_get_priority_mode(const ALSASeqEvent *self,
                                     ALSASeqEventPriorityMode *priority_mode);
void alsaseq_event_set_priority_mode(ALSASeqEvent *self, ALSASeqEventPriorityMode priority_mode);

void alsaseq_event_get_tag(const ALSASeqEvent *self, gint8 *tag);
void alsaseq_event_set_tag(ALSASeqEvent *self, gint8 tag);

void alsaseq_event_get_queue_id(const ALSASeqEvent *self, guint8 *queue_id);
void alsaseq_event_set_queue_id(ALSASeqEvent *self, guint8 queue_id);

void alsaseq_event_get_source(const ALSASeqEvent *self, const ALSASeqAddr **addr);
void alsaseq_event_set_source(ALSASeqEvent *self, const ALSASeqAddr *addr);

void alsaseq_event_get_destination(const ALSASeqEvent *self, const ALSASeqAddr **addr);
void alsaseq_event_set_destination(ALSASeqEvent *self, const ALSASeqAddr *addr);

gboolean alsaseq_event_get_tick_time(const ALSASeqEvent *self, guint *tick_time, GError **error);
gboolean alsaseq_event_set_tick_time(ALSASeqEvent *self, guint tick_time, GError **error);

gboolean alsaseq_event_get_real_time(const ALSASeqEvent *self, const guint32 *real_time[2],
                                     GError **error);
gboolean alsaseq_event_set_real_time(ALSASeqEvent *self, const guint32 real_time[2],
                                     GError **error);

gboolean alsaseq_event_get_note_data(const ALSASeqEvent *self, const ALSASeqEventDataNote **data,
                                     GError **error);
gboolean alsaseq_event_set_note_data(ALSASeqEvent *self, const ALSASeqEventDataNote *data,
                                     GError **error);

gboolean alsaseq_event_get_ctl_data(const ALSASeqEvent *self, const ALSASeqEventDataCtl **data,
                                    GError **error);
gboolean alsaseq_event_set_ctl_data(ALSASeqEvent *self, const ALSASeqEventDataCtl *data,
                                    GError **error);

gboolean alsaseq_event_get_byte_data(const ALSASeqEvent *self, const guint8 *data[12],
                                     GError **error);
gboolean alsaseq_event_set_byte_data(ALSASeqEvent *self, const guint8 data[12], GError **error);

gboolean alsaseq_event_get_quadlet_data(const ALSASeqEvent *self, const guint32 *data[3],
                                        GError **error);
gboolean alsaseq_event_set_quadlet_data(ALSASeqEvent *self, const guint32 data[3],
                                        GError **error);

gboolean alsaseq_event_get_blob_data(const ALSASeqEvent *self, const guint8 **data, gsize *length,
                                     GError **error);
gboolean alsaseq_event_set_blob_data(ALSASeqEvent *self, const guint8 *data, gsize length,
                                     GError **error);

gboolean alsaseq_event_get_pointer_data(const ALSASeqEvent *self, gconstpointer *data,
                                        gsize *length, GError **error);
gboolean alsaseq_event_set_pointer_data(ALSASeqEvent *self, const gpointer data, gsize length,
                                        GError **error);

gboolean alsaseq_event_get_queue_data(const ALSASeqEvent *self,
                                      const ALSASeqEventDataQueue **data, GError **error);
gboolean alsaseq_event_set_queue_data(ALSASeqEvent *self, const ALSASeqEventDataQueue *data,
                                      GError **error);

gboolean alsaseq_event_get_tick_time_data(const ALSASeqEvent *self, guint *tick_time,
                                          GError **error);
gboolean alsaseq_event_set_tick_time_data(ALSASeqEvent *self, guint tick_time, GError **error);

gboolean alsaseq_event_get_real_time_data(const ALSASeqEvent *self, const guint32 *real_time[2],
                                          GError **error);
gboolean alsaseq_event_set_real_time_data(ALSASeqEvent *self, const guint32 real_time[2],
                                          GError **error);

gboolean alsaseq_event_get_addr_data(const ALSASeqEvent *self, const ALSASeqAddr **data,
                                     GError **error);
gboolean alsaseq_event_set_addr_data(ALSASeqEvent *self, const ALSASeqAddr *data, GError **error);

gboolean alsaseq_event_get_connect_data(const ALSASeqEvent *self,
                                        const ALSASeqEventDataConnect **data, GError **error);
gboolean alsaseq_event_set_connect_data(ALSASeqEvent *self, const ALSASeqEventDataConnect *data,
                                     GError **error);

gboolean alsaseq_event_get_result_data(const ALSASeqEvent *self,
                                       const ALSASeqEventDataResult **data, GError **error);
gboolean alsaseq_event_set_result_data(ALSASeqEvent *self, const ALSASeqEventDataResult *data,
                                       GError **error);

void alsaseq_event_calculate_pool_consumption(const ALSASeqEvent *self, guint *cells);

G_END_DECLS

#endif
