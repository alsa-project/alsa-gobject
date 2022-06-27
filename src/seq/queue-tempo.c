// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqQueueTempo:
 * A GObject-derived object to express tempo of queue.
 *
 * A [class@QueueTempo] is a GObject-derived object to express tempo of queue.
 *
 * The object wraps `struct snd_seq_queue_tempo` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_seq_queue_tempo tempo;
} ALSASeqQueueTempoPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqQueueTempo, alsaseq_queue_tempo, G_TYPE_OBJECT)

enum seq_queue_tempo_prop_type {
    SEQ_QUEUE_TEMPO_PROP_QUEUE_ID = 1,
    SEQ_QUEUE_TEMPO_PROP_TEMPO,
    SEQ_QUEUE_TEMPO_PROP_RESOLUTION,
    SEQ_QUEUE_TEMPO_PROP_COUNT,
};
static GParamSpec *seq_queue_tempo_props[SEQ_QUEUE_TEMPO_PROP_COUNT] = { NULL, };

static void seq_queue_tempo_set_property(GObject *obj, guint id,
                                         const GValue *val, GParamSpec *spec)
{
    ALSASeqQueueTempo *self = ALSASEQ_QUEUE_TEMPO(obj);
    ALSASeqQueueTempoPrivate *priv =
                                alsaseq_queue_tempo_get_instance_private(self);

    switch (id) {
    case SEQ_QUEUE_TEMPO_PROP_QUEUE_ID:
        priv->tempo.queue = (int)g_value_get_uchar(val);
        break;
    case SEQ_QUEUE_TEMPO_PROP_TEMPO:
        priv->tempo.tempo = g_value_get_int(val);
        break;
    case SEQ_QUEUE_TEMPO_PROP_RESOLUTION:
        priv->tempo.ppq = g_value_get_int(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_queue_tempo_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqQueueTempo *self = ALSASEQ_QUEUE_TEMPO(obj);
    ALSASeqQueueTempoPrivate *priv =
                                alsaseq_queue_tempo_get_instance_private(self);

    switch (id) {
    case SEQ_QUEUE_TEMPO_PROP_QUEUE_ID:
        g_value_set_uchar(val, (guint8)priv->tempo.queue);
        break;
    case SEQ_QUEUE_TEMPO_PROP_TEMPO:
        g_value_set_uint(val, priv->tempo.tempo);
        break;
    case SEQ_QUEUE_TEMPO_PROP_RESOLUTION:
        g_value_set_int(val, priv->tempo.ppq);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_queue_tempo_class_init(ALSASeqQueueTempoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_queue_tempo_set_property;
    gobject_class->get_property = seq_queue_tempo_get_property;

    /**
     * ALSASeqQueueTempo:queue-id:
     *
     * The numeric ID of queue. An entry of ALSASeqSpecificClientId is available as well,
     */
    seq_queue_tempo_props[SEQ_QUEUE_TEMPO_PROP_QUEUE_ID] =
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numeric ID of queue. An entry of ALSASeqSpecificClientId is "
                           "available as well",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqQueueTempo:tempo:
     *
     * The number of micro second per tick as tempo.
     */
    seq_queue_tempo_props[SEQ_QUEUE_TEMPO_PROP_TEMPO] =
        g_param_spec_uint("tempo", "tempo",
                         "The number of micro second per tick as tempo.",
                         0, G_MAXUINT,
                         0,
                         G_PARAM_READWRITE);

    /**
     * ALSASeqQueueTempo:resolution:
     *
     * The number of pulse per quarter as resolution.
     */
    seq_queue_tempo_props[SEQ_QUEUE_TEMPO_PROP_RESOLUTION] =
        g_param_spec_int("resolution", "resolution",
                         "The number of pulse per quarter as resolution.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_QUEUE_TEMPO_PROP_COUNT,
                                      seq_queue_tempo_props);
}

static void alsaseq_queue_tempo_init(ALSASeqQueueTempo *self)
{
    return;
}

/**
 * alsaseq_queue_tempo_new:
 *
 * Allocate and return an instance of [class@QueueTempo].
 *
 * Returns: An instance of [class@QueueTempo].
 */
ALSASeqQueueTempo *alsaseq_queue_tempo_new()
{
    return g_object_new(ALSASEQ_TYPE_QUEUE_TEMPO, NULL);
}

/**
 * alsaseq_queue_tempo_get_skew:
 * @self: A [class@QueueTempo].
 * @skew: (array fixed-size=2)(out)(transfer none): The array with two elements for numerator and
 *        denominator of fraction for skew.
 *
 * Refer to numerator and denominator of fraction for skew.
 */
void alsaseq_queue_tempo_get_skew(ALSASeqQueueTempo *self, const guint32 *skew[2])
{
    ALSASeqQueueTempoPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_QUEUE_TEMPO(self));
    priv = alsaseq_queue_tempo_get_instance_private(self);

    g_return_if_fail(skew != NULL);

    // MEMO: I wish 32-bit storage size is aligned to 32 bit offset in all of
    // supported ABIs.
    *skew = (guint32 *)&priv->tempo.skew_value;
}


/**
 * alsaseq_queue_tempo_set_skew:
 * @self: A [class@QueueTempo].
 * @skew: (array fixed-size=2)(transfer none): The array with two elements for numerator and
 *        denominator of fraction for skew.
 *
 * Copy numerator and denominator of fraction for skew.
 */
void alsaseq_queue_tempo_set_skew(ALSASeqQueueTempo *self, const guint32 skew[2])
{
    ALSASeqQueueTempoPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_QUEUE_TEMPO(self));
    priv = alsaseq_queue_tempo_get_instance_private(self);

    g_return_if_fail(skew != NULL);

    priv->tempo.skew_value = skew[0];
    priv->tempo.skew_base = skew[1];
}

void seq_queue_tempo_refer_private(ALSASeqQueueTempo *self,
                                   struct snd_seq_queue_tempo **tempo)
{
    ALSASeqQueueTempoPrivate *priv =
                                alsaseq_queue_tempo_get_instance_private(self);

    *tempo = &priv->tempo;
}
