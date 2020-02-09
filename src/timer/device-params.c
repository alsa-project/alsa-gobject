// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

struct _ALSATimerDeviceParamsPrivate {
    struct snd_timer_gparams params;
};
G_DEFINE_TYPE(ALSATimerDeviceParams, alsatimer_device_params, G_TYPE_OBJECT)

enum timer_device_params_prop_type {
    TIMER_DEVICE_PARAMS_PROP_PERIOD_NUM = 1,
    TIMER_DEVICE_PARAMS_PROP_PERIOD_DEN,
    TIMER_DEVICE_PARAMS_PROP_COUNT,
};
static GParamSpec *timer_device_params_props[TIMER_DEVICE_PARAMS_PROP_COUNT] = { NULL, };

static void timer_device_params_set_property(GObject *obj, guint id,
                                            const GValue *val, GParamSpec *spec)
{
    ALSATimerDeviceParams *self = ALSATIMER_DEVICE_PARAMS(obj);
    ALSATimerDeviceParamsPrivate *priv =
                            alsatimer_device_params_get_instance_private(self);

    switch (id) {
    case TIMER_DEVICE_PARAMS_PROP_PERIOD_NUM:
        priv->params.period_num = (unsigned long)g_value_get_uint64(val);
        break;
    case TIMER_DEVICE_PARAMS_PROP_PERIOD_DEN:
        priv->params.period_den = (unsigned long)g_value_get_uint64(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void timer_device_params_get_property(GObject *obj, guint id,
                                             GValue *val, GParamSpec *spec)
{
    ALSATimerDeviceParams *self = ALSATIMER_DEVICE_PARAMS(obj);
    ALSATimerDeviceParamsPrivate *priv =
                            alsatimer_device_params_get_instance_private(self);

    switch (id) {
    case TIMER_DEVICE_PARAMS_PROP_PERIOD_NUM:
        g_value_set_uint64(val, priv->params.period_num);
        break;
    case TIMER_DEVICE_PARAMS_PROP_PERIOD_DEN:
        g_value_set_uint64(val, priv->params.period_den);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_device_params_class_init(ALSATimerDeviceParamsClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = timer_device_params_set_property;
    gobject_class->get_property = timer_device_params_get_property;

    timer_device_params_props[TIMER_DEVICE_PARAMS_PROP_PERIOD_NUM] =
        g_param_spec_uint64("period-numerator", "period-numerator",
                            "The numerator of period for timer.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READWRITE);

    timer_device_params_props[TIMER_DEVICE_PARAMS_PROP_PERIOD_DEN] =
        g_param_spec_uint64("period-denominator", "period-denominator",
                            "The denominator of period for timer.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                    TIMER_DEVICE_PARAMS_PROP_COUNT, timer_device_params_props);
}

static void alsatimer_device_params_init(ALSATimerDeviceParams *self)
{
    return;
}

/**
 * alsatimer_device_params_new:
 *
 * Instantiate #ALSATimerDeviceParams object and return the instance.
 *
 * Returns: an instance of #ALSATimerDeviceParams.
 */
ALSATimerDeviceParams *alsatimer_device_params_new()
{
    return g_object_new(ALSATIMER_TYPE_DEVICE_PARAMS, NULL);
}
