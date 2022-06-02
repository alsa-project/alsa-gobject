// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BOOLEAN_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BOOLEAN_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_BOOLEAN      (alsactl_elem_info_boolean_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemInfoBoolean, alsactl_elem_info_boolean, ALSACTL,
                         ELEM_INFO_BOOLEAN, GObject)

struct _ALSACtlElemInfoBooleanClass {
    GObjectClass parent_class;
};

ALSACtlElemInfoBoolean *alsactl_elem_info_boolean_new();

G_END_DECLS

#endif
