// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INTEGER_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INTEGER_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_INTEGER      (alsactl_elem_info_integer_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemInfoInteger, alsactl_elem_info_integer, ALSACTL,
                         ELEM_INFO_INTEGER, GObject)

struct _ALSACtlElemInfoIntegerClass {
    GObjectClass parent_class;
};

ALSACtlElemInfoInteger *alsactl_elem_info_integer_new();

G_END_DECLS

#endif
