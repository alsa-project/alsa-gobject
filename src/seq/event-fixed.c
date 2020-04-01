// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-fixed.h"
#include "privates.h"

#include <errno.h>

G_DEFINE_TYPE(ALSASeqEventFixed, alsaseq_event_fixed, ALSASEQ_TYPE_EVENT)

enum seq_event_fixed_prop_type {
    SEQ_EVENT_FIXED_PROP_RESULT_DATA = 1,
    SEQ_EVENT_FIXED_PROP_NOTE_DATA,
    SEQ_EVENT_FIXED_PROP_CTL_DATA,
    SEQ_EVENT_FIXED_PROP_QUEUE_DATA,
    SEQ_EVENT_FIXED_PROP_COUNT,
};
static GParamSpec *seq_event_fixed_props[SEQ_EVENT_FIXED_PROP_COUNT] = { NULL, };

static void seq_event_fixed_set_property(GObject *obj, guint id,
                                         const GValue *val, GParamSpec *spec)
{
    ALSASeqEvent *parent = ALSASEQ_EVENT(obj);
    struct snd_seq_event *ev;
    seq_event_refer_private(parent, &ev);

    switch (id) {
    case SEQ_EVENT_FIXED_PROP_RESULT_DATA:
    {
        ALSASeqEventDataResult *data = g_value_get_boxed(val);
        if (data != NULL)
            ev->data.result = *data;
        break;
    }
    case SEQ_EVENT_FIXED_PROP_NOTE_DATA:
    {
        ALSASeqEventDataNote *data = g_value_get_boxed(val);
        if (data != NULL)
            ev->data.note = *data;
        break;
    }
    case SEQ_EVENT_FIXED_PROP_CTL_DATA:
    {
        ALSASeqEventDataCtl *data = g_value_get_boxed(val);
        if (data != NULL)
            ev->data.control = *data;
        break;
    }
    case SEQ_EVENT_FIXED_PROP_QUEUE_DATA:
    {
        ALSASeqEventDataQueue *data = g_value_get_boxed(val);
        if (data != NULL)
            ev->data.queue = *data;
        break;
    }
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_event_fixed_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqEvent *parent = ALSASEQ_EVENT(obj);
    struct snd_seq_event *ev;
    seq_event_refer_private(parent, &ev);

    switch (id) {
    case SEQ_EVENT_FIXED_PROP_RESULT_DATA:
        g_value_set_static_boxed(val, &ev->data.result);
        break;
    case SEQ_EVENT_FIXED_PROP_NOTE_DATA:
        g_value_set_static_boxed(val, &ev->data.note);
        break;
    case SEQ_EVENT_FIXED_PROP_CTL_DATA:
        g_value_set_static_boxed(val, &ev->data.control);
        break;
    case SEQ_EVENT_FIXED_PROP_QUEUE_DATA:
        g_value_set_static_boxed(val, &ev->data.queue);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_event_fixed_class_init(ALSASeqEventFixedClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_event_fixed_set_property;
    gobject_class->get_property = seq_event_fixed_get_property;

    seq_event_fixed_props[SEQ_EVENT_FIXED_PROP_RESULT_DATA] =
        g_param_spec_boxed("result-data", "result-data",
                           "The data of result type. This shares the same "
                           "storage between the other properties",
                           ALSASEQ_TYPE_EVENT_DATA_RESULT,
                           G_PARAM_READWRITE);

    seq_event_fixed_props[SEQ_EVENT_FIXED_PROP_NOTE_DATA] =
        g_param_spec_boxed("note-data", "note-data",
                           "The data of note type. This shares the same "
                           "storage between the other properties",
                           ALSASEQ_TYPE_EVENT_DATA_NOTE,
                           G_PARAM_READWRITE);

    seq_event_fixed_props[SEQ_EVENT_FIXED_PROP_CTL_DATA] =
        g_param_spec_boxed("ctl-data", "ctl-data",
                           "The data of ctl type. This shares the same "
                           "storage between the other properties",
                           ALSASEQ_TYPE_EVENT_DATA_CTL,
                           G_PARAM_READWRITE);

    seq_event_fixed_props[SEQ_EVENT_FIXED_PROP_QUEUE_DATA] =
        g_param_spec_boxed("queue-data", "queue-data",
                           "The data of queue type. This shares the same "
                           "storage between the other properties",
                           ALSASEQ_TYPE_EVENT_DATA_QUEUE,
                           G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_EVENT_FIXED_PROP_COUNT,
                                      seq_event_fixed_props);
}

static void alsaseq_event_fixed_init(ALSASeqEventFixed *self)
{
    return;
}

/**
 * alsaseq_event_fixed_new:
 * @event_type: A #ALSASeqEventType.
 * @error: A #GError.
 *
 * Allocate and returns an instance of #ALSASeqEventFixed class.
 *
 * Returns: (transfer full): A #ALSASeqEventFixed.
 */
ALSASeqEventFixed *alsaseq_event_fixed_new(ALSASeqEventType event_type,
                                           GError **error)
{
    switch (event_type) {
    case ALSASEQ_EVENT_TYPE_BOUNCE:
    case ALSASEQ_EVENT_TYPE_USR_VAR0:
    case ALSASEQ_EVENT_TYPE_USR_VAR1:
    case ALSASEQ_EVENT_TYPE_USR_VAR2:
    case ALSASEQ_EVENT_TYPE_USR_VAR3:
    case ALSASEQ_EVENT_TYPE_USR_VAR4:
        generate_error(error, EINVAL);
        return NULL;
    default:
	break;
    }

    return g_object_new(ALSASEQ_TYPE_EVENT_FIXED,
                        "type", event_type,
                        "length-mode", ALSASEQ_EVENT_LENGTH_MODE_FIXED,
                        NULL);
}
