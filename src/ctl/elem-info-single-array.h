// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_SINGLE_ARRAY_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_SINGLE_ARRAY_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_SINGLE_ARRAY     (alsactl_elem_info_single_array_get_type())

G_DECLARE_INTERFACE(ALSACtlElemInfoSingleArray, alsactl_elem_info_single_array, ALSACTL,
                    ELEM_INFO_SINGLE_ARRAY, GObject)

struct _ALSACtlElemInfoSingleArrayInterface {
    GTypeInterface parent_iface;
};

G_END_DECLS

#endif
