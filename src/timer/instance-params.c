// SPDX-License-Identifier: LGPL-3.0-or-later
#include "instance-params.h"

G_DEFINE_TYPE(ALSATimerInstanceParams, alsatimer_instance_params, G_TYPE_OBJECT)

static void alsatimer_instance_params_class_init(ALSATimerInstanceParamsClass *klass)
{
    return;

}

static void alsatimer_instance_params_init(ALSATimerInstanceParams *self)
{
    return;
}

/**
 * alsatimer_instance_params_new:
 *
 * Allocate and return an instance of ALSATimerInstanceParams.
 */
ALSATimerInstanceParams *alsatimer_instance_params_new()
{
    return g_object_new(ALSATIMER_TYPE_INSTANCE_PARAMS, NULL);
}
