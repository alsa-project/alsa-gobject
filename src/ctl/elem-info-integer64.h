// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INTEGER64_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INTEGER64_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_INTEGER64        (alsactl_elem_info_integer64_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemInfoInteger64, alsactl_elem_info_integer64, ALSACTL,
                         ELEM_INFO_INTEGER64, GObject)

struct _ALSACtlElemInfoInteger64Class {
    GObjectClass parent_class;
};

ALSACtlElemInfoInteger64 *alsactl_elem_info_integer64_new();

G_END_DECLS

#endif
