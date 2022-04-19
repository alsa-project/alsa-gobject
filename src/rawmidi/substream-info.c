// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSARawmidiSubstreamInfo:
 * A GObject-derived object to represent information of substream.
 *
 * A [class@SubstreamInfo] is a GObject-derived object to represent information of substream
 * attached to the pair of streams. The call of [method@StreamPair.get_substream_info] or
 * [func@get_substream_info] return the instance of object.
 *
 * The object wraps `struct snd_rawmidi_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_rawmidi_info info;
} ALSARawmidiSubstreamInfoPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSARawmidiSubstreamInfo, alsarawmidi_substream_info, G_TYPE_OBJECT)

enum rawmidi_substream_info_prop_type {
    RAWMIDI_SUBSTREAM_INFO_PROP_DEVICE_ID = 1,
    RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICE_ID,
    RAWMIDI_SUBSTREAM_INFO_PROP_DIRECTION,
    RAWMIDI_SUBSTREAM_INFO_PROP_CARD_ID,
    RAWMIDI_SUBSTREAM_INFO_PROP_FLAGS,
    RAWMIDI_SUBSTREAM_INFO_PROP_ID,
    RAWMIDI_SUBSTREAM_INFO_PROP_NAME,
    RAWMIDI_SUBSTREAM_INFO_PROP_SUBNAME,
    RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICES_COUNT,
    RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICES_AVAIL,
    RAWMIDI_SUBSTREAM_INFO_PROP_COUNT,
};
static GParamSpec *rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_COUNT] = { NULL, };

static void rawmidi_substream_info_get_property(GObject *obj, guint id,
                                                GValue *val, GParamSpec *spec)
{
    ALSARawmidiSubstreamInfo *self = ALSARAWMIDI_SUBSTREAM_INFO(obj);
    ALSARawmidiSubstreamInfoPrivate *priv =
                        alsarawmidi_substream_info_get_instance_private(self);

    switch (id) {
    case RAWMIDI_SUBSTREAM_INFO_PROP_DEVICE_ID:
        g_value_set_uint(val, priv->info.device);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICE_ID:
        g_value_set_uint(val, priv->info.subdevice);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_DIRECTION:
        g_value_set_enum(val, priv->info.stream);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_CARD_ID:
        g_value_set_int(val, priv->info.card);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_FLAGS:
        g_value_set_flags(val, priv->info.flags);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_ID:
        g_value_set_static_string(val, (gchar *)priv->info.id);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_NAME:
        g_value_set_static_string(val, (gchar *)priv->info.name);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_SUBNAME:
        g_value_set_static_string(val, (gchar *)priv->info.subname);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICES_COUNT:
        g_value_set_uint(val, priv->info.subdevices_count);
        break;
    case RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICES_AVAIL:
        g_value_set_uint(val, priv->info.subdevices_avail);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsarawmidi_substream_info_class_init(ALSARawmidiSubstreamInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = rawmidi_substream_info_get_property;

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_DEVICE_ID] =
        g_param_spec_uint("device-id", "device-id",
                         "The numerical identifier of rawmidi device.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICE_ID] =
        g_param_spec_uint("subdevice-id", "subdevice-id",
                          "The numerical identifier of subdevice for rawmidi device..",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_DIRECTION] =
        g_param_spec_enum("direction", "direction",
                          "The direction of stream, one of "
                          "ALSARawmidiStreamDirection",
                          ALSARAWMIDI_TYPE_STREAM_DIRECTION,
                          ALSARAWMIDI_STREAM_DIRECTION_OUTPUT,
                          G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_CARD_ID] =
        g_param_spec_int("card-id", "card-id",
                         "The numerical identifier of sound card.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_FLAGS] =
        g_param_spec_flags("flags", "flags",
                           "The information flags of rawmidi device with flags "
                           "of ALSARawmidiStreamPairInfoFlag.",
                          ALSARAWMIDI_TYPE_STREAM_PAIR_INFO_FLAG,
                          0,
                          G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_ID] =
        g_param_spec_string("id", "id",
                            "The string identifier of rawmidi device.",
                            "",
                            G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of rawmidi device.",
                            "",
                            G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_SUBNAME] =
        g_param_spec_string("subdevice-name", "subdevice-name",
                            "The name of subdevice for the direction and the "
                            "subdevice-id",
                            "",
                            G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICES_COUNT] =
        g_param_spec_uint("subdevices-count", "subdevices-count",
                          "The number of subdevices on the rawmidi device for "
                          "the direction",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READABLE);

    rawmidi_substream_info_props[RAWMIDI_SUBSTREAM_INFO_PROP_SUBDEVICES_AVAIL] =
        g_param_spec_uint("subdevices-avail", "subdevices-avail",
                          "The current number of available subdevices on the "
                          "rawmidi device for the direction.",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      RAWMIDI_SUBSTREAM_INFO_PROP_COUNT,
                                      rawmidi_substream_info_props);
}

static void alsarawmidi_substream_info_init(ALSARawmidiSubstreamInfo *self)
{
    return;
}

void rawmidi_substream_info_refer_private(ALSARawmidiSubstreamInfo *self,
                                          struct snd_rawmidi_info **info)
{
    ALSARawmidiSubstreamInfoPrivate *priv =
                        alsarawmidi_substream_info_get_instance_private(self);

    *info = &priv->info;
}
