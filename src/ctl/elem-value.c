// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemValue:
 * A boxed object to represent the container of array of values for any type of element.
 *
 * A [class@ElemValue] is boxed object to represent the container of values for any type of
 * element. The arrays of values for each type of element shares the same storage, thus it's
 * important for applications to distinguish the type of element in advance of accessing the
 * array. The object is used for the call of [method@Card.write_elem_value] and
 * [method@Card.read_elem_value].
 *
 * The object wraps `struct snd_ctl_elem_value` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_ctl_elem_value value;
    gboolean boolean[128];
    gint32 integer[128];
} ALSACtlElemValuePrivate;
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

    /**
     * ALSACtlElemValue:elem-id:
     *
     * The identifier of element.
     */
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
 * Allocate and return an instance of [class@ElemValue].
 *
 * Returns: An instance of [class@ElemValue].
 */
ALSACtlElemValue *alsactl_elem_value_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_VALUE, NULL);
}

void ctl_elem_value_refer_private(ALSACtlElemValue *self, struct snd_ctl_elem_value **value)
{
    ALSACtlElemValuePrivate *priv =
                                alsactl_elem_value_get_instance_private(self);
    *value = &priv->value;
}

/**
 * alsactl_elem_value_set_bool:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count): The array for values of boolean type.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of boolean type into internal data.
 */
void alsactl_elem_value_set_bool(ALSACtlElemValue *self, const gboolean *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);

    value = &priv->value;
    memset(&value->value.integer.value, 0, sizeof(value->value.integer.value));
    value_count = MIN(value_count, G_N_ELEMENTS(value->value.integer.value));
    for (i = 0; i < value_count; ++i)
        value->value.integer.value[i] = (long)values[i];
}

/**
 * alsactl_elem_value_get_bool:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count) (out) (transfer none): The array for boolean values.
 * @value_count: The number of values up to 128.
 * 
 * Refer to the array specific to [enum@ElemType].BOOLEAN element in internal storage.
 */
void alsactl_elem_value_get_bool(ALSACtlElemValue *self, const gboolean **values,
                                 gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);
    g_return_if_fail(value_count != NULL);

    value = &priv->value;
    for (i = 0; i < G_N_ELEMENTS(value->value.integer.value); ++i)
        priv->boolean[i] = value->value.integer.value[i] > 0;

    *values = priv->boolean;
    *value_count = G_N_ELEMENTS(value->value.integer.value);
}

/**
 * alsactl_elem_value_set_int:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count): The array for values of integer type.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of integer type into internal storage.
 */
void alsactl_elem_value_set_int(ALSACtlElemValue *self, const gint32 *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);

    value = &priv->value;
    memset(&value->value.integer.value, 0, sizeof(value->value.integer.value));
    value_count = MIN(value_count, G_N_ELEMENTS(value->value.integer.value));
    for (i = 0; i < value_count; ++i)
        value->value.integer.value[i] = (long)values[i];
}

/**
 * alsactl_elem_value_get_int:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count) (out) (transfer none): The array for 32 bit signed integer
 *          values.
 * @value_count: The number of values up to 128.
 *
 * Refer to the array for [enum@ElemType].INTEGER element in internal storage.
 */
void alsactl_elem_value_get_int(ALSACtlElemValue *self, const gint32 **values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);
    g_return_if_fail(value_count != NULL);

    value = &priv->value;
    for (i = 0; i < G_N_ELEMENTS(value->value.integer.value); ++i)
            priv->integer[i] = (gint32)value->value.integer.value[i];

    *values = priv->integer;
    *value_count = G_N_ELEMENTS(value->value.integer.value);
}

/**
 * alsactl_elem_value_set_enum:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count): The array for values of enumeration index.
 * @value_count: The number of values up to 128.
 *
 * Copy the array for values of enumeration index into internal storage.
 */
void alsactl_elem_value_set_enum(ALSACtlElemValue *self, const guint32 *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);

    value = &priv->value;
    memset(&value->value.enumerated.item, 0, sizeof(value->value.enumerated.item));
    value_count = MIN(value_count, G_N_ELEMENTS(value->value.enumerated.item));
    for (i = 0; i < value_count; ++i)
        value->value.enumerated.item[i] = (unsigned int)values[i];
}

/**
 * alsactl_elem_value_get_enum:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count) (out) (transfer none): The array for enumeration index
 *          values.
 * @value_count: The number of values up to 128.
 *
 * Refer to the array specific to [enum@ElemType].ENUMERATED element in internal storage.
 */
void alsactl_elem_value_get_enum(ALSACtlElemValue *self, const guint32 **values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);
    g_return_if_fail(value_count != NULL);

    value = &priv->value;
    *values = value->value.enumerated.item;
    *value_count = G_N_ELEMENTS(value->value.enumerated.item);
}

/**
 * alsactl_elem_value_set_bytes:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count): The array for values of bytes type.
 * @value_count: The number of values up to 512.
 *
 * Copy the array for values of bytes type into internal storage.
 */
void alsactl_elem_value_set_bytes(ALSACtlElemValue *self, const guint8 *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);

    value = &priv->value;
    memset(&value->value.bytes.data, 0, sizeof(value->value.bytes.data));
    value_count = MIN(value_count, G_N_ELEMENTS(value->value.bytes.data));
    for (i = 0; i < value_count; ++i)
        value->value.bytes.data[i] = (long)values[i];
}

/**
 * alsactl_elem_value_get_bytes:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count)(inout): The array for values of bytes type.
 * @value_count: The number of values up to 512.
 *
 * Copy the array for values of bytes type into internal storage.
 */
void alsactl_elem_value_get_bytes(ALSACtlElemValue *self, guint8 *const *values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);
    g_return_if_fail(value_count != NULL);

    value = &priv->value;
    *value_count = MIN(*value_count, G_N_ELEMENTS(value->value.bytes.data));
    for (i = 0; i < *value_count; ++i)
        (*values)[i] = (guint8)value->value.bytes.data[i];
}

/**
 * alsactl_elem_value_set_iec60958_channel_status:
 * @self: A [class@ElemValue].
 * @status: (array length=length): The array of byte data for channel status bits in IEC 60958.
 * @length: The number of bytes in channel_status argument, up to 24.
 *
 * Copy the given channel status of IEC 60958 into internal storage.
 */
void alsactl_elem_value_set_iec60958_channel_status(ALSACtlElemValue *self, const guint8 *status,
                                                    gsize length)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(status != NULL);

    value = &priv->value;
    memset(&value->value.iec958.status, 0, sizeof(value->value.iec958.status));
    length = MIN(length, G_N_ELEMENTS(value->value.iec958.status));
    for (i = 0; i < length; ++i)
        value->value.iec958.status[i] = status[i];
}

/**
 * alsactl_elem_value_get_iec60958_channel_status:
 * @self: A [class@ElemValue].
 * @status: (array length=length)(inout): The array of byte data for channel status bits for
 *          IEC 60958 element.
 * @length: The number of bytes in status argument, up to 24.
 *
 * Copy channel status of IEC 60958 from internal storage.
 */
void alsactl_elem_value_get_iec60958_channel_status(ALSACtlElemValue *self, guint8 *const *status,
                                                    gsize *length)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(status != NULL);
    g_return_if_fail(length != NULL);

    value = &priv->value;
    *length = MIN(*length, G_N_ELEMENTS(value->value.iec958.status));
    for (i = 0; i < *length; ++i)
        (*status)[i] = value->value.iec958.status[i];
}

/**
 * alsactl_elem_value_set_iec60958_user_data:
 * @self: A [class@ElemValue].
 * @data: (array length=length): The array of byte data for user data bits in IEC 60958.
 * @length: The number of bytes in data argument, up to 147.
 *
 * Copy the given user data of IEC 60958 into internal storage.
 */
void alsactl_elem_value_set_iec60958_user_data(ALSACtlElemValue *self, const guint8 *data,
                                               gsize length)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(data != NULL);

    value = &priv->value;
    memset(&value->value.iec958.subcode, 0, sizeof(value->value.iec958.subcode));
    length = MIN(length, G_N_ELEMENTS(value->value.iec958.subcode));
    for (i = 0; i < length; ++i)
        value->value.iec958.subcode[i] = data[i];
}

/**
 * alsactl_elem_value_get_iec60958_user_data:
 * @self: A [class@ElemValue].
 * @data: (array length=length)(inout): The array of byte data for user data bits in IEC 60958.
 * @length: The number of bytes in user_data argument, up to 147.
 *
 * Copy user data of IEC 60958 from internal storage.
 */
void alsactl_elem_value_get_iec60958_user_data(ALSACtlElemValue *self, guint8 *const *data,
                                               gsize *length)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(data != NULL);
    g_return_if_fail(length != NULL);

    value = &priv->value;
    *length = MIN(*length, G_N_ELEMENTS(value->value.iec958.subcode));
    for (i = 0; i < *length; ++i)
        (*data)[i] = value->value.iec958.subcode[i];
}

/**
 * alsactl_elem_value_set_int64:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count): The array for values of integer64 type.
 * @value_count: The number of values up to 64.
 *
 * Copy the array for values of integer64 type into internal storage.
 */
void alsactl_elem_value_set_int64(ALSACtlElemValue *self, const gint64 *values, gsize value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);

    value = &priv->value;
    memset(&value->value.integer64.value, 0, sizeof(value->value.integer64.value));
    value_count = MIN(value_count, G_N_ELEMENTS(value->value.integer64.value));
    for (i = 0; i < value_count; ++i)
        value->value.integer64.value[i] = (long long)values[i];
}

/**
 * alsactl_elem_value_get_int64:
 * @self: A [class@ElemValue].
 * @values: (array length=value_count)(inout): The array for values of integer64 type.
 * @value_count: The number of values up to 64.
 *
 * Copy the array for values of integer64 type from internal storage.
 */
void alsactl_elem_value_get_int64(ALSACtlElemValue *self, gint64 *const *values, gsize *value_count)
{
    ALSACtlElemValuePrivate *priv;
    struct snd_ctl_elem_value *value;
    int i;

    g_return_if_fail(ALSACTL_IS_ELEM_VALUE(self));
    priv = alsactl_elem_value_get_instance_private(self);

    g_return_if_fail(values != NULL);
    g_return_if_fail(value_count != NULL);

    value = &priv->value;
    *value_count = MIN(*value_count, G_N_ELEMENTS(value->value.integer64.value));
    for (i = 0; i < *value_count; ++i)
        (*values)[i] = (gint64)value->value.integer64.value[i];
}

/**
 * alsactl_elem_value_equal:
 * @self: A [class@ElemValue].
 * @target: A [class@ElemValue] to compare.
 *
 * Returns: Whether the given object includes the same values as the instance. The other fields are
 *          ignored to be compared.
 */
gboolean alsactl_elem_value_equal(const ALSACtlElemValue *self, const ALSACtlElemValue *target)
{
    const ALSACtlElemValuePrivate *lhs, *rhs;

    g_return_val_if_fail(ALSACTL_IS_ELEM_VALUE((ALSACtlElemValue *)self), FALSE);
    g_return_val_if_fail(ALSACTL_IS_ELEM_VALUE((ALSACtlElemValue *)target), FALSE);
    lhs = alsactl_elem_value_get_instance_private((ALSACtlElemValue *)self);
    rhs = alsactl_elem_value_get_instance_private((ALSACtlElemValue *)target);

    return !memcmp(&lhs->value, &rhs->value, sizeof(lhs->value));
}
