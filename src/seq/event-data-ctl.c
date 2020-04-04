// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-ctl.h"

ALSASeqEventDataCtl *seq_event_data_ctl_copy(const ALSASeqEventDataCtl *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataCtl, alsaseq_event_data_ctl, seq_event_data_ctl_copy, g_free)

/**
 * alsaseq_event_data_ctl_get_channel:
 * @self: A #ALSASeqEventDataCtl.
 * @channel: (out): The value of channel for the control event.
 *
 * Get the value of channel for the control event.
 */
void alsaseq_event_data_ctl_get_channel(ALSASeqEventDataCtl *self,
                                        guint8 *channel)
{
    *channel = self->channel;
}

/**
 * alsaseq_event_data_ctl_set_channel:
 * @self: A #ALSASeqEventDataCtl.
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
 * @self: A #ALSASeqEventDataCtl.
 * @param: (out): The parameter for the control event.
 *
 * Get the parameter for the control event.
 */
void alsaseq_event_data_ctl_get_param(ALSASeqEventDataCtl *self, guint *param)
{
    *param = self->param;
}

/**
 * alsaseq_event_data_ctl_set_param:
 * @self: A #ALSASeqEventDataCtl.
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
 * @self: A #ALSASeqEventDataCtl.
 * @value: (out): The value for the control event.
 *
 * Get the value for the control event.
 */
void alsaseq_event_data_ctl_get_value(ALSASeqEventDataCtl *self, gint *value)
{
    *value = self->param;
}

/**
 * alsaseq_event_data_ctl_set_value:
 * @self: A #ALSASeqEventDataCtl.
 * @value: The value for the control event.
 *
 * Set the value for the control event.
 */
void alsaseq_event_data_ctl_set_value(ALSASeqEventDataCtl *self, gint value)
{
    self->value = value;
}
