// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_PRIVATES__H__
#define __ALSA_GOBJECT_ALSAHWDEP_PRIVATES__H__

#include "alsahwdep.h"

G_BEGIN_DECLS

void hwdep_device_info_refer_private(ALSAHwdepDeviceInfo *self,
                                     struct snd_hwdep_info **info);

G_END_DECLS

#endif
