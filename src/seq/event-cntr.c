// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <errno.h>

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
 * @error: A #GError.
 *
 * Allocates and return an instance of #ALSASeqEventCntr to store the count
 * of events.
 *
 * Returns: A #ALSASeqEventCntr.
 */
ALSASeqEventCntr *alsaseq_event_cntr_new(guint count, GError **error)
{
    ALSASeqEventCntr *self = g_object_new(ALSASEQ_TYPE_EVENT_CNTR, NULL);
    ALSASeqEventCntrPrivate *priv =
                                alsaseq_event_cntr_get_instance_private(self);
    struct snd_seq_event *ev;
    int i;

    priv->length = sizeof(struct snd_seq_event) * count;
    priv->buf = g_try_malloc0(priv->length);
    if (priv->buf == NULL) {
        generate_error(error, ENOMEM);
        return NULL;
    }
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
 * @self: A #ALSASeqEventCntr.
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

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    *count = 0;
    while ((ev = event_iterator_next(&iter)))
        ++(*count);
}

/**
 * alsaseq_event_cntr_calculate_pool_consumption:
 * @self: A #ALSASeqEventCntr.
 * @count: The amount of events for calculation.
 * @cells: (out): The amount of cells to be consumed in pool.
 * @error: A #GError.
 *
 * Calculate the amount of cells in client pool to be consumed by a part
 * of events in the container.
 */
void alsaseq_event_cntr_calculate_pool_consumption(ALSASeqEventCntr *self,
                                    gsize count, gsize *cells, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;
    gsize total;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

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
}

/**
 * alsaseq_event_cntr_get_event_type:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to refer to.
 * @ev_type: (out): The type of event.
 * @error: A #GError.
 *
 * Get the type of event pointed by the index.
 */
void alsaseq_event_cntr_get_event_type(ALSASeqEventCntr *self, gsize index,
                                    ALSASeqEventType *ev_type, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *ev_type = (ALSASeqEventType)ev->type;
}

/**
 * alsaseq_event_cntr_set_event_type:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @ev_type: A #ALSASeqEventType.
 * @error: A #GError.
 *
 * Set the type to event pointed by the index;
 */
void alsaseq_event_cntr_set_event_type(ALSASeqEventCntr *self,
                                         gsize index, ALSASeqEventType ev_type,
                                         GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->type = (snd_seq_event_type_t)ev_type;
}

/**
 * alsaseq_event_cntr_get_tstamp_mode:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @mode: (out): The mode of timestamping, one of #ALSASeqEventTimestampMode.
 * @error: A #GError.
 *
 * Get the mode of timestamping for the event pointed by the index.
 */
void alsaseq_event_cntr_get_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimestampMode *mode,
                                          GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *mode = (ALSASeqEventTimestampMode)(ev->flags & SNDRV_SEQ_TIME_STAMP_MASK);
}

/**
 * alsaseq_event_cntr_set_tstamp_mode:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @mode: The mode of timestamping, one of #ALSASeqEventTimestampMode.
 * @error: A #GError.
 *
 * Set the mode of timestamping for the event pointed by the index.
 */
void alsaseq_event_cntr_set_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimestampMode mode,
                                          GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->flags &= ~SNDRV_SEQ_TIME_STAMP_MASK;
    ev->flags |= (unsigned char)mode;
}

/**
 * alsaseq_event_cntr_get_time_mode:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @mode: (out): The mode of time, one of #ALSASeqEventTimeMode.
 * @error: A #GError.
 *
 * Get the mode of time for the event pointed by the index.
 */
void alsaseq_event_cntr_get_time_mode(ALSASeqEventCntr *self, gsize index,
                                        ALSASeqEventTimeMode *mode,
                                        GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *mode = (ALSASeqEventTimeMode)(ev->flags & SNDRV_SEQ_TIME_MODE_MASK);
}

/**
 * alsaseq_event_cntr_set_time_mode:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @mode: The mode of time, one of #ALSASeqEventTimeMode.
 * @error: A #GError.
 *
 * Set the mode of time for the event pointed by the index.
 */
void alsaseq_event_cntr_set_time_mode(ALSASeqEventCntr *self, gsize index,
                                        ALSASeqEventTimeMode mode,
                                        GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->flags &= ~SNDRV_SEQ_TIME_MODE_MASK;
    ev->flags |= (unsigned char)mode;
}

/**
 * alsaseq_event_cntr_get_length_mode:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @mode: (out): The mode of length, one of #ALSASeqEventLengthMode.
 * @error: A #GError.
 *
 * Get the mode of length for the event pointed by the index.
 */
void alsaseq_event_cntr_get_length_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventLengthMode *mode,
                                          GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *mode = (ALSASeqEventLengthMode)(ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK);
}

/**
 * alsaseq_event_cntr_get_priority_mode:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @mode: (out): The mode of priority, one of #ALSASeqEventPriorityMode.
 * @error: A #GError.
 *
 * Get the mode of priority for the event pointed by the index.
 */
void alsaseq_event_cntr_get_priority_mode(
                                ALSASeqEventCntr *self, gsize index,
                                ALSASeqEventPriorityMode *mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *mode = (ALSASeqEventPriorityMode)(ev->flags & SNDRV_SEQ_PRIORITY_MASK);
}

/**
 * alsaseq_event_cntr_set_priority_mode:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @mode: The mode of priority, one of #ALSASeqEventPriorityMode.
 * @error: A #GError.
 *
 * Set the mode of priority for the event pointed by the index.
 */
void alsaseq_event_cntr_set_priority_mode(
                                ALSASeqEventCntr *self, gsize index,
                                ALSASeqEventPriorityMode mode, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->flags &= ~SNDRV_SEQ_PRIORITY_MASK;
    ev->flags |= (unsigned char)mode;
}

/**
 * alsaseq_event_cntr_get_tag:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @tag: (out): The tag assigned to the event.
 * @error: A #GError.
 *
 * Get the tag assignd to the event pointed by the index.
 */
void alsaseq_event_cntr_get_tag(ALSASeqEventCntr *self, gsize index,
                                  gint8 *tag, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *tag = ev->tag;
}

/**
 * alsaseq_event_cntr_set_tag:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @tag: The tag going to be assignd to the event.
 * @error: A #GError.
 *
 * Get the tag assignd to the event pointed by the index.
 */
void alsaseq_event_cntr_set_tag(ALSASeqEventCntr *self, gsize index,
                                  gint8 tag, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->tag = tag;
}

/**
 * alsaseq_event_cntr_get_queue_id:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @queue_id: (out): The numerical ID of queue to deliver the event. One of
 *                   #ALSASeqSpecificQueueId is available as well.
 * @error: A #GError.
 *
 * Get the numerical ID of queue to deliver the event.
 */
void alsaseq_event_cntr_get_queue_id(ALSASeqEventCntr *self, gsize index,
                                       guint8 *queue_id, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *queue_id = ev->queue;
}

/**
 * alsaseq_event_cntr_set_queue_id:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @queue_id: The numerical ID of queue to deliver the event. One of
 *            #ALSASeqSpecificQueueId is available as well.
 * @error: A #GError.
 *
 * Set the numerical ID of queue to deliver the event.
 */
void alsaseq_event_cntr_set_queue_id(ALSASeqEventCntr *self, gsize index,
                                       guint8 queue_id, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->queue = queue_id;
}

/**
 * alsaseq_event_cntr_get_tstamp:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @tstamp: (out)(transfer none): The timestamp for the event. The content is
 *          affected by the mode of tstamping.
 * @error: A #GError.
 *
 * Get the timestamp of event pointed by index.
 */
void alsaseq_event_cntr_get_tstamp(ALSASeqEventCntr *self, gsize index,
                                const ALSASeqTstamp **tstamp, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *tstamp = (const ALSASeqTstamp *)&ev->time;
}

/**
 * alsaseq_event_cntr_set_tstamp:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @tstamp: The timestamp for the event. The content is affected by the mode of
 *          tstamping.
 * @error: A #GError.
 *
 * Set the timestamp for the event pointed by index.
 */
void alsaseq_event_cntr_set_tstamp(ALSASeqEventCntr *self, gsize index,
                                    const ALSASeqTstamp *tstamp, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->time = *tstamp;
}

/**
 * alsaseq_event_cntr_get_dst:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @dst: (out)(transfer none): The destination of event.
 * @error: A #GError.
 *
 * Get the destination of event pointed by index.
 */
void alsaseq_event_cntr_get_dst(ALSASeqEventCntr *self, gsize index,
                                  const ALSASeqAddr **dst, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *dst = (const ALSASeqAddr *)&ev->dest;
}

/**
 * alsaseq_event_cntr_set_dst:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @dst: The destination of event.
 * @error: A #GError.
 *
 * Set the destination of event pointed by index.
 */
void alsaseq_event_cntr_set_dst(ALSASeqEventCntr *self, gsize index,
                                  const ALSASeqAddr *dst, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->dest = *dst;
}

/**
 * alsaseq_event_cntr_get_src:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @src: (out)(transfer none): The source of event.
 * @error: A #GError.
 *
 * Get the destination of event pointed by index.
 */
void alsaseq_event_cntr_get_src(ALSASeqEventCntr *self, gsize index,
                                  const ALSASeqAddr **src, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    *src = (const ALSASeqAddr *)&ev->source;
}

/**
 * alsaseq_event_cntr_set_src:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @src: The source of event.
 * @error: A #GError.
 *
 * Set the destination of event pointed by index.
 */
void alsaseq_event_cntr_set_src(ALSASeqEventCntr *self, gsize index,
                                  const ALSASeqAddr *src, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, ENOENT);
        return;
    }

    ev->source = *src;
}

static void ensure_fixed_length_event(ALSASeqEventCntrPrivate *priv,
                                      struct snd_seq_event *ev, GError **error)
{
    if (!priv->allocated) {
        generate_error(error, ENOBUFS);
        return;
    }

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
}

static void ensure_variable_length_event(ALSASeqEventCntrPrivate *priv,
                                struct snd_seq_event *ev, const guint8 *data,
                                gsize size, GError **error)
{
    guint8 *pos = (guint8 *)ev;
    ptrdiff_t from_head = pos + sizeof(*ev) - priv->buf;
    ptrdiff_t to_tail;
    guint8 *next_ev;
    guint8 *new;

    if (!priv->allocated) {
        generate_error(error, ENOBUFS);
        return;
    }

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

    new = g_try_malloc(from_head + size + to_tail);
    if (new == NULL) {
        generate_error(error, ENOMEM);
        return;
    }

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
}

/**
 * alsaseq_event_cntr_get_note_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (out)(transfer none): The note data of event.
 * @error: A #GError.
 *
 * Get the note data of event pointed by the index.
 */
void alsaseq_event_cntr_get_note_data(ALSASeqEventCntr *self, gsize index,
                            const ALSASeqEventDataNote **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = (const ALSASeqEventDataNote *)&ev->data.note;
}

/**
 * alsaseq_event_cntr_set_note_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: The note data of event.
 * @error: A #GError.
 *
 * Copy the note data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_note_data(ALSASeqEventCntr *self, gsize index,
                            const ALSASeqEventDataNote *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    ev->data.note = *(struct snd_seq_ev_note *)data;
}

/**
 * alsaseq_event_cntr_get_ctl_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (out)(transfer none): The control data of event.
 * @error: A #GError.
 *
 * Get the control data of event pointed by the index.
 */
void alsaseq_event_cntr_get_ctl_data(ALSASeqEventCntr *self, gsize index,
                            const ALSASeqEventDataCtl **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = (const ALSASeqEventDataCtl *)&ev->data.control;
}

/**
 * alsaseq_event_cntr_set_ctl_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: The control data of event.
 * @error: A #GError.
 *
 * Copy the control data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_ctl_data(ALSASeqEventCntr *self, gsize index,
                            const ALSASeqEventDataCtl *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    ev->data.control = *(struct snd_seq_ev_ctrl *)data;
}

/**
 * alsaseq_event_cntr_get_byte_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (array fixed-size=12)(out)(transfer none): The byte data of event.
 * @error: A #GError.
 *
 * Get the byte data of event pointed by the index.
 */
void alsaseq_event_cntr_get_byte_data(ALSASeqEventCntr *self, gsize index,
                                        const guint8 *data[12], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = ev->data.raw8.d;
}

/**
 * alsaseq_event_cntr_set_byte_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (array fixed-size=12): The byte data of event.
 * @error: A #GError.
 *
 * Copy the byte data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_byte_data(ALSASeqEventCntr *self, gsize index,
                                        const guint8 data[12], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    memcpy(ev->data.raw8.d, data, sizeof(ev->data.raw8.d));
}

/**
 * alsaseq_event_cntr_get_quadlet_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (array fixed-size=3)(out)(transfer none): The quadlet data of event.
 * @error: A #GError.
 *
 * Get the quadlet data of event pointed by the index.
 */
void alsaseq_event_cntr_get_quadlet_data(ALSASeqEventCntr *self, gsize index,
                                        const guint32 *data[3], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = ev->data.raw32.d;
}

/**
 * alsaseq_event_cntr_set_quadlet_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (array fixed-size=3): The quadlet data of event.
 * @error: A #GError.
 *
 * Copy the quadlet data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_quadlet_data(ALSASeqEventCntr *self, gsize index,
                                        const guint32 data[3], GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);

    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    memcpy(ev->data.raw32.d, data, sizeof(ev->data.raw32.d));
}

/**
 * alsaseq_event_cntr_get_blob_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (array length=size)(out)(transfer none): The pointer to blob data.
 * @size: The size of data.
 * @error: A #GError.
 *
 * Refer to the blob data of event.
 */
void alsaseq_event_cntr_get_blob_data(ALSASeqEventCntr *self, gsize index,
                                        const guint8 **data, gsize *size,
                                        GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

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
        generate_error(error, ENODATA);
        break;
    }
}

/**
 * alsaseq_event_cntr_set_blob_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (array length=size): The pointer to blob data for the event.
 * @size: The size of data.
 * @error: A #GError.
 *
 * Copy the quadlet data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_blob_data(ALSASeqEventCntr *self, gsize index,
                                        const guint8 *data, gsize size,
                                        GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_variable_length_event(priv, ev, data, size, error);
}

/**
 * alsaseq_event_cntr_get_queue_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (out)(transfer none): The queue data of event.
 * @error: A #GError.
 *
 * Get the queue data of event pointed by the index.
 */
void alsaseq_event_cntr_get_queue_data(ALSASeqEventCntr *self, gsize index,
                            const ALSASeqEventDataQueue **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = (const ALSASeqEventDataQueue *)&ev->data.queue;
}

/**
 * alsaseq_event_cntr_set_queue_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: The queue data of event.
 * @error: A #GError.
 *
 * Copy the queue data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_queue_data(ALSASeqEventCntr *self, gsize index,
                            const ALSASeqEventDataQueue *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    ev->data.queue = *(struct snd_seq_ev_queue_control *)data;
}

/**
 * alsaseq_event_cntr_get_tstamp_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (out)(transfer none): The timestamp data of event.
 * @error: A #GError.
 *
 * Get the timestamp data of event pointed by the index.
 */
void alsaseq_event_cntr_get_tstamp_data(ALSASeqEventCntr *self, gsize index,
                                    const ALSASeqTstamp **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = (const ALSASeqTstamp *)&ev->data.time;
}

/**
 * alsaseq_event_cntr_set_tstamp_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: The timestamp data of event.
 * @error: A #GError.
 *
 * Copy the timestamp data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_tstamp_data(ALSASeqEventCntr *self, gsize index,
                                    const ALSASeqTstamp *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    ev->data.time = *(union snd_seq_timestamp *)data;
}

/**
 * alsaseq_event_cntr_get_addr_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (out)(transfer none): The address data of event.
 * @error: A #GError.
 *
 * Get the address data of event pointed by the index.
 */
void alsaseq_event_cntr_get_addr_data(ALSASeqEventCntr *self, gsize index,
                                    const ALSASeqAddr **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = (const ALSASeqAddr *)&ev->data.time;
}

/**
 * alsaseq_event_cntr_set_addr_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: The address data of event.
 * @error: A #GError.
 *
 * Copy the address data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_addr_data(ALSASeqEventCntr *self, gsize index,
                                        const ALSASeqAddr *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    ev->data.addr = *(struct snd_seq_addr *)data;
}

/**
 * alsaseq_event_cntr_get_connect_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: (out)(transfer none): The connect data of event.
 * @error: A #GError.
 *
 * Get the connect data of event pointed by the index.
 */
void alsaseq_event_cntr_get_connect_data(ALSASeqEventCntr *self, gsize index,
                        const ALSASeqEventDataConnect **data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    *data = (const ALSASeqEventDataConnect *)&ev->data.connect;
}

/**
 * alsaseq_event_cntr_set_connect_data:
 * @self: A #ALSASeqEventCntr.
 * @index: The index of event to set.
 * @data: The connect data of event.
 * @error: A #GError.
 *
 * Copy the connect data to the event pointed by the index.
 */
void alsaseq_event_cntr_set_connect_data(ALSASeqEventCntr *self, gsize index,
                        const ALSASeqEventDataConnect *data, GError **error)
{
    ALSASeqEventCntrPrivate *priv;
    struct event_iterator iter;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_CNTR(self));
    priv = alsaseq_event_cntr_get_instance_private(self);

    event_iterator_init(&iter, priv->buf, priv->length, priv->allocated);
    ev = event_iterator_find(&iter, index);
    if (ev == NULL) {
        generate_error(error, EINVAL);
        return;
    }

    ensure_fixed_length_event(priv, ev, error);
    if (*error != NULL)
        return;

    ev->data.connect = *(struct snd_seq_connect *)data;
}
