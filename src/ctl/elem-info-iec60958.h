// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_IEC60958_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_IEC60958_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_IEC60958   (alsactl_elem_info_iec60958_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemInfoIec60958, alsactl_elem_info_iec60958, ALSACTL,
                         ELEM_INFO_IEC60958, GObject)

struct _ALSACtlElemInfoIec60958Class {
    GObjectClass parent_class;
};

ALSACtlElemInfoIec60958 *alsactl_elem_info_iec60958_new();

G_END_DECLS

#endif
