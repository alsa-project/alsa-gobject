// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-note.h"

/**
 * SECTION: event-data-note
 * @Title: ALSASeqEventDataNote
 * @Short_description: A boxed object to represent data of note event
 *
 * A #ALSASeqEventDataNote is a boxed object to represent data of note event.
 * The instance of object is one of data properties in #ALSASeqEventFixed.
 *
 * The object wraps 'struct snd_seq_ev_note' in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataNote *seq_event_data_note_copy(const ALSASeqEventDataNote *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataNote, alsaseq_event_data_note, seq_event_data_note_copy, g_free)

/**
 * alsaseq_event_data_note_get_channel:
 * @self: A #ALSASeqEventDataNote.
 * @channel: (out): The value of channel in the note event.
 *
 * Get the value of channel in the note event.
 */
void alsaseq_event_data_note_get_channel(ALSASeqEventDataNote *self,
                                         guint8 *channel)
{
    *channel = self->channel;
}

/**
 * alsaseq_event_data_note_set_channel:
 * @self: A #ALSASeqEventDataNote.
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
 * @self: A #ALSASeqEventDataNote.
 * @note: (out): The value of note in the note event.
 *
 * Get the value of note in the note event.
 */
void alsaseq_event_data_note_get_note(ALSASeqEventDataNote *self, guint8 *note)
{
    *note = self->note;
}

/**
 * alsaseq_event_data_note_set_note:
 * @self: A #ALSASeqEventDataNote.
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
 * @self: A #ALSASeqEventDataNote.
 * @velocity: (out): The value of velocity in the note event.
 *
 * Get the value of velocity in the note event.
 */
void alsaseq_event_data_note_get_velocity(ALSASeqEventDataNote *self,
                                          guint8 *velocity)
{
    *velocity = self->velocity;
}

/**
 * alsaseq_event_data_note_set_velocity:
 * @self: A #ALSASeqEventDataNote.
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
 * @self: A #ALSASeqEventDataNote.
 * @off_velocity: (out): The value of off-velocity in the note event.
 *
 * Get the value of off-velocity in the note event.
 */
void alsaseq_event_data_note_get_off_velocity(ALSASeqEventDataNote *self,
                                              guint8 *off_velocity)
{
    *off_velocity = self->off_velocity;
}

/**
 * alsaseq_event_data_note_set_off_velocity:
 * @self: A #ALSASeqEventDataNote.
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
 * @self: A #ALSASeqEventDataNote.
 * @duration: (out): The value of duratino in the note event.
 *
 * Get the value of duration in the note event.
 */
void alsaseq_event_data_note_get_duration(ALSASeqEventDataNote *self,
                                          guint8 *duration)
{
    *duration = self->duration;
}

/**
 * alsaseq_event_data_note_set_duration:
 * @self: A #ALSASeqEventDataNote.
 * @duration: The value of duration for the note event.
 *
 * Set the value of duration for the note event.
 */
void alsaseq_event_data_note_set_duration(ALSASeqEventDataNote *self,
                                          guint8 duration)
{
    self->duration = duration;
}
