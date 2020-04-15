// SPDX-License-Identifier: LGPL-3.0-or-later
#include "tstamp.h"

ALSASeqTstamp *seq_tstamp_copy(const ALSASeqTstamp *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqTstamp, alsaseq_tstamp, seq_tstamp_copy, g_free)

/**
 * alsaseq_tstamp_get_tick_time:
 * @self: A #ALSASeqTstamp.
 * @tick_time: (out): The number of MIDI ticks.
 *
 * Get time as MIDI ticks.
 */
void alsaseq_tstamp_get_tick_time(ALSASeqTstamp *self, guint32 *tick_time)
{
    *tick_time = self->tstamp.tick;
}

/**
 * alsaseq_tstamp_set_tick_time:
 * @self: A #ALSASeqTstamp.
 * @tick_time: The number of MIDI ticks.
 *
 * Set time as MIDI ticks.
 */
void alsaseq_tstamp_set_tick_time(ALSASeqTstamp *self, const guint32 tick_time)
{
    self->tstamp.tick = tick_time;
}

/**
 * alsaseq_tstamp_get_real_time:
 * @self: A #ALSASeqTstamp.
 * @real_time: (array fixed-size=2)(out)(transfer none): The array with two
 *             elements for sec part and nsec part of real time.
 *
 * Refer to the time as wall-clock time.
 */
void alsaseq_tstamp_get_real_time(ALSASeqTstamp *self, const guint32 *real_time[2])
{
    // MEMO: I wish 32-bit storage size is aligned to 32 bit offset in all of
    // supported ABIs.
    *real_time = (guint32 *)&self->tstamp.time;
}


/**
 * alsaseq_tstamp_set_real_time:
 * @self: A #ALSASeqTstamp.
 * @real_time: (array fixed-size=2)(transfer none): The array with two elements
 *             for sec part and nsec part of real time.
 *
 * Copy the time as wall-clock time.
 */
void alsaseq_tstamp_set_real_time(ALSASeqTstamp *self, const guint32 real_time[2])
{
    self->tstamp.time.tv_sec = real_time[0];
    self->tstamp.time.tv_nsec = real_time[1];
}
