// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_VARIABLE_H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_VARIABLE_H__

#include <glib.h>
#include <glib-object.h>

#include <seq/event.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_VARIABLE     (alsaseq_event_variable_get_type())

#define ALSASEQ_EVENT_VARIABLE(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSASEQ_TYPE_EVENT_VARIABLE,    \
                                ALSASeqEventVariable))
#define ALSASEQ_IS_EVENT_VARIABLE(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSASEQ_TYPE_EVENT_VARIABLE))

#define ALSASEQ_EVENT_VARIABLE_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSASEQ_TYPE_EVENT_VARIABLE,       \
                             ALSASeqEventVariableClass))
#define ALSASEQ_IS_EVENT_VARIABLE_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSASEQ_TYPE_EVENT_VARIABLE))
#define ALSASEQ_EVENT_VARIABLE_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSASEQ_TYPE_EVENT_VARIABLE,     \
                               ALSASeqEventVariableClass))

typedef struct _ALSASeqEventVariable        ALSASeqEventVariable;
typedef struct _ALSASeqEventVariableClass   ALSASeqEventVariableClass;

struct _ALSASeqEventVariable {
    ALSASeqEvent parent_instance;
};

struct _ALSASeqEventVariableClass {
    ALSASeqEventClass parent_class;
};

GType alsaseq_event_variable_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
