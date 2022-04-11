// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_INSTANCE_INFO_H__
#define __ALSA_GOBJECT_ALSATIMER_INSTANCE_INFO_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_INSTANCE_INFO    (alsatimer_instance_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSATimerInstanceInfo, alsatimer_instance_info, ALSATIMER, INSTANCE_INFO,
                         GObject);

struct _ALSATimerInstanceInfoClass {
    GObjectClass parent_class;
};

G_END_DECLS

#endif
