// SPDX-License-Identifier: LGPL-3.0-or-later
#include "elem-info-int.h"
#include "privates.h"

/**
 * SECTION: elem-info-int
 * @Title: ALSACtlElemInfoInt
 * @Short_description: A GObject-derived object to represent the information
 *                     of integer type of element
 *
 * A #ALSACtlElemInfoInt is a GObject-derived object to represent the
 * information of integer type of element, which contains integer values. The
 * object inherits methods and properties of #ALSACtlElemInfo. A call of
 * alsactl_card_get_elem_info() can returns the instance of object, or a call
 * of alsactl_card_add_elems() requires the instance of object as argument.
 */
G_DEFINE_TYPE(ALSACtlElemInfoInt, alsactl_elem_info_int, ALSACTL_TYPE_ELEM_INFO)

enum ctl_elem_info_int_prop_type {
    CTL_ELEM_INFO_INT_PROP_VALUE_COUNT = 1,
    CTL_ELEM_INFO_INT_PROP_MIN,
    CTL_ELEM_INFO_INT_PROP_MAX,
    CTL_ELEM_INFO_INT_PROP_STEP,
    CTL_ELEM_INFO_INT_PROP_COUNT,
};
static GParamSpec *ctl_elem_info_int_props[CTL_ELEM_INFO_INT_PROP_COUNT] = { NULL, };

static void ctl_elem_info_int_set_property(GObject *obj, guint id,
                                           const GValue *val, GParamSpec *spec)
{
    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_INT_PROP_VALUE_COUNT:
        info->count = g_value_get_uint(val);
        break;
    case CTL_ELEM_INFO_INT_PROP_MIN:
        info->value.integer.min = (long)g_value_get_int(val);
        break;
    case CTL_ELEM_INFO_INT_PROP_MAX:
        info->value.integer.max = (long)g_value_get_int(val);
        break;
    case CTL_ELEM_INFO_INT_PROP_STEP:
        info->value.integer.step = (long)g_value_get_int(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void ctl_elem_info_int_get_property(GObject *obj, guint id, GValue *val,
                                           GParamSpec *spec)
{
    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_INT_PROP_VALUE_COUNT:
        g_value_set_uint(val, info->count);
        break;
    case CTL_ELEM_INFO_INT_PROP_MIN:
        g_value_set_int(val, (int)info->value.integer.min);
        break;
    case CTL_ELEM_INFO_INT_PROP_MAX:
        g_value_set_int(val, (int)info->value.integer.max);
        break;
    case CTL_ELEM_INFO_INT_PROP_STEP:
        g_value_set_int(val, (int)info->value.integer.step);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsactl_elem_info_int_class_init(ALSACtlElemInfoIntClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_int_set_property;
    gobject_class->get_property = ctl_elem_info_int_get_property;

    ctl_elem_info_int_props[CTL_ELEM_INFO_INT_PROP_VALUE_COUNT] =
        g_param_spec_uint("value-count", "value-count",
                          "The number of members as value of this element",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READWRITE);

    ctl_elem_info_int_props[CTL_ELEM_INFO_INT_PROP_MIN] =
        g_param_spec_int("min", "min",
                         "The minimum value for data of this element",
                         G_MININT, G_MAXINT,
                         0,
                         G_PARAM_READWRITE);

    ctl_elem_info_int_props[CTL_ELEM_INFO_INT_PROP_MAX] =
        g_param_spec_int("max", "max",
                         "The maximum value for data of this element",
                         G_MININT, G_MAXINT,
                         0,
                         G_PARAM_READWRITE);

    ctl_elem_info_int_props[CTL_ELEM_INFO_INT_PROP_STEP] =
        g_param_spec_int("step", "step",
                         "The gap of value for data of this element",
                         G_MININT, G_MAXINT,
                         0,
                         G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                        CTL_ELEM_INFO_INT_PROP_COUNT, ctl_elem_info_int_props);
}

static void alsactl_elem_info_int_init(ALSACtlElemInfoInt *self)
{
    return;
}

/**
 * alsactl_elem_info_int_new:
 *
 * Allocate and return an instance of ALSACtlElemInfoInt.
 *
 * Returns: A #ALSACtlElemInfoInt.
 */
ALSACtlElemInfoInt *alsactl_elem_info_int_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_INT,
                        "type", ALSACTL_ELEM_TYPE_INTEGER, NULL);
}
