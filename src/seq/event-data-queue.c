// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqEventDataQueue:
 * A boxed object to represent data of queue event.
 *
 * A [struct@EventDataQueue] is a boxed object to represent data of queue event. The instance of
 * object is one of data properties in event.
 *
 * The object wraps `struct snd_seq_ev_queue_control` in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataQueue *seq_event_data_queue_copy(const ALSASeqEventDataQueue *self)
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

G_DEFINE_BOXED_TYPE(ALSASeqEventDataQueue, alsaseq_event_data_queue, seq_event_data_queue_copy, g_free)

/**
 * alsaseq_event_data_queue_get_queue_id:
 * @self: A [struct@EventDataQueue].
 * @queue_id: (out): the numeric identifier of queue for the event.
 *
 * Get the numeric identifier of queue for the event.
 */
void alsaseq_event_data_queue_get_queue_id(const ALSASeqEventDataQueue *self,
                                           guint8 *queue_id)
{
    *queue_id = self->queue;
}

/**
 * alsaseq_event_data_queue_set_queue_id:
 * @self: A [struct@EventDataQueue].
 * @queue_id: The numeric identifier of queue for the event.
 *
 * Se the numeric identifier of queue for the event.
 */
void alsaseq_event_data_queue_set_queue_id(ALSASeqEventDataQueue *self,
                                           guint8 queue_id)
{
    self->queue = queue_id;
}

/**
 * alsaseq_event_data_queue_get_value_param:
 * @self: A [struct@EventDataQueue].
 * @value: (out): The value as param of the queue event.
 *
 * Get the value as param of the queue event.
 */
void alsaseq_event_data_queue_get_value_param(const ALSASeqEventDataQueue *self,
                                              gint *value)
{
    *value = self->param.value;
}

/**
 * alsaseq_event_data_queue_set_value_param:
 * @self: A [struct@EventDataQueue].
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
 * @self: A [struct@EventDataQueue].
 * @tstamp: (out)(transfer none): The timestamp as param of the queue event.
 *
 * Get the timestamp as param of the queue event.
 */
void alsaseq_event_data_queue_get_tstamp_param(const ALSASeqEventDataQueue *self,
                                               const ALSASeqTstamp **tstamp)
{
    *tstamp = &self->param.time;
}

/**
 * alsaseq_event_data_queue_set_tstamp_param:
 * @self: A [struct@EventDataQueue].
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
 * @self: A [struct@EventDataQueue].
 * @position: (out): The position as param of the queue event.
 *
 * Get the position as param of the queue event.
 */
void alsaseq_event_data_queue_get_position_param(const ALSASeqEventDataQueue *self,
                                                 guint *position)
{
    *position = self->param.position;
}

/**
 * alsaseq_event_data_queue_set_position_param:
 * @self: A [struct@EventDataQueue].
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
 * @self: A [struct@EventDataQueue].
 * @skew: (array fixed-size=2)(out)(transfer none): The array with two elements for numerator and
 *        denominator of fraction for skew.
 *
 * Refer to numerator and denominator of fraction for skew as the parameter of queue event.
 */
void alsaseq_event_data_queue_get_skew_param(const ALSASeqEventDataQueue *self,
                                             const guint *skew[2])
{
    // MEMO: I wish 32-bit storage size is aligned to 32 bit offset in all of
    // supported ABIs.
    *skew = (const guint *)&self->param.skew;
}

/**
 * alsaseq_event_data_queue_set_skew_param:
 * @self: A [struct@EventDataQueue].
 * @skew: (array fixed-size=2)(transfer none): The array with two elements for numerator and
 *        denominator of fraction for skew.
 *
 * Copy numerator and denominator of fraction for skew from the given buffer as the parameter of
 * queue event.
 */
void alsaseq_event_data_queue_set_skew_param(ALSASeqEventDataQueue *self,
                                             const guint skew[2])
{
    self->param.skew.value = skew[0];
    self->param.skew.base = skew[1];
}

/**
 * alsaseq_event_data_queue_get_quadlet_param:
 * @self: A [struct@EventDataQueue].
 * @quadlets: (array fixed-size=2)(out)(transfer none): The array with two elements for quadlets as
 *            the parameter of queue event.
 *
 * Refer to two quadlets as the parameter of queue event.
 */
void alsaseq_event_data_queue_get_quadlet_param(const ALSASeqEventDataQueue *self,
                                                const guint32 *quadlets[2])
{
    *quadlets = self->param.d32;
}

/**
 * alsaseq_event_data_queue_set_quadlet_param:
 * @self: A [struct@EventDataQueue].
 * @quadlets: (array fixed-size=2)(transfer none): The array with two elements for quadlets as the
 *            parameter of queue event.
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
 * @self: A [struct@EventDataQueue].
 * @bytes: (array fixed-size=8)(out)(transfer none): The array with eight elements for bytes
 *         parameter of the queue event.
 *
 * Refer to eight bytes as the parameter of queue event.
 */
void alsaseq_event_data_queue_get_byte_param(const ALSASeqEventDataQueue *self,
                                             const guint8 *bytes[8])
{
    *bytes = self->param.d8;
}

/**
 * alsaseq_event_data_queue_set_byte_param:
 * @self: A [struct@EventDataQueue].
 * @bytes: (array fixed-size=8)(transfer none): The array with eight elements for bytes parameter
 *         of the queue event.
 *
 * Copy eight bytes from the given buffer as the parameter of queue event.
 */
void alsaseq_event_data_queue_set_byte_param(ALSASeqEventDataQueue *self,
                                             const guint8 bytes[8])
{
    memcpy(self->param.d8, bytes, sizeof(self->param.d8));
}
