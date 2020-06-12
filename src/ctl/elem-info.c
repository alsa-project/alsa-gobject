// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <errno.h>

/**
 * SECTION: elem-info
 * @Title: ALSACtlElemInfo
 * @Short_description: An abstract object to represent the common information
 *                     of any type of element
 *
 * A #ALSACtlElemInfo is an abstract object to represent the common information
 * of any type of element.
 *
 * The object wraps 'struct snd_ctl_elem_info' in UAPI of Linux sound subsystem.
 */
struct _ALSACtlElemInfoPrivate {
    struct snd_ctl_elem_info info;

    struct {
        gint32 min;
        gint32 max;
        gint32 step;
    } int_data;
    gchar **enum_data;
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

static void ctl_elem_info_finalize(GObject *obj)
{
    ALSACtlElemInfo *self = ALSACTL_ELEM_INFO(obj);
    ALSACtlElemInfoPrivate *priv = alsactl_elem_info_get_instance_private(self);

    g_strfreev(priv->enum_data);

    G_OBJECT_CLASS(alsactl_elem_info_parent_class)->finalize(obj);
}

static void alsactl_elem_info_class_init(ALSACtlElemInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_set_property;
    gobject_class->get_property = ctl_elem_info_get_property;
    gobject_class->finalize = ctl_elem_info_finalize;

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
    ALSACtlElemInfoPrivate *priv = alsactl_elem_info_get_instance_private(self);

    priv->enum_data = NULL;
}

/**
 * alsactl_elem_info_get_int_data:
 * @self: A #ALSACtlElemInfo.
 * @data: (array fixed-size=3)(out)(transfer none): The array with elements for
 *        the data of integer element; minimum value, maximum value, and value
 *        step in the order.
 * @error: A #GError.
 *
 * Refer to the array with elements for the data of integer element; minimum
 * value, maximum value, and value step in the order. The call of function is
 * successful as long as the information is for integer type.
 */
void alsactl_elem_info_get_int_data(ALSACtlElemInfo *self,
                                    const gint32 *data[3], GError **error)
{
    ALSACtlElemInfoPrivate *priv;

    g_return_if_fail(ALSACTL_IS_ELEM_INFO(self));
    priv = alsactl_elem_info_get_instance_private(self);

    if (priv->info.type != SNDRV_CTL_ELEM_TYPE_INTEGER) {
        generate_error(error, ENXIO);
        return;
    }

    priv->int_data.min = (gint32)priv->info.value.integer.min;
    priv->int_data.max = (gint32)priv->info.value.integer.max;
    priv->int_data.step = (gint32)priv->info.value.integer.step;

    *data = (const gint32 *)&priv->int_data;
}

/**
 * alsactl_elem_info_set_int_data:
 * @self: A #ALSACtlElemInfo.
 * @data: (array fixed-size=3)(transfer none): The array with elements for
 *        the data of integer element; minimum value, maximum value, and value
 *        step in the order.
 * @error: A #GError.
 *
 * Get the array with elements for the data of integer element; minimum value,
 * maximum value, and value step in the order. The call of function is
 * successful as long as the information is for integer type.
 */
void alsactl_elem_info_set_int_data(ALSACtlElemInfo *self,
                                    const gint32 data[3], GError **error)
{
    ALSACtlElemInfoPrivate *priv;

    g_return_if_fail(ALSACTL_IS_ELEM_INFO(self));
    priv = alsactl_elem_info_get_instance_private(self);

    if (priv->info.type != SNDRV_CTL_ELEM_TYPE_INTEGER) {
        generate_error(error, ENXIO);
        return;
    }

    priv->info.value.integer.min = (long)data[0];
    priv->info.value.integer.max = (long)data[1];
    priv->info.value.integer.step = (long)data[2];
}

/**
 * alsactl_elem_info_get_int64_data:
 * @self: A #ALSACtlElemInfo.
 * @data: (array fixed-size=3)(out)(transfer none): The array with elements for
 *        the data of integer64 element; minimum value, maximum value, and value
 *        step in the order.
 * @error: A #GError.
 *
 * Refer to the array with elements for the data of integer64 element; minimum
 * value, maximum value, and value step in the order. The call of function is
 * successful as long as the information is for integer64 type.
 */
void alsactl_elem_info_get_int64_data(ALSACtlElemInfo *self,
                                      const gint64 *data[3], GError **error)
{
    ALSACtlElemInfoPrivate *priv;

    g_return_if_fail(ALSACTL_IS_ELEM_INFO(self));
    priv = alsactl_elem_info_get_instance_private(self);

    if (priv->info.type != SNDRV_CTL_ELEM_TYPE_INTEGER64) {
        generate_error(error, ENXIO);
        return;
    }

    priv->int_data.min = (gint64)priv->info.value.integer.min;
    priv->int_data.max = (gint64)priv->info.value.integer.max;
    priv->int_data.step = (gint64)priv->info.value.integer.step;

    *data = (const gint64 *)&priv->info.value.integer64;
}

/**
 * alsactl_elem_info_set_int64_data:
 * @self: A #ALSACtlElemInfo.
 * @data: (array fixed-size=3)(transfer none): The array with elements for
 *        the data of integer64 element; minimum value, maximum value, and value
 *        step in the order.
 * @error: A #GError.
 *
 * Get the array with elements for the data of integer64 element; minimum value,
 * maximum value, and value step in the order. The call of function is
 * successful as long as the information is for integer64 type.
 */
void alsactl_elem_info_set_int64_data(ALSACtlElemInfo *self,
                                      const gint64 data[3], GError **error)
{
    ALSACtlElemInfoPrivate *priv;

    g_return_if_fail(ALSACTL_IS_ELEM_INFO(self));
    priv = alsactl_elem_info_get_instance_private(self);

    if (priv->info.type != SNDRV_CTL_ELEM_TYPE_INTEGER64) {
        generate_error(error, ENXIO);
        return;
    }

    priv->info.value.integer.min = (long long)data[0];
    priv->info.value.integer.max = (long long)data[1];
    priv->info.value.integer.step = (long long)data[2];
}

/**
 * alsactl_elem_info_get_enum_data:
 * @self: A #ALSACtlElemInfo.
 * @data: (array zero-terminated=1)(out)(transfer none): The array with elements
 *        for the label entries of enumerated element.
 * @error: A #GError.
 *
 * Refer to the array with elements for the label entries of enumerated element
 * in internal storage. The call of function is successful as long as the
 * information is for enumerated type.
 */
void alsactl_elem_info_get_enum_data(ALSACtlElemInfo *self,
                                     const gchar ***data, GError **error)
{
    ALSACtlElemInfoPrivate *priv;

    g_return_if_fail(ALSACTL_IS_ELEM_INFO(self));
    priv = alsactl_elem_info_get_instance_private(self);

    if (priv->info.type != SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
        generate_error(error, ENXIO);
        return;
    }

    *data = (const gchar **)priv->enum_data;
}

/**
 * alsactl_elem_info_set_enum_data:
 * @self: A #ALSACtlElemInfo.
 * @data: (array zero-terminated=1): The array with elements for the label
 *        entries of enumerated element.
 * @error: A #GError.
 *
 * Copy the array with elements for the label entries of enumerated element
 * into internal storage. The call of function is successful as long as the
 * information is for enumerated type.
 */
void alsactl_elem_info_set_enum_data(ALSACtlElemInfo *self,
                                     const gchar **data, GError **error)
{
    ALSACtlElemInfoPrivate *priv;

    g_return_if_fail(ALSACTL_IS_ELEM_INFO(self));
    priv = alsactl_elem_info_get_instance_private(self);

    if (priv->info.type != SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
        generate_error(error, ENXIO);
        return;
    }

    g_strfreev(priv->enum_data);

    priv->enum_data = g_strdupv((gchar **)data);
}

void ctl_elem_info_refer_private(ALSACtlElemInfo *self,
                                 struct snd_ctl_elem_info **info)
{
    ALSACtlElemInfoPrivate *priv = alsactl_elem_info_get_instance_private(self);

    *info = &priv->info;
}
