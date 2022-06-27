// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqEventDataCtl:
 * A boxed object to express data of control event.
 *
 * A [struct@EventDataCtl] is a boxed object to express data of control event. The instance of
 * object is one of data properties in event.
 *
 * The object wraps `struct snd_seq_ev_ctrl` in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataCtl *seq_event_data_ctl_copy(const ALSASeqEventDataCtl *self)
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

G_DEFINE_BOXED_TYPE(ALSASeqEventDataCtl, alsaseq_event_data_ctl, seq_event_data_ctl_copy, g_free)

/**
 * alsaseq_event_data_ctl_get_channel:
 * @self: A [struct@EventDataCtl].
 * @channel: (out): The value of channel for the control event.
 *
 * Get the value of channel for the control event.
 */
void alsaseq_event_data_ctl_get_channel(const ALSASeqEventDataCtl *self,
                                        guint8 *channel)
{
    *channel = self->channel;
}

/**
 * alsaseq_event_data_ctl_set_channel:
 * @self: A [struct@EventDataCtl].
 * @channel: The channel for the control event.
 *
 * Set the channel for the control event.
 */
void alsaseq_event_data_ctl_set_channel(ALSASeqEventDataCtl *self,
                                        guint8 channel)
{
    self->channel = channel;
}

/**
 * alsaseq_event_data_ctl_get_param:
 * @self: A [struct@EventDataCtl].
 * @param: (out): The parameter for the control event.
 *
 * Get the parameter for the control event.
 */
void alsaseq_event_data_ctl_get_param(const ALSASeqEventDataCtl *self,
                                      guint *param)
{
    *param = self->param;
}

/**
 * alsaseq_event_data_ctl_set_param:
 * @self: A [struct@EventDataCtl].
 * @param: The parameter for the control event.
 *
 * Set the parameter for the control event.
 */
void alsaseq_event_data_ctl_set_param(ALSASeqEventDataCtl *self, guint param)
{
    self->param = param;
}

/**
 * alsaseq_event_data_ctl_get_value:
 * @self: A [struct@EventDataCtl].
 * @value: (out): The value for the control event.
 *
 * Get the value for the control event.
 */
void alsaseq_event_data_ctl_get_value(const ALSASeqEventDataCtl *self,
                                      gint *value)
{
    *value = self->value;
}

/**
 * alsaseq_event_data_ctl_set_value:
 * @self: A [struct@EventDataCtl].
 * @value: The value for the control event.
 *
 * Set the value for the control event.
 */
void alsaseq_event_data_ctl_set_value(ALSASeqEventDataCtl *self, gint value)
{
    self->value = value;
}
