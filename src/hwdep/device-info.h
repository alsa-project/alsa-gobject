// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_DEVICE_INFO_H__
#define __ALSA_GOBJECT_ALSAHWDEP_DEVICE_INFO_H__

#include <alsahwdep.h>

G_BEGIN_DECLS

#define ALSAHWDEP_TYPE_DEVICE_INFO      (alsahwdep_device_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSAHwdepDeviceInfo, alsahwdep_device_info, ALSAHWDEP, DEVICE_INFO, GObject);

struct _ALSAHwdepDeviceInfoClass {
    GObjectClass parent_class;
};

G_END_DECLS

#endif
