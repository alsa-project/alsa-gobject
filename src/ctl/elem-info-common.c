// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoCommon:
 * An interface to express common features of element information.
 *
 * A [iface@ElemInfoCommon] should be implemented by any type of element information.
 *
 * Since: 0.3
 */

static void alsactl_elem_info_common_default_init(ALSACtlElemInfoCommonInterface *iface);

G_DEFINE_INTERFACE(ALSACtlElemInfoCommon, alsactl_elem_info_common, G_TYPE_OBJECT)

static void alsactl_elem_info_common_default_init(ALSACtlElemInfoCommonInterface *iface)
{
    /**
     * ALSACtlElemInfoCommon:elem-id:
     *
     * The identifier of element.
     *
     * Since: 0.3
     */
    g_object_interface_install_property(iface,
        g_param_spec_boxed(ELEM_ID_PROP_NAME, ELEM_ID_PROP_NAME,
                           "The identifier of element",
                           ALSACTL_TYPE_ELEM_ID,
                           G_PARAM_READABLE));

    /**
     * ALSACtlElemInfoCommon:elem-type:
     *
     * The type of element, one of [enum@ElemType].
     *
     * Since: 0.3
     */
    g_object_interface_install_property(iface,
        g_param_spec_enum(ELEM_TYPE_PROP_NAME, ELEM_TYPE_PROP_NAME,
                          "The type for element, one of ALSACtlElemType",
                          ALSACTL_TYPE_ELEM_TYPE,
                          ALSACTL_ELEM_TYPE_NONE,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    /**
     * ALSACtlElemInfoCommon:access:
     *
     * The access permission for the element with [flags@ElemAccessFlag].
     *
     * Since: 0.3
     */
    g_object_interface_install_property(iface,
        g_param_spec_flags(ACCESS_PROP_NAME, ACCESS_PROP_NAME,
                           "The access permission for the element with ALSACtlElemAccessFlag",
                           ALSACTL_TYPE_ELEM_ACCESS_FLAG,
                           ALSACTL_ELEM_ACCESS_FLAG_READ,
                           G_PARAM_READWRITE));

    /**
     * ALSACtlElemInfoCommon:owner:
     *
     * The value of PID for process to own the element.
     *
     * Since: 0.3
     */
    g_object_interface_install_property(iface,
        g_param_spec_int(OWNER_PROP_NAME, OWNER_PROP_NAME,
                         "The value of PID for process to own the element",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READABLE));
}

void elem_info_common_class_override_properties(GObjectClass *gobject_class)
{
    g_object_class_override_property(gobject_class, ELEM_INFO_COMMON_PROP_ELEM_ID,
                                     ELEM_ID_PROP_NAME);

    g_object_class_override_property(gobject_class, ELEM_INFO_COMMON_PROP_ELEM_TYPE,
                                     ELEM_TYPE_PROP_NAME);

    g_object_class_override_property(gobject_class, ELEM_INFO_COMMON_PROP_ACCESS,
                                     ACCESS_PROP_NAME);

    g_object_class_override_property(gobject_class, ELEM_INFO_COMMON_PROP_OWNER,
                                     OWNER_PROP_NAME);
}

void elem_info_common_set_property(struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                   const GValue *val, GParamSpec *spec)
{
    switch (id) {
    case ELEM_INFO_COMMON_PROP_ELEM_ID:
    {
        const struct snd_ctl_elem_id *src = g_value_get_boxed(val);
        data->id = *src;
        break;
    }
    case ELEM_INFO_COMMON_PROP_ELEM_TYPE:
        data->type = (snd_ctl_elem_type_t)g_value_get_enum(val);
        break;
    case ELEM_INFO_COMMON_PROP_ACCESS:
        data->access = (unsigned int)g_value_get_flags(val);
        break;
    case ELEM_INFO_COMMON_PROP_OWNER:
        data->owner = (__kernel_pid_t)g_value_get_int(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

void elem_info_common_get_property(const struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                   GValue *val, GParamSpec *spec)
{
    switch (id) {
    case ELEM_INFO_COMMON_PROP_ELEM_ID:
        g_value_set_static_boxed(val, &data->id);
        break;
    case ELEM_INFO_COMMON_PROP_ELEM_TYPE:
        g_value_set_enum(val, (ALSACtlElemType)data->type);
        break;
    case ELEM_INFO_COMMON_PROP_ACCESS:
        g_value_set_flags(val, data->access);
        break;
    case ELEM_INFO_COMMON_PROP_OWNER:
        g_value_set_int(val, data->owner);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}
