// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_CNTR     (alsaseq_event_cntr_get_type())

typedef struct {
    /*< private >*/
    guint8 *buf;
    gsize length;
    gboolean aligned;
} ALSASeqEventCntr;

GType alsaseq_event_cntr_get_type() G_GNUC_CONST;

void alsaseq_event_cntr_deserialize(const ALSASeqEventCntr *self, GList **events);

G_END_DECLS

#endif
