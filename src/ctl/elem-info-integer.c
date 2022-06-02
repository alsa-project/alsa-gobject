// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoInteger:
 * An object to express information for integer type of element.
 *
 * A [class@GObject.Object] derived object class for integer type of element.
 *
 * The object wraps `struct snd_ctl_elem_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_ctl_elem_info data;
} ALSACtlElemInfoIntegerPrivate;

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface);
static void elem_info_single_array_iface_init(ALSACtlElemInfoSingleArrayInterface *iface);

G_DEFINE_TYPE_WITH_CODE(ALSACtlElemInfoInteger, alsactl_elem_info_integer, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(ALSACtlElemInfoInteger)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_COMMON,
                                              elem_info_common_iface_init)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_SINGLE_ARRAY,
                                              elem_info_single_array_iface_init))

enum elem_info_integer_prop_type {
    ELEM_INFO_INTEGER_PROP_MIN = ELEM_INFO_SINGLE_ARRAY_PROP_COUNT,
    ELEM_INFO_INTEGER_PROP_MAX,
    ELEM_INFO_INTEGER_PROP_STEP,
    ELEM_INFO_INTEGER_PROP_COUNT,
};

static void ctl_elem_info_integer_set_property(GObject *obj, guint id, const GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoInteger *self = ALSACTL_ELEM_INFO_INTEGER(obj);
    ALSACtlElemInfoIntegerPrivate *priv = alsactl_elem_info_integer_get_instance_private(self);
    struct snd_ctl_elem_info *data = &priv->data;

    switch (id) {
    case ELEM_INFO_INTEGER_PROP_MIN:
        data->value.integer.min = (long)g_value_get_int(val);
        break;
    case ELEM_INFO_INTEGER_PROP_MAX:
        data->value.integer.max = (long)g_value_get_int(val);
        break;
    case ELEM_INFO_INTEGER_PROP_STEP:
        data->value.integer.step = (long)g_value_get_int(val);
        break;
    default:
        elem_info_single_array_set_property(data, obj, id, val, spec);
        break;
    }
}

static void ctl_elem_info_integer_get_property(GObject *obj, guint id, GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoInteger *self = ALSACTL_ELEM_INFO_INTEGER(obj);
    ALSACtlElemInfoIntegerPrivate *priv = alsactl_elem_info_integer_get_instance_private(self);
    const struct snd_ctl_elem_info *data = &priv->data;

    switch (id) {
    case ELEM_INFO_INTEGER_PROP_MIN:
        g_value_set_int(val, (int)data->value.integer.min);
        break;
    case ELEM_INFO_INTEGER_PROP_MAX:
        g_value_set_int(val, (int)data->value.integer.max);
        break;
    case ELEM_INFO_INTEGER_PROP_STEP:
        g_value_set_int(val, (int)data->value.integer.step);
        break;
    default:
        elem_info_single_array_get_property(data, obj, id, val, spec);
        break;
    }
}

static void alsactl_elem_info_integer_class_init(ALSACtlElemInfoIntegerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_integer_set_property;
    gobject_class->get_property = ctl_elem_info_integer_get_property;

    elem_info_single_array_class_override_properties(gobject_class);

    /**
     * ALSACtlElemInfoInteger:value-min:
     *
     * The minimum value of element in value array for the element.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, ELEM_INFO_INTEGER_PROP_MIN,
        g_param_spec_int("value-min", "value-min",
                         "The minimum value of element in value array for the element",
                         G_MININT, G_MAXINT, 0,
                         G_PARAM_READWRITE));

    /**
     * ALSACtlElemInfoInteger:value-max:
     *
     * The maximum value of element in value array for the element.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, ELEM_INFO_INTEGER_PROP_MAX,
        g_param_spec_int("value-max", "value-max",
                         "The maximum value of element in value array for the element",
                         G_MININT, G_MAXINT, 0,
                         G_PARAM_READWRITE));

    /**
     * ALSACtlElemInfoInteger:value-step:
     *
     * The step value of element in value array for the element.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, ELEM_INFO_INTEGER_PROP_STEP,
        g_param_spec_int("value-step", "value-step",
                         "The step value of element in value array for the element",
                         G_MININT, G_MAXINT, 0,
                         G_PARAM_READWRITE));
}

static void alsactl_elem_info_integer_init(ALSACtlElemInfoInteger *self)
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
 * alsactl_elem_info_integer_new:
 *
 * Allocate and return an instance of [class@ElemInfoInteger].
 *
 * Returns: An instance of [class@ElemInfoInteger].
 */
ALSACtlElemInfoInteger *alsactl_elem_info_integer_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_INTEGER, ELEM_TYPE_PROP_NAME, ALSACTL_ELEM_TYPE_INTEGER,
                        NULL);
}

void ctl_elem_info_integer_refer_private(ALSACtlElemInfoInteger *self,
                                         struct snd_ctl_elem_info **data)
{
    ALSACtlElemInfoIntegerPrivate *priv = alsactl_elem_info_integer_get_instance_private(self);

    *data = &priv->data;
}
