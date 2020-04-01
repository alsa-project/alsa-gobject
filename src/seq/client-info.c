// SPDX-License-Identifier: LGPL-3.0-or-later
#include "client-info.h"

G_DEFINE_TYPE(ALSASeqClientInfo, alsaseq_client_info, G_TYPE_OBJECT)

static void alsaseq_client_info_class_init(ALSASeqClientInfoClass *klass)
{
    return;
}

static void alsaseq_client_info_init(ALSASeqClientInfo *self)
{
    return;
}

/**
 * alsaseq_client_info_new:
 *
 * Allocate and return an instance of ALSASeqClientinfo class.
 */
ALSASeqClientInfo *alsaseq_client_info_new()
{
    return g_object_new(ALSASEQ_TYPE_CLIENT_INFO, NULL);
}
