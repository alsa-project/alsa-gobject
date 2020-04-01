// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_FIXED_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_FIXED_H__

#include <glib.h>
#include <glib-object.h>

#include <seq/event.h>
#include <seq/event-data-result.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_FIXED    (alsaseq_event_fixed_get_type())

#define ALSASEQ_EVENT_FIXED(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_EVENT_FIXED,   \
                                ALSASeqEventFixed))
#define ALSASEQ_IS_EVENT_FIXED(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_EVENT_FIXED))

#define ALSASEQ_EVENT_FIXED_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_EVENT_FIXED,      \
                             ALSASeqEventFixedClass))
#define ALSASEQ_IS_EVENT_FIXED_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_EVENT_FIXED))
#define ALSASEQ_EVENT_FIXED_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_EVENT_FIXED,    \
                               ALSASeqEventFixedClass))

typedef struct _ALSASeqEventFixed           ALSASeqEventFixed;
typedef struct _ALSASeqEventFixedClass      ALSASeqEventFixedClass;

struct _ALSASeqEventFixed {
    ALSASeqEvent parent_instance;
};

struct _ALSASeqEventFixedClass {
    ALSASeqEventClass parent_class;
};

GType alsaseq_event_fixed_get_type() G_GNUC_CONST;

ALSASeqEventFixed *alsaseq_event_fixed_new(ALSASeqEventType event_type,
                                           GError **error);

G_END_DECLS

#endif
