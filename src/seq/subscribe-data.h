// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_SUBSCRIBE_DATA_H__
#define __ALSA_GOBJECT_ALSASEQ_SUBSCRIBE_DATA_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_SUBSCRIBE_DATA     (alsaseq_subscribe_data_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqSubscribeData, alsaseq_subscribe_data, ALSASEQ, SUBSCRIBE_DATA, GObject);

struct _ALSASeqSubscribeDataClass {
    GObjectClass parent_class;
};

ALSASeqSubscribeData *alsaseq_subscribe_data_new();

G_END_DECLS

#endif
