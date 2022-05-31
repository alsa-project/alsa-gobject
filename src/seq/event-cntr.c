// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <errno.h>

/**
 * ALSASeqEventCntr:
 * A GObject-derived object to represent container for a batch of events.
 *
 * A [class@EventCntr] is a GObject-derived object to represent container for a batch of events.
 * The instance of object has accessor methods to properties and data for each events expanded to
 * the flat memory space. The object is designed for applications to maintain collections of event
 * by the convenient way which each programming language produces.
 *
 * This is the list of properties for event:
 * - the type of event
 * - the mode of time stamp
 * - the mode of time
 * - the mode of length
 * - the mode of priority
 * - associated tag
 * - the numeric ID of associated queue
 * - time stamp
 * - destination address
 * - source address
 *
 * This is the list of data for event. These data shared the same storage and an event can have the
 * sole type of data:
 * - note
 * - control
 * - 12 bytes
 * - 3 quadlets
 * - blob as variable length of bytes
 * - queue control
 * - arbitrary time stamp
 * - arbitrary address
 * - connection with source and destination addresses
 * - result
 *
 * The data shares the same storage in event. An event can have the sole type of data. The type of
 * data is not associated to the type of event directly.
 */
typedef struct _ALSASeqEventCntrPrivate {
    guint8 *buf;
    gsize length;

    gboolean allocated;
} ALSASeqEventCntrPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqEventCntr, alsaseq_event_cntr, G_TYPE_OBJECT)

static void seq_event_cntr_finaize(GObject *obj)
{
    ALSASeqEventCntr *self = ALSASEQ_EVENT_CNTR(obj);
    ALSASeqEventCntrPrivate *priv =
                                alsaseq_event_cntr_get_instance_private(self);

    if (priv->allocated)
        g_free(priv->buf);

    G_OBJECT_CLASS(alsaseq_event_cntr_parent_class)->finalize(obj);
}

static void alsaseq_event_cntr_class_init(ALSASeqEventCntrClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = seq_event_cntr_finaize;
}

static void alsaseq_event_cntr_init(ALSASeqEventCntr *self)
{
    ALSASeqEventCntrPrivate *priv =
                                alsaseq_event_cntr_get_instance_private(self);

    priv->buf = NULL;
    priv->length = 0;
    priv->allocated = FALSE;
}

/**
 * alsaseq_event_cntr_new:
 * @count: The number of events going to be allocated.
 * @error: A [struct@GLib.Error].
 *
 * Allocates and return an instance of [class@EventCntr] to store the count of events.
 *
 * Returns: A [class@EventCntr].
 */
ALSASeqEventCntr *alsaseq_event_cntr_new(guint count, GError **error)
{
    ALSASeqEventCntr *self;
    ALSASeqEventCntrPrivate *priv;
    struct snd_seq_event *ev;
    int i;

    g_return_val_if_fail(error == NULL || *error == NULL, NULL);

    self = g_object_new(ALSASEQ_TYPE_EVENT_CNTR, NULL);
    priv = alsaseq_event_cntr_get_instance_private(self);

    priv->length = sizeof(struct snd_seq_event) * count;
    priv->buf = g_malloc0(priv->length);

    priv->allocated = TRUE;

    ev = (struct snd_seq_event *)priv->buf;
    for (i = 0; i < count; ++i) {
        ev[i].type = SNDRV_SEQ_EVENT_NONE;
        ev[i].queue = SNDRV_SEQ_QUEUE_DIRECT;
    }

    return self;
}

static gsize calculate_event_size(struct snd_seq_event *ev, gboolean aligned)
{
    gsize size = sizeof(*ev);

    switch (ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
    {
        const struct snd_seq_ev_ext *ext = &ev->data.ext;
        if (aligned)
            size += ext->len;
        else
            size += (ext->len + sizeof(*ev) - 1) / sizeof(*ev) * sizeof(*ev);
        break;
    }
    default:
        break;
    }

    return size;
}

struct event_iterator {
    guint8 *buf;
    gsize length;
    gboolean aligned;

    gsize offset;
};

static void event_iterator_init(struct event_iterator *iter, guint8 *buf,
                                gsize length, gboolean aligned)
{
    iter->buf = buf;
    iter->length = length;
    iter->aligned = aligned;

    iter->offset = 0;
}

static struct snd_seq_event *event_iterator_next(struct event_iterator *iter)
{
    struct snd_seq_event *ev;
    gsize ev_size;

    if (iter->offset + sizeof(*ev) > iter->length)
        return NULL;
    ev = (struct snd_seq_event *)(iter->buf + iter->offset);

    ev_size = calculate_event_size(ev, iter->aligned);
    if (iter->offset + ev_size > iter->length)
        return NULL;
    iter->offset += ev_size;

    return ev;
}

static struct snd_seq_event *event_iterator_find(struct event_iterator *iter,
                                                 gsize index)
{
    struct snd_seq_event *ev;
    gsize count = 0;

    while ((ev = event_iterator_next(iter))) {
        if (index == count)
            return ev;
        ++count;
    }

    return NULL;
}

/**
 * alsaseq_event_cntr_count_events:
 * @self: A [class@EventCntr].
 * @count: (out): The count of stored events.
 *
 * count stored events.
 */
void alsaseq_event_cntr_count_events(ALSASeqEventCntr *self, gsize *count)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_if_fail(count != NULL);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    *count = 0;
    while ((ev = event_iterator_next(&iter)))
        ++(*count);
}

/**
 * alsaseq_event_cntr_calculate_pool_consumption:
 * @self: A [class@EventCntr].
 * @count: The amount of events for calculation.
 * @cells: (out): The amount of cells to be consumed in pool.
 * @error: A [struct@GLib.Error].
 *
 * Calculate the amount of cells in client pool to be consumed by a part of events in the container.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_calculate_pool_consumption(ALSASeqEventCntr *self, gsize count,
                                                       gsize *cells, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;
    gsize total;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(cells != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *cells = 0;
    total = 0;
    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    while ((ev = event_iterator_next(&iter))) {
        gsize size = calculate_event_size(ev, TRUE);
        *cells += size /sizeof(*ev);

        if (total == count)
            break;
        ++total;
    }

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_event_type:
 * @self: A [class@EventCntr].
 * @index: The index of event to refer to.
 * @ev_type: (out): The type of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the type of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_event_type(ALSASeqEventCntr *self, gsize index,
                                           ALSASeqEventType *ev_type, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(ev_type != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *ev_type = (ALSASeqEventType)ev->type;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_event_type:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @ev_type: A [enum@EventType].
 * @error: A [struct@GLib.Error].
 *
 * Set the type to event pointed by the index;
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_event_type(ALSASeqEventCntr *self, gsize index,
                                           ALSASeqEventType ev_type, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->type = (snd_seq_event_type_t)ev_type;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_tstamp_mode:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @mode: (out): The mode of timestamping, one of [enum@EventTimestampMode].
 * @error: A [struct@GLib.Error].
 *
 * Get the mode of timestamping for the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                            ALSASeqEventTimestampMode *mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(mode != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *mode = (ALSASeqEventTimestampMode)(ev->flags & SNDRV_SEQ_TIME_STAMP_MASK);

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_tstamp_mode:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @mode: The mode of timestamping, one of [enum@EventTimestampMode].
 * @error: A [struct@GLib.Error].
 *
 * Set the mode of timestamping for the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                            ALSASeqEventTimestampMode mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->flags &= ~SNDRV_SEQ_TIME_STAMP_MASK;
    ev->flags |= (unsigned char)mode;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_time_mode:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @mode: (out): The mode of time, one of [enum@EventTimestampMode].
 * @error: A [struct@GLib.Error].
 *
 * Get the mode of time for the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_time_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimeMode *mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(mode != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *mode = (ALSASeqEventTimeMode)(ev->flags & SNDRV_SEQ_TIME_MODE_MASK);

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_time_mode:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @mode: The mode of time, one of [enum@EventTimestampMode].
 * @error: A [struct@GLib.Error].
 *
 * Set the mode of time for the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_time_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimeMode mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->flags &= ~SNDRV_SEQ_TIME_MODE_MASK;
    ev->flags |= (unsigned char)mode;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_length_mode:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @mode: (out): The mode of length, one of [enum@EventTimestampMode].
 * @error: A [struct@GLib.Error].
 *
 * Get the mode of length for the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_length_mode(ALSASeqEventCntr *self, gsize index,
                                            ALSASeqEventLengthMode *mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(mode != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *mode = (ALSASeqEventLengthMode)(ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK);

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_priority_mode:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @mode: (out): The mode of priority, one of [enum@EventTimestampMode].
 * @error: A [struct@GLib.Error].
 *
 * Get the mode of priority for the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_priority_mode(ALSASeqEventCntr *self, gsize index,
                                              ALSASeqEventPriorityMode *mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(mode != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *mode = (ALSASeqEventPriorityMode)(ev->flags & SNDRV_SEQ_PRIORITY_MASK);

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_priority_mode:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @mode: The mode of priority, one of [enum@EventTimestampMode].
 * @error: A [struct@GLib.Error].
 *
 * Set the mode of priority for the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_priority_mode(ALSASeqEventCntr *self, gsize index,
                                              ALSASeqEventPriorityMode mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->flags &= ~SNDRV_SEQ_PRIORITY_MASK;
    ev->flags |= (unsigned char)mode;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_tag:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @tag: (out): The tag assigned to the event.
 * @error: A [struct@GLib.Error].
 *
 * Get the tag assignd to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_tag(ALSASeqEventCntr *self, gsize index, gint8 *tag,
                                    GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *tag = ev->tag;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_tag:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @tag: The tag going to be assignd to the event.
 * @error: A [struct@GLib.Error].
 *
 * Get the tag assignd to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_tag(ALSASeqEventCntr *self, gsize index, gint8 tag, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->tag = tag;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_queue_id:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @queue_id: (out): The numeric ID of queue to deliver the event. One of [enum@SpecificQueueId]
 *            is available as well.
 * @error: A [struct@GLib.Error].
 *
 * Get the numeric ID of queue to deliver the event.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_queue_id(ALSASeqEventCntr *self, gsize index, guint8 *queue_id,
                                         GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(queue_id != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *queue_id = ev->queue;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_queue_id:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @queue_id: The numeric ID of queue to deliver the event. One of [enum@SpecificQueueId] is
 *            available as well.
 * @error: A [struct@GLib.Error].
 *
 * Set the numeric ID of queue to deliver the event.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_queue_id(ALSASeqEventCntr *self, gsize index, guint8 queue_id,
                                         GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->queue = queue_id;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_tstamp:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @tstamp: (out)(transfer none): The timestamp for the event. The content is affected by the mode
 *          of tstamping.
 * @error: A [struct@GLib.Error].
 *
 * Get the timestamp of event pointed by index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_tstamp(ALSASeqEventCntr *self, gsize index,
                                       const ALSASeqTstamp **tstamp, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(tstamp != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *tstamp = (const ALSASeqTstamp *)&ev->time;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_tstamp:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @tstamp: The timestamp for the event. The content is affected by the mode of tstamping.
 * @error: A [struct@GLib.Error].
 *
 * Set the timestamp for the event pointed by index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_tstamp(ALSASeqEventCntr *self, gsize index,
                                       const ALSASeqTstamp *tstamp, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(tstamp != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->time = *tstamp;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_dst:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @dst: (out)(transfer none): The destination of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the destination of event pointed by index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_dst(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr **dst,
                                    GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(dst != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *dst = (const ALSASeqAddr *)&ev->dest;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_dst:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @dst: The destination of event.
 * @error: A [struct@GLib.Error].
 *
 * Set the destination of event pointed by index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_dst(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr *dst,
                                    GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(dst != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->dest = *dst;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_src:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @src: (out)(transfer none): The source of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the destination of event pointed by index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_src(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr **src,
                                    GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(src != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *src = (const ALSASeqAddr *)&ev->source;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_src:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @src: The source of event.
 * @error: A [struct@GLib.Error].
 *
 * Set the destination of event pointed by index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_src(ALSASeqEventCntr *self, gsize index, const ALSASeqAddr *src,
                                    GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(src != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    ev->source = *src;

    return TRUE;
}

static gboolean ensure_fixed_length_event(ALSASeqEventCntrPrivate *priv, struct snd_seq_event *ev,
                                          GError **error)
{
    if (!priv->allocated)
        g_return_val_if_fail(priv->allocated, FALSE);

    switch (ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
    {
        // Truncate blob area.
        guint8 *pos = (guint8 *)ev;
        guint8 *next_ev = pos + sizeof(*ev) + ev->data.ext.len;
        ptrdiff_t to_tail = priv->length - (next_ev - priv->buf);

        memcpy(pos + sizeof(*ev), next_ev, to_tail);

        priv->length -= ev->data.ext.len;
        break;
    }
    default:
        break;
    }

    ev->data.ext.ptr = NULL;
    ev->data.ext.len = 0;

    ev->flags &= ~SNDRV_SEQ_EVENT_LENGTH_MASK;
    ev->flags |= SNDRV_SEQ_EVENT_LENGTH_FIXED;

    return TRUE;
}

static gboolean ensure_variable_length_event(ALSASeqEventCntrPrivate *priv,
                                             struct snd_seq_event *ev, const guint8 *data,
                                             gsize size, GError **error)
{
    guint8 *pos = (guint8 *)ev;
    ptrdiff_t from_head = pos + sizeof(*ev) - priv->buf;
    ptrdiff_t to_tail;
    guint8 *next_ev;
    guint8 *new;

    if (!priv->allocated)
        g_return_val_if_fail(priv->allocated, FALSE);

    switch (ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
        // Expand or shrink total area for new blob.
        next_ev = pos + sizeof(*ev) + ev->data.ext.len;
        break;
    default:
        // Expand total area with blob size.
        next_ev = pos + sizeof(*ev);
        break;
    }

    to_tail = priv->length - (next_ev - priv->buf);

    new = g_malloc(from_head + size + to_tail);

    memcpy(new, priv->buf, from_head);
    memcpy(new + from_head, data, size);
    memcpy(new + from_head + size, next_ev, to_tail);

    g_free(priv->buf);
    priv->buf = new;
    priv->length = from_head + size + to_tail;

    from_head -= sizeof(*ev);
    pos = priv->buf + from_head;
    ev = (struct snd_seq_event *)pos;
    ev->data.ext.len = size;

    ev->flags &= ~SNDRV_SEQ_EVENT_LENGTH_MASK;
    ev->flags |= SNDRV_SEQ_EVENT_LENGTH_VARIABLE;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_note_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (out)(transfer none): The note data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the note data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_note_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqEventDataNote **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = (const ALSASeqEventDataNote *)&ev->data.note;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_note_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: The note data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the note data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_note_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqEventDataNote *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    ev->data.note = *(struct snd_seq_ev_note *)data;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_ctl_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (out)(transfer none): The control data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the control data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_ctl_data(ALSASeqEventCntr *self, gsize index,
                                         const ALSASeqEventDataCtl **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = (const ALSASeqEventDataCtl *)&ev->data.control;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_ctl_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: The control data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the control data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_ctl_data(ALSASeqEventCntr *self, gsize index,
                                         const ALSASeqEventDataCtl *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    ev->data.control = *(struct snd_seq_ev_ctrl *)data;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_byte_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (array fixed-size=12)(out)(transfer none): The byte data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the byte data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_byte_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 *data[12], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = ev->data.raw8.d;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_byte_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (array fixed-size=12): The byte data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the byte data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_byte_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 data[12], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    memcpy(ev->data.raw8.d, data, sizeof(ev->data.raw8.d));

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_quadlet_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (array fixed-size=3)(out)(transfer none): The quadlet data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the quadlet data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_quadlet_data(ALSASeqEventCntr *self, gsize index,
                                             const guint32 *data[3], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = ev->data.raw32.d;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_quadlet_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (array fixed-size=3): The quadlet data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the quadlet data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_quadlet_data(ALSASeqEventCntr *self, gsize index,
                                             const guint32 data[3], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    memcpy(ev->data.raw32.d, data, sizeof(ev->data.raw32.d));

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_blob_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (array length=size)(out)(transfer none): The pointer to blob data.
 * @size: The size of data.
 * @error: A [struct@GLib.Error].
 *
 * Refer to the blob data of event.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_blob_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 **data, gsize *size, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(size != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    switch (ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
    {
        const struct snd_seq_ev_ext *ext = &ev->data.ext;
        if (priv->allocated)
            *data = ((guint8 *)ev) + sizeof(*ev);
        else
            *data = ext->ptr;
        *size = ext->len;
        break;
    }
    default:
        g_return_val_if_reached(FALSE);
    }

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_blob_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (array length=size): The pointer to blob data for the event.
 * @size: The size of data.
 * @error: A [struct@GLib.Error].
 *
 * Copy the quadlet data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_blob_data(ALSASeqEventCntr *self, gsize index,
                                          const guint8 *data, gsize size, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    return ensure_variable_length_event(priv, ev, data, size, error);
}

/**
 * alsaseq_event_cntr_get_queue_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (out)(transfer none): The queue data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the queue data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_queue_data(ALSASeqEventCntr *self, gsize index,
                                           const ALSASeqEventDataQueue **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = (const ALSASeqEventDataQueue *)&ev->data.queue;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_queue_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: The queue data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the queue data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_queue_data(ALSASeqEventCntr *self, gsize index,
                                           const ALSASeqEventDataQueue *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    ev->data.queue = *(struct snd_seq_ev_queue_control *)data;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_tstamp_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (out)(transfer none): The timestamp data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the timestamp data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_tstamp_data(ALSASeqEventCntr *self, gsize index,
                                            const ALSASeqTstamp **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = (const ALSASeqTstamp *)&ev->data.time;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_tstamp_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: The timestamp data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the timestamp data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_tstamp_data(ALSASeqEventCntr *self, gsize index,
                                            const ALSASeqTstamp *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    ev->data.time = *(union snd_seq_timestamp *)data;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_addr_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (out)(transfer none): The address data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the address data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_addr_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqAddr **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = (const ALSASeqAddr *)&ev->data.time;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_addr_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: The address data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the address data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_addr_data(ALSASeqEventCntr *self, gsize index,
                                          const ALSASeqAddr *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    ev->data.addr = *(struct snd_seq_addr *)data;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_connect_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (out)(transfer none): The connect data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the connect data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_connect_data(ALSASeqEventCntr *self, gsize index,
                                             const ALSASeqEventDataConnect **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = (const ALSASeqEventDataConnect *)&ev->data.connect;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_connect_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: The connect data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the connect data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_connect_data(ALSASeqEventCntr *self, gsize index,
                                             const ALSASeqEventDataConnect *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    ev->data.connect = *(struct snd_seq_connect *)data;

    return TRUE;
}

/**
 * alsaseq_event_cntr_get_result_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: (out)(transfer none): The result data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the result data of event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_get_result_data(ALSASeqEventCntr *self, gsize index,
                                            const ALSASeqEventDataResult **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    *data = (const ALSASeqEventDataResult *)&ev->data.result;

    return TRUE;
}

/**
 * alsaseq_event_cntr_set_result_data:
 * @self: A [class@EventCntr].
 * @index: The index of event to set.
 * @data: The result data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the result data to the event pointed by the index.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_cntr_set_result_data(ALSASeqEventCntr *self, gsize index,
                                            const ALSASeqEventDataResult *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_val_if_fail(ALSASEQ_IS_EVENT_CNTR(self), FALSE);
    priv = alsaseq_event_cntr_get_instance_private(self);

    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    g_return_val_if_fail(ev != NULL, FALSE);

    if (!ensure_fixed_length_event(priv, ev, error))
        return FALSE;

    ev->data.result = *(struct snd_seq_result *)data;

    return TRUE;
}

void seq_event_cntr_set_buf(ALSASeqEventCntr *self, guint8 *buf,
                              gsize length)
{
    ALSASeqEventCntrPrivate *priv =
                                alsaseq_event_cntr_get_instance_private(self);
    priv->buf = buf;
    priv->length = length;
}

void seq_event_cntr_get_buf(ALSASeqEventCntr *self, gsize count,
                              const guint8 **buf, gsize *length)
{
    ALSASeqEventCntrPrivate *priv =
                                alsaseq_event_cntr_get_instance_private(self);
    struct event_iterator iter;

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    if (event_iterator_find(&iter, count - 1) != NULL) {
        *buf = priv->buf;
        *length = iter.offset;
    } else {
        *buf = NULL;
        *length = 0;
    }
}
