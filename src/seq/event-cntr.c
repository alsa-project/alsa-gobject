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
