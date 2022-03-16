// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER__H__
#define __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER__H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_REMOVE_FILTER     (alsaseq_remove_filter_get_type())

typedef struct snd_seq_remove_events    ALSASeqRemoveFilter;

GType alsaseq_remove_filter_get_type() G_GNUC_CONST;

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_dest_addr(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqAddr *dest, GError **error);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note_channel(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                guint8 channel, GError **error);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_event_type(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqEventType ev_type, GError **error);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                GError **error);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tag(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint8 tag, GError **error);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tick_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tick_time, gboolean after,
                                GError **error);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_real_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tv_sec, guint32 tv_nsec, gboolean after,
                                GError **error);

G_END_DECLS

#endif
