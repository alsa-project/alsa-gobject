// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqEventCntr:
 * A boxed structure for container which includes batch of events.
 *
 * For batch of events, [struct@EventCntr] keeps flatten buffer which serialize the events without
 * pointing to extra data blob for variable type.
 */

static ALSASeqEventCntr *seq_event_cntr_copy(const ALSASeqEventCntr *src)
{
    ALSASeqEventCntr *dst;

    dst = g_malloc0(sizeof(*dst));
    memcpy(dst, src, sizeof(*dst));

    dst->buf = g_malloc0(src->length);
    memcpy(dst->buf, src->buf, src->length);

    return dst;
}

static void seq_event_cntr_free(ALSASeqEventCntr *self)
{
    g_free(self->buf);
    g_free(self);
}

G_DEFINE_BOXED_TYPE(ALSASeqEventCntr, alsaseq_event_cntr, seq_event_cntr_copy, seq_event_cntr_free);

struct seq_event_iter {
    guint8 *buf;
    gsize length;
    gsize offset;
    gboolean aligned;
};

static void seq_event_iter_init(struct seq_event_iter *iter, guint8 *buf, gsize length,
                                gboolean aligned)
{
    iter->buf = buf;
    iter->length = length;
    iter->offset = 0;
    iter->aligned = aligned;
}

static struct snd_seq_event *seq_event_iter_next(struct seq_event_iter *iter)
{
    gsize length;

    if (iter->buf == NULL)
        return NULL;

    if (iter->offset < iter->length) {
        struct snd_seq_event *ev = (struct snd_seq_event *)(iter->buf + iter->offset);
        length = seq_event_calculate_flattened_length(ev, iter->aligned);

        if (iter->offset + length <= iter->length) {
            iter->offset += length;
            return ev;
        }
    }

    return NULL;
}

/**
 * alsaseq_event_cntr_deserialize:
 * @self: A [struct@EventCntr].
 * @events: (element-type ALSASeq.Event) (out) (transfer full): The list of deserialized events.
 *
 * Retrieve [struct@GLib.List] including batch of deserialized [struct@Event].
 */
void alsaseq_event_cntr_deserialize(const ALSASeqEventCntr *self, GList **events)
{
    struct seq_event_iter iter;
    struct snd_seq_event *ev;

    seq_event_iter_init(&iter, self->buf, self->length, self->aligned);
    while ((ev = seq_event_iter_next(&iter))) {
        struct snd_seq_event *event;

        // MEMO: For [enum@EventLengthMode].VARIABLE type of event, a memory object is allocated
        // for blob data, since the size of boxed structure should have fixed size.
        event = g_boxed_copy(ALSASEQ_TYPE_EVENT, ev);
        *events = g_list_append(*events, event);
    }
}
