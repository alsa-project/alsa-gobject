// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoIec60958:
 * An object to express information for iec60958 type of element.
 *
 * A [class@GObject.Object] derived object class for iec60958 type of element.
 *
 * The object wraps `struct snd_ctl_elem_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_ctl_elem_info data;
} ALSACtlElemInfoIec60958Private;

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface);

G_DEFINE_TYPE_WITH_CODE(ALSACtlElemInfoIec60958, alsactl_elem_info_iec60958, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(ALSACtlElemInfoIec60958)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_COMMON,
                                              elem_info_common_iface_init))

static void ctl_elem_info_iec60958_set_property(GObject *obj, guint id, const GValue *val,
                                                  GParamSpec *spec)
{
    ALSACtlElemInfoIec60958 *self = ALSACTL_ELEM_INFO_IEC60958(obj);
    ALSACtlElemInfoIec60958Private *priv = alsactl_elem_info_iec60958_get_instance_private(self);

    elem_info_common_set_property(&priv->data, obj, id, val, spec);
}

static void ctl_elem_info_iec60958_get_property(GObject *obj, guint id, GValue *val,
                                                  GParamSpec *spec)
{
    ALSACtlElemInfoIec60958 *self = ALSACTL_ELEM_INFO_IEC60958(obj);
    ALSACtlElemInfoIec60958Private *priv = alsactl_elem_info_iec60958_get_instance_private(self);

    elem_info_common_get_property(&priv->data, obj, id, val, spec);
}

static void alsactl_elem_info_iec60958_class_init(ALSACtlElemInfoIec60958Class *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ctl_elem_info_iec60958_set_property;
    gobject_class->get_property = ctl_elem_info_iec60958_get_property;

    elem_info_common_class_override_properties(gobject_class);
}

static void alsactl_elem_info_iec60958_init(ALSACtlElemInfoIec60958 *self)
{
    ALSACtlElemInfoIec60958Private *priv = alsactl_elem_info_iec60958_get_instance_private(self);
    // MEMO: ALSA control core check it but meaningless.
    priv->data.count = 1;
}

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface)
{
    return;
}

/**
 * alsactl_elem_info_iec60958_new:
 *
 * Allocate and return an instance of [class@ElemInfoIec60958].
 *
 * Returns: An instance of [class@ElemInfoIec60958].
 */
ALSACtlElemInfoIec60958 *alsactl_elem_info_iec60958_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_IEC60958,
                        ELEM_TYPE_PROP_NAME, ALSACTL_ELEM_TYPE_IEC60958, NULL);
}

void ctl_elem_info_iec60958_refer_private(ALSACtlElemInfoIec60958 *self,
                                          struct snd_ctl_elem_info **data)
{
    ALSACtlElemInfoIec60958Private *priv = alsactl_elem_info_iec60958_get_instance_private(self);

    *data = &priv->data;
}
