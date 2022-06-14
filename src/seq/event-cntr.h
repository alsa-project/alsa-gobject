// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_CNTR     (alsaseq_event_cntr_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqEventCntr, alsaseq_event_cntr, ALSASEQ, EVENT_CNTR, GObject);

struct _ALSASeqEventCntrClass {
    GObjectClass parent_class;
};

ALSASeqEventCntr *alsaseq_event_cntr_new(guint count, GError **error);

void alsaseq_event_cntr_count_events(ALSASeqEventCntr *self, gsize *count);

gboolean alsaseq_event_cntr_calculate_pool_consumption(ALSASeqEventCntr *self, gsize count,
                                                       gsize *cells, GError **error);

gboolean alsaseq_event_cntr_get_event_type(ALSASeqEventCntr *self, gsize index,
                                           ALSASeqEventType *ev_type, GError **error);
gboolean alsaseq_event_cntr_set_event_type(ALSASeqEventCntr *self, gsize index,
                                           ALSASeqEventType ev_type, GError **error);

gboolean alsaseq_event_cntr_get_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                            ALSASeqEventTstampMode *mode, GError **error);
gboolean alsaseq_event_cntr_set_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                            ALSASeqEventTstampMode mode, GError **error);

gboolean alsaseq_event_cntr_get_time_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimeMode *mode, GError **error);
gboolean alsaseq_event_cntr_set_time_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimeMode mode, GError **error);

gboolean alsaseq_event_cntr_get_length_mode(ALSASeqEventCntr *self, gsize index,
                                            ALSASeqEventLengthMode *mode, GError **error);

gboolean alsaseq_event_cntr_get_priority_mode(ALSASeqEventCntr *self, gsize index,
                                              ALSASeqEventPriorityMode *mode, GError **error);
gboolean alsaseq_event_cntr_set_priority_mode(ALSASeqEventCntr *self, gsize index,
                                              ALSASeqEventPriorityMode mode, GError **error);

gboolean alsaseq_event_cntr_get_tag(ALSASeqEventCntr *self, gsize index, gint8 *tag,
                                    GError **error);
gboolean alsaseq_event_cntr_set_tag(ALSASeqEventCntr *self, gsize index, gint8 tag, GError **error);

gboolean alsaseq_event_cntr_get_queue_id(ALSASeqEventCntr *self, gsize index, guint8 *queue_id,
                                         GError **error);
gboolean alsaseq_event_cntr_set_queue_id(ALSASeqEventCntr *self, gsize index, guint8 queue_id,
                                         GError **error);

gboolean alsaseq_event_cntr_get_tick_time(ALSASeqEventCntr *self, gsize index,
                                          guint *tick_time, GError **error);
gboolean alsaseq_event_cntr_set_tick_time(ALSASeqEventCntr *self, gsize index,
                                          const guint tick_time, GError **error);

gboolean alsaseq_event_cntr_get_real_time(ALSASeqEventCntr *self, gsize index,
                                          const guint32 *real_time[2], GError **error);
gboolean alsaseq_event_cntr_set_real_time(ALSASeqEventCntr *self, gsize index,
                                          const guint32 real_time[2], GError **error);

gboolean alsaseq_event_cntr_get_dst(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr **dst,
                                    GError **error);
gboolean alsaseq_event_cntr_set_dst(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr *dst,
                                    GError **error);

gboolean alsaseq_event_cntr_get_src(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr **src,
                                    GError **error);
gboolean alsaseq_event_cntr_set_src(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr *src,
                                    GError **error);

gboolean alsaseq_event_cntr_get_note_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqEventDataNote **data, GError **error);
gboolean alsaseq_event_cntr_set_note_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqEventDataNote *data, GError **error);

gboolean alsaseq_event_cntr_get_ctl_data(ALSASeqEventCntr *self, gsize index,
                                         const ALSASeqEventDataCtl **data, GError **error);
gboolean alsaseq_event_cntr_set_ctl_data(ALSASeqEventCntr *self, gsize index,
                                         const ALSASeqEventDataCtl *data, GError **error);

gboolean alsaseq_event_cntr_get_byte_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 *data[12], GError **error);
gboolean alsaseq_event_cntr_set_byte_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 data[12], GError **error);

gboolean alsaseq_event_cntr_get_quadlet_data(ALSASeqEventCntr *self, gsize index,
                                             const guint32 *data[3], GError **error);
gboolean alsaseq_event_cntr_set_quadlet_data(ALSASeqEventCntr *self, gsize index,
                                             const guint32 data[3], GError **error);

gboolean alsaseq_event_cntr_get_blob_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 **data, gsize *size, GError **error);
gboolean alsaseq_event_cntr_set_blob_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 *data, gsize size, GError **error);

gboolean alsaseq_event_cntr_get_queue_data(ALSASeqEventCntr *self, gsize index,
                                           const ALSASeqEventDataQueue **data, GError **error);
gboolean alsaseq_event_cntr_set_queue_data(ALSASeqEventCntr *self, gsize index,
                                           const ALSASeqEventDataQueue *data, GError **error);

gboolean alsaseq_event_cntr_get_tick_time_data(ALSASeqEventCntr *self, gsize index,
                                               guint *tick_time, GError **error);
gboolean alsaseq_event_cntr_set_tick_time_data(ALSASeqEventCntr *self, gsize index,
                                               const guint tick_time, GError **error);

gboolean alsaseq_event_cntr_get_real_time_data(ALSASeqEventCntr *self, gsize index,
                                               const guint *real_time[2], GError **error);
gboolean alsaseq_event_cntr_set_real_time_data(ALSASeqEventCntr *self, gsize index,
                                               const guint real_time[2], GError **error);

gboolean alsaseq_event_cntr_get_addr_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqAddr **data, GError **error);
gboolean alsaseq_event_cntr_set_addr_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqAddr *data, GError **error);

gboolean alsaseq_event_cntr_get_connect_data(ALSASeqEventCntr *self, gsize index,
                                             const ALSASeqEventDataConnect **data, GError **error);
gboolean alsaseq_event_cntr_set_connect_data(ALSASeqEventCntr *self, gsize index,
                                             const ALSASeqEventDataConnect *data, GError **error);

gboolean alsaseq_event_cntr_get_result_data(ALSASeqEventCntr *self, gsize index,
                                            const ALSASeqEventDataResult **data, GError **error);
gboolean alsaseq_event_cntr_set_result_data(ALSASeqEventCntr *self, gsize index,
                                            const ALSASeqEventDataResult *data, GError **error);

G_END_DECLS

#endif
