// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"
#include <linux/version.h>

#include <errno.h>

/**
 * ALSASeqClientInfo:
 * A GObject-derived object to represent information of client.
 *
 * A [class@ClientInfo] is a GObject-derived object to represent information of client. The call
 * of [func@get_client_info] returns the instance of object.  The call of
 * [method@UserClient.set_info] and [method@UserClient.get_info] require the instance of object.
 *
 * The object wraps `struct snd_seq_client_info` in UAPI of Linux sound subsystem.
 */
typedef struct {
    struct snd_seq_client_info info;
} ALSASeqClientInfoPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqClientInfo, alsaseq_client_info, G_TYPE_OBJECT)

enum seq_client_info_prop_type {
    SEQ_CLIENT_INFO_PROP_CLIENT_ID = 1,
    SEQ_CLIENT_INFO_PROP_CLIENT_TYPE,
    SEQ_CLIENT_INFO_PROP_NAME,
    SEQ_CLIENT_INFO_PROP_FILTER_ATTR_FLAGS,
    SEQ_CLIENT_INFO_PROP_USE_FILTER,
    SEQ_CLIENT_INFO_PROP_PORT_COUNT,
    SEQ_CLIENT_INFO_PROP_LOST_COUNT,
    SEQ_CLIENT_INFO_PROP_CARD_ID,
    SEQ_CLIENT_INFO_PROP_PROCESS_ID,
    SEQ_CLIENT_INFO_PROP_COUNT,
};
static GParamSpec *seq_client_info_props[SEQ_CLIENT_INFO_PROP_COUNT] = { NULL, };

static void seq_client_info_set_property(GObject *obj, guint id,
                                         const GValue *val, GParamSpec *spec)
{
    ALSASeqClientInfo *self = ALSASEQ_CLIENT_INFO(obj);
    ALSASeqClientInfoPrivate *priv =
                                alsaseq_client_info_get_instance_private(self);

    switch (id) {
    case SEQ_CLIENT_INFO_PROP_CLIENT_ID:
        priv->info.client = (int)g_value_get_uchar(val);
        break;
    case SEQ_CLIENT_INFO_PROP_CLIENT_TYPE:
        priv->info.type = (snd_seq_client_type_t)g_value_get_enum(val);
        break;
    case SEQ_CLIENT_INFO_PROP_NAME:
        g_strlcpy(priv->info.name, g_value_get_string(val), sizeof(priv->info.name));
        break;
    case SEQ_CLIENT_INFO_PROP_FILTER_ATTR_FLAGS:
        priv->info.filter &= SNDRV_SEQ_FILTER_USE_EVENT;
        priv->info.filter |= (unsigned int)g_value_get_flags(val);
        break;
    case SEQ_CLIENT_INFO_PROP_USE_FILTER:
        priv->info.filter &= ~SNDRV_SEQ_FILTER_USE_EVENT;
        if (g_value_get_boolean(val))
            priv->info.filter |= SNDRV_SEQ_FILTER_USE_EVENT;
        break;
    case SEQ_CLIENT_INFO_PROP_LOST_COUNT:
        priv->info.event_lost = g_value_get_int(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void seq_client_info_get_property(GObject *obj, guint id, GValue *val,
                                         GParamSpec *spec)
{
    ALSASeqClientInfo *self = ALSASEQ_CLIENT_INFO(obj);
    ALSASeqClientInfoPrivate *priv =
                                alsaseq_client_info_get_instance_private(self);

    switch (id) {
    case SEQ_CLIENT_INFO_PROP_CLIENT_ID:
        g_value_set_uchar(val, (guint8)priv->info.client);
        break;
    case SEQ_CLIENT_INFO_PROP_CLIENT_TYPE:
        g_value_set_enum(val, (ALSASeqClientType)priv->info.type);
        break;
    case SEQ_CLIENT_INFO_PROP_NAME:
        g_value_set_static_string(val, priv->info.name);
        break;
    case SEQ_CLIENT_INFO_PROP_FILTER_ATTR_FLAGS:
        g_value_set_flags(val, (ALSASeqFilterAttrFlag)
                          (priv->info.filter & SNDRV_SEQ_FILTER_USE_EVENT));
        break;
    case SEQ_CLIENT_INFO_PROP_USE_FILTER:
        g_value_set_boolean(val,
                            !!(priv->info.filter & SNDRV_SEQ_FILTER_USE_EVENT));
        break;
    case SEQ_CLIENT_INFO_PROP_PORT_COUNT:
        g_value_set_int(val, priv->info.num_ports);
        break;
    case SEQ_CLIENT_INFO_PROP_LOST_COUNT:
        g_value_set_int(val, priv->info.event_lost);
        break;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,6,0)
    case SEQ_CLIENT_INFO_PROP_CARD_ID:
        g_value_set_int(val, priv->info.card);
        break;
    case SEQ_CLIENT_INFO_PROP_PROCESS_ID:
        g_value_set_int64(val, (gint64)priv->info.pid);
        break;
#endif
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsaseq_client_info_class_init(ALSASeqClientInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = seq_client_info_set_property;
    gobject_class->get_property = seq_client_info_get_property;

    /**
     * ALSASeqClientInfo:client-id:
     *
     * The numeric ID of client. One of [enum@SpecificClientId] is available as well as any
     * numeric value.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_CLIENT_ID] =
        g_param_spec_uchar("client-id", "client-id",
                           "The numeric ID of client. One of ALSASeqSpecificClientId is "
                           "available as well as any numeric value.",
                           0, G_MAXUINT8,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqClientInfo:type:
     *
     * The type of client, one of [enum@ClientType].
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_CLIENT_TYPE] =
        g_param_spec_enum("type", "type",
                         "The type of client, one of ALSASeqClientType.",
                         ALSASEQ_TYPE_CLIENT_TYPE,
                         ALSASEQ_CLIENT_TYPE_NONE,
                         G_PARAM_READWRITE);

    /**
     * ALSASeqClientInfo:name:
     *
     * The name of client.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_NAME] =
        g_param_spec_string("name", "name",
                            "The name of client.",
                            "",
                            G_PARAM_READWRITE);

    /**
     * ALSASeqClientInfo:filter-attributes:
     *
     * The attributes for event filter.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_FILTER_ATTR_FLAGS] =
        g_param_spec_flags("filter-attributes", "filter-attributes",
                           "The attributes for event filter.",
                           ALSASEQ_TYPE_FILTER_ATTR_FLAG,
                           0,
                           G_PARAM_READWRITE);

    /**
     * ALSASeqClientInfo:use-filter:
     *
     * Whether using filter to receive event or not.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_USE_FILTER] =
        g_param_spec_boolean("use-filter", "use-filter",
                             "Whether using filter to receive event or not.",
                             FALSE,
                             G_PARAM_READWRITE);

    /**
     * ALSASeqClientInfo:port-count:
     *
     * The number of ports opened by the client.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_PORT_COUNT] =
        g_param_spec_int("port-count", "port-count",
                         "The number of ports opened by the client.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    /**
     * ALSASeqClientInfo:lost-count:
     *
     * The number of lost events.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_LOST_COUNT] =
        g_param_spec_int("lost-count", "lost-count",
                         "The number of lost events.",
                         0, G_MAXINT,
                         0,
                         G_PARAM_READABLE);

    /**
     * ALSASeqClientInfo:card-id:
     *
     * The numeric ID of sound card. Available in Linux kernel 4.6.0 or later.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_CARD_ID] =
        g_param_spec_int("card-id", "card-id",
                         "The numeric ID of sound card. Available in Linux kernel 4.6.0 or "
                         "later.",
                         G_MININT, G_MAXINT,
                         -1,
                         G_PARAM_READWRITE);

    /**
     * ALSASeqClientInfo:process-id:
     *
     * The process ID for user client, otherwise -1. Available in Linux kernel 4.6.0 or later.
     */
    seq_client_info_props[SEQ_CLIENT_INFO_PROP_PROCESS_ID] =
        g_param_spec_int64("process-id", "process-id",
                           "The process ID for user client, otherwise -1. Available in Linux "
                           "kernel 4.6.0 or later",
                           G_MININT64, G_MAXINT64,
                           -1,
                           G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class,
                                      SEQ_CLIENT_INFO_PROP_COUNT,
                                      seq_client_info_props);
}

static void alsaseq_client_info_init(ALSASeqClientInfo *self)
{
    return;
}

/**
 * alsaseq_client_info_new:
 *
 * Allocate and return an instance of [class@ClientInfo].
 *
 * Returns: An instance of [class@ClientInfo].
 */
ALSASeqClientInfo *alsaseq_client_info_new()
{
    return g_object_new(ALSASEQ_TYPE_CLIENT_INFO, NULL);
}

/**
 * alsaseq_client_info_set_event_filter:
 * @self: A [class@ClientInfo].
 * @event_types: (array length=event_type_count): The array with elements for the type of event to
 *               listen.
 * @event_type_count: The number of elements for the type of event.
 * @error: A [struct@GLib.Error].
 *
 * Set the list of type of events configured to be listen.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_client_info_set_event_filter(ALSASeqClientInfo *self,
                                              const ALSASeqEventType *event_types,
                                              gsize event_type_count, GError **error)
{
    ALSASeqClientInfoPrivate *priv;
    int i;

    g_return_val_if_fail(ALSASEQ_IS_CLIENT_INFO(self), FALSE);
    priv = alsaseq_client_info_get_instance_private(self);

    g_return_val_if_fail(event_types != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    memset(priv->info.event_filter, 0, sizeof(priv->info.event_filter));

    for (i = 0; i < event_type_count; ++i) {
        ALSASeqEventType event_type = (int)event_types[i];
        unsigned int order = event_type / (sizeof(priv->info.event_filter[0]) * 8);
        unsigned int idx = event_type % (sizeof(priv->info.event_filter[0]) * 8);

        if (order < G_N_ELEMENTS(priv->info.event_filter))
            priv->info.event_filter[order] |= 1u << idx;
    }

    return TRUE;
}

/**
 * alsaseq_client_info_get_event_filter:
 * @self: A [class@ClientInfo].
 * @event_types: (array length=event_type_count)(out): The array with elements for the type of
 *               event to listen.
 * @event_type_count: The number of elements for the type of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the list of type of events configured to be listen.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_client_info_get_event_filter(ALSASeqClientInfo *self,
                                              ALSASeqEventType **event_types,
                                              gsize *event_type_count, GError **error)
{
    ALSASeqClientInfoPrivate *priv;
    unsigned int count;
    unsigned int index;
    int i;

    g_return_val_if_fail(ALSASEQ_IS_CLIENT_INFO(self), FALSE);
    priv = alsaseq_client_info_get_instance_private(self);

    g_return_val_if_fail(event_types != NULL, FALSE);
    g_return_val_if_fail(event_type_count != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    count = 0;
    for (i = 0; i < SNDRV_SEQ_EVENT_NONE + 1; ++i) {
        unsigned int order = i / (sizeof(priv->info.event_filter[0]) * 8);
        unsigned int idx = i % (sizeof(priv->info.event_filter[0]) * 8);

        if (order < G_N_ELEMENTS(priv->info.event_filter) &&
            priv->info.event_filter[order] & (1u << idx))
            ++count;
    }

    if (count == 0) {
        *event_types = NULL;
        *event_type_count = 0;
        return TRUE;
    }

    *event_types = g_malloc0_n(count, sizeof(*event_types));

    *event_type_count = count;

    index = 0;
    for (i = 0; i < SNDRV_SEQ_EVENT_NONE + 1; ++i) {
        unsigned int order = i / (sizeof(priv->info.event_filter[0]) * 8);
        unsigned int idx = i % (sizeof(priv->info.event_filter[0]) * 8);

        if (order < G_N_ELEMENTS(priv->info.event_filter) &&
            priv->info.event_filter[order] & (1u << idx)) {
            (*event_types)[index] = (ALSASeqEventType)i;
            ++index;
        }
    }

    return TRUE;
}

void seq_client_info_refer_private(ALSASeqClientInfo *self,
                                   struct snd_seq_client_info **info)
{
    ALSASeqClientInfoPrivate *priv =
                                alsaseq_client_info_get_instance_private(self);

    *info = &priv->info;
}
