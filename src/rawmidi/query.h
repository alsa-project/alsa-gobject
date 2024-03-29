// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_QUERY_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_QUERY_H__

#include <alsarawmidi.h>

G_BEGIN_DECLS

gboolean alsarawmidi_get_device_id_list(guint card_id, guint **entries, gsize *entry_count,
                                        GError **error);

gboolean alsarawmidi_get_rawmidi_sysname(guint card_id, guint device_id, char **sysname,
                                         GError **error);

gboolean alsarawmidi_get_rawmidi_devnode(guint card_id, guint device_id, char **devnode,
                                         GError **error);

gboolean alsarawmidi_get_subdevice_id_list(guint card_id, guint device_id,
                                           ALSARawmidiStreamDirection direction,
                                           guint **entries, gsize *entry_count, GError **error);

gboolean alsarawmidi_get_substream_info(guint card_id, guint device_id,
                                        ALSARawmidiStreamDirection direction,
                                        guint subdevice_id,
                                        ALSARawmidiSubstreamInfo **substream_info, GError **error);

G_END_DECLS

#endif
