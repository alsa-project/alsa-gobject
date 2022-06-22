// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSATIMER_INSTANCE_PARAMS_H__
#define __ALSA_GOBJECT_ALSATIMER_INSTANCE_PARAMS_H__

#include <alsatimer.h>

G_BEGIN_DECLS

#define ALSATIMER_TYPE_INSTANCE_PARAMS  (alsatimer_instance_params_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSATimerInstanceParams, alsatimer_instance_params, ALSATIMER,
                         INSTANCE_PARAMS, GObject);

struct _ALSATimerInstanceParamsClass {
    GObjectClass parent_class;
};

ALSATimerInstanceParams *alsatimer_instance_params_new();

gboolean alsatimer_instance_params_set_event_filter(ALSATimerInstanceParams *self,
                                            const ALSATimerRealTimeEventType *entries,
                                            gsize entry_count, GError **error);

gboolean alsatimer_instance_params_get_event_filter(ALSATimerInstanceParams *self,
                                            ALSATimerRealTimeEventType **entries,
                                            gsize *entry_count, GError **error);

G_END_DECLS

#endif
