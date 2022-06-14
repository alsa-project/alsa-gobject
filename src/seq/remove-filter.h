// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER_H__
#define __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_REMOVE_FILTER     (alsaseq_remove_filter_get_type())

typedef struct snd_seq_remove_events    ALSASeqRemoveFilter;

GType alsaseq_remove_filter_get_type() G_GNUC_CONST;

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_dest_addr(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqAddr *dest);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note_channel(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                guint8 channel);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_event_type(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqEventType ev_type);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tag(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint8 tag);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tick_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tick_time, gboolean after);

ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_real_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tv_sec, guint32 tv_nsec, gboolean after);

G_END_DECLS

#endif
