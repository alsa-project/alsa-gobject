// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSATimerInstanceInfo:
 * A GObject-derived object to represent information of user instance.
 *
 * A [class@InstanceInfo] is a GObject-derived object to represent information of user instance
 * attached to any timer device or the other instance as slave. The call of
 * [method@UserInstance.get_info] returns the instance of object.
 *
 * The object wraps `struct snd_timer_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_timer_info info;
} ALSATimerInstanceInfoPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerInstanceInfo, alsatimer_instance_info, G_TYPE_OBJECT)

enum timer_instance_info_props {
    TIMER_INSTANCE_INFO_PROP_FLAGS = 1,
    TIMER_INSTANCE_INFO_PROP_CARD_ID,
    TIMER_INSTANCE_INFO_PROP_ID,
    TIMER_INSTANCE_INFO_PROP_NAME,
    TIMER_INSTANCE_INFO_PROP_RESOLUTION,
    TIMER_INSTANCE_INFO_PROP_COUNT,
};
static GParamSpec *timer_instance_info_props[TIMER_INSTANCE_INFO_PROP_COUNT] = { NULL, };

static void timer_instance_info_get_property(GObject *obj, guint id, GValue *val,
                                           GParamSpec *spec)
{
    ALSATimerInstanceInfo *self = ALSATIMER_INSTANCE_INFO(obj);
    ALSATimerInstanceInfoPrivate *priv =
                            alsatimer_instance_info_get_instance_private(self);

    switch (id) {
    case TIMER_INSTANCE_INFO_PROP_FLAGS:
        g_value_set_flags(val, (ALSATimerDeviceInfoFlag)priv->info.flags);
        break;
    case TIMER_INSTANCE_INFO_PROP_CARD_ID:
        g_value_set_int(val, priv->info.card);
        break;
    case TIMER_INSTANCE_INFO_PROP_ID:
        g_value_set_static_string(val, (const char *)priv->info.id);
        break;
    case TIMER_INSTANCE_INFO_PROP_NAME:
        g_value_set_static_string(val, (const char*)priv->info.name);
        break;
    case TIMER_INSTANCE_INFO_PROP_RESOLUTION:
        g_value_set_uint64(val, (guint64)priv->info.resolution);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_instance_info_class_init(ALSATimerInstanceInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = timer_instance_info_get_property;

    /**
     * ALSATimerInstanceInfo:flags:
     *
     * The flags for attached timer.
     */
    timer_instance_info_props[TIMER_INSTANCE_INFO_PROP_FLAGS] =
        g_param_spec_flags("flags", "flags",
                           "The flags for attached timer.",
                           ALSATIMER_TYPE_DEVICE_INFO_FLAG,
                           0,
                           G_PARAM_READABLE);

    /**
     * ALSATimerInstanceInfo:card-id:
     *
     * The numeric ID of sound card for attached timer.
     */
    timer_instance_info_props[TIMER_INSTANCE_INFO_PROP_CARD_ID] =
        g_param_spec_int("card-id", "card-id",
                         "The numeric ID of sound card for attached timer.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READABLE);

    /**
     * ALSATimerInstanceInfo:id:
     *
     * The string ID of attached timer.
     */
    timer_instance_info_props[TIMER_INSTANCE_INFO_PROP_ID] =
        g_param_spec_string("id", "id",
                            "The string ID of attached timer.",
                            "",
                            G_PARAM_READABLE);

    /**
     * ALSATimerInstanceInfo:name:
     *
     * The name of attached timer.
     */
    timer_instance_info_props[TIMER_INSTANCE_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of attached timer.",
                            "",
                            G_PARAM_READABLE);

    /**
     * ALSATimerInstanceInfo:resolution:
     *
     * The resolution of attached timer in nano seconds.
     */
    timer_instance_info_props[TIMER_INSTANCE_INFO_PROP_RESOLUTION] =
        g_param_spec_uint64("resolution", "resolution",
                            "The resolution of attached timer in nano seconds.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      TIMER_INSTANCE_INFO_PROP_COUNT,
                                      timer_instance_info_props);
}

static void alsatimer_instance_info_init(ALSATimerInstanceInfo *self)
{
    return;
}

void timer_instance_info_refer_private(ALSATimerInstanceInfo *self,
                                     struct snd_timer_info **info)
{
    ALSATimerInstanceInfoPrivate *priv =
                            alsatimer_instance_info_get_instance_private(self);

    *info = &priv->info;
}
