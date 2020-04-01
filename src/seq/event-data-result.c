// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-result.h"

ALSASeqEventDataResult *seq_event_data_result_copy(const ALSASeqEventDataResult *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataResult, alsaseq_event_data_result, seq_event_data_result_copy, g_free)

/**
 * alsaseq_event_data_result_get_event:
 * @self: A #ALSASeqEventDataResult.
 *
 * Get the type of event in which the data results.
 *
 * Returns: The type of event in which the data results.
 */
ALSASeqEventType alsaseq_event_data_result_get_event(ALSASeqEventDataResult *self)
{
    return (ALSASeqEventType)self->event;
}

/**
 * alsaseq_event_data_result_set_event:
 * @self: A #ALSASeqEventDataResult.
 * @event_type: A #ALSASeqEventType.
 *
 * Set the type of event in which the data results.
 */
void alsaseq_event_data_result_set_event(ALSASeqEventDataResult *self,
                                         ALSASeqEventType event_type)
{
    self->event = (int)event_type;
}

/**
 * alsaseq_event_data_result_get_result:
 * @self: A #ALSASeqEventDataResult.
 *
 * Get the status of event.
 *
 * Returns: the status of the event.
 */
gint alsaseq_event_data_result_get_result(ALSASeqEventDataResult *self)
{
    return self->result;
}

/**
 * alsaseq_event_data_result_set_result:
 * @self: A #ALSASeqEventDataResult.
 * @result: The status of event.
 *
 * Set the status of event.
 */
void alsaseq_event_data_result_set_result(ALSASeqEventDataResult *self,
                                          gint result)
{
    self->result = result;
}
