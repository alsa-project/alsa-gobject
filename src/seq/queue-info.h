// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_INFO_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_INFO_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_INFO     (alsaseq_queue_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqQueueInfo, alsaseq_queue_info, ALSASEQ, QUEUE_INFO, GObject);
struct _ALSASeqQueueInfoClass {
    GObjectClass parent_class;
};

ALSASeqQueueInfo *alsaseq_queue_info_new();

G_END_DECLS

#endif
