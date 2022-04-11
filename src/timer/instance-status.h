// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_INSTANCE_STATUS_H__
#define __ALSA_GOBJECT_ALSATIMER_INSTANCE_STATUS_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_INSTANCE_STATUS  (alsatimer_instance_status_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSATimerInstanceStatus, alsatimer_instance_status, ALSATIMER,
                         INSTANCE_STATUS, GObject);

struct _ALSATimerInstanceStatusClass {
    GObjectClass parent_class;
};

ALSATimerInstanceStatus *alsatimer_instance_status_new();

void alsatimer_instance_status_get_tstamp(ALSATimerInstanceStatus *self,
                                          const gint64 *tstamp[2]);

G_END_DECLS

#endif
