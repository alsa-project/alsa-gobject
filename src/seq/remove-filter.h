// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER__H__
#define __ALSA_GOBJECT_ALSASEQ_REMOVE_FILTER__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asequencer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_REMOVE_FILTER     (alsaseq_remove_filter_get_type())

typedef struct snd_seq_remove_events    ALSASeqRemoveFilter;

GType alsaseq_remove_filter_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
