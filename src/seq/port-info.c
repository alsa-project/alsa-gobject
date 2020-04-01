// SPDX-License-Identifier: LGPL-3.0-or-later
#include "port-info.h"

G_DEFINE_TYPE(ALSASeqPortInfo, alsaseq_port_info, G_TYPE_OBJECT)

static void alsaseq_port_info_class_init(ALSASeqPortInfoClass *klass)
{
    return;
}

static void alsaseq_port_info_init(ALSASeqPortInfo *self)
{
    return;
}

/**
 * alsaseq_port_info_new:
 *
 * Allocate and return an instance of ALSASeqPortInfo class.
 */
ALSASeqPortInfo *alsaseq_port_info_new()
{
    return g_object_new(ALSASEQ_TYPE_PORT_INFO, NULL);
}
