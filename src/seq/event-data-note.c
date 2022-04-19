// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqEventDataNote:
 * A boxed object to represent data of note event.
 *
 * A [struct@EventDataNote] is a boxed object to represent data of note event. The instance of
 * object is one of data properties in event.
 *
 * The object wraps `struct snd_seq_ev_note` in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataNote *seq_event_data_note_copy(const ALSASeqEventDataNote *self)
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

G_DEFINE_BOXED_TYPE(ALSASeqEventDataNote, alsaseq_event_data_note, seq_event_data_note_copy, g_free)

/**
 * alsaseq_event_data_note_get_channel:
 * @self: A [struct@EventDataNote].
 * @channel: (out): The value of channel in the note event.
 *
 * Get the value of channel in the note event.
 */
void alsaseq_event_data_note_get_channel(const ALSASeqEventDataNote *self,
                                         guint8 *channel)
{
    *channel = self->channel;
}

/**
 * alsaseq_event_data_note_set_channel:
 * @self: A [struct@EventDataNote].
 * @channel: The value of channel for the note event.
 *
 * Set the value of channel for the note event.
 */
void alsaseq_event_data_note_set_channel(ALSASeqEventDataNote *self,
                                         guint8 channel)
{
    self->channel = channel;
}

/**
 * alsaseq_event_data_note_get_note:
 * @self: A [struct@EventDataNote].
 * @note: (out): The value of note in the note event.
 *
 * Get the value of note in the note event.
 */
void alsaseq_event_data_note_get_note(const ALSASeqEventDataNote *self,
                                      guint8 *note)
{
    *note = self->note;
}

/**
 * alsaseq_event_data_note_set_note:
 * @self: A [struct@EventDataNote].
 * @note: The value of note for the note event.
 *
 * Set the value of note for the note event.
 */
void alsaseq_event_data_note_set_note(ALSASeqEventDataNote *self, guint8 note)
{
    self->note = note;
}

/**
 * alsaseq_event_data_note_get_velocity:
 * @self: A [struct@EventDataNote].
 * @velocity: (out): The value of velocity in the note event.
 *
 * Get the value of velocity in the note event.
 */
void alsaseq_event_data_note_get_velocity(const ALSASeqEventDataNote *self,
                                          guint8 *velocity)
{
    *velocity = self->velocity;
}

/**
 * alsaseq_event_data_note_set_velocity:
 * @self: A [struct@EventDataNote].
 * @velocity: The value of note for the velocity event.
 *
 * Set the value of note for the velocity event.
 */
void alsaseq_event_data_note_set_velocity(ALSASeqEventDataNote *self,
                                          guint8 velocity)
{
    self->velocity = velocity;
}

/**
 * alsaseq_event_data_note_get_off_velocity:
 * @self: A [struct@EventDataNote].
 * @off_velocity: (out): The value of off-velocity in the note event.
 *
 * Get the value of off-velocity in the note event.
 */
void alsaseq_event_data_note_get_off_velocity(const ALSASeqEventDataNote *self,
                                              guint8 *off_velocity)
{
    *off_velocity = self->off_velocity;
}

/**
 * alsaseq_event_data_note_set_off_velocity:
 * @self: A [struct@EventDataNote].
 * @off_velocity: The value of note for the off-velocity event.
 *
 * Set the value of note for the off-velocity event.
 */
void alsaseq_event_data_note_set_off_velocity(ALSASeqEventDataNote *self,
                                              guint8 off_velocity)
{
    self->off_velocity = off_velocity;
}

/**
 * alsaseq_event_data_note_get_duration:
 * @self: A [struct@EventDataNote].
 * @duration: (out): The value of duratino in the note event.
 *
 * Get the value of duration in the note event.
 */
void alsaseq_event_data_note_get_duration(const ALSASeqEventDataNote *self,
                                          guint8 *duration)
{
    *duration = self->duration;
}

/**
 * alsaseq_event_data_note_set_duration:
 * @self: A [struct@EventDataNote].
 * @duration: The value of duration for the note event.
 *
 * Set the value of duration for the note event.
 */
void alsaseq_event_data_note_set_duration(ALSASeqEventDataNote *self,
                                          guint8 duration)
{
    self->duration = duration;
}
