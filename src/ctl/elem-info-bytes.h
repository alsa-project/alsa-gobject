// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BYTES_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BYTES_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_BYTES    (alsactl_elem_info_bytes_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemInfoBytes, alsactl_elem_info_bytes, ALSACTL, ELEM_INFO_BYTES, GObject)

struct _ALSACtlElemInfoBytesClass {
    GObjectClass parent_class;
};

ALSACtlElemInfoBytes *alsactl_elem_info_bytes_new();

G_END_DECLS

#endif
