// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqClientPool:
 * A GObject-derived object to represent information of pool owned by client.
 *
 * A [class@ClientPool] is a GObject-derived object to represent information of pool owned by
 * client. The pool consists of a batch of cells to store message contents in kernel space. The
 * call of [func@get_client_pool] returns the instance of object. The call of
 * [method@UserClient.set_pool]) and [method@UserClient.get_pool] require the instance of object.
 *
 * The object wraps `struct snd_seq_client_pool` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_seq_client_pool pool;
} ALSASeqClientPoolPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqClientPool, alsaseq_client_pool, G_TYPE_OBJECT)

enum seq_client_pool_prop_type {
    SEQ_CLIENT_POOL_PROP_CLIENT_ID = 1,
    SEQ_CLIENT_POOL_PROP_OUTPUT_POOL,
    SEQ_CLIENT_POOL_PROP_INPUT_POOL,
    SEQ_CLIENT_POOL_PROP_OUTPUT_ROOM,
    SEQ_CLIENT_POOL_PROP_OUTPUT_FREE,
    SEQ_CLIENT_POOL_PROP_INPUT_FREE,
    SEQ_CLIENT_POOL_PROP_COUNT,
};
static GParamSpec *seq_client_pool_props[SEQ_CLIENT_POOL_PROP_COUNT] = { NULL, };

static void seq_client_pool_set_property(GObject *obj, guint id,
                                         const GValue *val, GParamSpec *spec)
{
    ALSASeqClientPool *self = ALSASEQ_CLIENT_POOL(obj);
    ALSASeqClientPoolPrivate *priv =
                                alsaseq_client_pool_get_instance_private(self);

    switch (id) {
    case SEQ_CLIENT_POOL_PROP_OUTPUT_POOL:
        priv->pool.output_pool = g_value_get_uint(val);
        break;
    case SEQ_CLIENT_POOL_PROP_INPUT_POOL:
        priv->pool.input_pool = g_value_get_uint(val);
        break;
    case SEQ_CLIENT_POOL_PROP_OUTPUT_ROOM:
        priv->pool.output_room = g_value_get_uint(val);
        break;
    case SEQ_CLIENT_POOL_PROP_OUTPUT_FREE:
        priv->pool.output_free = g_value_get_uint(val);
        break;
    case SEQ_CLIENT_POOL_PROP_INPUT_FREE:
        priv->pool.input_free = g_value_get_uint(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_client_pool_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqClientPool *self = ALSASEQ_CLIENT_POOL(obj);
    ALSASeqClientPoolPrivate *priv =
                                alsaseq_client_pool_get_instance_private(self);

    switch (id) {
    case SEQ_CLIENT_POOL_PROP_CLIENT_ID:
        g_value_set_uchar(val, (guint8)priv->pool.client);
        break;
    case SEQ_CLIENT_POOL_PROP_OUTPUT_POOL:
        g_value_set_uint(val, priv->pool.output_pool);
        break;
    case SEQ_CLIENT_POOL_PROP_INPUT_POOL:
        g_value_set_uint(val, priv->pool.input_pool);
        break;
    case SEQ_CLIENT_POOL_PROP_OUTPUT_ROOM:
        g_value_set_uint(val, priv->pool.output_room);
        break;
    case SEQ_CLIENT_POOL_PROP_OUTPUT_FREE:
        g_value_set_uint(val, priv->pool.output_free);
        break;
    case SEQ_CLIENT_POOL_PROP_INPUT_FREE:
        g_value_set_uint(val, priv->pool.input_free);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_client_pool_class_init(ALSASeqClientPoolClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_client_pool_set_property;
    gobject_class->get_property = seq_client_pool_get_property;

    /**
     * ALSASeqClientPool:client-id:
     *
     * The numeric ID of client. One of [enum@SpecificClientId] is available as well as any
     * numeric value.
     */
    seq_client_pool_props[SEQ_CLIENT_POOL_PROP_CLIENT_ID] =
        g_param_spec_uchar("client-id", "client-id",
                           "The numeric ID of client. One of ALSASeqSpecificClientId is available "
                           "as well as any numeric value.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READABLE);

    /**
     * ALSASeqClientPool:output-pool:
     *
     * The total number of cells in memory pool for output direction.
     */
    seq_client_pool_props[SEQ_CLIENT_POOL_PROP_OUTPUT_POOL] =
        g_param_spec_uint("output-pool", "output-pool",
                          "The total number of cells in memory pool for output direction.",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READWRITE);

    /**
     * ALSASeqClientPool:input-pool:
     *
     * The total number of cells in memory pool for input direction.
     */
    seq_client_pool_props[SEQ_CLIENT_POOL_PROP_INPUT_POOL] =
        g_param_spec_uint("input-pool", "input-pool",
                          "The total number of cells in memory pool for input direction.",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READWRITE);

    /**
     * ALSASeqClientPool:output-room:
     *
     * The number of cells in memory pool for output direction to block user process.
     */
    seq_client_pool_props[SEQ_CLIENT_POOL_PROP_OUTPUT_ROOM] =
        g_param_spec_uint("output-room", "output-room",
                          "The number of cells in memory pool for output direction to block user "
                          "process",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READWRITE);

    /**
     * ALSASeqClientPool:output-free:
     *
     * The free number of cells in memory pool for output direction.
     */
    seq_client_pool_props[SEQ_CLIENT_POOL_PROP_OUTPUT_FREE] =
        g_param_spec_uint("output-free", "output-free",
                          "The free number of cells in memory pool for output direction.",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READWRITE);

    /**
     * ALSASeqClientPool:input-free:
     *
     * The free number of cells in memory pool for input direction.
     */
    seq_client_pool_props[SEQ_CLIENT_POOL_PROP_INPUT_FREE] =
        g_param_spec_uint("input-free", "input-free",
                          "The free number of cells in memory pool for input direction.",
                          0, G_MAXINT,
                          0,
                          G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_CLIENT_POOL_PROP_COUNT,
                                      seq_client_pool_props);
}

static void alsaseq_client_pool_init(ALSASeqClientPool *self)
{
    return;
}

void seq_client_pool_refer_private(ALSASeqClientPool *self,
                                   struct snd_seq_client_pool **pool)
{
    ALSASeqClientPoolPrivate *priv =
                                alsaseq_client_pool_get_instance_private(self);

    *pool = &priv->pool;
}
