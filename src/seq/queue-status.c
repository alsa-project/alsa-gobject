// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqQueueStatus:
 * A GObject-derived object to represent status of queue.
 *
 * A [class@QueueStatus] is a GObject-derived object to represent status of queue. The call of
 * [func@get_queue_status] returns the instance of object.
 *
 * The object wraps `struct snd_seq_queue_status` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_seq_queue_status status;
} ALSASeqQueueStatusPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqQueueStatus, alsaseq_queue_status, G_TYPE_OBJECT)

enum seq_queue_status_prop_type {
    SEQ_QUEUE_STATUS_PROP_QUEUE_ID = 1,
    SEQ_QUEUE_STATUS_PROP_EVENT_COUNT,
    SEQ_QUEUE_STATUS_PROP_RUNNING,
    SEQ_QUEUE_STATUS_PROP_COUNT,
};
static GParamSpec *seq_queue_status_props[SEQ_QUEUE_STATUS_PROP_COUNT] = { NULL, };

static void seq_queue_status_get_property(GObject *obj, guint id, GValue *val,
                                          GParamSpec *spec)
{
    ALSASeqQueueStatus *self = ALSASEQ_QUEUE_STATUS(obj);
    ALSASeqQueueStatusPrivate *priv =
                                alsaseq_queue_status_get_instance_private(self);

    switch (id) {
    case SEQ_QUEUE_STATUS_PROP_QUEUE_ID:
        g_value_set_int(val, priv->status.queue);
        break;
    case SEQ_QUEUE_STATUS_PROP_EVENT_COUNT:
        g_value_set_int(val, priv->status.events);
        break;
    case SEQ_QUEUE_STATUS_PROP_RUNNING:
        g_value_set_boolean(val, priv->status.running);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_queue_status_class_init(ALSASeqQueueStatusClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->get_property = seq_queue_status_get_property;

    seq_queue_status_props[SEQ_QUEUE_STATUS_PROP_QUEUE_ID] =
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numerical ID of queue, except for entries in "
                           "ALSASeqSpecificQueueId.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READABLE);

    seq_queue_status_props[SEQ_QUEUE_STATUS_PROP_EVENT_COUNT] =
        g_param_spec_int("event-count", "event-count",
                         "The number of available events in the queue.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    seq_queue_status_props[SEQ_QUEUE_STATUS_PROP_RUNNING] =
        g_param_spec_boolean("running", "running",
                             "Whether the queue is running or not.",
                             FALSE,
                             G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_QUEUE_STATUS_PROP_COUNT,
                                      seq_queue_status_props);
}

static void alsaseq_queue_status_init(ALSASeqQueueStatus *self)
{
    return;
}

/**
 * alsaseq_queue_status_new:
 *
 * Allocate and returns an instance of [class@QueueStatus].
 *
 * Returns: An instance of [class@QueueStatus].
 */
ALSASeqQueueStatus *alsaseq_queue_status_new()
{
    return g_object_new(ALSASEQ_TYPE_QUEUE_STATUS, NULL);
}

/**
 * alsaseq_queue_status_get_tick_time:
 * @self: A [class@QueueStatus].
 * @tick_time: (out): The value of MIDI ticks.
 *
 * Get time as MIDI ticks.
 */
void alsaseq_queue_status_get_tick_time(ALSASeqQueueStatus *self,
                                        guint *tick_time)
{
    ALSASeqQueueStatusPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_QUEUE_STATUS(self));
    priv = alsaseq_queue_status_get_instance_private(self);

    g_return_if_fail(tick_time != NULL);

    *tick_time = priv->status.tick;
}

/**
 * alsaseq_queue_status_get_real_time:
 * @self: A [class@QueueStatus].
 * @real_time: (array fixed-size=2)(out)(transfer none): The array with two elements for sec part
 *             and nsec part of real time.
 *
 * Get time as wall-clock time.
 */
void alsaseq_queue_status_get_real_time(ALSASeqQueueStatus *self,
                                        const guint32 *real_time[2])
{
    ALSASeqQueueStatusPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_QUEUE_STATUS(self));
    priv = alsaseq_queue_status_get_instance_private(self);

    g_return_if_fail(real_time != NULL);

    // MEMO: I wish 32-bit storage size is aligned to 32 bit offset in all of
    // supported ABIs.
    *real_time = (const guint32 *)&priv->status.time;
}

void seq_queue_status_refer_private(ALSASeqQueueStatus *self,
                                    struct snd_seq_queue_status **status)
{
    ALSASeqQueueStatusPrivate *priv =
                                alsaseq_queue_status_get_instance_private(self);

    *status = &priv->status;
}
