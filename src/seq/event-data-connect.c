// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-connect.h"

ALSASeqEventDataConnect *seq_event_data_connect_copy(const ALSASeqEventDataConnect *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataConnect, alsaseq_event_data_connect, seq_event_data_connect_copy, g_free)

/**
 * alsaseq_event_data_connect_get_src:
 * @self: A #ALSASeqEventDataConnect.
 *
 * Get the source of connection event.
 *
 * Returns: (transfer none): The source of connection event.
 */
const ALSASeqAddr *alsaseq_event_data_connect_get_src(ALSASeqEventDataConnect *self)
{
    return &self->sender;
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
 *
 * Get the source of connection event.
 *
 * Returns: (transfer none): The source of connection event.
 */
const ALSASeqAddr *alsaseq_event_data_connect_get_dst(ALSASeqEventDataConnect *self)
{
    return &self->dest;
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
