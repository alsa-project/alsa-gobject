// SPDX-License-Identifier: LGPL-3.0-or-later
#include "queue-timer-data-alsa.h"

ALSASeqQueueTimerDataAlsa *seq_queue_timer_data_alsa_copy(const ALSASeqQueueTimerDataAlsa *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqQueueTimerDataAlsa, alsaseq_queue_timer_data_alsa, seq_queue_timer_data_alsa_copy, g_free)

/**
 * alsaseq_queue_timer_data_alsa_get_device_id:
 * @self: A #ALSASeqQueueTimerDataAlsa.
 * @device_id: (out)(transfer none): A #ALSATimerDeviceId.
 *
 * Refer to the device ID of timer which drives the queue.
 */
void alsaseq_queue_timer_data_alsa_get_device_id(ALSASeqQueueTimerDataAlsa *self,
                                        const ALSATimerDeviceId **device_id)
{
    *device_id = (ALSATimerDeviceId *)&self->device_id;
}

/**
 * alsaseq_queue_timer_data_alsa_set_device_id:
 * @self: A #ALSASeqQueueTimerDataAlsa.
 * @device_id: A #ALSATimerDeviceId.
 *
 * Copy the device ID of timer which drives the queue.
 */
void alsaseq_queue_timer_data_alsa_set_device_id(ALSASeqQueueTimerDataAlsa *self,
                                        const ALSATimerDeviceId *device_id)
{
    self->device_id = *(struct snd_timer_id *)device_id;
}

/**
 * alsaseq_queue_timer_data_alsa_get_resolution:
 * @self: A #ALSASeqQueueTimerDataAlsa.
 * @resolution: (out): The resolution of timer.
 *
 * Get the resolution of timer which drives the queue.
 */
void alsaseq_queue_timer_data_alsa_get_resolution(ALSASeqQueueTimerDataAlsa *self,
                                                  guint *resolution)
{
    *resolution = self->resolution;
}

/**
 * alsaseq_queue_timer_data_alsa_set_resolution:
 * @self: A #ALSASeqQueueTimerDataAlsa.
 * @resolution: The resolution of timer.
 *
 * Set the resolution of timer which drives the queue.
 */
void alsaseq_queue_timer_data_alsa_set_resolution(ALSASeqQueueTimerDataAlsa *self,
                                                  guint resolution)
{
    self->resolution = resolution;
}
