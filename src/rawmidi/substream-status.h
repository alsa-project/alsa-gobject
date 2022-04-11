// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_STATUS_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_STATUS_H__

#include <alsarawmidi.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_SUBSTREAM_STATUS   (alsarawmidi_substream_status_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSARawmidiSubstreamStatus, alsarawmidi_substream_status, ALSARAWMIDI,
                         SUBSTREAM_STATUS, GObject);

struct _ALSARawmidiSubstreamStatusClass {
    GObjectClass parent_class;
};

ALSARawmidiSubstreamStatus *alsarawmidi_substream_status_new();

G_END_DECLS

#endif
