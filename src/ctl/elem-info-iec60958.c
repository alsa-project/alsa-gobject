// SPDX-License-Identifier: LGPL-3.0-or-later
#include "elem-info-iec60958.h"

G_DEFINE_TYPE(ALSACtlElemInfoIec60958, alsactl_elem_info_iec60958, ALSACTL_TYPE_ELEM_INFO)

static void alsactl_elem_info_iec60958_class_init(ALSACtlElemInfoIec60958Class *klass)
{
    return;
}

static void alsactl_elem_info_iec60958_init(ALSACtlElemInfoIec60958 *self)
{
    return;
}

/**
 * alsactl_elem_info_iec60958_new:
 *
 * Allocate and return an instance of ALSACtlElemInfoIec60958.
 *
 * Returns: (transfer full): A #ALSACtlElemInfoIec60958.
 */
ALSACtlElemInfoIec60958 *alsactl_elem_info_iec60958_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_IEC60958,
                        "type", ALSACTL_ELEM_TYPE_IEC60958, NULL);
}
