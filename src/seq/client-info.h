// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_CLIENT_INFO_H__
#define __ALSA_GOBJECT_ALSASEQ_CLIENT_INFO_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_CLIENT_INFO     (alsaseq_client_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqClientInfo, alsaseq_client_info, ALSASEQ, CLIENT_INFO, GObject);

struct _ALSASeqClientInfoClass {
    GObjectClass parent_class;
};

ALSASeqClientInfo *alsaseq_client_info_new();

gboolean alsaseq_client_info_set_event_filter(ALSASeqClientInfo *self,
                                              const ALSASeqEventType *event_types,
                                              gsize event_type_count, GError **error);

gboolean alsaseq_client_info_get_event_filter(ALSASeqClientInfo *self,
                                              ALSASeqEventType **event_types,
                                              gsize *event_type_count, GError **error);

G_END_DECLS

#endif
