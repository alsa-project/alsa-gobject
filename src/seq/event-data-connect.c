// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqEventDataConnect:
 * A boxed object to represent data of connect event.
 *
 * A [struct@EventDataConnect] is a boxed object to represent data of connect event. The instance
 * of object is one of data properties in event.
 *
 * The object wraps `struct snd_seq_connect` in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataConnect *seq_event_data_connect_copy(const ALSASeqEventDataConnect *self)
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

G_DEFINE_BOXED_TYPE(ALSASeqEventDataConnect, alsaseq_event_data_connect, seq_event_data_connect_copy, g_free)

/**
 * alsaseq_event_data_connect_get_src:
 * @self: A [struct@EventDataConnect].
 * @src: (out)(transfer none): The source of connection event.
 *
 * Get the source of connection event.
 */
void alsaseq_event_data_connect_get_src(const ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr **src)
{
    *src = &self->sender;
}

/**
 * alsaseq_event_data_connect_set_src:
 * @self: A [struct@EventDataConnect].
 * @src: A [struct@Addr].
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
 * @self: A [struct@EventDataConnect].
 * @dst: (out)(transfer none): The destination of connection event.
 *
 * Get the destination of connection event.
 */
void alsaseq_event_data_connect_get_dst(const ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr **dst)
{
    *dst = &self->dest;
}

/**
 * alsaseq_event_data_connect_set_dst:
 * @self: A [struct@EventDataConnect].
 * @dst: A [struct@Addr].
 *
 * Set the source to the connection event.
 */
void alsaseq_event_data_connect_set_dst(ALSASeqEventDataConnect *self,
                                        const ALSASeqAddr *dst)
{
    self->dest = *dst;
}
