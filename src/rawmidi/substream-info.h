// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_INFO_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_INFO_H__

#include <alsarawmidi.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_SUBSTREAM_INFO (alsarawmidi_substream_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSARawmidiSubstreamInfo, alsarawmidi_substream_info, ALSARAWMIDI,
                         SUBSTREAM_INFO, GObject);
struct _ALSARawmidiSubstreamInfoClass {
    GObjectClass parent_class;
};

G_END_DECLS

#endif
