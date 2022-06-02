// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_ENUMERATED_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_ENUMERATED_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_ENUMERATED   (alsactl_elem_info_enumerated_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemInfoEnumerated, alsactl_elem_info_enumerated, ALSACTL,
                         ELEM_INFO_ENUMERATED, GObject)

struct _ALSACtlElemInfoEnumeratedClass {
    GObjectClass parent_class;
};

ALSACtlElemInfoEnumerated *alsactl_elem_info_enumerated_new();

G_END_DECLS

#endif
