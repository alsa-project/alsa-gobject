// SPDX-License-Identifier: LGPL-3.0-or-later
#include "event-fixed.h"
#include "privates.h"

#include <errno.h>

G_DEFINE_TYPE(ALSASeqEventFixed, alsaseq_event_fixed, ALSASEQ_TYPE_EVENT)

static void alsaseq_event_fixed_class_init(ALSASeqEventFixedClass *klass)
{
    return;
}

static void alsaseq_event_fixed_init(ALSASeqEventFixed *self)
{
    return;
}

/**
 * alsaseq_event_fixed_new:
 * @event_type: A #ALSASeqEventType.
 * @error: A #GError.
 *
 * Allocate and returns an instance of #ALSASeqEventFixed class.
 *
 * Returns: (transfer full): A #ALSASeqEventFixed.
 */
ALSASeqEventFixed *alsaseq_event_fixed_new(ALSASeqEventType event_type,
                                           GError **error)
{
    switch (event_type) {
    case ALSASEQ_EVENT_TYPE_BOUNCE:
    case ALSASEQ_EVENT_TYPE_USR_VAR0:
    case ALSASEQ_EVENT_TYPE_USR_VAR1:
    case ALSASEQ_EVENT_TYPE_USR_VAR2:
    case ALSASEQ_EVENT_TYPE_USR_VAR3:
    case ALSASEQ_EVENT_TYPE_USR_VAR4:
        generate_error(error, EINVAL);
        return NULL;
    default:
	break;
    }

    return g_object_new(ALSASEQ_TYPE_EVENT_FIXED,
                        "type", event_type,
                        "length-mode", ALSASEQ_EVENT_LENGTH_MODE_FIXED,
                        NULL);
}
