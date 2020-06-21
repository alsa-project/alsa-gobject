// SPDX-License-Identifier: LGPL-3.0-or-later
#include "instance-params.h"
#include "privates.h"

#include <sound/asound.h>

#include <errno.h>

/**
 * SECTION: instance-params
 * @Title: ALSATimerInstanceParams
 * @Short_description: A GObject-derived object to represent parameters of user
 *                     instance
 *
 * A #ALSATimerInstanceParams is a GObject-derived object to represent
 * parameters of user instance attached to any timer device or the other
 * instance as slave. The call of alsatimer_user_instance_set_params() requires
 * the instance of object.
 *
 * The object wraps 'struct snd_timer_params' in UAPI of Linux sound subsystem.
 */
struct _ALSATimerInstanceParamsPrivate{
    struct snd_timer_params params;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerInstanceParams, alsatimer_instance_params, G_TYPE_OBJECT)

enum timer_instance_params_prop_type {
    TIMER_INSTANCE_PARAMS_PROP_FLAGS = 1,
    TIMER_INSTANCE_PARAMS_PROP_INTERVAL,
    TIMER_INSTANCE_PARAMS_PROP_QUEUE_SIZE,
    TIMER_INSTANCE_PARAMS_PROP_COUNT,
};
static GParamSpec *timer_instance_params_props[TIMER_INSTANCE_PARAMS_PROP_COUNT] = { NULL, };

static void timer_instance_params_set_property(GObject *obj, guint id,
                                        const GValue *val, GParamSpec *spec)
{
    ALSATimerInstanceParams *self = ALSATIMER_INSTANCE_PARAMS(obj);
    ALSATimerInstanceParamsPrivate *priv =
                            alsatimer_instance_params_get_instance_private(self);

    switch (id) {
    case TIMER_INSTANCE_PARAMS_PROP_FLAGS:
        priv->params.flags = (unsigned int)g_value_get_flags(val);
        break;
    case TIMER_INSTANCE_PARAMS_PROP_INTERVAL:
        priv->params.ticks = g_value_get_uint(val);
        break;
    case TIMER_INSTANCE_PARAMS_PROP_QUEUE_SIZE:
        priv->params.queue_size = g_value_get_uint(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void timer_instance_params_get_property(GObject *obj, guint id,
                                             GValue *val, GParamSpec *spec)
{
    ALSATimerInstanceParams *self = ALSATIMER_INSTANCE_PARAMS(obj);
    ALSATimerInstanceParamsPrivate *priv =
                            alsatimer_instance_params_get_instance_private(self);

    switch (id) {
    case TIMER_INSTANCE_PARAMS_PROP_FLAGS:
        g_value_set_flags(val, (ALSATimerInstanceParamFlag)priv->params.flags);
        break;
    case TIMER_INSTANCE_PARAMS_PROP_INTERVAL:
        g_value_set_uint(val, priv->params.ticks);
        break;
    case TIMER_INSTANCE_PARAMS_PROP_QUEUE_SIZE:
        g_value_set_uint(val, priv->params.queue_size);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void alsatimer_instance_params_class_init(ALSATimerInstanceParamsClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = timer_instance_params_set_property;
    gobject_class->get_property = timer_instance_params_get_property;

    timer_instance_params_props[TIMER_INSTANCE_PARAMS_PROP_FLAGS] =
        g_param_spec_flags("flags", "flags",
                           "The flags for user instance, as a set of "
                           "ALSATimerInstanceParamFlag",
                           ALSATIMER_TYPE_INSTANCE_PARAM_FLAG,
                           0,
                           G_PARAM_READWRITE);

    timer_instance_params_props[TIMER_INSTANCE_PARAMS_PROP_INTERVAL] =
        g_param_spec_uint("interval", "interval",
                          "The interval to generate event in tick count.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READWRITE);

    timer_instance_params_props[TIMER_INSTANCE_PARAMS_PROP_QUEUE_SIZE] =
        g_param_spec_uint("queue-size", "queue-size",
                          "The size of queue.",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class,
                                      TIMER_INSTANCE_PARAMS_PROP_COUNT,
                                      timer_instance_params_props);
}

static void alsatimer_instance_params_init(ALSATimerInstanceParams *self)
{
    return;
}

/**
 * alsatimer_instance_params_new:
 *
 * Allocate and return an instance of ALSATimerInstanceParams.
 */
ALSATimerInstanceParams *alsatimer_instance_params_new()
{
    return g_object_new(ALSATIMER_TYPE_INSTANCE_PARAMS, NULL);
}

/**
 * alsatimer_instance_params_set_event_filter:
 * @self: A #ALSATimerInstanceParams.
 * @entries: (array length=entry_count): The array with elements for entries of
 *           #ALSATimerEventType.
 * @entry_count: The number of elements in the above array.
 * @error: A #GError at failure.
 *
 * Set the list of ALSATimerEventType to filter events. This parameter is only
 * effective for target instance with ALSATimerEventDataType.TIMESTAMP.
 */
void alsatimer_instance_params_set_event_filter(ALSATimerInstanceParams *self,
                                            const ALSATimerEventType *entries,
                                            gsize entry_count, GError **error)
{
    ALSATimerInstanceParamsPrivate *priv;
    unsigned int filter;
    int i;

    g_return_if_fail(ALSATIMER_IS_INSTANCE_PARAMS(self));
    g_return_if_fail(entries != NULL);
    priv = alsatimer_instance_params_get_instance_private(self);

    priv->params.filter = 0;

    // Clear the event filter.
    if (entries == NULL || entry_count == 0)
        return;

    filter = 0;
    for (i = 0; i < entry_count; ++i) {
        int val = (int)entries[i];
        if (val < SNDRV_TIMER_EVENT_RESOLUTION ||
            (val > SNDRV_TIMER_EVENT_RESUME &&
             val < SNDRV_TIMER_EVENT_MSTART) ||
            val > SNDRV_TIMER_EVENT_MRESUME) {
            generate_error(error, EINVAL);
            return;
        }
        filter |= (1u << val);
    }

    priv->params.filter = filter;
}

/**
 * alsatimer_instance_params_get_event_filter:
 * @self: A #ALSATimerInstanceParams.
 * @entries: (array length=entry_count)(out): The array with elements for
 *           entries of #ALSATimerEventType.
 * @entry_count: The number of elements in the above array.
 * @error: A #GError at failure.
 *
 * Get the list of ALSATimerEventType to filter events. This parameter is only
 * effective for target instance with ALSATimerEventDataType.TIMESTAMP.
 */
void alsatimer_instance_params_get_event_filter(ALSATimerInstanceParams *self,
                                            ALSATimerEventType **entries,
                                            gsize *entry_count, GError **error)
{
    ALSATimerInstanceParamsPrivate *priv;
    ALSATimerEventType *list;
    unsigned int filter;
    unsigned int count;
    unsigned int index;
    int i;

    g_return_if_fail(ALSATIMER_IS_INSTANCE_PARAMS(self));
    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    priv = alsatimer_instance_params_get_instance_private(self);

    count = 0;
    filter = priv->params.filter;
    for (i = 0; i < sizeof(filter) * 8; ++i) {
        if ((1u << i) & filter)
            ++count;
    }

    if (count == 0)
        return;

    list = g_try_malloc0_n(count, sizeof(*list));
    if (list == NULL) {
        generate_error(error, ENOMEM);
        return;
    }

    index = 0;
    for (i = 0; i < sizeof(filter) * 8; ++i) {
        if ((1u << i) & filter) {
            list[index] = (ALSATimerEventType)i;
            if (++index >= count)
                break;
        }
    }

    if (index != count) {
        generate_error(error, ENXIO);
        g_free(list);
        return;
    }

    *entries = list;
    *entry_count = count;
}

void timer_instance_params_refer_private(ALSATimerInstanceParams *self,
                                         struct snd_timer_params **params)
{
    ALSATimerInstanceParamsPrivate *priv =
                        alsatimer_instance_params_get_instance_private(self);

    *params = &priv->params;
}
