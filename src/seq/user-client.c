// SPDX-License-Identifier: LGPL-3.0-or-later
#include "user-client.h"

G_DEFINE_TYPE(ALSASeqUserClient, alsaseq_user_client, G_TYPE_OBJECT)

static void alsaseq_user_client_class_init(ALSASeqUserClientClass *klass)
{
    return;
}

static void alsaseq_user_client_init(ALSASeqUserClient *self)
{
    return;
}

/**
 * alsaseq_user_client_new:
 *
 * Allocate and return an instance of ALSASeqUserClient class.
 */
ALSASeqUserClient *alsaseq_user_client_new()
{
    return g_object_new(ALSASEQ_TYPE_USER_CLIENT, NULL);
}
