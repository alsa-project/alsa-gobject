// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSARawmidiSubstreamStatus:
 * A GObject-derived object to express status of substream.
 *
 * A [class@SubstreamStatus] is a GObject-derived object to express status of substream attached
 * to the pair of stream. The call of [method@StreamPair.get_substream_status] returns the instance
 * of object.
 *
 * The object wraps `struct snd_rawmidi_status` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_rawmidi_status status;
} ALSARawmidiSubstreamStatusPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSARawmidiSubstreamStatus, alsarawmidi_substream_status, G_TYPE_OBJECT)

enum rawmidi_substream_status_prop_type {
    RAWMIDI_SUBSTREAM_STATUS_PROP_AVAIL = 1,
    RAWMIDI_SUBSTREAM_STATUS_PROP_XRUN,
    RAWMIDI_SUBSTREAM_STATUS_PROP_COUNT,
};
static GParamSpec *rawmidi_substream_status_props[RAWMIDI_SUBSTREAM_STATUS_PROP_COUNT] = { NULL, };

static void rawmidi_substream_status_get_property(GObject *obj, guint id,
                                                GValue *val, GParamSpec *spec)
{
    ALSARawmidiSubstreamStatus *self = ALSARAWMIDI_SUBSTREAM_STATUS(obj);
    ALSARawmidiSubstreamStatusPrivate *priv =
                        alsarawmidi_substream_status_get_instance_private(self);

    switch (id) {
    case RAWMIDI_SUBSTREAM_STATUS_PROP_AVAIL:
        g_value_set_uint64(val, priv->status.avail);
        break;
    case RAWMIDI_SUBSTREAM_STATUS_PROP_XRUN:
        g_value_set_uint64(val, priv->status.xruns);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsarawmidi_substream_status_class_init(ALSARawmidiSubstreamStatusClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = rawmidi_substream_status_get_property;

    /**
     * ALSARawmidiSubstreamStatus:avail:
     *
     * The size of available space in intermediate buffer.
     */
    rawmidi_substream_status_props[RAWMIDI_SUBSTREAM_STATUS_PROP_AVAIL] =
        g_param_spec_uint64("avail", "avail",
                            "The size of available space in intermediate buffer.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    /**
     * ALSARawmidiSubstreamStatus:xruns:
     *
     * The count of XRUNs since the last query of status.
     */
    rawmidi_substream_status_props[RAWMIDI_SUBSTREAM_STATUS_PROP_XRUN] =
        g_param_spec_uint64("xruns", "xruns",
                            "The count of XRUNs since the last query of status.",
                            0, G_MAXUINT64,
                            0,
                            G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      RAWMIDI_SUBSTREAM_STATUS_PROP_COUNT,
                                      rawmidi_substream_status_props);
}

static void alsarawmidi_substream_status_init(ALSARawmidiSubstreamStatus *self)
{
    return;
}

/**
 * alsarawmidi_substream_status_new:
 *
 * Allocate and return an instance of [class@SubstreamStatus].
 *
 * Returns: An instance of [class@SubstreamStatus].
 */
ALSARawmidiSubstreamStatus *alsarawmidi_substream_status_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_STATUS, NULL);
}

void rawmidi_substream_status_refer_private(ALSARawmidiSubstreamStatus *self,
                                            struct snd_rawmidi_status **status)
{
    ALSARawmidiSubstreamStatusPrivate *priv =
                        alsarawmidi_substream_status_get_instance_private(self);

    *status = &priv->status;
}
