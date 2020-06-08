// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR__H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_CNTR     (alsaseq_event_cntr_get_type())

#define ALSASEQ_EVENT_CNTR(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_EVENT_CNTR,    \
                                ALSASeqEventCntr))
#define ALSASEQ_IS_EVENT_CNTR(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_EVENT_CNTR))

#define ALSASEQ_EVENT_CNTR_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_EVENT_CNTR,       \
                             ALSASeqEventCntrClass))
#define ALSASEQ_IS_EVENT_CNTR_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_EVENT_CNTR))
#define ALSASEQ_EVENT_CNTR_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_EVENT_CNTR,     \
                               ALSASeqEventCntrClass))

typedef struct _ALSASeqEventCntr            ALSASeqEventCntr;
typedef struct _ALSASeqEventCntrClass       ALSASeqEventCntrClass;

struct _ALSASeqEventCntr {
    GObject parent_instance;
};

struct _ALSASeqEventCntrClass {
    GObjectClass parent_class;
};

GType alsaseq_event_cntr_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
