// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

struct _ALSACtlElemValuePrivate {
    struct snd_ctl_elem_value value;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSACtlElemValue, alsactl_elem_value, G_TYPE_OBJECT)

enum ctl_elem_value_prop_type {
    CTL_ELEM_VALUE_PROP_ELEM_ID = 1,
    CTL_ELEM_VALUE_PROP_COUNT,
};
static GParamSpec *ctl_elem_value_props[CTL_ELEM_VALUE_PROP_COUNT] = { NULL, };

static void ctl_elem_value_get_property(GObject *obj, guint id, GValue *val,
                                        GParamSpec *spec)
{
    ALSACtlElemValue *self = ALSACTL_ELEM_VALUE(obj);
    ALSACtlElemValuePrivate *priv =
                                alsactl_elem_value_get_instance_private(self);

    switch (id) {
    case CTL_ELEM_VALUE_PROP_ELEM_ID:
    {
        g_value_set_static_boxed(val, (const ALSACtlElemId *)&priv->value.id);
        break;
    }
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsactl_elem_value_class_init(ALSACtlElemValueClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = ctl_elem_value_get_property;

    ctl_elem_value_props[CTL_ELEM_VALUE_PROP_ELEM_ID] =
        g_param_spec_boxed("elem-id", "elem-id",
                           "The identifier of element",
                           ALSACTL_TYPE_ELEM_ID,
                           G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class, CTL_ELEM_VALUE_PROP_COUNT,
                                      ctl_elem_value_props);
}

static void alsactl_elem_value_init(ALSACtlElemValue *self)
{
    return;
}

/**
 * alsactl_elem_value_new:
 *
 * Allocate and return an instance of ALSACtlElemValue.
 *
 * Returns: (transfer full): A #ALSACtlElemValue.
 */
ALSACtlElemValue *alsactl_elem_value_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_VALUE, NULL);
}

void ctl_elem_value_refer_private(ALSACtlElemValue *self,
                                  struct snd_ctl_elem_value **value)
{
    ALSACtlElemValuePrivate *priv =
                                alsactl_elem_value_get_instance_private(self);
    *value = &priv->value;
}

/**
 * alsactl_elem_value_set_bool:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count): The array for values of boolean type.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of boolean type into internal data.
 */
void alsactl_elem_value_set_bool(ALSACtlElemValue *self,
                                 const gboolean *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    value_count = MIN(value_count, G_N_ELEMENTS(value->value.integer.value));
    for (i = 0; i < value_count; ++i)
        value->value.integer.value[i] = (long)values[i];
}

/**
 * alsactl_elem_value_get_bool:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count)(inout): The array for values of boolean
 *          type.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of boolean type from internal data.
 */
void alsactl_elem_value_get_bool(ALSACtlElemValue *self,
                                 gboolean *const *values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    *value_count = MIN(*value_count, G_N_ELEMENTS(value->value.integer.value));
    for (i = 0; i < *value_count; ++i)
        (*values)[i] = (gboolean)value->value.integer.value[i];
}
