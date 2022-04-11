// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <unistd.h>

/**
 * SECTION: substream-params
 * @Title: ALSARawmidiSubstreamParams
 * @Short_description: A GObject-derived object to represent parameters of
 *                     substream.
 *
 * A #ALSARawmidiSubstreamParams is a GObject-derived object to represent
 * parameters of substream attached to the pair of streams. The call of
 * alsarawmidi_stream_pair_set_substream_params() requires the instance of
 * object.
 *
 * The object wraps 'struct snd_rawmidi_params' in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_rawmidi_params params;
} ALSARawmidiSubstreamParamsPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSARawmidiSubstreamParams, alsarawmidi_substream_params, G_TYPE_OBJECT)

enum rawmidi_substream_params_prop_type {
    RAWMIDI_SUBSTREAM_PARAMS_PROP_BUFFER_SIZE = 1,
    RAWMIDI_SUBSTREAM_PARAMS_PROP_AVAIL_MIN,
    RAWMIDI_SUBSTREAM_PARAMS_PROP_ACTIVE_SENSING,
    RAWMIDI_SUBSTREAM_PARAMS_PROP_COUNT,
};
static GParamSpec *rawmidi_substream_params_props[RAWMIDI_SUBSTREAM_PARAMS_PROP_COUNT] = { NULL, };

static void rawmidi_substream_params_set_property(GObject *obj, guint id,
                                        const GValue *val, GParamSpec *spec)
{
    ALSARawmidiSubstreamParams *self = ALSARAWMIDI_SUBSTREAM_PARAMS(obj);
    ALSARawmidiSubstreamParamsPrivate *priv =
                        alsarawmidi_substream_params_get_instance_private(self);

    switch (id) {
    case RAWMIDI_SUBSTREAM_PARAMS_PROP_BUFFER_SIZE:
        priv->params.buffer_size = g_value_get_uint(val);
        break;
    case RAWMIDI_SUBSTREAM_PARAMS_PROP_AVAIL_MIN:
        priv->params.avail_min = g_value_get_uint(val);
        break;
    case RAWMIDI_SUBSTREAM_PARAMS_PROP_ACTIVE_SENSING:
        priv->params.no_active_sensing = !g_value_get_boolean(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void rawmidi_substream_params_get_property(GObject *obj, guint id,
                                                GValue *val, GParamSpec *spec)
{
    ALSARawmidiSubstreamParams *self = ALSARAWMIDI_SUBSTREAM_PARAMS(obj);
    ALSARawmidiSubstreamParamsPrivate *priv =
                        alsarawmidi_substream_params_get_instance_private(self);

    switch (id) {
    case RAWMIDI_SUBSTREAM_PARAMS_PROP_BUFFER_SIZE:
        g_value_set_uint(val, priv->params.buffer_size);
        break;
    case RAWMIDI_SUBSTREAM_PARAMS_PROP_AVAIL_MIN:
        g_value_set_uint(val, priv->params.avail_min);
        break;
    case RAWMIDI_SUBSTREAM_PARAMS_PROP_ACTIVE_SENSING:
        g_value_set_boolean(val, !priv->params.no_active_sensing);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsarawmidi_substream_params_class_init(ALSARawmidiSubstreamParamsClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    long page_size = sysconf(_SC_PAGESIZE);

    gobject_class->set_property = rawmidi_substream_params_set_property;
    gobject_class->get_property = rawmidi_substream_params_get_property;

    rawmidi_substream_params_props[RAWMIDI_SUBSTREAM_PARAMS_PROP_BUFFER_SIZE] =
        g_param_spec_uint("buffer-size", "buffer-size",
                          "The size of intermediate buffer for substream.",
                          32, 1024 * 1024,
                          page_size,
                          G_PARAM_READWRITE);

    rawmidi_substream_params_props[RAWMIDI_SUBSTREAM_PARAMS_PROP_AVAIL_MIN] =
        g_param_spec_uint("avail-min", "avail-min",
                          "The threshold to wake up from any blocking "
                          "operation such as poll(2), read(2) and write(2).",
                          1, G_MAXUINT,
                          1,
                          G_PARAM_READWRITE);

    rawmidi_substream_params_props[RAWMIDI_SUBSTREAM_PARAMS_PROP_ACTIVE_SENSING] =
        g_param_spec_boolean("active-sensing", "active-sensing",
                             "Whether to emit 0xfe one time when closing "
                             "substream.",
                             FALSE,
                             G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      RAWMIDI_SUBSTREAM_PARAMS_PROP_COUNT,
                                      rawmidi_substream_params_props);
}

static void alsarawmidi_substream_params_init(ALSARawmidiSubstreamParams *self)
{
    return;
}

/**
 * alsarawmidi_substream_params_new:
 *
 * Allocate and return an instance of ALSARawmidiSubstreamParams class.
 */
ALSARawmidiSubstreamParams *alsarawmidi_substream_params_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS, NULL);
}

void rawmidi_substream_params_refer_private(ALSARawmidiSubstreamParams *self,
                                            struct snd_rawmidi_params **params)
{
    ALSARawmidiSubstreamParamsPrivate *priv =
                        alsarawmidi_substream_params_get_instance_private(self);

    *params = &priv->params;
}
