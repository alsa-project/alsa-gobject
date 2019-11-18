// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INT__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INT__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-info.h>

#define ALSACTL_TYPE_ELEM_INFO_INT  (alsactl_elem_info_int_get_type())

#define ALSACTL_ELEM_INFO_INT(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_INT,     \
                                ALSACtlElemInfoInt))
#define ALSACTL_IS_ELEM_INFO_INT(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_INT))

#define ALSACTL_ELEM_INFO_INT_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_INT,        \
                             ALSACtlElemInfoIntClass))
#define ALSACTL_IS_ELEM_INFO_INT_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_INT))
#define ALSACTL_ELEM_INFO_INT_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSACTL_TYPE_ELEM_INFO_INT,      \
                               ALSACtlElemInfoIntClass))

typedef struct _ALSACtlElemInfoInt      ALSACtlElemInfoInt;
typedef struct _ALSACtlElemInfoIntClass ALSACtlElemInfoIntClass;

struct _ALSACtlElemInfoInt {
    ALSACtlElemInfo parent_instance;
};

struct _ALSACtlElemInfoIntClass {
    ALSACtlElemInfoClass parent_class;
};

GType alsactl_elem_info_int_get_type(void) G_GNUC_CONST;

G_END_DECLS

#endif
