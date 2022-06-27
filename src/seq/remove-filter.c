// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <errno.h>

/**
 * ALSASeqRemoveFilter:
 * A boxed object to express filter to remove scheduled event in queue.
 *
 * A [struct@RemoveFilter] is a boxed object to express filter to remove scheduled event in
 * queue. The call of [method@UserClient.remove_events] requires the instance of object. In the
 * object, data shares the same storage, thus it's not possible to use several purposes.
 *
 * The object wraps `struct snd_seq_remove_events` in UAPI of Linux sound subsystem.
 */
ALSASeqRemoveFilter *seq_remove_filter_copy(const ALSASeqRemoveFilter *self)
{
#ifdef g_memdup2
    return g_memdup2(self, sizeof(*self));
#else
    // GLib v2.68 deprecated g_memdup() with concern about overflow by narrow conversion from size_t to
    // unsigned int however it's safe in the local case.
    gpointer ptr = g_malloc(sizeof(*self));
    memcpy(ptr, self, sizeof(*self));
    return ptr;
#endif
}

G_DEFINE_BOXED_TYPE(ALSASeqRemoveFilter, alsaseq_remove_filter, seq_remove_filter_copy, g_free)

/**
 * alsaseq_remove_filter_new_with_dest_addr:
 * @inout: The direction of queue; [flags@RemoveFilterFlag].INPUT or
 *         [flags@RemoveFilterFlag].OUTPUT.
 * @queue_id: The numeric ID of queue, excluding [enum@SpecificQueueId].DIRECT.
 * @dest: The address of destination.
 *
 * Allocate and return a memory object of [struct@RemoveFilter] to remove queued events towards the
 * destination.
 *
 * Returns: A [struct@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_dest_addr(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqAddr *dest)
{
    struct snd_seq_remove_events filter;

    g_return_val_if_fail(!(inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)), NULL);

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_DEST;
    filter.queue = queue_id;
    filter.dest = *dest;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_note_channel:
 * @inout: The direction of queue; [flags@RemoveFilterFlag].INPUT or
 *         [flags@RemoveFilterFlag].OUTPUT.
 * @queue_id: The numeric ID of queue, excluding [enum@SpecificQueueId].DIRECT.
 * @channel: The channel for note event.
 *
 * Allocate and return a memory object of [struct@RemoveFilter] to remove queued events with note
 * type towards the channel.
 *
 * Returns: A [struct@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note_channel(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                guint8 channel)
{
    struct snd_seq_remove_events filter;

    g_return_val_if_fail(!(inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)), NULL);

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_DEST_CHANNEL;
    filter.queue = queue_id;
    filter.channel = channel;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_event_type:
 * @inout: The direction of queue; [flags@RemoveFilterFlag].INPUT or
 *         [flags@RemoveFilterFlag].OUTPUT.
 * @queue_id: The numeric ID of queue, excluding [enum@SpecificQueueId].DIRECT.
 * @ev_type: The type of event.
 *
 * Allocate and return a memory object of [struct@RemoveFilter] to remove queued events with the
 * type.
 *
 * Returns: A [struct@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_event_type(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                ALSASeqEventType ev_type)
{
    struct snd_seq_remove_events filter;

    g_return_val_if_fail(!(inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)), NULL);

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_EVENT_TYPE;
    filter.queue = queue_id;
    filter.type = ev_type;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_note:
 * @inout: The direction of queue; [flags@RemoveFilterFlag].INPUT or [flags@RemoveFilterFlag].OUTPUT.
 * @queue_id: The numeric ID of queue, excluding [enum@SpecificQueueId].DIRECT.
 *
 * Allocate and return a memory object of [struct@RemoveFilter] to remove queued events for note,
 * excluding [enum@EventType:NOTEOFF].
 *
 * Returns: A [struct@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_note(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id)
{
    struct snd_seq_remove_events filter;

    g_return_val_if_fail(!(inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)), NULL);

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_IGNORE_OFF;
    filter.queue = queue_id;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_tag:
 * @inout: The direction of queue; [flags@RemoveFilterFlag].INPUT or
 *         [flags@RemoveFilterFlag].OUTPUT.
 * @queue_id: The numeric ID of queue, excluding [enum@SpecificQueueId].DIRECT.
 * @tag: The tag of event to remove.
 *
 * Allocate and return a memory object of [struct@RemoveFilter] to remove queued events with the
 * tag.
 *
 * Returns: A [struct@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tag(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint8 tag)
{
    struct snd_seq_remove_events filter;

    g_return_val_if_fail(!(inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)), NULL);

    filter.remove_mode = inout | SNDRV_SEQ_REMOVE_TAG_MATCH;
    filter.queue = queue_id;
    filter.tag = tag;

    return g_boxed_copy(ALSASEQ_TYPE_REMOVE_FILTER, &filter);
}

/**
 * alsaseq_remove_filter_new_with_tick_time:
 * @inout: The direction of queue; [flags@RemoveFilterFlag].INPUT or
 *         [flags@RemoveFilterFlag].OUTPUT.
 * @queue_id: The numeric ID of queue, excluding [enum@SpecificQueueId].DIRECT.
 * @tick_time: The count of tick.
 * @after: Remove events after the tick time if true, else remove events before the tick time.
 *
 * Allocate and return a memory object of [struct@RemoveFilter] to remove queued events
 * before/after the tick time.
 *
 * Returns: A [struct@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_tick_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tick_time, gboolean after)
{
    struct snd_seq_remove_events filter;

    g_return_val_if_fail(!(inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)), NULL);

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
 * @inout: The direction of queue; [flags@RemoveFilterFlag].INPUT or
 *         [flags@RemoveFilterFlag].OUTPUT.
 * @queue_id: The numeric ID of queue, excluding [enum@SpecificQueueId].DIRECT.
 * @tv_sec: The second part of time.
 * @tv_nsec: The nanosecond part of time.
 * @after: Remove events after the real time if true, else remove events before the real time.
 *
 * Allocate and return a memory object of [struct@RemoveFilter] to remove queued events
 * before/after the real time.
 *
 * Returns: A [struct@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new_with_real_time(
                                ALSASeqRemoveFilterFlag inout, guint8 queue_id,
                                gint32 tv_sec, guint32 tv_nsec, gboolean after)
{
    struct snd_seq_remove_events filter;

    g_return_val_if_fail(!(inout & ~(SNDRV_SEQ_REMOVE_INPUT | SNDRV_SEQ_REMOVE_OUTPUT)), NULL);

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
