// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_STATUS_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_STATUS_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_STATUS       (alsaseq_queue_status_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqQueueStatus, alsaseq_queue_status, ALSASEQ, QUEUE_STATUS, GObject);

struct _ALSASeqQueueStatusClass {
    GObjectClass parent_class;
};

ALSASeqQueueStatus *alsaseq_queue_status_new();

void alsaseq_queue_status_get_tick_time(ALSASeqQueueStatus *self,
                                        guint *tick_time);

void alsaseq_queue_status_get_real_time(ALSASeqQueueStatus *self,
                                        const guint32 *real_time[2]);

G_END_DECLS

#endif
