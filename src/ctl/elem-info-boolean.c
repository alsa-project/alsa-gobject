// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoBoolean:
 * An object to express information for boolean type of element.
 *
 * A [class@GObject.Object] derived object class for boolean type of element.
 *
 * The object wraps `struct snd_ctl_elem_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_ctl_elem_info data;
} ALSACtlElemInfoBooleanPrivate;

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface);
static void elem_info_single_array_iface_init(ALSACtlElemInfoSingleArrayInterface *iface);

G_DEFINE_TYPE_WITH_CODE(ALSACtlElemInfoBoolean, alsactl_elem_info_boolean, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(ALSACtlElemInfoBoolean)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_COMMON,
                                              elem_info_common_iface_init)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_SINGLE_ARRAY,
                                              elem_info_single_array_iface_init))

static void ctl_elem_info_boolean_set_property(GObject *obj, guint id, const GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoBoolean *self = ALSACTL_ELEM_INFO_BOOLEAN(obj);
    ALSACtlElemInfoBooleanPrivate *priv = alsactl_elem_info_boolean_get_instance_private(self);
    struct snd_ctl_elem_info *data = &priv->data;

    elem_info_single_array_set_property(data, obj, id, val, spec);
}

static void ctl_elem_info_boolean_get_property(GObject *obj, guint id, GValue *val,
                                              GParamSpec *spec)
{
    ALSACtlElemInfoBoolean *self = ALSACTL_ELEM_INFO_BOOLEAN(obj);
    ALSACtlElemInfoBooleanPrivate *priv = alsactl_elem_info_boolean_get_instance_private(self);
    const struct snd_ctl_elem_info *data = &priv->data;

    elem_info_single_array_get_property(data, obj, id, val, spec);
}

static void alsactl_elem_info_boolean_class_init(ALSACtlElemInfoBooleanClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_boolean_set_property;
    gobject_class->get_property = ctl_elem_info_boolean_get_property;

    elem_info_single_array_class_override_properties(gobject_class);
}

static void alsactl_elem_info_boolean_init(ALSACtlElemInfoBoolean *self)
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
 * alsactl_elem_info_boolean_new:
 *
 * Allocate and return an instance of [class@ElemInfoBoolean].
 *
 * Returns: An instance of [class@ElemInfoBoolean].
 */
ALSACtlElemInfoBoolean *alsactl_elem_info_boolean_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_BOOLEAN,
                        ELEM_TYPE_PROP_NAME, ALSACTL_ELEM_TYPE_BOOLEAN, NULL);
}

void ctl_elem_info_boolean_refer_private(ALSACtlElemInfoBoolean *self,
                                         struct snd_ctl_elem_info **data)
{
    ALSACtlElemInfoBooleanPrivate *priv = alsactl_elem_info_boolean_get_instance_private(self);

    *data = &priv->data;
}
