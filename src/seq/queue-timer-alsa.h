// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_ALSA__H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_ALSA__H__

#include <glib.h>
#include <glib-object.h>

#include <seq/queue-timer.h>

G_BEGIN_DECLS


#define ALSASEQ_TYPE_QUEUE_TIMER_ALSA     (alsaseq_queue_timer_alsa_get_type())

#define ALSASEQ_QUEUE_TIMER_ALSA(obj)                           \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSASEQ_TYPE_QUEUE_TIMER_ALSA,  \
                                ALSASeqQueueTimerAlsa))
#define ALSASEQ_IS_QUEUE_TIMER_ALSA(obj)                        \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSASEQ_TYPE_QUEUE_TIMER_ALSA))

#define ALSASEQ_QUEUE_TIMER_ALSA_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSASEQ_TYPE_QUEUE_TIMER_ALSA,     \
                             ALSASeqQueueTimerAlsaClass))
#define ALSASEQ_IS_QUEUE_TIMER_ALSA_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSASEQ_TYPE_QUEUE_TIMER_ALSA))
#define ALSASEQ_QUEUE_TIMER_ALSA_GET_CLASS(obj)                 \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSASEQ_TYPE_QUEUE_TIMER_ALSA,   \
                               ALSASeqQueueTimerAlsaClass))

typedef struct _ALSASeqQueueTimerAlsa           ALSASeqQueueTimerAlsa;
typedef struct _ALSASeqQueueTimerAlsaClass      ALSASeqQueueTimerAlsaClass;

struct _ALSASeqQueueTimerAlsa {
    ALSASeqQueueTimer parent_instance;
};

struct _ALSASeqQueueTimerAlsaClass {
    ALSASeqQueueTimerClass parent_class;
};

GType alsaseq_queue_timer_alsa_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
