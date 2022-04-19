// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqEventDataResult:
 * A boxed object to represent data of result event.
 *
 * A [struct@EventDataResult] is a boxed object to represent data of result event. The instance of
 * object is one of data properties in event.
 *
 * The object wraps `struct snd_seq_result` in UAPI of Linux sound subsystem.
 */
ALSASeqEventDataResult *seq_event_data_result_copy(const ALSASeqEventDataResult *self)
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

G_DEFINE_BOXED_TYPE(ALSASeqEventDataResult, alsaseq_event_data_result, seq_event_data_result_copy, g_free)

/**
 * alsaseq_event_data_result_get_event:
 * @self: A [struct@EventDataResult].
 * @event_type: (out): The type of event in which the data results.
 *
 * Get the type of event in which the data results.
 */
void alsaseq_event_data_result_get_event(const ALSASeqEventDataResult *self,
                                         ALSASeqEventType *event_type)
{
    *event_type = (ALSASeqEventType)self->event;
}

/**
 * alsaseq_event_data_result_set_event:
 * @self: A [struct@EventDataResult].
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
 * @self: A [struct@EventDataResult].
 * @result: (out): the status of the event.
 *
 * Get the status of event.
 */
void alsaseq_event_data_result_get_result(const ALSASeqEventDataResult *self,
                                          gint *result)
{
    *result = self->result;
}

/**
 * alsaseq_event_data_result_set_result:
 * @self: A [struct@EventDataResult].
 * @result: The status of event.
 *
 * Set the status of event.
 */
void alsaseq_event_data_result_set_result(ALSASeqEventDataResult *self,
                                          gint result)
{
    self->result = result;
}
