// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_TSTAMP__H__
#define __ALSA_GOBJECT_ALSASEQ_TSTAMP__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asequencer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_TSTAMP   (alsaseq_tstamp_get_type())

typedef union snd_seq_timestamp ALSASeqTstamp;

GType alsaseq_tstamp_get_type() G_GNUC_CONST;

void alsaseq_tstamp_get_tick_time(ALSASeqTstamp *self, guint32 *tick_time);
void alsaseq_tstamp_set_tick_time(ALSASeqTstamp *self, const guint32 tick_time);

void alsaseq_tstamp_get_real_time(ALSASeqTstamp *self, const guint32 *real_time[2]);
void alsaseq_tstamp_set_real_time(ALSASeqTstamp *self, const guint32 real_time[2]);

G_END_DECLS

#endif
