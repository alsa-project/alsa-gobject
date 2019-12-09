// SPDX-License-Identifier: LGPL-3.0-or-later
#include "remove-filter.h"
#include "privates.h"

#include <errno.h>

ALSASeqRemoveFilter *seq_remove_filter_copy(const ALSASeqRemoveFilter *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqRemoveFilter, alsaseq_remove_filter, seq_remove_filter_copy, g_free)

/**
 * alsaseq_remove_filter_new_with_dest_addr:
 * @inout: The direction of queue; ALSASEQ_REMOVE_FILTER_FLAG_INPUT or
 *         ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT.
 * @queue_id: The numerical ID of queue.
 * @dest: The address of destination.
 * @error: A #GError.
 *
 * Allocate and return a memory object of ALSASeqRemoveFilter to remove queued
 * events towards the destination.
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_dest_addr(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqAddr *dest, GError **error)
{
    struct snd_seq_remove_events filter;

    if (inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)) {
        generate_error(error, EINVAL);
        return NULL;
    }

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_DEST;
    filter.queue = queue_id;
    filter.dest = *dest;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_note_channel:
 * @inout: The direction of queue; ALSASEQ_REMOVE_FILTER_FLAG_INPUT or
 *         ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT.
 * @queue_id: The numerical ID of queue.
 * @channel: The channel for note event.
 * @error: A #GError.
 *
 * Allocate and return a memory object of ALSASeqRemoveFilter to remove queued
 * events with note type towards the channel.
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note_channel(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                guint8 channel, GError **error)
{
    struct snd_seq_remove_events filter;

    if (inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)) {
        generate_error(error, EINVAL);
        return NULL;
    }

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_DEST_CHANNEL;
    filter.queue = queue_id;
    filter.channel = channel;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_event_type:
 * @inout: The direction of queue; ALSASEQ_REMOVE_FILTER_FLAG_INPUT or
 *         ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT.
 * @queue_id: The numerical ID of queue.
 * @ev_type: The type of event.
 * @error: A #GError.
 *
 * Allocate and return a memory object of ALSASeqRemoveFilter to remove queued
 * events with the type.
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_event_type(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqEventType ev_type, GError **error)
{
    struct snd_seq_remove_events filter;

    if (inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)) {
        generate_error(error, EINVAL);
        return NULL;
    }

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_EVENT_TYPE;
    filter.queue = queue_id;
    filter.type = ev_type;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_note:
 * @inout: The direction of queue; ALSASEQ_REMOVE_FILTER_FLAG_INPUT or
 *         ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT.
 * @queue_id: The numerical ID of queue.
 * @error: A #GError.
 *
 * Allocate and return a memory object of ALSASeqRemoveFilter to remove queued
 * events for note, except for ALSASEQ_EVENT_TYPE_NOTEOFF.
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                GError **error)
{
    struct snd_seq_remove_events filter;

    if (inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)) {
        generate_error(error, EINVAL);
        return NULL;
    }

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_IGNORE_OFF;
    filter.queue = queue_id;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_tag:
 * @inout: The direction of queue; ALSASEQ_REMOVE_FILTER_FLAG_INPUT or
 *         ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT.
 * @queue_id: The numerical ID of queue.
 * @tag: The tag of event to remove.
 * @error: A #GError.
 *
 * Allocate and return a memory object of ALSASeqRemoveFilter to remove queued
 * events with the tag.
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tag(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint8 tag, GError **error)
{
    struct snd_seq_remove_events filter;

    if (inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)) {
        generate_error(error, EINVAL);
        return NULL;
    }

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_TAG_MATCH;
    filter.queue = queue_id;
    filter.tag = tag;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_tick_time:
 * @inout: The direction of queue; ALSASEQ_REMOVE_FILTER_FLAG_INPUT or
 *         ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT.
 * @queue_id: The numerical ID of queue.
 * @tick_time: The count of tick.
 * @after: Remove events after the tick time if true, else remove events before
 *         the tick time.
 * @error: A #GError.
 *
 * Allocate and return a memory object of ALSASeqRemoveFilter to remove queued
 * events before/after the tick time.
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tick_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tick_time, gboolean after,
                                GError **error)
{
    struct snd_seq_remove_events filter;

    if (inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)) {
        generate_error(error, EINVAL);
        return NULL;
    }

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_TIME_TICK;
    if (after)
        filter.remove_mode |= SNDRV_SEQ_REMOVE_TIME_BEFORE;
    else
        filter.remove_mode |= SNDRV_SEQ_REMOVE_TIME_AFTER;
    filter.queue = queue_id;
    filter.time.tick = tick_time;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_real_time:
 * @inout: The direction of queue; ALSASEQ_REMOVE_FILTER_FLAG_INPUT or
 *         ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT.
 * @queue_id: The numerical ID of queue.
 * @tv_sec: The second part of time.
 * @tv_nsec: The nanosecond part of time.
 * @after: Remove events after the real time if true, else remove events before
 *         the real time.
 * @error: A #GError.
 *
 * Allocate and return a memory object of ALSASeqRemoveFilter to remove queued
 * events before/after the real time.
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_real_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tv_sec, guint32 tv_nsec, gboolean after,
                                GError **error)
{
    struct snd_seq_remove_events filter;

    if (inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)) {
        generate_error(error, EINVAL);
        return NULL;
    }

    filter.remove_mode = inout;
    if (after)
        filter.remove_mode |= SNDRV_SEQ_REMOVE_TIME_AFTER;
    else
        filter.remove_mode |= SNDRV_SEQ_REMOVE_TIME_BEFORE;
    filter.queue = queue_id;
    filter.time.time.tv_sec = tv_sec;
    filter.time.time.tv_nsec = tv_nsec;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}
