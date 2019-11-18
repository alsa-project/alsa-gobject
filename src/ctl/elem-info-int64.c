// SPDX-License-Identifier: LGPL-3.0-or-later
#include "elem-info-int64.h"
#include "privates.h"

G_DEFINE_TYPE(ALSACtlElemInfoInt64, alsactl_elem_info_int64, ALSACTL_TYPE_ELEM_INFO)

enum ctl_elem_info_int64_prop_type {
    CTL_ELEM_INFO_INT64_PROP_VALUE_COUNT = 1,
    CTL_ELEM_INFO_INT64_PROP_MIN,
    CTL_ELEM_INFO_INT64_PROP_MAX,
    CTL_ELEM_INFO_INT64_PROP_STEP,
    CTL_ELEM_INFO_INT64_PROP_COUNT,
};
static GParamSpec *ctl_elem_info_int64_props[CTL_ELEM_INFO_INT64_PROP_COUNT] = { NULL, };

static void ctl_elem_info_int64_set_property(GObject *obj, guint id,
                                           const GValue *val, GParamSpec *spec)
{
    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_INT64_PROP_VALUE_COUNT:
        info->count = g_value_get_uint(val);
        break;
    case CTL_ELEM_INFO_INT64_PROP_MIN:
        info->value.integer64.min = g_value_get_int64(val);
        break;
    case CTL_ELEM_INFO_INT64_PROP_MAX:
        info->value.integer64.min = g_value_get_int64(val);
        break;
    case CTL_ELEM_INFO_INT64_PROP_STEP:
        info->value.integer64.min = g_value_get_int64(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void ctl_elem_info_int64_get_property(GObject *obj, guint id,
                                             GValue *val, GParamSpec *spec)
{
    ALSACtlElemInfo *parent = ALSACTL_ELEM_INFO(obj);
    struct snd_ctl_elem_info *info;

    ctl_elem_info_refer_private(parent, &info);

    switch (id) {
    case CTL_ELEM_INFO_INT64_PROP_VALUE_COUNT:
        g_value_set_uint(val, info->count);
        break;
    case CTL_ELEM_INFO_INT64_PROP_MIN:
        g_value_set_int64(val, info->value.integer64.min);
        break;
    case CTL_ELEM_INFO_INT64_PROP_MAX:
        g_value_set_int64(val, info->value.integer64.max);
        break;
    case CTL_ELEM_INFO_INT64_PROP_STEP:
        g_value_set_int64(val, info->value.integer64.step);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsactl_elem_info_int64_class_init(ALSACtlElemInfoInt64Class *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_int64_set_property;
    gobject_class->get_property = ctl_elem_info_int64_get_property;

    ctl_elem_info_int64_props[CTL_ELEM_INFO_INT64_PROP_VALUE_COUNT] =
        g_param_spec_uint("value-count", "value-count",
                          "The number of members as value of this element",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READWRITE);

    ctl_elem_info_int64_props[CTL_ELEM_INFO_INT64_PROP_MIN] =
        g_param_spec_int64("min", "min",
                           "The minimum value for data of this element",
                           G_MININT64, G_MAXINT64,
                           0,
                           G_PARAM_READWRITE);

    ctl_elem_info_int64_props[CTL_ELEM_INFO_INT64_PROP_MAX] =
        g_param_spec_int64("max", "max",
                           "The maximum value for data of this element",
                           G_MININT64, G_MAXINT64,
                           0,
                           G_PARAM_READWRITE);

    ctl_elem_info_int64_props[CTL_ELEM_INFO_INT64_PROP_STEP] =
        g_param_spec_int64("step", "step",
                           "The gap of value for data of this element",
                           G_MININT64, G_MAXINT64,
                           0,
                           G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                    CTL_ELEM_INFO_INT64_PROP_COUNT, ctl_elem_info_int64_props);
}

static void alsactl_elem_info_int64_init(ALSACtlElemInfoInt64 *self)
{
    return;
}

/**
 * alsactl_elem_info_int64_new:
 *
 * Allocate and return an instance of ALSACtlElemInfoInt64.
 *
 * Returns: (transfer full): A #ALSACtlElemInfoInt64.
 */
ALSACtlElemInfoInt64 *alsactl_elem_info_int64_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_INT64,
                        "type", ALSACTL_ELEM_TYPE_INTEGER64, NULL);
}