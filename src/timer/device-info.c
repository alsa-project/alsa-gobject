// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * SECTION: device-info
 * @Title: ALSATimerDeviceInfo
 * @Short_description: A GObject-derived object to represent information of
 *                     timer device.
 *
 * A #ALSATimerDeviceInfo is a GObject-derived object to represent information
 * of timer device. The call of alsatimer_get_device_info() returns an instance
 * of the object according to the identifier of timer device.
 *
 * The object wraps 'struct snd_timer_ginfo' in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_timer_ginfo info;
} ALSATimerDeviceInfoPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerDeviceInfo, alsatimer_device_info, G_TYPE_OBJECT)

enum timer_device_info_prop_type {
    TIMER_DEVICE_INFO_PROP_FLAGS = 1,
    TIMER_DEVICE_INFO_PROP_CARD_ID,
    TIMER_DEVICE_INFO_PROP_ID,
    TIMER_DEVICE_INFO_PROP_NAME,
    TIMER_DEVICE_INFO_PROP_RESOLUTION,
    TIMER_DEVICE_INFO_PROP_RESOLUTION_MIN,
    TIMER_DEVICE_INFO_PROP_RESOLUTION_MAX,
    TIMER_DEVICE_INFO_PROP_INSTANCE_COUNT,
    TIMER_DEVICE_INFO_PROP_COUNT,
};
static GParamSpec *timer_device_info_props[TIMER_DEVICE_INFO_PROP_COUNT] = { NULL, };

static void timer_device_info_get_property(GObject *obj, guint id, GValue *val,
                                           GParamSpec *spec)
{
    ALSATimerDeviceInfo *self = ALSATIMER_DEVICE_INFO(obj);
    ALSATimerDeviceInfoPrivate *priv =
                            alsatimer_device_info_get_instance_private(self);

    switch (id) {
    case TIMER_DEVICE_INFO_PROP_FLAGS:
        g_value_set_flags(val, (ALSATimerDeviceInfoFlag)priv->info.flags);
        break;
    case TIMER_DEVICE_INFO_PROP_CARD_ID:
        g_value_set_int(val, priv->info.card);
        break;
    case TIMER_DEVICE_INFO_PROP_ID:
        g_value_set_static_string(val, (const gchar *)priv->info.id);
        break;
    case TIMER_DEVICE_INFO_PROP_NAME:
        g_value_set_static_string(val, (const gchar *)priv->info.name);
        break;
    case TIMER_DEVICE_INFO_PROP_RESOLUTION:
        g_value_set_uint64(val, priv->info.resolution);
        break;
    case TIMER_DEVICE_INFO_PROP_RESOLUTION_MIN:
        g_value_set_uint64(val, priv->info.resolution_min);
        break;
    case TIMER_DEVICE_INFO_PROP_RESOLUTION_MAX:
        g_value_set_uint64(val, priv->info.resolution_max);
        break;
    case TIMER_DEVICE_INFO_PROP_INSTANCE_COUNT:
        g_value_set_uint(val, priv->info.clients);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_device_info_class_init(ALSATimerDeviceInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = timer_device_info_get_property;

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_FLAGS] =
        g_param_spec_flags("flags", "flags",
                           "The flags of timer, one of ALSATimerDeviceInfoFlag.",
                           ALSATIMER_TYPE_DEVICE_INFO_FLAG,
                           0,
                           G_PARAM_READABLE);

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_CARD_ID] =
        g_param_spec_int("card-id", "card-id",
                         "The numerical ID of sound card.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READABLE);

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_ID] =
        g_param_spec_string("id", "id",
                            "The string ID of timer.",
                            "",
                            G_PARAM_READABLE);

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of timer.",
                            "",
                            G_PARAM_READABLE);

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_RESOLUTION] =
        g_param_spec_uint64("resolution", "resolution",
                            "The resolution in nano seconds.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_RESOLUTION_MIN] =
        g_param_spec_uint64("resolution-min", "resolution-min",
                            "The resolution in nano seconds.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_RESOLUTION_MAX] =
        g_param_spec_uint64("resolution-max", "resolution-max",
                            "The resolution in nano seconds.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    timer_device_info_props[TIMER_DEVICE_INFO_PROP_INSTANCE_COUNT] =
        g_param_spec_uint("instance-count", "instance-count",
                          "The number of instances for the timer.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      TIMER_DEVICE_INFO_PROP_COUNT,
                                      timer_device_info_props);
}

static void alsatimer_device_info_init(ALSATimerDeviceInfo *self)
{
    return;
}

void timer_device_info_refer_private(ALSATimerDeviceInfo *self,
                                     struct snd_timer_ginfo **info)
{
    ALSATimerDeviceInfoPrivate *priv =
                            alsatimer_device_info_get_instance_private(self);

    *info = &priv->info;
}
