// SPDX-License-Identifier: LGPL-3.0-or-later
#include "user-instance.h"

G_DEFINE_TYPE(ALSATimerUserInstance, alsatimer_user_instance, G_TYPE_OBJECT)

static void alsatimer_user_instance_class_init(ALSATimerUserInstanceClass *klass)
{
    return;
}

static void alsatimer_user_instance_init(ALSATimerUserInstance *self)
{
    return;
}

ALSATimerUserInstance *alsatimer_user_instance_new()
{
    return g_object_new(ALSATIMER_TYPE_USER_INSTANCE, NULL);
}
