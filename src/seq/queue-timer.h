// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_TIMER     (alsaseq_queue_timer_get_type())

#define ALSASEQ_QUEUE_TIMER(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_QUEUE_TIMER,   \
                                ALSASeqQueueTimer))
#define ALSASEQ_IS_QUEUE_TIMER(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_QUEUE_TIMER))

#define ALSASEQ_QUEUE_TIMER_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_QUEUE_TIMER,      \
                             ALSASeqQueueTimerClass))
#define ALSASEQ_IS_QUEUE_TIMER_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_QUEUE_TIMER))
#define ALSASEQ_QUEUE_TIMER_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_QUEUE_TIMER,    \
                               ALSASeqQueueTimerClass))

typedef struct _ALSASeqQueueTimer           ALSASeqQueueTimer;
typedef struct _ALSASeqQueueTimerClass      ALSASeqQueueTimerClass;
typedef struct _ALSASeqQueueTimerPrivate    ALSASeqQueueTimerPrivate;

struct _ALSASeqQueueTimer {
    GObject parent_instance;

    ALSASeqQueueTimerPrivate *priv;
};

struct _ALSASeqQueueTimerClass {
    GObjectClass parent_class;
};

GType alsaseq_queue_timer_get_type() G_GNUC_CONST;

ALSASeqQueueTimer *alsaseq_queue_timer_new();

void alsaseq_queue_timer_get_alsa_data(ALSASeqQueueTimer *self,
                                       const ALSASeqQueueTimerDataAlsa **data);
void alsaseq_queue_timer_set_alsa_data(ALSASeqQueueTimer *self,
                                       const ALSASeqQueueTimerDataAlsa *data);

G_END_DECLS

#endif
