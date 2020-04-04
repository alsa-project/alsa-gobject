// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_NOTE__H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_NOTE__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asequencer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_DATA_NOTE (alsaseq_event_data_note_get_type())

typedef struct snd_seq_ev_note ALSASeqEventDataNote;

GType alsaseq_event_data_note_get_type() G_GNUC_CONST;

void alsaseq_event_data_note_get_channel(ALSASeqEventDataNote *self,
                                         guint8 *channel);
void alsaseq_event_data_note_set_channel(ALSASeqEventDataNote *self,
                                         guint8 channel);

void alsaseq_event_data_note_get_note(ALSASeqEventDataNote *self, guint8 *note);
void alsaseq_event_data_note_set_note(ALSASeqEventDataNote *self, guint8 note);

void alsaseq_event_data_note_get_velocity(ALSASeqEventDataNote *self,
                                          guint8 *velocity);
void alsaseq_event_data_note_set_velocity(ALSASeqEventDataNote *self,
                                          guint8 velocity);

void alsaseq_event_data_note_get_off_velocity(ALSASeqEventDataNote *self,
                                              guint8 *off_velocity);
void alsaseq_event_data_note_set_off_velocity(ALSASeqEventDataNote *self,
                                              guint8 off_velocity);

void alsaseq_event_data_note_get_duration(ALSASeqEventDataNote *self,
                                          guint8 *duration);
void alsaseq_event_data_note_set_duration(ALSASeqEventDataNote *self,
                                          guint8 duration);

G_END_DECLS

#endif
