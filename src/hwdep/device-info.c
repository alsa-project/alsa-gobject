// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSAHwdepDeviceInfo:
 * A GObject-derived object to represent information of ALSA hwdep device.
 *
 * A [class@DeviceInfo] is a GObject-derived object to represent information of ALSA hwdep device.
 * The call of [func@get_device_info] returns an instance of the object.
 *
 * The object wraps `struct snd_hwdep_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_hwdep_info info;
} ALSAHwdepDeviceInfoPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSAHwdepDeviceInfo, alsahwdep_device_info, G_TYPE_OBJECT)

enum hwdep_device_info_prop_type {
    HWDEP_DEVICE_INFO_PROP_DEVICE_ID = 1,
    HWDEP_DEVICE_INFO_PROP_CARD_ID,
    HWDEP_DEVICE_INFO_PROP_ID,
    HWDEP_DEVICE_INFO_PROP_NAME,
    HWDEP_DEVICE_INFO_PROP_IFACE,
    HWDEP_DEVICE_INFO_PROP_COUNT,
};
static GParamSpec *hwdep_device_info_props[HWDEP_DEVICE_INFO_PROP_COUNT] = { NULL, };

static void hwdep_device_info_set_property(GObject *obj, guint id, const GValue *val,
                                           GParamSpec *spec)
{
    ALSAHwdepDeviceInfo *self = ALSAHWDEP_DEVICE_INFO(obj);
    ALSAHwdepDeviceInfoPrivate *priv = alsahwdep_device_info_get_instance_private(self);

    switch (id) {
    case HWDEP_DEVICE_INFO_PROP_DEVICE_ID:
        priv->info.device = g_value_get_uint(val);
        break;
    case HWDEP_DEVICE_INFO_PROP_CARD_ID:
        priv->info.card = g_value_get_int(val);
        break;
    case HWDEP_DEVICE_INFO_PROP_ID:
        g_strlcpy((gchar *)priv->info.id, g_value_get_string(val), sizeof(priv->info.id));
        break;
    case HWDEP_DEVICE_INFO_PROP_NAME:
        g_strlcpy((gchar *)priv->info.name, g_value_get_string(val), sizeof(priv->info.name));
        break;
    case HWDEP_DEVICE_INFO_PROP_IFACE:
        priv->info.iface = (int)g_value_get_enum(val);
        break;
    default:
        break;
    }
}

static void hwdep_device_info_get_property(GObject *obj, guint id, GValue *val,
                                           GParamSpec *spec)
{
    ALSAHwdepDeviceInfo *self = ALSAHWDEP_DEVICE_INFO(obj);
    ALSAHwdepDeviceInfoPrivate *priv =
                            alsahwdep_device_info_get_instance_private(self);

    switch (id) {
    case HWDEP_DEVICE_INFO_PROP_DEVICE_ID:
        g_value_set_uint(val, priv->info.device);
        break;
    case HWDEP_DEVICE_INFO_PROP_CARD_ID:
        g_value_set_int(val, priv->info.card);
        break;
    case HWDEP_DEVICE_INFO_PROP_ID:
        g_value_set_static_string(val, (char *)priv->info.id);
        break;
    case HWDEP_DEVICE_INFO_PROP_NAME:
        g_value_set_static_string(val, (char *)priv->info.name);
        break;
    case HWDEP_DEVICE_INFO_PROP_IFACE:
        g_value_set_enum(val, (ALSAHwdepIfaceType)priv->info.iface);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsahwdep_device_info_class_init(ALSAHwdepDeviceInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = hwdep_device_info_set_property;
    gobject_class->get_property = hwdep_device_info_get_property;

    hwdep_device_info_props[HWDEP_DEVICE_INFO_PROP_DEVICE_ID] =
        g_param_spec_uint("device-id", "device-id",
                          "The numerical ID of device.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    hwdep_device_info_props[HWDEP_DEVICE_INFO_PROP_CARD_ID] =
        g_param_spec_int("card-id", "card-id",
                         "The numerical ID of sound card.",
                         G_MININT, G_MAXINT,
                         0,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    hwdep_device_info_props[HWDEP_DEVICE_INFO_PROP_ID] =
        g_param_spec_string("id", "id",
                            "The ID string of the hwdep device",
                            "",
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    hwdep_device_info_props[HWDEP_DEVICE_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of the hwdep device",
                            "",
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    hwdep_device_info_props[HWDEP_DEVICE_INFO_PROP_IFACE] =
        g_param_spec_enum("iface", "iface",
                          "The type of interface for the hwdep device, one of "
                          "ALSAHwdepIfaceType.",
                          ALSAHWDEP_TYPE_IFACE_TYPE,
                          SNDRV_HWDEP_IFACE_OPL2,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(gobject_class,
                                      HWDEP_DEVICE_INFO_PROP_COUNT,
                                      hwdep_device_info_props);
}

static void alsahwdep_device_info_init(ALSAHwdepDeviceInfo *self)
{
    return;
}

void hwdep_device_info_refer_private(ALSAHwdepDeviceInfo *self,
                                     struct snd_hwdep_info **info)
{
    ALSAHwdepDeviceInfoPrivate *priv =
                            alsahwdep_device_info_get_instance_private(self);

    *info = &priv->info;
}
