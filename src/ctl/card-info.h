// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_CARD_INFO_H__
#define __ALSA_GOBJECT_ALSACTL_CARD_INFO_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_CARD_INFO      (alsactl_card_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlCardInfo, alsactl_card_info, ALSACTL, CARD_INFO, GObject);

struct _ALSACtlCardInfoClass {
    GObjectClass parent_class;
};

G_END_DECLS

#endif
