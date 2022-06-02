// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoBytes:
 * An object to express information for bytes type of element.
 *
 * A [class@GObject.Object] derived object class for bytes type of element.
 *
 * The object wraps `struct snd_ctl_elem_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_ctl_elem_info data;
} ALSACtlElemInfoBytesPrivate;

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface);
static void elem_info_single_array_iface_init(ALSACtlElemInfoSingleArrayInterface *iface);

G_DEFINE_TYPE_WITH_CODE(ALSACtlElemInfoBytes, alsactl_elem_info_bytes, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(ALSACtlElemInfoBytes)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_COMMON,
                                              elem_info_common_iface_init)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_SINGLE_ARRAY,
                                              elem_info_single_array_iface_init))

static void ctl_elem_info_bytes_set_property(GObject *obj, guint id, const GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoBytes *self = ALSACTL_ELEM_INFO_BYTES(obj);
    ALSACtlElemInfoBytesPrivate *priv = alsactl_elem_info_bytes_get_instance_private(self);
    struct snd_ctl_elem_info *data = &priv->data;

    elem_info_single_array_set_property(data, obj, id, val, spec);
}

static void ctl_elem_info_bytes_get_property(GObject *obj, guint id, GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoBytes *self = ALSACTL_ELEM_INFO_BYTES(obj);
    ALSACtlElemInfoBytesPrivate *priv = alsactl_elem_info_bytes_get_instance_private(self);
    const struct snd_ctl_elem_info *data = &priv->data;

    elem_info_single_array_get_property(data, obj, id, val, spec);
}

static void alsactl_elem_info_bytes_class_init(ALSACtlElemInfoBytesClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_bytes_set_property;
    gobject_class->get_property = ctl_elem_info_bytes_get_property;

    elem_info_single_array_class_override_properties(gobject_class);
}

static void alsactl_elem_info_bytes_init(ALSACtlElemInfoBytes *self)
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
 * alsactl_elem_info_bytes_new:
 *
 * Allocate and return an instance of [class@ElemInfoBytes].
 *
 * Returns: An instance of [class@ElemInfoBytes].
 */
ALSACtlElemInfoBytes *alsactl_elem_info_bytes_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_BYTES, ELEM_TYPE_PROP_NAME, ALSACTL_ELEM_TYPE_BYTES,
                        NULL);
}
