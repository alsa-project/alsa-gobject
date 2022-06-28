// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER_H__
#define __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_REMOVE_FILTER     (alsaseq_remove_filter_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqRemoveFilter, alsaseq_remove_filter, ALSASEQ, REMOVE_FILTER, GObject);

struct _ALSASeqRemoveFilterClass {
    GObjectClass parent_class;
};

ALSASeqRemoveFilter *alsaseq_remove_filter_new(void);

void alsaseq_remove_filter_get_tick_time(ALSASeqRemoveFilter *self, guint *tick_time);
void alsaseq_remove_filter_set_tick_time(ALSASeqRemoveFilter *self, guint tick_time);

void alsaseq_remove_filter_get_real_time(ALSASeqRemoveFilter *self, const guint32 *real_time[2]);
void alsaseq_remove_filter_set_real_time(ALSASeqRemoveFilter *self, const guint32 real_time[2]);

G_END_DECLS

#endif
