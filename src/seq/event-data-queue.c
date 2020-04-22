// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-queue.h"

ALSASeqEventDataQueue *seq_event_data_queue_copy(const ALSASeqEventDataQueue *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataQueue, alsaseq_event_data_queue, seq_event_data_queue_copy, g_free)

/**
 * alsaseq_event_data_queue_get_queue_id:
 * @self: A #ALSASeqEventDataQueue.
 * @queue_id: (out): the numerical ID of queue for the event.
 *
 * Get the numerical ID of queue for the event.
 */
void alsaseq_event_data_queue_get_queue_id(ALSASeqEventDataQueue *self,
                                           guint8 *queue_id)
{
    *queue_id = self->queue;
}

/**
 * alsaseq_event_data_queue_set_queue_id:
 * @self: A #ALSASeqEventDataQueue.
 * @queue_id: The numerical ID of queue for the event.
 *
 * Se the numerical ID of queue for the event.
 */
void alsaseq_event_data_queue_set_queue_id(ALSASeqEventDataQueue *self,
                                           guint8 queue_id)
{
    self->queue = queue_id;
}

/**
 * alsaseq_event_data_queue_get_value_param:
 * @self: A #ALSASeqEventDataQueue.
 * @value: (out): The value as param of the queue event.
 *
 * Get the value as param of the queue event.
 */
void alsaseq_event_data_queue_get_value_param(ALSASeqEventDataQueue *self,
                                              gint *value)
{
    *value = self->param.value;
}

/**
 * alsaseq_event_data_queue_set_value_param:
 * @self: A #ALSASeqEventDataQueue.
 * @value: The value as param of the queue event.
 *
 * Set the value as param of the queue event.
 */
void alsaseq_event_data_queue_set_value_param(ALSASeqEventDataQueue *self,
                                              gint value)
{
    self->param.value = value;
}

/**
 * alsaseq_event_data_queue_get_tstamp_param:
 * @self: A #ALSASeqEventDataQueue.
 * @tstamp: (out)(transfer none): The timestamp as param of the queue event.
 *
 * Get the timestamp as param of the queue event.
 */
void alsaseq_event_data_queue_get_tstamp_param(ALSASeqEventDataQueue *self,
                                               const ALSASeqTstamp **tstamp)
{
    *tstamp = &self->param.time;
}

/**
 * alsaseq_event_data_queue_set_tstamp_param:
 * @self: A #ALSASeqEventDataQueue.
 * @tstamp: (transfer none): The timestamp as param of the queue event.
 *
 * Set the timestamp as param of the queue event.
 */
void alsaseq_event_data_queue_set_tstamp_param(ALSASeqEventDataQueue *self,
                                               const ALSASeqTstamp *tstamp)
{
    self->param.time = *tstamp;
}

/**
 * alsaseq_event_data_queue_get_position_param:
 * @self: A #ALSASeqEventDataQueue.
 * @position: (out): The position as param of the queue event.
 *
 * Get the position as param of the queue event.
 */
void alsaseq_event_data_queue_get_position_param(ALSASeqEventDataQueue *self,
                                                 guint *position)
{
    *position = self->param.position;
}

/**
 * alsaseq_event_data_queue_set_position_param:
 * @self: A #ALSASeqEventDataQueue.
 * @position: the position as param of the queue event.
 *
 * Set the position as param of the queue event.
 */
void alsaseq_event_data_queue_set_position_param(ALSASeqEventDataQueue *self,
                                                 guint position)
{
    self->param.position = position;
}

/**
 * alsaseq_event_data_queue_get_skew_param:
 * @self: A #ALSASeqEventDataQueue.
 * @skew: (array fixed-size=2)(out)(transfer none): The array with two elements
 *        for numerator and denominator of fraction for skew.
 *
 * Refer to numerator and denominator of fraction for skew as the parameter of
 * queue event.
 */
void alsaseq_event_data_queue_get_skew_param(ALSASeqEventDataQueue *self,
                                             const guint *skew[2])
{
    // MEMO: I wish 32-bit storage size is aligned to 32 bit offset in all of
    // supported ABIs.
    *skew = (const guint *)&self->param.skew;
}

/**
 * alsaseq_event_data_queue_set_skew_param:
 * @self: A #ALSASeqEventDataQueue.
 * @skew: (array fixed-size=2)(transfer none): The array with two elements for
 *        numerator and denominator of fraction for skew.
 *
 * Copy numerator and denominator of fraction for skew from the given buffer as
 * the parameter of queue event.
 */
void alsaseq_event_data_queue_set_skew_param(ALSASeqEventDataQueue *self,
                                             const guint skew[2])
{
    self->param.skew.value = skew[0];
    self->param.skew.base = skew[1];
}

/**
 * alsaseq_event_data_queue_get_quadlet_param:
 * @self: A #ALSASeqEventDataQueue.
 * @quadlets: (array fixed-size=2)(out)(transfer none): The array with two
 *            elements for quadlets as the parameter of queue event.
 *
 * Refer to two quadlets as the parameter of queue event.
 */
void alsaseq_event_data_queue_get_quadlet_param(ALSASeqEventDataQueue *self,
                                                const guint32 *quadlets[2])
{
    *quadlets = self->param.d32;
}

/**
 * alsaseq_event_data_queue_set_quadlet_param:
 * @self: A #ALSASeqEventDataQueue.
 * @quadlets: (array fixed-size=2)(transfer none): The array with two elements
 *            for quadlets as the parameter of queue event.
 *
 * Set two quadlets from the given buffer as the parameter of queue event.
 */
void alsaseq_event_data_queue_set_quadlet_param(ALSASeqEventDataQueue *self,
                                                const guint32 quadlets[2])
{
    memcpy(self->param.d32, quadlets, sizeof(self->param.d32));
}

/**
 * alsaseq_event_data_queue_get_byte_param:
 * @self: A #ALSASeqEventDataQueue.
 * @bytes: (array fixed-size=8)(out)(transfer none): The array with eight
 *         elements for bytes parameter of the queue event.
 *
 * Refer to eight bytes as the parameter of queue event.
 */
void alsaseq_event_data_queue_get_byte_param(ALSASeqEventDataQueue *self,
                                             const guint8 *bytes[8])
{
    *bytes = self->param.d8;
}

/**
 * alsaseq_event_data_queue_set_byte_param:
 * @self: A #ALSASeqEventDataQueue.
 * @bytes: (array fixed-size=8)(transfer none): The array with eight elements
 *         for bytes parameter of the queue event.
 *
 * Copy eight bytes from the given buffer as the parameter of queue event.
 */
void alsaseq_event_data_queue_set_byte_param(ALSASeqEventDataQueue *self,
                                             const guint8 bytes[8])
{
    memcpy(self->param.d8, bytes, sizeof(self->param.d8));
}
