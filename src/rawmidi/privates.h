// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_PRIVATES_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_PRIVATES_H__

#include "alsarawmidi.h"

G_BEGIN_DECLS

void rawmidi_substream_info_refer_private(ALSARawmidiSubstreamInfo *self,
                                          struct snd_rawmidi_info **info);

void rawmidi_select_subdevice(guint card_id, guint subdevice_id, int *ctl_fd, GError **error);

void rawmidi_substream_params_refer_private(ALSARawmidiSubstreamParams *self,
                                            struct snd_rawmidi_params **params);

void rawmidi_substream_status_refer_private(ALSARawmidiSubstreamStatus *self,
                                            struct snd_rawmidi_status **status);

G_END_DECLS

#endif
