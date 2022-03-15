// SPDX-License-Identifier: LGPL-3.0-or-later
#include "queue-timer-data-alsa.h"

/**
 * SECTION: queue-timer-data-alsa
 * @Title: ALSASeqQueueTimerDataAlsa
 * @Short_description: A boxed object to represent data of queue timer in the
 *                     case of ALSATimer
 *
 * A #ALSASeqQueueTimerDataAlsa is a boxed object to represent data of queue
 * timer in the case of ALSATimer. The instance of object is one of data
 * properties in queue timer.
 */
ALSASeqQueueTimerDataAlsa *seq_queue_timer_data_alsa_copy(const ALSASeqQueueTimerDataAlsa *self)
{
#ifdef g_memdup2
    return g_memdup2(self, sizeof(*self));
#else
    // GLib v2.68 deprecated g_memdup() with concern about overflow by narrow conversion from size_t to
    // unsigned int however it's safe in the local case.
    gpointer ptr = g_malloc(sizeof(*self));
    memcpy(ptr, self, sizeof(*self));
    return ptr;
#endif
}

G_DEFINE_BOXED_TYPE(ALSASeqQueueTimerDataAlsa, alsaseq_queue_timer_data_alsa, seq_queue_timer_data_alsa_copy, g_free)

/**
 * alsaseq_queue_timer_data_alsa_get_device_id:
 * @self: A #ALSASeqQueueTimerDataAlsa.
 * @device_id: (out)(transfer none): A #ALSATimerDeviceId.
 *
 * Refer to the device ID of timer which drives the queue.
 */
void alsaseq_queue_timer_data_alsa_get_device_id(const ALSASeqQueueTimerDataAlsa *self,
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
void alsaseq_queue_timer_data_alsa_get_resolution(const ALSASeqQueueTimerDataAlsa *self,
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
