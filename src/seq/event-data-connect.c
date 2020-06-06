// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-connect.h"

/**
 * SECTION: event-data-connect
 * @Title: ALSASeqEventDataConnect
 * @Short_description: A boxed object to represent data of connect event
 *
 * A #ALSASeqEventDataConnect is a boxed object to represent data of connect
 * event. The instance of object is one of data properties in
 * #ALSASeqEventFixed.
 *
 * The object wraps 'struct snd_seq_connect' in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataConnect *seq_event_data_connect_copy(const ALSASeqEventDataConnect *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataConnect, alsaseq_event_data_connect, seq_event_data_connect_copy, g_free)

/**
 * alsaseq_event_data_connect_get_src:
 * @self: A #ALSASeqEventDataConnect.
 * @src: (out)(transfer none): The source of connection event.
 *
 * Get the source of connection event.
 */
void alsaseq_event_data_connect_get_src(ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr **src)
{
    *src = &self->sender;
}

/**
 * alsaseq_event_data_connect_set_src:
 * @self: A #ALSASeqEventDataConnect.
 * @src: A #ALSASeqAddr.
 *
 * Set the source to the connection event.
 */
void alsaseq_event_data_connect_set_src(ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr *src)
{
    self->sender = *src;
}

/**
 * alsaseq_event_data_connect_get_dst:
 * @self: A #ALSASeqEventDataConnect.
 * @dst: (out)(transfer none): The destination of connection event.
 *
 * Get the destination of connection event.
 */
void alsaseq_event_data_connect_get_dst(ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr **dst)
{
    *dst = &self->dest;
}

/**
 * alsaseq_event_data_connect_set_dst:
 * @self: A #ALSASeqEventDataConnect.
 * @dst: A #ALSASeqAddr.
 *
 * Set the source to the connection event.
 */
void alsaseq_event_data_connect_set_dst(ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr *dst)
{
    self->dest = *dst;
}
