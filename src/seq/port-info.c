// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

struct _ALSASeqPortInfoPrivate {
    struct snd_seq_port_info info;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqPortInfo, alsaseq_port_info, G_TYPE_OBJECT)

enum seq_port_info_prop_type {
    SEQ_PORT_INFO_PROP_ADDR = 1,
    SEQ_PORT_INFO_PROP_NAME,
    SEQ_PORT_INFO_PROP_CAPS,
    SEQ_PORT_INFO_PROP_ATTRS,
    SEQ_PORT_INFO_PROP_MIDI_CHANNELS,
    SEQ_PORT_INFO_PROP_MIDI_VOICES,
    SEQ_PORT_INFO_PROP_SYNTH_VOICES,
    SEQ_PORT_INFO_PROP_READ_USERS,
    SEQ_PORT_INFO_PROP_WRITE_USERS,
    SEQ_PORT_INFO_PROP_TSTAMP_OVERWRITE,
    SEQ_PORT_INFO_PROP_TSTAMP_MODE,
    SEQ_PORT_INFO_PROP_TIME_QUEUE,
    SEQ_PORT_INFO_PROP_COUNT,
};
static GParamSpec *seq_port_info_props[SEQ_PORT_INFO_PROP_COUNT] = { NULL, };

static void seq_port_info_set_property(GObject *obj, guint id,
                                       const GValue *val, GParamSpec *spec)
{
    ALSASeqPortInfo *self = ALSASEQ_PORT_INFO(obj);
    ALSASeqPortInfoPrivate *priv = alsaseq_port_info_get_instance_private(self);

    switch (id) {
    case SEQ_PORT_INFO_PROP_ADDR:
    {
        ALSASeqAddr *addr = g_value_get_boxed(val);
        if (addr != NULL)
            priv->info.addr = *addr;
        break;
    }
    case SEQ_PORT_INFO_PROP_NAME:
        strncpy(priv->info.name, g_value_get_string(val), sizeof(priv->info.name));
        break;
    case SEQ_PORT_INFO_PROP_CAPS:
        priv->info.capability = (unsigned int)g_value_get_flags(val);
        break;
    case SEQ_PORT_INFO_PROP_ATTRS:
        priv->info.type = (unsigned int)g_value_get_flags(val);
        break;
    case SEQ_PORT_INFO_PROP_MIDI_CHANNELS:
        priv->info.midi_channels = g_value_get_int(val);
        break;
    case SEQ_PORT_INFO_PROP_MIDI_VOICES:
        priv->info.midi_voices = g_value_get_int(val);
        break;
    case SEQ_PORT_INFO_PROP_SYNTH_VOICES:
        priv->info.synth_voices = g_value_get_int(val);
        break;
    case SEQ_PORT_INFO_PROP_READ_USERS:
        priv->info.read_use = g_value_get_int(val);
        break;
    case SEQ_PORT_INFO_PROP_WRITE_USERS:
        priv->info.write_use = g_value_get_int(val);
        break;
    case SEQ_PORT_INFO_PROP_TSTAMP_OVERWRITE:
        if (g_value_get_boolean(val))
            priv->info.flags |= SNDRV_SEQ_PORT_FLG_TIMESTAMP;
        else
            priv->info.flags &= ~SNDRV_SEQ_PORT_FLG_TIMESTAMP;
        break;
    case SEQ_PORT_INFO_PROP_TSTAMP_MODE:
        if (g_value_get_enum(val))
            priv->info.flags |= SNDRV_SEQ_PORT_FLG_TIME_REAL;
        else
            priv->info.flags &= ~SNDRV_SEQ_PORT_FLG_TIME_REAL;
        break;
    case SEQ_PORT_INFO_PROP_TIME_QUEUE:
        priv->info.time_queue = (unsigned char)g_value_get_uchar(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_port_info_get_property(GObject *obj, guint id, GValue *val,
                                       GParamSpec *spec)
{
    ALSASeqPortInfo *self = ALSASEQ_PORT_INFO(obj);
    ALSASeqPortInfoPrivate *priv = alsaseq_port_info_get_instance_private(self);

    switch (id) {
    case SEQ_PORT_INFO_PROP_ADDR:
        g_value_set_static_boxed(val, &priv->info.addr);
        break;
    case SEQ_PORT_INFO_PROP_NAME:
        g_value_set_static_string(val, priv->info.name);
        break;
    case SEQ_PORT_INFO_PROP_CAPS:
        g_value_set_flags(val, (ALSASeqPortCapFlag)priv->info.capability);
        break;
    case SEQ_PORT_INFO_PROP_ATTRS:
        g_value_set_flags(val, (ALSASeqPortAttrFlag)priv->info.type);
        break;
    case SEQ_PORT_INFO_PROP_MIDI_CHANNELS:
        g_value_set_int(val, priv->info.midi_channels);
        break;
    case SEQ_PORT_INFO_PROP_MIDI_VOICES:
        g_value_set_int(val, priv->info.midi_voices);
        break;
    case SEQ_PORT_INFO_PROP_SYNTH_VOICES:
        g_value_set_int(val, priv->info.synth_voices);
        break;
    case SEQ_PORT_INFO_PROP_READ_USERS:
        g_value_set_int(val, priv->info.read_use);
        break;
    case SEQ_PORT_INFO_PROP_WRITE_USERS:
        g_value_set_int(val, priv->info.write_use);
        break;
    case SEQ_PORT_INFO_PROP_TSTAMP_OVERWRITE:
        g_value_set_boolean(val, priv->info.flags & SNDRV_SEQ_PORT_FLG_TIMESTAMP);
        break;
    case SEQ_PORT_INFO_PROP_TSTAMP_MODE:
        if (priv->info.flags & SNDRV_SEQ_PORT_FLG_TIME_REAL)
            g_value_set_enum(val, ALSASEQ_EVENT_TIMESTAMP_MODE_REAL);
        else
            g_value_set_enum(val, ALSASEQ_EVENT_TIMESTAMP_MODE_TICK);
        break;
    case SEQ_PORT_INFO_PROP_TIME_QUEUE:
        g_value_set_uchar(val, priv->info.time_queue);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_port_info_class_init(ALSASeqPortInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_port_info_set_property;
    gobject_class->get_property = seq_port_info_get_property;

    seq_port_info_props[SEQ_PORT_INFO_PROP_ADDR] =
        g_param_spec_boxed("addr", "addr",
                           "The address of port.",
                           ALSASEQ_TYPE_ADDR,
                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    seq_port_info_props[SEQ_PORT_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of port.",
                            "",
                            G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_CAPS] =
        g_param_spec_flags("caps", "caps",
                           "The capabilities of port, a set of "
                           "ALSASeqPortCapFlag",
                           ALSASEQ_TYPE_PORT_CAP_FLAG,
                           0,
                           G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_ATTRS] =
        g_param_spec_flags("attrs", "attrs",
                           "The attributes of port, a set of "
                           "ALSASeqPortAttrFlag",
                           ALSASEQ_TYPE_PORT_ATTR_FLAG,
                           0,
                           G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_MIDI_CHANNELS] =
        g_param_spec_int("midi-channels", "midi-channels",
                         "The number of channels per MIDI port.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_MIDI_VOICES] =
        g_param_spec_int("midi-voices", "midi-voices",
                         "The number of voices per MIDI port",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_SYNTH_VOICES] =
        g_param_spec_int("synth-voices", "synth-voices",
                         "The number of voices per synth port",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_READ_USERS] =
        g_param_spec_int("read-users", "read-users",
                         "The current number of subscribers to read.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_WRITE_USERS] =
        g_param_spec_int("write-users", "write-users",
                         "The current number of subscribers to write.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_TSTAMP_OVERWRITE] =
        g_param_spec_boolean("timestamp-overwrite", "timestamp-overwrite",
                             "The mode whether to overwrite timestamp for "
                             "event when the event is delivered from the port.",
                             FALSE,
                             G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_TSTAMP_MODE] =
        g_param_spec_enum("timestamp-mode", "timestamp-mode",
                          "The type of timestamp. This is effective when the "
                          "timestamp-overwrite property is enabled.",
                          ALSASEQ_TYPE_EVENT_TIMESTAMP_MODE,
                          ALSASEQ_EVENT_TIMESTAMP_MODE_TICK,
                          G_PARAM_READWRITE);

    seq_port_info_props[SEQ_PORT_INFO_PROP_TIME_QUEUE] =
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numerical ID of queue to update timestamp "
                           "when timestamp-overwrite property is set to "
                           "ALSASeq.PortTstampOverwrite.TICK or "
                           "ALSASeq.PortTstampOverwrite.REAL. One of "
                           "ALSASeqSpecificQueueId is available as well.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_PORT_INFO_PROP_COUNT,
                                      seq_port_info_props);
}

static void alsaseq_port_info_init(ALSASeqPortInfo *self)
{
    return;
}

/**
 * alsaseq_port_info_new:
 *
 * Allocate and return an instance of ALSASeqPortInfo class.
 */
ALSASeqPortInfo *alsaseq_port_info_new()
{
    return g_object_new(ALSASEQ_TYPE_PORT_INFO, NULL);
}

void seq_port_info_refer_private(ALSASeqPortInfo *self,
                                 struct snd_seq_port_info **info)
{
    ALSASeqPortInfoPrivate *priv = alsaseq_port_info_get_instance_private(self);

    *info = &priv->info;
}
