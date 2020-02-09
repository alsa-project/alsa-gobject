// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data.h"

G_DEFINE_ABSTRACT_TYPE(ALSATimerEventData, alsatimer_event_data, G_TYPE_OBJECT)

static void alsatimer_event_data_class_init(ALSATimerEventDataClass *klass)
{
    return;
}

static void alsatimer_event_data_init(ALSATimerEventData *self)
{
    return;
}
