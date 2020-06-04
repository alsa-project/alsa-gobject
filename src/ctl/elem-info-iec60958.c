// SPDX-License-Identifier: LGPL-3.0-or-later
#include "elem-info-iec60958.h"

/**
 * SECTION: elem-info-iec60958
 * @Title: ALSACtlElemInfoIec60958
 * @Short_description: A GObject-derived object to represent the information
 *                     of IEC 60958 type of element
 *
 * A #ALSACtlElemInfoIec60958 is a GObject-derived object to represent the
 * information of IEC 60958 type of element, which contains channel status and
 * user data. The object inherits methods and properties of #ALSACtlElemInfo.
 * A call of alsactl_card_get_elem_info() can returns the instance of object,
 * or a call of alsactl_card_add_elems() requires the instance of object as
 * argument.
 */
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
 * Returns: A #ALSACtlElemInfoIec60958.
 */
ALSACtlElemInfoIec60958 *alsactl_elem_info_iec60958_new()
{
    return g_object_new(ALSACTL_TYPE_ELEM_INFO_IEC60958,
                        "type", ALSACTL_ELEM_TYPE_IEC60958, NULL);
}
