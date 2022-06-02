// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_COMMON_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_COMMON_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_COMMON       (alsactl_elem_info_common_get_type())

G_DECLARE_INTERFACE(ALSACtlElemInfoCommon, alsactl_elem_info_common, ALSACTL, ELEM_INFO_COMMON,
                    GObject)

struct _ALSACtlElemInfoCommonInterface {
    GTypeInterface parent_iface;
};

G_END_DECLS

#endif
