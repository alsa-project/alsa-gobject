// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <errno.h>

/**
 * ALSASeqRemoveFilter:
 * An object to express filter to remove scheduled event in queue.
 *
 * A [class@RemoveFilter] is a GObject-derived object to express filter to remove scheduled event in
 * queue. The call of [method@UserClient.remove_events] requires the instance of object.
 *
 * The object wraps `struct snd_seq_remove_events` in UAPI of Linux sound subsystem.
 */

typedef struct {
    struct snd_seq_remove_events data;
} ALSASeqRemoveFilterPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqRemoveFilter, alsaseq_remove_filter, G_TYPE_OBJECT)

enum seq_remove_filter_prop_type {
    SEQ_REMOVE_FILTER_PROP_FLAGS = 1,
    SEQ_REMOVE_FILTER_PROP_QUEUE_ID,
    SEQ_REMOVE_FILTER_PROP_DST,
    SEQ_REMOVE_FILTER_PROP_CHANNEL,
    SEQ_REMOVE_FILTER_PROP_EVENT_TYPE,
    SEQ_REMOVE_FILTER_PROP_TAG,
    SEQ_REMOVE_FILTER_PROP_COUNT,
};
static GParamSpec *seq_remove_filter_props[SEQ_REMOVE_FILTER_PROP_COUNT] = { NULL };

static void seq_remove_filter_get_property(GObject *obj, guint id, GValue *val, GParamSpec *spec)
{
    ALSASeqRemoveFilter *self = ALSASEQ_REMOVE_FILTER(obj);
    ALSASeqRemoveFilterPrivate *priv = alsaseq_remove_filter_get_instance_private(self);
    const struct snd_seq_remove_events *data = &priv->data;

    switch (id) {
    case SEQ_REMOVE_FILTER_PROP_FLAGS:
        g_value_set_flags(val, (ALSASeqRemoveFilterFlag)data->remove_mode);
        break;
    case SEQ_REMOVE_FILTER_PROP_QUEUE_ID:
        g_value_set_uchar(val, data->queue);
        break;
    case SEQ_REMOVE_FILTER_PROP_DST:
        g_value_set_static_boxed(val, &data->dest);
        break;
    case SEQ_REMOVE_FILTER_PROP_CHANNEL:
        g_value_set_uchar(val, data->channel);
        break;
    case SEQ_REMOVE_FILTER_PROP_EVENT_TYPE:
        g_value_set_enum(val, data->type);
        break;
    case SEQ_REMOVE_FILTER_PROP_TAG:
        g_value_set_schar(val, data->tag);
        break;
    default:
        break;
    }
}

static void seq_remove_filter_set_property(GObject *obj, guint id, const GValue *val,
                                           GParamSpec *spec)
{
    ALSASeqRemoveFilter *self = ALSASEQ_REMOVE_FILTER(obj);
    ALSASeqRemoveFilterPrivate *priv = alsaseq_remove_filter_get_instance_private(self);
    struct snd_seq_remove_events *data = &priv->data;

    switch (id) {
    case SEQ_REMOVE_FILTER_PROP_FLAGS:
        data->remove_mode = g_value_get_flags(val);
        break;
    case SEQ_REMOVE_FILTER_PROP_QUEUE_ID:
        data->queue = g_value_get_uchar(val);
        break;
    case SEQ_REMOVE_FILTER_PROP_DST:
    {
        const ALSASeqAddr *src = g_value_get_boxed(val);
        data->dest = *src;
        break;
    }
    case SEQ_REMOVE_FILTER_PROP_CHANNEL:
        data->channel = g_value_get_uchar(val);
        break;
    case SEQ_REMOVE_FILTER_PROP_EVENT_TYPE:
        data->type = g_value_get_enum(val);
        break;
    case SEQ_REMOVE_FILTER_PROP_TAG:
        data->tag = g_value_get_schar(val);
        break;
    default:
        break;
    }
}

static void alsaseq_remove_filter_class_init(ALSASeqRemoveFilterClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_remove_filter_set_property;
    gobject_class->get_property = seq_remove_filter_get_property;

    /**
     * ALSASeqRemoveFilter:flags:
     *
     * The set of flags to filter events. They decide how to evaluate included data at call of
     * [method@UserClient.remove_events].
     *
     * Since: 0.3
     */
    seq_remove_filter_props[SEQ_REMOVE_FILTER_PROP_FLAGS] =
        g_param_spec_flags("flags", "flags",
                           "The set of flags to filter events",
                           ALSASEQ_TYPE_REMOVE_FILTER_FLAG,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqRemoveFilter:queue-id:
     *
     * The numeric identifier of queue as filter condition. This is always evaluated at call of
     * [method@UserClient.remove_events].
     *
     * Since: 0.3
     */
    seq_remove_filter_props[SEQ_REMOVE_FILTER_PROP_QUEUE_ID] =
        g_param_spec_uchar("queue-id", "queue-id",
                           "The numeric identifier of queue as filter condition",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqRemoveFilter:destination:
     *
     * The destionation of event as filter condition. This is evaluated with
     * [flags@RemoveFilterFlag].DEST at call of [method@UserClient.remove_events].
     *
     * Since: 0.3
     */
    seq_remove_filter_props[SEQ_REMOVE_FILTER_PROP_DST] =
        g_param_spec_boxed("destination", "destination",
                           "The destionation of event as filter condition",
                           ALSASEQ_TYPE_ADDR,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqRemoveFilter:channel:
     *
     * The channel of event as filter condition. This is evaluated with
     * [flags@RemoveFilterFlag].DEST_CHANNEL at call of [method@UserClient.remove_events].
     *
     * Since: 0.3
     */
    seq_remove_filter_props[SEQ_REMOVE_FILTER_PROP_CHANNEL] =
        g_param_spec_uchar("channel", "channel",
                           "The channel of event as filter condition",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqRemoveFilter:event-type:
     *
     * The type of event as filter condition. This is evaluated with
     * [flags@RemoveFilterFlag].EVENT_TYPE at call of [method@UserClient.remove_events].
     *
     * Since: 0.3
     */
    seq_remove_filter_props[SEQ_REMOVE_FILTER_PROP_EVENT_TYPE] =
        g_param_spec_enum("event-type", "event-type",
                          "The type of event as filter condition",
                          ALSASEQ_TYPE_EVENT_TYPE,
                          ALSASEQ_EVENT_TYPE_NONE,
                          G_PARAM_READWRITE);

    /**
     * ALSASeqRemoveFilter:tag:
     *
     * The tag of event as filter condition. This is evaluated with
     * [flags@RemoveFilterFlag].TAG_MATCH at call of [method@UserClient.remove_events].
     *
     * Since: 0.3
     */
    seq_remove_filter_props[SEQ_REMOVE_FILTER_PROP_TAG] =
        g_param_spec_char("tag", "tag",
                          "The tag of event as filter condition",
                          G_MININT8, G_MAXINT8,
                          0,
                          G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_REMOVE_FILTER_PROP_COUNT,
                                      seq_remove_filter_props);
}

static void alsaseq_remove_filter_init(ALSASeqRemoveFilter *self)
{
}

/**
 * alsaseq_remove_filter_new:
 *
 * Allocate and return an instance of [class@RemoveFilter].
 *
 * Returns: An instance of [class@RemoveFilter].
 */
ALSASeqRemoveFilter *alsaseq_remove_filter_new(void)
{
    return g_object_new(ALSASEQ_TYPE_REMOVE_FILTER, NULL);
}

/**
 * alsaseq_remove_filter_get_tick_time:
 * @self: A [class@RemoveFilter].
 * @tick_time: (out): The count of tick.
 *
 * Refer to tick count in internal storage. The call works expectedly as long as
 * [property@RemoveFilter:flags] contains [flags@RemoveFilterFlag].TICK. This is evaluated with
 * [flags@RemoveFilterFlag].TIME_BEFORE and [flags@RemoveFilterFlag].TIME_AFTER at call of
 * [method@UserClient.remove_events].
 */
void alsaseq_remove_filter_get_tick_time(ALSASeqRemoveFilter *self, guint *tick_time)
{
    ALSASeqRemoveFilterPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_REMOVE_FILTER(self));
    g_return_if_fail(tick_time != NULL);

    priv = alsaseq_remove_filter_get_instance_private(self);
    g_return_if_fail((priv->data.remove_mode & SNDRV_SEQ_REMOVE_TIME_TICK) > 0);

    *tick_time = priv->data.time.tick;
}

/**
 * alsaseq_remove_filter_set_tick_time:
 * @self: A [class@RemoveFilter].
 * @tick_time: The count of tick.
 *
 * Copy tick count into internal storage. The call works expectedly as long as
 * [property@RemoveFilter:flags] contains [flags@RemoveFilterFlag].TICK. This is evaluated with
 * [flags@RemoveFilterFlag].TIME_BEFORE and [flags@RemoveFilterFlag].TIME_AFTER at call of
 * [method@UserClient.remove_events].
 *
 */
void alsaseq_remove_filter_set_tick_time(ALSASeqRemoveFilter *self, guint tick_time)
{
    ALSASeqRemoveFilterPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_REMOVE_FILTER(self));

    priv = alsaseq_remove_filter_get_instance_private(self);
    g_return_if_fail((priv->data.remove_mode & SNDRV_SEQ_REMOVE_TIME_TICK) > 0);

    priv->data.time.tick = tick_time;
}

/**
 * alsaseq_remove_filter_get_real_time:
 * @self: A [class@RemoveFilter].
 * @real_time: (array fixed-size=2) (out) (transfer none): The real time data of event.
 *
 * Refer to doublet of real time in internal storage. The call works expectedly as long as
 * [property@RemoveFilter:flags] doesn't contain [flags@RemoveFilterFlag].TICK. This is evaluated
 * with [flags@RemoveFilterFlag].TIME_BEFORE and [flags@RemoveFilterFlag].TIME_AFTER at call of
 * [method@UserClient.remove_events].
 */
void alsaseq_remove_filter_get_real_time(ALSASeqRemoveFilter *self, const guint32 *real_time[2])
{
    ALSASeqRemoveFilterPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_REMOVE_FILTER(self));
    g_return_if_fail(real_time != NULL);

    priv = alsaseq_remove_filter_get_instance_private(self);
    g_return_if_fail((priv->data.remove_mode & SNDRV_SEQ_REMOVE_TIME_TICK) == 0);

    *real_time = (const guint32 *)&priv->data.time.time;
}

/**
 * alsaseq_remove_filter_set_real_time:
 * @self: A [class@RemoveFilter].
 * @real_time: (array fixed-size=2): The real time data of event.
 *
 * Copy doublet of real time into internal storage. The call works expectedly as long as
 * [property@RemoveFilter:flags] doesn't contain [flags@RemoveFilterFlag].TICK. This is evaluated
 * with [flags@RemoveFilterFlag].TIME_BEFORE and [flags@RemoveFilterFlag].TIME_AFTER at call of
 * [method@UserClient.remove_events].
 */
void alsaseq_remove_filter_set_real_time(ALSASeqRemoveFilter *self, const guint32 real_time[2])
{
    ALSASeqRemoveFilterPrivate *priv;

    g_return_if_fail(ALSASEQ_IS_REMOVE_FILTER(self));

    priv = alsaseq_remove_filter_get_instance_private(self);
    g_return_if_fail((priv->data.remove_mode & SNDRV_SEQ_REMOVE_TIME_TICK) == 0);

    priv->data.time.time.tv_sec = real_time[0];
    priv->data.time.time.tv_nsec = real_time[1];
}

void seq_remove_filter_refer_private(ALSASeqRemoveFilter *self,
                                     struct snd_seq_remove_events **data)
{
    ALSASeqRemoveFilterPrivate *priv = alsaseq_remove_filter_get_instance_private(self);

    *data = &priv->data;
}
