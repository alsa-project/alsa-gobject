// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqQueueTimerCommon:
 * An interface to express common features of queue timer.
 *
 * A [iface@QueueTimerCommon] should be implemented by any type of queue timer.
 *
 * The object wraps `struct snd_seq_queue_timer` in UAPI of Linux sound subsystem.
 *
 * Since: 0.3.
 */

static void alsaseq_queue_timer_common_default_init(ALSASeqQueueTimerCommonInterface *iface);

G_DEFINE_INTERFACE(ALSASeqQueueTimerCommon, alsaseq_queue_timer_common, G_TYPE_OBJECT)

static void alsaseq_queue_timer_common_default_init(ALSASeqQueueTimerCommonInterface *iface)
{
    /**
     * ALSASeqQueueTimerCommon:queue-id:
     *
     * The numeric identifier of queue, including one of [enum@SpecificClientId].
     *
     * Since: 0.3.
     */
    g_object_interface_install_property(iface,
        g_param_spec_uchar(QUEUE_ID_PROP_NAME, QUEUE_ID_PROP_NAME,
                           "The numeric identifier of queue including ALSASeqSpecificClientId.",
                           0, G_MAXUINT8, 0,
                           G_PARAM_READABLE));

    /**
     * ALSASeqQueueTimerCommon:timer-type:
     *
     * The type of timer for the queue, one of [enum@QueueTimerType].
     *
     * Since: 0.3.
     */
    g_object_interface_install_property(iface,
        g_param_spec_enum(TIMER_TYPE_PROP_NAME, TIMER_TYPE_PROP_NAME,
                          "The type of timer for the queue, one of ALSASeqQueueTimerType.",
                          ALSASEQ_TYPE_QUEUE_TIMER_TYPE, ALSASEQ_QUEUE_TIMER_TYPE_ALSA,
                          G_PARAM_READABLE));
}
