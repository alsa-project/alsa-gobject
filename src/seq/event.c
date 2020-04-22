// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <errno.h>

struct _ALSASeqEventPrivate {
    struct snd_seq_event ev;
};
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(ALSASeqEvent, alsaseq_event, G_TYPE_OBJECT)

enum seq_event_prop_type {
    SEQ_EVENT_PROP_EVENT_TYPE = 1,
    SEQ_EVENT_PROP_MODE_TIMESTAMP,
    SEQ_EVENT_PROP_MODE_TIME,
    SEQ_EVENT_PROP_MODE_LENGTH,
    SEQ_EVENT_PROP_MODE_PRIORITY,
    SEQ_EVENT_PROP_TAG,
    SEQ_EVENT_PROP_QUEUE_ID,
    SEQ_EVENT_PROP_TSTAMP,
    SEQ_EVENT_PROP_SRC_ADDR,
    SEQ_EVENT_PROP_DST_ADDR,
    SEQ_EVENT_PROP_COUNT,
};
static GParamSpec *seq_event_props[SEQ_EVENT_PROP_COUNT] = { NULL, };

static void seq_event_set_property(GObject *obj, guint id, const GValue *val,
                                   GParamSpec *spec)
{
    ALSASeqEvent *self = ALSASEQ_EVENT(obj);
    ALSASeqEventPrivate *priv = alsaseq_event_get_instance_private(self);
    struct snd_seq_event *ev = &priv->ev;

    switch (id) {
    case SEQ_EVENT_PROP_EVENT_TYPE:
	ev->type = (snd_seq_event_type_t)g_value_get_enum(val);
        break;
    case SEQ_EVENT_PROP_MODE_TIMESTAMP:
        ev->flags &= ~SNDRV_SEQ_TIME_STAMP_MASK;
        ev->flags |= (unsigned char)g_value_get_enum(val);
        break;
    case SEQ_EVENT_PROP_MODE_TIME:
        ev->flags &= ~SNDRV_SEQ_TIME_MODE_MASK;
        ev->flags |= (unsigned char)g_value_get_enum(val);
        break;
    case SEQ_EVENT_PROP_MODE_LENGTH:
        ev->flags &= ~SNDRV_SEQ_EVENT_LENGTH_MASK;
        ev->flags |= (unsigned char)g_value_get_enum(val);
        break;
    case SEQ_EVENT_PROP_MODE_PRIORITY:
        ev->flags &= ~SNDRV_SEQ_PRIORITY_MASK;
        ev->flags |= (unsigned char)g_value_get_enum(val);
        break;
    case SEQ_EVENT_PROP_TAG:
        ev->tag = g_value_get_schar(val);
        break;
    case SEQ_EVENT_PROP_QUEUE_ID:
        ev->queue = g_value_get_uchar(val);
        break;
    case SEQ_EVENT_PROP_TSTAMP:
    {
        ALSASeqTstamp *tstamp = g_value_get_boxed(val);
        if (tstamp != NULL)
            ev->time = *tstamp;
        break;
    }
    case SEQ_EVENT_PROP_SRC_ADDR:
    {
        ALSASeqAddr *addr = g_value_get_boxed(val);
        if (addr != NULL)
            ev->source = *addr;
        break;
    }
    case SEQ_EVENT_PROP_DST_ADDR:
    {
        ALSASeqAddr *addr = g_value_get_boxed(val);
        if (addr != NULL)
            ev->dest = *addr;
        break;
    }
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_event_get_property(GObject *obj, guint id, GValue *val,
                                   GParamSpec *spec)
{
    ALSASeqEvent *self = ALSASEQ_EVENT(obj);
    ALSASeqEventPrivate *priv = alsaseq_event_get_instance_private(self);
    struct snd_seq_event *ev = &priv->ev;

    switch (id) {
    case SEQ_EVENT_PROP_EVENT_TYPE:
        g_value_set_enum(val, (ALSASeqEventType)ev->type);
        break;
    case SEQ_EVENT_PROP_MODE_TIMESTAMP:
    {
        ALSASeqEventTimestampMode entry;
        entry = ev->flags & SNDRV_SEQ_TIME_STAMP_MASK;
        g_value_set_enum(val, entry);
        break;
    }
    case SEQ_EVENT_PROP_MODE_TIME:
    {
        ALSASeqEventTimeMode entry;
        entry = ev->flags & SNDRV_SEQ_TIME_MODE_MASK;
        g_value_set_enum(val, entry);
        break;
    }
    case SEQ_EVENT_PROP_MODE_LENGTH:
    {
        ALSASeqEventLengthMode entry;
        entry = ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK;
        g_value_set_enum(val, entry);
        break;
    }
    case SEQ_EVENT_PROP_MODE_PRIORITY:
    {
        ALSASeqEventPriorityMode entry;
        entry = ev->flags & SNDRV_SEQ_PRIORITY_MASK;
        g_value_set_enum(val, entry);
        break;
    }
    case SEQ_EVENT_PROP_TAG:
        g_value_set_schar(val, ev->tag);
        break;
    case SEQ_EVENT_PROP_QUEUE_ID:
        g_value_set_uchar(val, ev->queue);
        break;
    case SEQ_EVENT_PROP_TSTAMP:
        g_value_set_static_boxed(val, &ev->time);
        break;
    case SEQ_EVENT_PROP_SRC_ADDR:
        g_value_set_static_boxed(val, &ev->source);
        break;
    case SEQ_EVENT_PROP_DST_ADDR:
        g_value_set_static_boxed(val, &ev->dest);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_event_class_init(ALSASeqEventClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_event_set_property;
    gobject_class->get_property = seq_event_get_property;

    seq_event_props[SEQ_EVENT_PROP_EVENT_TYPE] =
        g_param_spec_enum("type", "type",
                         "The type of event, one of ALSASeqEventType.",
                         ALSASEQ_TYPE_EVENT_TYPE,
                         ALSASEQ_EVENT_TYPE_NONE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    seq_event_props[SEQ_EVENT_PROP_MODE_TIMESTAMP] =
        g_param_spec_enum("timestamp-mode", "timestamp-mode",
                          "The mode of timestamp for the event, one of "
                          "ALSASeqEventTimestampMode",
                          ALSASEQ_TYPE_EVENT_TIMESTAMP_MODE,
                          ALSASEQ_EVENT_TIMESTAMP_MODE_TICK,
                          G_PARAM_READWRITE);

    seq_event_props[SEQ_EVENT_PROP_MODE_TIME] =
        g_param_spec_enum("time-mode", "time-mode",
                          "The mode of time for the event, one of "
                          "ALSASeqEventTimeMode",
                          ALSASEQ_TYPE_EVENT_TIME_MODE,
                          ALSASEQ_EVENT_TIME_MODE_ABS,
                          G_PARAM_READWRITE);

    seq_event_props[SEQ_EVENT_PROP_MODE_LENGTH] =
        g_param_spec_enum("length-mode", "length-mode",
                          "The mode of length for the event, one of "
                          "ALSASeqEventLengthMode",
                          ALSASEQ_TYPE_EVENT_LENGTH_MODE,
                          ALSASEQ_EVENT_LENGTH_MODE_FIXED,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    seq_event_props[SEQ_EVENT_PROP_MODE_PRIORITY] =
        g_param_spec_enum("priority-mode", "priority-mode",
                          "The mode of priority for the event, one of "
                          "ALSASeqEventPriorityMode",
                          ALSASEQ_TYPE_EVENT_PRIORITY_MODE,
                          ALSASEQ_EVENT_PRIORITY_MODE_NORMAL,
                          G_PARAM_READWRITE);

    seq_event_props[SEQ_EVENT_PROP_TAG] =
        g_param_spec_char("tag", "tag",
                          "The arbitrary tag associated to the event.",
                          G_MININT8, G_MAXINT8,
                          0,
                          G_PARAM_READWRITE);

    seq_event_props[SEQ_EVENT_PROP_QUEUE_ID] =
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numerical ID of queue to deliver the event. "
                           "One of ALSASeqSpecificQueueId is available as well.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    seq_event_props[SEQ_EVENT_PROP_TSTAMP] =
        g_param_spec_boxed("tstamp", "tstamp",
                           "The timestamp for the event. The content is "
                           "decided according to timestamp-mode property.",
                           ALSASEQ_TYPE_TSTAMP,
                           G_PARAM_READWRITE);

    seq_event_props[SEQ_EVENT_PROP_SRC_ADDR] =
        g_param_spec_boxed("src-addr", "src-addr",
                           "The address of source for the event.",
                           ALSASEQ_TYPE_ADDR,
                           G_PARAM_READWRITE);

    seq_event_props[SEQ_EVENT_PROP_DST_ADDR] =
        g_param_spec_boxed("dst-addr", "dst-addr",
                           "The address of destination for the event.",
                           ALSASEQ_TYPE_ADDR,
                           G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_EVENT_PROP_COUNT,
                                      seq_event_props);
}

static void alsaseq_event_init(ALSASeqEvent *self)
{
    return;
}

void seq_event_refer_private(ALSASeqEvent *self, struct snd_seq_event **ev)
{
    ALSASeqEventPrivate *priv = alsaseq_event_get_instance_private(self);
    *ev = &priv->ev;
}
