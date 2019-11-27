// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_PRIVATES__H__
#define __ALSA_GOBJECT_ALSAHWDEP_PRIVATES__H__

#include "device-info.h"

#include <sound/asound.h>

G_BEGIN_DECLS

GQuark alsahwdep_error_quark(void);

#define generate_error(err, errno)                              \
    g_set_error(err, alsahwdep_error_quark(), errno,            \
                __FILE__ ":%d: %s", __LINE__, strerror(errno))

void hwdep_device_info_refer_private(ALSAHwdepDeviceInfo *self,
                                     struct snd_hwdep_info **info);

G_END_DECLS

#endif
