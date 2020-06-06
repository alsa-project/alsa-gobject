// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-variable.h"
#include "privates.h"

#include <errno.h>

/**
 * SECTION: event-variable
 * @Title: ALSASeqEventVariable
 * @Short_description: A GObject-derived object to represent event with the
 *                     various kind of variable-size data.
 *
 * A #ALSASeqEventVariable is a GObject-derived object to represent event with
 * the various kind of variable-size data. The object has accessor methods for
 * the variable-size data.
 * The object inherits properties and methods from #ALSASeqEvent.
 */
struct _ALSASeqEventVariablePrivate {
    void *data;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSASeqEventVariable, alsaseq_event_variable, ALSASEQ_TYPE_EVENT)

static void seq_event_variable_finalize(GObject *obj)
{
    ALSASeqEventVariable *self = ALSASEQ_EVENT_VARIABLE(obj);
    ALSASeqEventVariablePrivate *priv =
                            alsaseq_event_variable_get_instance_private(self);

    if (priv->data != NULL)
        g_free(priv->data);

    G_OBJECT_CLASS(alsaseq_event_variable_parent_class)->finalize(obj);
}

static void alsaseq_event_variable_class_init(ALSASeqEventVariableClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = seq_event_variable_finalize;
}

static void alsaseq_event_variable_init(ALSASeqEventVariable *self)
{
    ALSASeqEventVariablePrivate *priv =
                            alsaseq_event_variable_get_instance_private(self);

    priv->data = NULL;
}

/**
 * alsaseq_event_variable_new:
 * @event_type: A #ALSASeqEventType.
 * @error: A #GError.
 *
 * Allocate and return an instance of #ALSASeqEventVariable class.
 *
 * Returns: A #ALSASeqEventVariable.
 */
ALSASeqEventVariable *alsaseq_event_variable_new(ALSASeqEventType event_type,
                                                 GError **error)
{
    switch (event_type) {
    case SNDRV_SEQ_EVENT_SYSEX:
    case SNDRV_SEQ_EVENT_BOUNCE:
    case SNDRV_SEQ_EVENT_USR_VAR0:
    case SNDRV_SEQ_EVENT_USR_VAR1:
    case SNDRV_SEQ_EVENT_USR_VAR2:
    case SNDRV_SEQ_EVENT_USR_VAR3:
    case SNDRV_SEQ_EVENT_USR_VAR4:
        break;
    default:
        generate_error(error, EINVAL);
        break;
    }

    return g_object_new(ALSASEQ_TYPE_EVENT_VARIABLE,
                        "type", event_type,
                        "length-mode", ALSASEQ_EVENT_LENGTH_MODE_VARIABLE,
                        NULL);
}

/**
 * alsaseq_event_variable_get_data:
 * @self: A #ALSASeqEventVariable.
 * @data: (array length=size)(out)(transfer none): The pointer to external data.
 * @size: The size of data.
 *
 * Refer to the external data for the event.
 */
void alsaseq_event_variable_get_data(ALSASeqEventVariable *self,
                                     const guint8 **data, gsize *size)
{
    ALSASeqEvent *parent;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_VARIABLE(self));
    parent = ALSASEQ_EVENT(self);
    seq_event_refer_private(parent, &ev);

    *data = ev->data.ext.ptr;
    *size = ev->data.ext.len;
}

/**
 * alsaseq_event_variable_set_data:
 * @self: A #ALSASeqEventVariable.
 * @data: (array length=size)(transfer none): The pointer to external data.
 * @size: The size of data.
 *
 * Allocate and copy the external data for the event.
 */
void alsaseq_event_variable_set_data(ALSASeqEventVariable *self,
                                     const guint8 *data, gsize size)
{
    ALSASeqEventVariablePrivate *priv;
    ALSASeqEvent *parent;
    struct snd_seq_event *ev;

    g_return_if_fail(ALSASEQ_IS_EVENT_VARIABLE(self));
    priv = alsaseq_event_variable_get_instance_private(self);
    parent = ALSASEQ_EVENT(self);
    seq_event_refer_private(parent, &ev);

    if (priv->data != NULL)
        g_free(priv->data);
    priv->data = NULL;
    ev->data.ext.ptr = NULL;
    ev->data.ext.len = 0;

    priv->data = g_memdup(data, size);
    if (priv->data == NULL)
        return;

    ev->data.ext.ptr = priv->data;
    ev->data.ext.len = size;
}
