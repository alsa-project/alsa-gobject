// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoInteger64:
 * An object to express information for integer64 type of element.
 *
 * A [class@GObject.Object] derived object class for integer64 type of element.
 *
 * The object wraps `struct snd_ctl_elem_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_ctl_elem_info data;
} ALSACtlElemInfoInteger64Private;

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface);
static void elem_info_single_array_iface_init(ALSACtlElemInfoSingleArrayInterface *iface);

G_DEFINE_TYPE_WITH_CODE(ALSACtlElemInfoInteger64, alsactl_elem_info_integer64, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(ALSACtlElemInfoInteger64)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_COMMON,
                                              elem_info_common_iface_init)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_SINGLE_ARRAY,
                                              elem_info_single_array_iface_init))

enum elem_info_integer64_prop_type {
    ELEM_INFO_INTEGER64_PROP_MIN = ELEM_INFO_SINGLE_ARRAY_PROP_COUNT,
    ELEM_INFO_INTEGER64_PROP_MAX,
    ELEM_INFO_INTEGER64_PROP_STEP,
    ELEM_INFO_INTEGER64_PROP_COUNT,
};

static void ctl_elem_info_integer64_set_property(GObject *obj, guint id, const GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoInteger64 *self = ALSACTL_ELEM_INFO_INTEGER64(obj);
    ALSACtlElemInfoInteger64Private *priv = alsactl_elem_info_integer64_get_instance_private(self);
    struct snd_ctl_elem_info *data = &priv->data;

    switch (id) {
    case ELEM_INFO_INTEGER64_PROP_MIN:
        data->value.integer64.min = g_value_get_int64(val);
        break;
    case ELEM_INFO_INTEGER64_PROP_MAX:
        data->value.integer64.max = g_value_get_int64(val);
        break;
    case ELEM_INFO_INTEGER64_PROP_STEP:
        data->value.integer64.step = g_value_get_int64(val);
        break;
    default:
        elem_info_single_array_set_property(data, obj, id, val, spec);
        break;
    }
}

static void ctl_elem_info_integer64_get_property(GObject *obj, guint id, GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoInteger64 *self = ALSACTL_ELEM_INFO_INTEGER64(obj);
    ALSACtlElemInfoInteger64Private *priv = alsactl_elem_info_integer64_get_instance_private(self);
    const struct snd_ctl_elem_info *data = &priv->data;

    switch (id) {
    case ELEM_INFO_INTEGER64_PROP_MIN:
        g_value_set_int64(val, data->value.integer64.min);
        break;
    case ELEM_INFO_INTEGER64_PROP_MAX:
        g_value_set_int64(val, data->value.integer64.max);
        break;
    case ELEM_INFO_INTEGER64_PROP_STEP:
        g_value_set_int64(val, data->value.integer64.step);
        break;
    default:
        elem_info_single_array_get_property(data, obj, id, val, spec);
        break;
    }
}

static void alsactl_elem_info_integer64_class_init(ALSACtlElemInfoInteger64Class *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_integer64_set_property;
    gobject_class->get_property = ctl_elem_info_integer64_get_property;

    elem_info_single_array_class_override_properties(gobject_class);

    /**
     * ALSACtlElemInfoInteger64:value-min:
     *
     * The minimum value of element in value array for the element.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, ELEM_INFO_INTEGER64_PROP_MIN,
        g_param_spec_int64("value-min", "value-min",
                           "The minimum value of element in value array for the element",
                           G_MININT64, G_MAXINT64, 0,
                           G_PARAM_READWRITE));

    /**
     * ALSACtlElemInfoInteger64:value-max:
     *
     * The maximum value of element in value array for the element.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, ELEM_INFO_INTEGER64_PROP_MAX,
        g_param_spec_int64("value-max", "value-max",
                           "The maximum value of element in value array for the element",
                           G_MININT64, G_MAXINT64, 0,
                           G_PARAM_READWRITE));

    /**
     * ALSACtlElemInfoInteger64:value-step:
     *
     * The step value of element in value array for the element.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, ELEM_INFO_INTEGER64_PROP_STEP,
        g_param_spec_int64("value-step", "value-step",
                           "The step value of element in value array for the element",
                           G_MININT64, G_MAXINT64, 0,
                           G_PARAM_READWRITE));
}

static void alsactl_elem_info_integer64_init(ALSACtlElemInfoInteger64 *self)
{
    return;
}

static void elem_info_single_array_iface_init(ALSACtlElemInfoSingleArrayInterface *iface)
{
    return;
}

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface)
{
    return;
}

/**
 * alsactl_elem_info_integer64_new:
 *
 * Allocate and return an instance of [class@ElemInfoInteger64].
 *
 * Returns: An instance of [class@ElemInfoInteger64].
 */
ALSACtlElemInfoInteger64 *alsactl_elem_info_integer64_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_INTEGER64,
                        ELEM_TYPE_PROP_NAME, ALSACTL_ELEM_TYPE_INTEGER64, NULL);
}
