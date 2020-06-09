// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_DATA_ALSA__H__
#define __ALSA_GOBJECT_ALSASEQ_QUEUE_TIMER_DATA_ALSA__H__

#include <glib.h>
#include <glib-object.h>

#include <timer/device-id.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_QUEUE_TIMER_DATA_ALSA  (alsaseq_queue_time_data_alsa_get_type())

// This structure should be the same layout as 'struct snd_seq_queue_timer.u.alsa'
typedef struct {
    /*< private >*/
    ALSATimerDeviceId device_id;
    unsigned int resolution;
} ALSASeqQueueTimerDataAlsa;

GType alsaseq_queue_timer_data_alsa_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
