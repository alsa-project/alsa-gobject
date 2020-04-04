// SPDX-License-Identifier: LGPL-3.0-or-later
#include "addr.h"

ALSASeqAddr *seq_addr_copy(const ALSASeqAddr *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqAddr, alsaseq_addr, seq_addr_copy, g_free)

/**
 * alsaseq_addr_new:
 * @client_id: The numerical ID of client to address.
 * @port_id: The numerical ID of port to address.
 *
 * Allocate and return an instance of ALSASeqAddr.
 *
 * Returns: A #ALSASeqAddr.
 */
ALSASeqAddr *alsaseq_addr_new(guint8 client_id, guint8 port_id)
{
    ALSASeqAddr *self = g_malloc0(sizeof(*self));

    self->client = client_id;
    self->port = port_id;

    return self;
}

/**
 * alsaseq_addr_get_client_id:
 * @self: A #ALSASeqAddr.
 * @client_id: (out): The numerical ID of client to address.
 *
 * Get the numerical ID of client to address.
 */
void alsaseq_addr_get_client_id(const ALSASeqAddr *self, guint8 *client_id)
{
    *client_id = self->client;
}

/**
 * alsaseq_addr_get_port_id:
 * @self: A #ALSASeqAddr.
 * @port_id: (out): The numerical ID of port to address.
 *
 * Get the numerical ID of port to address.
 */
void alsaseq_addr_get_port_id(const ALSASeqAddr *self, guint8 *port_id)
{
    *port_id = self->port;
}
