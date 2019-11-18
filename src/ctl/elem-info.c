// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

struct _ALSACtlElemInfoPrivate {
    struct snd_ctl_elem_info info;
};
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(ALSACtlElemInfo, alsactl_elem_info, G_TYPE_OBJECT)

enum ctl_elem_info_prop_type {
    CTL_ELEM_INFO_PROP_ELEM_ID = 1,
    CTL_ELEM_INFO_PROP_TYPE,
    CTL_ELEM_INFO_PROP_ACCESS,
    CTL_ELEM_INFO_PROP_OWNER,
    CTL_ELEM_INFO_PROP_COUNT,
};
static GParamSpec *ctl_elem_info_props[CTL_ELEM_INFO_PROP_COUNT] = { NULL, };

static void ctl_elem_info_set_property(GObject *obj, guint id,
                                       const GValue *val, GParamSpec *spec)
{
    ALSACtlElemInfo *self = ALSACTL_ELEM_INFO(obj);
    ALSACtlElemInfoPrivate *priv = alsactl_elem_info_get_instance_private(self);

    switch (id) {
    case CTL_ELEM_INFO_PROP_TYPE:
        priv->info.type = (ALSACtlElemType)g_value_get_enum(val);
        break;
    case CTL_ELEM_INFO_PROP_ACCESS:
        priv->info.access = (ALSACtlElemAccessFlag)g_value_get_flags(val);
        break;
    case CTL_ELEM_INFO_PROP_OWNER:
         priv->info.owner = g_value_get_int(val);
         break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void ctl_elem_info_get_property(GObject *obj, guint id, GValue *val,
                                       GParamSpec *spec)
{
    ALSACtlElemInfo *self = ALSACTL_ELEM_INFO(obj);
    ALSACtlElemInfoPrivate *priv = alsactl_elem_info_get_instance_private(self);

    switch (id) {
    case CTL_ELEM_INFO_PROP_ELEM_ID:
        g_value_set_static_boxed(val, (const ALSACtlElemId* )&priv->info.id);
        break;
    case CTL_ELEM_INFO_PROP_TYPE:
        g_value_set_enum(val, (ALSACtlElemType)priv->info.type);
        break;
    case CTL_ELEM_INFO_PROP_ACCESS:
        g_value_set_flags(val, priv->info.access);
        break;
    case CTL_ELEM_INFO_PROP_OWNER:
         g_value_set_int(val, priv->info.owner);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsactl_elem_info_class_init(ALSACtlElemInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_set_property;
    gobject_class->get_property = ctl_elem_info_get_property;

    ctl_elem_info_props[CTL_ELEM_INFO_PROP_ELEM_ID] =
        g_param_spec_boxed("elem-id", "elem-id",
                           "The identifier of the element, one of ALSACtlElemId",
                           ALSACTL_TYPE_ELEM_ID,
                           G_PARAM_READABLE);

    ctl_elem_info_props[CTL_ELEM_INFO_PROP_TYPE] =
        g_param_spec_enum("type", "type",
                          "The type for this element, one of ALSACtlElemType",
                          ALSACTL_TYPE_ELEM_TYPE,
                          ALSACTL_ELEM_TYPE_NONE,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    ctl_elem_info_props[CTL_ELEM_INFO_PROP_ACCESS] =
        g_param_spec_flags("access", "access",
                           "The access permission for this element with "
                           "flags of ALSACtlElemAccessFlag.",
                           ALSACTL_TYPE_ELEM_ACCESS_FLAG,
                           ALSACTL_ELEM_ACCESS_FLAG_READ,
                           G_PARAM_READWRITE);

    ctl_elem_info_props[CTL_ELEM_INFO_PROP_OWNER] =
        g_param_spec_int("owner", "owner",
                         "The value of PID for process to own the element",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class, CTL_ELEM_INFO_PROP_COUNT,
                                      ctl_elem_info_props);
}

static void alsactl_elem_info_init(ALSACtlElemInfo *self)
{
    return;
}

void ctl_elem_info_refer_private(ALSACtlElemInfo *self,
                                 struct snd_ctl_elem_info **info)
{
    ALSACtlElemInfoPrivate *priv = alsactl_elem_info_get_instance_private(self);

    *info = &priv->info;
}