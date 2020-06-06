// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-data-result.h"

/**
 * SECTION: event-data-result
 * @Title: ALSASeqEventDataResult
 * @Short_description: A boxed object to represent data of result event
 *
 * A #ALSASeqEventDataResult is a boxed object to represent data of result
 * event. The instance of object is one of data properties in
 * #ALSASeqEventFixed.
 *
 * The object wraps 'struct snd_seq_result' in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataResult *seq_event_data_result_copy(const ALSASeqEventDataResult *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSASeqEventDataResult, alsaseq_event_data_result, seq_event_data_result_copy, g_free)

/**
 * alsaseq_event_data_result_get_event:
 * @self: A #ALSASeqEventDataResult.
 * @event_type: (out): The type of event in which the data results.
 *
 * Get the type of event in which the data results.
 */
void alsaseq_event_data_result_get_event(ALSASeqEventDataResult *self,
                                         ALSASeqEventType *event_type)
{
    *event_type = (ALSASeqEventType)self->event;
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
 * @result: (out): the status of the event.
 *
 * Get the status of event.
 */
void alsaseq_event_data_result_get_result(ALSASeqEventDataResult *self,
                                          gint *result)
{
    *result = self->result;
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
