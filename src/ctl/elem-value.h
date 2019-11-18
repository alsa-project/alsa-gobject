// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_VALUE__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_VALUE__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_VALUE     (alsactl_elem_value_get_type())

#define ALSACTL_ELEM_VALUE(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSACTL_TYPE_ELEM_VALUE,    \
                                ALSACtlElemValue))
#define ALSACTL_IS_ELEM_VALUE(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSACTL_TYPE_ELEM_VALUE))

#define ALSACTL_ELEM_VALUE_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSACTL_TYPE_ELEM_VALUE,       \
                             ALSACtlElemValueClass))
#define ALSACTL_IS_ELEM_VALUE_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSACTL_TYPE_ELEM_VALUE))
#define ALSACTL_ELEM_VALUE_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSACTL_TYPE_ELEM_VALUE,     \
                               ALSACtlElemValueClass))

typedef struct _ALSACtlElemValue         ALSACtlElemValue;
typedef struct _ALSACtlElemValueClass    ALSACtlElemValueClass;

struct _ALSACtlElemValue {
    GObject parent_instance;
};

struct _ALSACtlElemValueClass {
    GObjectClass parent_class;
};

GType alsactl_elem_value_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
