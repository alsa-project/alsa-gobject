// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BOOL__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BOOL__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-info.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_BOOL  (alsactl_elem_info_bool_get_type())

#define ALSACTL_ELEM_INFO_BOOL(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_BOOL,    \
                                ALSACtlElemInfoBool))
#define ALSACTL_IS_ELEM_INFO_BOOL(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_BOOL))

#define ALSACTL_ELEM_INFO_BOOL_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_BOOL,       \
                             ALSACtlElemInfoBoolClass))
#define ALSACTL_IS_ELEM_INFO_BOOL_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_BOOL))
#define ALSACTL_ELEM_INFO_BOOL_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSACTL_TYPE_ELEM_INFO_BOOL,     \
                               ALSACtlElemInfoBoolClass))

typedef struct _ALSACtlElemInfoBool      ALSACtlElemInfoBool;
typedef struct _ALSACtlElemInfoBoolClass ALSACtlElemInfoBoolClass;

struct _ALSACtlElemInfoBool {
    ALSACtlElemInfo parent_instance;
};

struct _ALSACtlElemInfoBoolClass {
    ALSACtlElemInfoClass parent_class;
};

GType alsactl_elem_info_bool_get_type(void) G_GNUC_CONST;

ALSACtlElemInfoBool *alsactl_elem_info_bool_new();

G_END_DECLS

#endif
