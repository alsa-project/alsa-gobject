// SPDX-License-Identifier: LGPL-3.0-or-later
#include "substream-status.h"

#include <sound/asound.h>

struct _ALSARawmidiSubstreamStatusPrivate {
    struct snd_rawmidi_status status;
};
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
        g_value_set_long(val, priv->status.avail);
        break;
    case RAWMIDI_SUBSTREAM_STATUS_PROP_XRUN:
        g_value_set_long(val, priv->status.xruns);
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

    rawmidi_substream_status_props[RAWMIDI_SUBSTREAM_STATUS_PROP_AVAIL] =
        g_param_spec_ulong("avail", "avail",
               "The size of available space in intermediate buffer.",
                          0, G_MAXULONG,
                          0,
                          G_PARAM_READABLE);

    rawmidi_substream_status_props[RAWMIDI_SUBSTREAM_STATUS_PROP_XRUN] =
        g_param_spec_ulong("xruns", "xruns",
               "The count of XRUNs since the last query of status.",
                           0, G_MAXULONG,
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
 * Allocate and return an instance of ALSARawmidiSubstreamStatus class.
 */
ALSARawmidiSubstreamStatus *alsarawmidi_substream_status_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_STATUS, NULL);
}
