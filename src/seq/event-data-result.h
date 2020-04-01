// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_RESULT__H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_DATA_RESULT__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asequencer.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_DATA_RESULT (alsaseq_event_data_result_get_type())

typedef struct snd_seq_result ALSASeqEventDataResult;

GType alsaseq_event_data_result_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
