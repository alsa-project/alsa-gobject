// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSATimerDeviceStatus:
 * A GObject-derived object to express status of timer device.
 *
 * A [class@DeviceStatus] is a GObject-derived object to express status of timer device. The
 * call of [func@get_device_status] returns the instance of object.
 *
 * The object wraps 'struct snd_timer_gstatus' in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_timer_gstatus status;
} ALSATimerDeviceStatusPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerDeviceStatus, alsatimer_device_status, G_TYPE_OBJECT)

enum timer_device_status_prop_type {
    TIMER_DEVICE_STATUS_PROP_RESOLUTION = 1,
    TIMER_DEVICE_STATUS_PROP_RESOLUTION_NUM,
    TIMER_DEVICE_STATUS_PROP_RESOLUTION_DEN,
    TIMER_DEVICE_STATUS_PROP_COUNT,
};
static GParamSpec *timer_device_status_props[TIMER_DEVICE_STATUS_PROP_COUNT] = { NULL, };


static void timer_device_status_get_property(GObject *obj, guint id,
                                             GValue *val, GParamSpec *spec)
{
    ALSATimerDeviceStatus *self = ALSATIMER_DEVICE_STATUS(obj);
    ALSATimerDeviceStatusPrivate *priv =
                            alsatimer_device_status_get_instance_private(self);

    switch (id) {
    case TIMER_DEVICE_STATUS_PROP_RESOLUTION:
        g_value_set_uint64(val, priv->status.resolution);
        break;
    case TIMER_DEVICE_STATUS_PROP_RESOLUTION_NUM:
        g_value_set_uint64(val, priv->status.resolution_num);
        break;
    case TIMER_DEVICE_STATUS_PROP_RESOLUTION_DEN:
        g_value_set_uint64(val, priv->status.resolution_den);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_device_status_class_init(ALSATimerDeviceStatusClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = timer_device_status_get_property;

    /**
     * ALSATimerDeviceStatus:resolution:
     *
     * The current resolution in nano seconds.
     */
    timer_device_status_props[TIMER_DEVICE_STATUS_PROP_RESOLUTION] =
        g_param_spec_uint64("resolution", "resolution",
                            "The current resolution in nano seconds.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    /**
     * ALSATimerDeviceStatus:resolution-numerator:
     *
     * The numerator of current resolution in seconds.
     */
    timer_device_status_props[TIMER_DEVICE_STATUS_PROP_RESOLUTION_NUM] =
        g_param_spec_uint64("resolution-numerator", "resolution-numerator",
                            "The numerator of current resolution in seconds.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    /**
     * ALSATimerDeviceStatus:resolution-denominator:
     *
     * The denominator of current resolution in seconds.
     */
    timer_device_status_props[TIMER_DEVICE_STATUS_PROP_RESOLUTION_DEN] =
        g_param_spec_uint64("resolution-denominator", "resolution-denominator",
                            "The denominator of current resolution in seconds.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      TIMER_DEVICE_STATUS_PROP_COUNT,
                                      timer_device_status_props);
}

static void alsatimer_device_status_init(ALSATimerDeviceStatus *self)
{
    return;
}

/**
 * alsatimer_device_status_new:
 *
 * Allocate and return an instance of [class@DeviceStatus].
 *
 * Returns: An instance of [class@DeviceStatus].
 */
ALSATimerDeviceStatus *alsatimer_device_status_new()
{
    return g_object_new(ALSATIMER_TYPE_DEVICE_STATUS, NULL);
}

void timer_device_status_refer_private(ALSATimerDeviceStatus *self,
                                       struct snd_timer_gstatus **status)
{
    ALSATimerDeviceStatusPrivate *priv =
                            alsatimer_device_status_get_instance_private(self);

    *status = &priv->status;
}
