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
