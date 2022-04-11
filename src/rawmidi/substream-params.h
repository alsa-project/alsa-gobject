// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_PARAMS_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_PARAMS_H__

#include <alsarawmidi.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS (alsarawmidi_substream_params_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSARawmidiSubstreamParams, alsarawmidi_substream_params, ALSARAWMIDI,
                         SUBSTREAM_PARAMS, GObject);
struct _ALSARawmidiSubstreamParamsClass {
    GObjectClass parent_class;
};

ALSARawmidiSubstreamParams *alsarawmidi_substream_params_new();

G_END_DECLS

#endif
