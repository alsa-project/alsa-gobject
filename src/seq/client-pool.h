// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_CLIENT_POOL_H__
#define __ALSA_GOBJECT_ALSASEQ_CLIENT_POOL_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_CLIENT_POOL        (alsaseq_client_pool_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqClientPool, alsaseq_client_pool, ALSASEQ, CLIENT_POOL, GObject);

struct _ALSASeqClientPoolClass {
    GObjectClass parent_class;
};

ALSASeqClientPool *alsaseq_client_pool_new();

G_END_DECLS

#endif
