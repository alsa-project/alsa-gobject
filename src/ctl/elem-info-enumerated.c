// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSACtlElemInfoEnumerated:
 * An object to express information for enumerated type of element.
 *
 * A [class@GObject.Object] derived object class for enumerated type of element.
 *
 * The object wraps `struct snd_ctl_elem_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_ctl_elem_info data;
    gchar **labels;
} ALSACtlElemInfoEnumeratedPrivate;

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface);
static void elem_info_single_array_iface_init(ALSACtlElemInfoSingleArrayInterface *iface);

G_DEFINE_TYPE_WITH_CODE(ALSACtlElemInfoEnumerated, alsactl_elem_info_enumerated, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(ALSACtlElemInfoEnumerated)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_COMMON,
                                              elem_info_common_iface_init)
                        G_IMPLEMENT_INTERFACE(ALSACTL_TYPE_ELEM_INFO_SINGLE_ARRAY,
                                              elem_info_single_array_iface_init))

enum elem_info_enumerated_prop_type {
    ELEM_INFO_ENUMERATED_PROP_LABELS = ELEM_INFO_SINGLE_ARRAY_PROP_COUNT,
    ELEM_INFO_ENUMERATED_PROP_COUNT,
};

static void ctl_elem_info_enumerated_finalize(GObject *obj)
{
    ALSACtlElemInfoEnumerated *self = ALSACTL_ELEM_INFO_ENUMERATED(obj);
    ALSACtlElemInfoEnumeratedPrivate *priv = alsactl_elem_info_enumerated_get_instance_private(self);

    g_strfreev(priv->labels);

    G_OBJECT_CLASS(alsactl_elem_info_enumerated_parent_class)->finalize(obj);
}

static void ctl_elem_info_enumerated_set_property(GObject *obj, guint id, const GValue *val,
                                                  GParamSpec *spec)
{
    ALSACtlElemInfoEnumerated *self = ALSACTL_ELEM_INFO_ENUMERATED(obj);
    ALSACtlElemInfoEnumeratedPrivate *priv = alsactl_elem_info_enumerated_get_instance_private(self);

    switch (id) {
    case ELEM_INFO_ENUMERATED_PROP_LABELS:
        if (priv->labels != NULL)
            g_strfreev(priv->labels);
        priv->labels = g_strdupv(g_value_get_boxed(val));
        break;
    default:
        elem_info_single_array_set_property(&priv->data, obj, id, val, spec);
        break;
    }
}

static void ctl_elem_info_enumerated_get_property(GObject *obj, guint id, GValue *val,
                                                  GParamSpec *spec)
{
    ALSACtlElemInfoEnumerated *self = ALSACTL_ELEM_INFO_ENUMERATED(obj);
    ALSACtlElemInfoEnumeratedPrivate *priv = alsactl_elem_info_enumerated_get_instance_private(self);

    switch (id) {
    case ELEM_INFO_ENUMERATED_PROP_LABELS:
        g_value_set_static_boxed(val, priv->labels);
        break;
    default:
        elem_info_single_array_get_property(&priv->data, obj, id, val, spec);
        break;
    }
}

static void alsactl_elem_info_enumerated_class_init(ALSACtlElemInfoEnumeratedClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = ctl_elem_info_enumerated_finalize;
    gobject_class->set_property = ctl_elem_info_enumerated_set_property;
    gobject_class->get_property = ctl_elem_info_enumerated_get_property;

    elem_info_single_array_class_override_properties(gobject_class);

    /**
     * ALSACtlElemInfoEnumerated:labels:
     *
     * The list of indexed labels for the element. There is limitation that:
     *
     *  - The length of label including terminator should be within 64 bytes.
     *  - The total length of labels including terminators should be within (64 * 1024) bytes.
     *
     * Since: 0.3.
     */
    g_object_class_install_property(gobject_class, ELEM_INFO_ENUMERATED_PROP_LABELS,
        g_param_spec_boxed("labels", "labels",
                           "The list of indexed labels for the element",
                           G_TYPE_STRV,
                           G_PARAM_READWRITE));
}

static void alsactl_elem_info_enumerated_init(ALSACtlElemInfoEnumerated *self)
{
    return;
}

static void elem_info_common_iface_init(ALSACtlElemInfoCommonInterface *iface)
{
    return;
}

static void elem_info_single_array_iface_init(ALSACtlElemInfoSingleArrayInterface *iface)
{
    return;
}

/**
 * alsactl_elem_info_enumerated_new:
 *
 * Allocate and return an instance of [class@ElemInfoEnumerated].
 *
 * Returns: An instance of [class@ElemInfoEnumerated].
 */
ALSACtlElemInfoEnumerated *alsactl_elem_info_enumerated_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_ENUMERATED,
                        ELEM_TYPE_PROP_NAME, ALSACTL_ELEM_TYPE_ENUMERATED, NULL);
}

void ctl_elem_info_enumerated_refer_private(ALSACtlElemInfoEnumerated *self,
                                            struct snd_ctl_elem_info **data)
{
    ALSACtlElemInfoEnumeratedPrivate *priv = alsactl_elem_info_enumerated_get_instance_private(self);

    *data = &priv->data;
}
