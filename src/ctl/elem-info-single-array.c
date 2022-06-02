// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoSingleArray:
 * An interface to express information of element which has single value array.
 *
 * A [iface@ElemInfoSingleArray] should be implemented by the type of information for element
 * whieh has single value array.
 *
 * Since: 0.3.
 */

static void alsactl_elem_info_single_array_default_init(ALSACtlElemInfoSingleArrayInterface *iface);

G_DEFINE_INTERFACE(ALSACtlElemInfoSingleArray, alsactl_elem_info_single_array,
                   ALSACTL_TYPE_ELEM_INFO_COMMON)

static void alsactl_elem_info_single_array_default_init(ALSACtlElemInfoSingleArrayInterface *iface)
{
    /**
     * ALSACtlElemInfoSingleArray:value-count:
     *
     * The count of elements in value array of the element.
     *
     * Since: 0.3.
     */
    g_object_interface_install_property(iface,
        g_param_spec_uint(VALUE_COUNT_PROP_NAME, VALUE_COUNT_PROP_NAME,
                          "The count of elements in value array of the element",
                          0, G_MAXUINT,
                          1,
                          G_PARAM_READWRITE));
}

void elem_info_single_array_class_override_properties(GObjectClass *gobject_class)
{
    elem_info_common_class_override_properties(gobject_class);

    g_object_class_override_property(gobject_class, ELEM_INFO_SINGLE_ARRAY_PROP_VALUE_COUNT,
                                     VALUE_COUNT_PROP_NAME);
}

void elem_info_single_array_set_property(struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                         const GValue *val, GParamSpec *spec)
{
    switch (id) {
    case ELEM_INFO_SINGLE_ARRAY_PROP_VALUE_COUNT:
        data->count = g_value_get_uint(val);
        break;
    default:
        elem_info_common_set_property(data, obj, id, val, spec);
        break;
    }
}

void elem_info_single_array_get_property(const struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                         GValue *val, GParamSpec *spec)
{
    switch (id) {
    case ELEM_INFO_SINGLE_ARRAY_PROP_VALUE_COUNT:
        g_value_set_uint(val, data->count);
        break;
    default:
        elem_info_common_get_property(data, obj, id, val, spec);
        break;
    }
}
