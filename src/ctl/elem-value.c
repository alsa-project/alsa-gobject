// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * SECTION: elem-value
 * @Title: ALSACtlElemValue
 * @Short_description: A boxed object to represent the container of array of
 *                     values for any type of element.
 *
 * A #ALSACtlElemValue is boxed object to represent the container of values for
 * any type of element. The arrays of values for each type of element shares the
 * same storage, thus it's important for applications to distinguish the type of
 * element in advance of accessing the array. The object is used for the call of
 * alsactl_card_write_elem_value() and alsactl_card_read_elem_value().
 *
 * The object wraps 'struct snd_ctl_elem_value' in UAPI of Linux sound subsystem.
 */
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
 * Returns: A #ALSACtlElemValue.
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

/**
 * alsactl_elem_value_set_int:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count): The array for values of integer type.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of integer type into internal storage.
 */
void alsactl_elem_value_set_int(ALSACtlElemValue *self, const gint32 *values,
                                gsize value_count)
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
 * alsactl_elem_value_get_int:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count)(inout): The array for values of integer
 *          type.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of integer type from internal storage.
 */
void alsactl_elem_value_get_int(ALSACtlElemValue *self, gint32 *const *values,
                                gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    *value_count = MIN(*value_count, G_N_ELEMENTS(value->value.integer.value));
    for (i = 0; i < *value_count; ++i)
        (*values)[i] = (gint32)value->value.integer.value[i];
}

/**
 * alsactl_elem_value_set_enum:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count): The array for values of enumeration
 *          index.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of enumeration index into internal storage.
 */
void alsactl_elem_value_set_enum(ALSACtlElemValue *self,
                                 const guint32 *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    value_count = MIN(value_count, G_N_ELEMENTS(value->value.enumerated.item));
    for (i = 0; i < value_count; ++i)
        value->value.enumerated.item[i] = (unsigned int)values[i];
}

/**
 * alsactl_elem_value_get_enum:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count)(inout): The array for values of
 *          enumeration index.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of enumeration index from internal storage.
 */
void alsactl_elem_value_get_enum(ALSACtlElemValue *self,
                                 guint32 *const *values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    *value_count = MIN(*value_count, G_N_ELEMENTS(value->value.enumerated.item));
    for (i = 0; i < *value_count; ++i)
        (*values)[i] = (guint32)value->value.enumerated.item[i];
}

/**
 * alsactl_elem_value_set_bytes:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count): The array for values of bytes type.
 * @value_count: The number of values up to 512.
 *
 * Copy the array for values of bytes type into internal storage.
 */
void alsactl_elem_value_set_bytes(ALSACtlElemValue *self,
                                  const guint8 *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    value_count = MIN(value_count, G_N_ELEMENTS(value->value.bytes.data));
    for (i = 0; i < value_count; ++i)
        value->value.bytes.data[i] = (long)values[i];
}

/**
 * alsactl_elem_value_get_bytes:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count)(inout): The array for values of bytes type.
 * @value_count: The number of values up to 512.
 *
 * Copy the array for values of bytes type into internal storage.
 */
void alsactl_elem_value_get_bytes(ALSACtlElemValue *self,
                                  guint8 *const *values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    *value_count = MIN(*value_count, G_N_ELEMENTS(value->value.bytes.data));
    for (i = 0; i < *value_count; ++i)
        (*values)[i] = (guint8)value->value.bytes.data[i];
}

/**
 * alsactl_elem_value_set_iec60958:
 * @self: A #ALSACtlElemValue.
 * @channel_status: (array length=channel_status_length)(nullable): The array of
 *                  byte data for channel status bits in IEC 60958.
 * @channel_status_length: The number of bytes in channel_status argument, up
 *                         to 24.
 * @user_data: (array length=user_data_length)(nullable): The array of byte data
 *             for user data bits in IEC 60958.
 * @user_data_length: The number of bytes in user_data argument, up to 147.
 *
 * Copy a pair of array for channel status and user data of IEC 60958 into
 * internal storage.
 */
void alsactl_elem_value_set_iec60958(ALSACtlElemValue *self,
                const guint8 *channel_status, gsize channel_status_length,
                const guint8 *user_data, gsize user_data_length)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    if (channel_status != NULL) {
        channel_status_length = MIN(channel_status_length,
                                    G_N_ELEMENTS(value->value.iec958.status));
        for (i = 0; i < channel_status_length; ++i)
            value->value.iec958.status[i] = channel_status[i];
    }

    if (user_data != NULL) {
        user_data_length = MIN(user_data_length,
                               G_N_ELEMENTS(value->value.iec958.subcode));
        for (i = 0; i < user_data_length; ++i)
            value->value.iec958.subcode[i] = user_data[i];
    }
}

/**
 * alsactl_elem_value_get_iec60958:
 * @self: A #ALSACtlElemValue.
 * @channel_status: (array length=channel_status_length)(inout)(nullable): The
 *                  array of byte data for channel status bits in IEC 60958.
 * @channel_status_length: The number of bytes in channel_status argument, up
 *                         to 24.
 * @user_data: (array length=user_data_length)(inout)(nullable): The array of
 *             byte data for user data bits in IEC 60958.
 * @user_data_length: The number of bytes in user_data argument, up to 147.
 *
 * Copy a pair of array for channel status and user data of IEC 60958 from
 * internal storage.
 */
void alsactl_elem_value_get_iec60958(ALSACtlElemValue *self,
                guint8 *const *channel_status, gsize *channel_status_length,
                guint8 *const *user_data, gsize *user_data_length)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    if (channel_status != NULL) {
        *channel_status_length = MIN(*channel_status_length,
                                     G_N_ELEMENTS(value->value.iec958.status));
        for (i = 0; i < *channel_status_length; ++i)
            (*channel_status)[i] = value->value.iec958.status[i];
    }

    if (user_data != NULL) {
        *user_data_length = MIN(*user_data_length,
                                G_N_ELEMENTS(value->value.iec958.subcode));
        for (i = 0; i < *user_data_length; ++i)
            (*user_data)[i] = value->value.iec958.subcode[i];
    }
}

/**
 * alsactl_elem_value_set_int64:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count): The array for values of integer64 type.
 * @value_count: The number of values up to 64.
 *
 * Copy the array for values of integer64 type into internal storage.
 */
void alsactl_elem_value_set_int64(ALSACtlElemValue *self, const gint64 *values,
                                  gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    value_count = MIN(value_count, G_N_ELEMENTS(value->value.integer64.value));
    for (i = 0; i < value_count; ++i)
        value->value.integer64.value[i] = (long long)values[i];
}

/**
 * alsactl_elem_value_get_int64:
 * @self: A #ALSACtlElemValue.
 * @values: (array length=value_count)(inout): The array for values of integer64
 *          type.
 * @value_count: The number of values up to 64.
 *
 * Copy the array for values of integer64 type from internal storage.
 */
void alsactl_elem_value_get_int64(ALSACtlElemValue *self,
                                  gint64 *const *values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);
    value = &priv->value;

    *value_count = MIN(*value_count, G_N_ELEMENTS(value->value.integer64.value));
    for (i = 0; i < *value_count; ++i)
        (*values)[i] = (gint64)value->value.integer64.value[i];
}

/**
 * alsactl_elem_value_equal:
 * @self: A #ALSACtlElemValue.
 * @target: A #ALSACtlElemValue to compare.
 *
 * Returns: whether the given object includes the same values as the instance.
 *          The other fields are ignored to be compared.
 */
gboolean alsactl_elem_value_equal(const ALSACtlElemValue *self,
                                  const ALSACtlElemValue *target) {
    const ALSACtlElemValuePrivate *lhs, *rhs;

    g_return_val_if_fail(ALSACTL_IS_ELEM_VALUE(self), FALSE);
    g_return_val_if_fail(ALSACTL_IS_ELEM_VALUE(target), FALSE);
    lhs = alsactl_elem_value_get_instance_private((ALSACtlElemValue *)self);
    rhs = alsactl_elem_value_get_instance_private((ALSACtlElemValue *)target);

    return !memcmp(&lhs->value, &rhs->value, sizeof(lhs->value));
}
