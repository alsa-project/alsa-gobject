// SPDX-License-Identifier: LGPL-3.0-or-later
#include "elem-info-enum.h"
#include "privates.h"

/**
 * SECTION: elem-info-enum
 * @Title: ALSACtlElemInfoEnum
 * @Short_description: A GObject-derived object to represent the information
 *                     of enumeration type of element
 *
 * A #ALSACtlElemInfoEnum is a GObject-derived object to represent the
 * information of enumeration type of element. The object inherits methods and
 * properties of #ALSACtlElemInfo. The call of alsactl_card_get_elem_info() can
 * returns the instance of object, or The call of alsactl_card_add_elems()
 * requires the instance of object as argument.
 */
struct _ALSACtlElemInfoEnumPrivate {
    const gchar **labels;     // should have sentinel member with NULL value.
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSACtlElemInfoEnum, alsactl_elem_info_enum, ALSACTL_TYPE_ELEM_INFO)

enum ctl_elem_info_enum_prop_type {
    CTL_ELEM_INFO_ENUM_PROP_VALUE_COUNT = 1,
    CTL_ELEM_INFO_ENUM_PROP_LABELS,
    CTL_ELEM_INFO_ENUM_PROP_COUNT,
};
static GParamSpec *ctl_elem_info_enum_props[CTL_ELEM_INFO_ENUM_PROP_COUNT] = { NULL, };

static void ctl_elem_info_enum_finalize(GObject *obj)
{
    ALSACtlElemInfoEnum *self = ALSACTL_ELEM_INFO_ENUM(obj);
    ALSACtlElemInfoEnumPrivate *priv =
                            alsactl_elem_info_enum_get_instance_private(self);

    g_strfreev((gchar **)priv->labels);

    G_OBJECT_CLASS(alsactl_elem_info_enum_parent_class)->finalize(obj);
}

static void ctl_elem_info_enum_set_property(GObject *obj, guint id,
                                            const GValue *val, GParamSpec *spec)
{
    ALSACtlElemInfoEnum *self = ALSACTL_ELEM_INFO_ENUM(obj);
    ALSACtlElemInfoEnumPrivate *priv =
                            alsactl_elem_info_enum_get_instance_private(self);
    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_ENUM_PROP_VALUE_COUNT:
        info->count = g_value_get_uint(val);
        break;
    case CTL_ELEM_INFO_ENUM_PROP_LABELS:
        g_strfreev((gchar **)priv->labels);
        priv->labels = (const gchar **)g_strdupv(g_value_get_boxed(val));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void ctl_elem_info_enum_get_property(GObject *obj, guint id,
                                            GValue *val, GParamSpec *spec)
{
    ALSACtlElemInfoEnum *self = ALSACTL_ELEM_INFO_ENUM(obj);
    ALSACtlElemInfoEnumPrivate *priv =
                            alsactl_elem_info_enum_get_instance_private(self);

    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_ENUM_PROP_VALUE_COUNT:
        g_value_set_uint(val, info->count);
        break;
    case CTL_ELEM_INFO_ENUM_PROP_LABELS:
        g_value_set_boxed(val, priv->labels);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsactl_elem_info_enum_class_init(ALSACtlElemInfoEnumClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = ctl_elem_info_enum_finalize;
    gobject_class->set_property = ctl_elem_info_enum_set_property;
    gobject_class->get_property = ctl_elem_info_enum_get_property;

    ctl_elem_info_enum_props[CTL_ELEM_INFO_ENUM_PROP_VALUE_COUNT] =
        g_param_spec_uint("value-count", "value-count",
                          "The number of members as value of this element",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READWRITE);

    ctl_elem_info_enum_props[CTL_ELEM_INFO_ENUM_PROP_LABELS] =
        g_param_spec_boxed("labels", "labels",
                           "The list of labels for this element",
                           G_TYPE_STRV,
                           G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                    CTL_ELEM_INFO_ENUM_PROP_COUNT, ctl_elem_info_enum_props);
}

static void alsactl_elem_info_enum_init(ALSACtlElemInfoEnum *self)
{
    return;
}

/**
 * alsactl_elem_info_enum_new:
 *
 * Allocate and return an instance of ALSACtlElemInfoEnum.
 *
 * Returns: A #ALSACtlElemInfoEnum.
 */
ALSACtlElemInfoEnum *alsactl_elem_info_enum_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_ENUM,
                        "type", ALSACTL_ELEM_TYPE_ENUMERATED, NULL);
}
