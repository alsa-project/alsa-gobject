// SPDX-License-Identifier: LGPL-3.0-or-later
#include "addr.h"

/**
 * SECTION: addr
 * @Title: ALSASeqAddr
 * @Short_description: A boxed object to represent address in ALSA Sequencer.
 *
 * A #ALSASeqAddr is a boxed object to represent address in ALSA Sequencer. The
 * address consists of two parts; the numerical ID of client and port.
 *
 * The object wraps 'struct snd_seq_addr' in UAPI of Linux sound subsystem.
 */
ALSASeqAddr *seq_addr_copy(const ALSASeqAddr *self)
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

/**
 * alsaseq_addr_equal:
 * @self: A #ALSASeqAddr.
 * @target: A #ALSASeqAddr to compare.
 *
 * Returns: whether the given object indicates the same element.
 */
gboolean alsaseq_addr_equal(const ALSASeqAddr *self, const ALSASeqAddr *target)
{
    const struct snd_seq_addr *lhs, *rhs;

    lhs = (const struct snd_seq_addr *)self;
    rhs = (const struct snd_seq_addr *)target;

    return lhs->client == rhs->client && lhs->port == rhs->port;
}
