// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_PRIVATES__H__
#define __ALSA_GOBJECT_ALSACTL_PRIVATES__H__

#include <string.h>

#include <glib.h>
#include <glib-object.h>

#include "card-info.h"

#include <sound/asound.h>

G_BEGIN_DECLS

GQuark alsactl_error_quark(void);

#define generate_error(err, errno)                      \
    g_set_error(err, alsactl_error_quark(), errno,      \
                __FILE__ ":%d: %s", __LINE__, strerror(errno))

void ctl_card_info_refer_private(ALSACtlCardInfo *self,
                                 struct snd_ctl_card_info **info);

G_END_DECLS

#endif
