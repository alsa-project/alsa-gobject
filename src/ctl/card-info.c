// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

struct _ALSACtlCardInfoPrivate {
    struct snd_ctl_card_info info;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSACtlCardInfo, alsactl_card_info, G_TYPE_OBJECT)

enum ctl_card_info_prop_type {
    CTL_CARD_INFO_PROP_CARD_ID = 1,
    CTL_CARD_INFO_PROP_ID,
    CTL_CARD_INFO_PROP_DRIVER,
    CTL_CARD_INFO_PROP_NAME,
    CTL_CARD_INFO_PROP_LONG_NAME,
    CTL_CARD_INFO_PROP_MIXER_NAME,
    CTL_CARD_INFO_PROP_COMPONENTS,
    CTL_CARD_INFO_PROP_COUNT,
};
static GParamSpec *ctl_card_info_props[CTL_CARD_INFO_PROP_COUNT] = { NULL, };

static void ctl_card_info_get_property(GObject *obj, guint id, GValue *val,
                                       GParamSpec *spec)
{
    ALSACtlCardInfo *self = ALSACTL_CARD_INFO(obj);
    ALSACtlCardInfoPrivate *priv = alsactl_card_info_get_instance_private(self);

    switch (id) {
    case CTL_CARD_INFO_PROP_CARD_ID:
        g_value_set_int(val, priv->info.card);
        break;
    case CTL_CARD_INFO_PROP_ID:
        g_value_set_static_string(val, (gchar *)priv->info.id);
        break;
    case CTL_CARD_INFO_PROP_DRIVER:
        g_value_set_static_string(val, (gchar *)priv->info.driver);
        break;
    case CTL_CARD_INFO_PROP_NAME:
        g_value_set_static_string(val, (gchar *)priv->info.name);
        break;
    case CTL_CARD_INFO_PROP_LONG_NAME:
        g_value_set_static_string(val, (gchar *)priv->info.longname);
        break;
    case CTL_CARD_INFO_PROP_MIXER_NAME:
        g_value_set_static_string(val, (gchar *)priv->info.mixername);
        break;
    case CTL_CARD_INFO_PROP_COMPONENTS:
        g_value_set_static_string(val, (gchar *)priv->info.components);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsactl_card_info_class_init(ALSACtlCardInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = ctl_card_info_get_property;

    ctl_card_info_props[CTL_CARD_INFO_PROP_CARD_ID] =
        g_param_spec_int("card-id", "card-id",
                         "The numerical ID of sound card.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READABLE);

    ctl_card_info_props[CTL_CARD_INFO_PROP_ID] =
        g_param_spec_string("id", "id",
                            "The string ID of sound card.",
                            "",
                            G_PARAM_READABLE);

    ctl_card_info_props[CTL_CARD_INFO_PROP_DRIVER] =
        g_param_spec_string("driver", "driver",
                            "The name of driver bound to the sound card.",
                            "",
                            G_PARAM_READABLE);

    ctl_card_info_props[CTL_CARD_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of sound card.",
                            "",
                            G_PARAM_READABLE);

    ctl_card_info_props[CTL_CARD_INFO_PROP_LONG_NAME] =
        g_param_spec_string("long-name", "long-name",
                            "The long name of sound card.",
                            "",
                            G_PARAM_READABLE);

    ctl_card_info_props[CTL_CARD_INFO_PROP_MIXER_NAME] =
        g_param_spec_string("mixer-name", "mixer-name",
                            "The name of mixer for the sound card.",
                            "",
                            G_PARAM_READABLE);

    ctl_card_info_props[CTL_CARD_INFO_PROP_COMPONENTS] =
        g_param_spec_string("components", "components",
                            "The The string with space-separated components "
                            "for the sound card.",
                            "",
                            G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class, CTL_CARD_INFO_PROP_COUNT,
                                      ctl_card_info_props);
}

static void alsactl_card_info_init(ALSACtlCardInfo *self)
{
    return;
}

void ctl_card_info_refer_private(ALSACtlCardInfo *self,
                                 struct snd_ctl_card_info **info)
{
    ALSACtlCardInfoPrivate *priv = alsactl_card_info_get_instance_private(self);

    *info = &priv->info;
}
