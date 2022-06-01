// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_ALSA_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_ALSA_H__

#include <alsaseq.h>
#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_TIMER_ALSA   (alsaseq_queue_timer_alsa_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqQueueTimerAlsa, alsaseq_queue_timer_alsa, ALSASEQ, QUEUE_TIMER_ALSA, GObject)

struct _ALSASeqQueueTimerAlsaClass {
    GObjectClass parent_class;
};

ALSASeqQueueTimerAlsa *alsaseq_queue_timer_alsa_new();

G_END_DECLS

#endif
