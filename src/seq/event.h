// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT__H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT     (alsaseq_event_get_type())

#define ALSASEQ_EVENT(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                  \
                                ALSASEQ_TYPE_EVENT,     \
                                ALSASeqEvent))
#define ALSASEQ_IS_EVENT(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                  \
                                ALSASEQ_TYPE_EVENT))

#define ALSASEQ_EVENT_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                   \
                             ALSASEQ_TYPE_EVENT,        \
                             ALSASeqEventClass))
#define ALSASEQ_IS_EVENT_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                   \
                             ALSASEQ_TYPE_EVENT))
#define ALSASEQ_EVENT_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                   \
                               ALSASEQ_TYPE_EVENT,      \
                               ALSASeqEventClass))

typedef struct _ALSASeqEvent        ALSASeqEvent;
typedef struct _ALSASeqEventClass   ALSASeqEventClass;

struct _ALSASeqEvent {
    GObject parent_instance;
};

struct _ALSASeqEventClass {
    GObjectClass parent_class;
};

GType alsaseq_event_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
