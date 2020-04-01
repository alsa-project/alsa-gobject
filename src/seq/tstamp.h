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

G_END_DECLS

#endif
