// SPDX-License-Identifier: LGPL-3.0-or-later
#include "elem-info-bool.h"
#include "privates.h"

G_DEFINE_TYPE(ALSACtlElemInfoBool, alsactl_elem_info_bool, ALSACTL_TYPE_ELEM_INFO)

enum ctl_elem_info_bool_prop_type {
    CTL_ELEM_INFO_BOOL_PROP_VALUE_COUNT = 1,
    CTL_ELEM_INFO_BOOL_PROP_COUNT,
};
static GParamSpec *ctl_elem_info_bool_props[CTL_ELEM_INFO_BOOL_PROP_COUNT] = { NULL, };

static void ctl_elem_info_bool_set_property(GObject *obj, guint id,
                                       const GValue *val, GParamSpec *spec)
{
    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_BOOL_PROP_VALUE_COUNT:
        info->count = g_value_get_uint(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}
static void ctl_elem_info_bool_get_property(GObject *obj, guint id, GValue *val,
                                            GParamSpec *spec)
{
    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_BOOL_PROP_VALUE_COUNT:
        g_value_set_uint(val, info->count);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}


static void alsactl_elem_info_bool_class_init(ALSACtlElemInfoBoolClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_bool_set_property;
    gobject_class->get_property = ctl_elem_info_bool_get_property;

    ctl_elem_info_bool_props[CTL_ELEM_INFO_BOOL_PROP_VALUE_COUNT] =
        g_param_spec_uint("value-count", "value-count",
                          "The number of members as value of this element",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      CTL_ELEM_INFO_BOOL_PROP_COUNT,
                                      ctl_elem_info_bool_props);
}

static void alsactl_elem_info_bool_init(ALSACtlElemInfoBool *self)
{
    return;
}

/**
 * alsactl_elem_info_bool_new:
 *
 * Allocate and return an instance of ALSACtlElemInfoBool.
 *
 * Returns: (transfer full): A #ALSACtlElemInfoBool.
 */
ALSACtlElemInfoBool *alsactl_elem_info_bool_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_BOOL,
                        "type", ALSACTL_ELEM_TYPE_BOOLEAN, NULL);
}
