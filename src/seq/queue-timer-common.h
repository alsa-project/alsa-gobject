// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_COMMON_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_COMMON_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_TIMER_COMMON     (alsaseq_queue_timer_common_get_type())

G_DECLARE_INTERFACE(ALSASeqQueueTimerCommon, alsaseq_queue_timer_common, ALSASEQ,
                    QUEUE_TIMER_COMMON, GObject)

struct _ALSASeqQueueTimerCommonInterface {
    GTypeInterface parent_iface;
};

G_END_DECLS

#endif
