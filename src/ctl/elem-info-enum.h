// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_ENUM__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_ENUM__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-info.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_ENUM     (alsactl_elem_info_enum_get_type())

#define ALSACTL_ELEM_INFO_ENUM(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_ENUM,    \
                                ALSACtlElemInfoEnum))
#define ALSACTL_IS_ELEM_INFO_ENUM(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_ENUM))

#define ALSACTL_ELEM_INFO_ENUM_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_ENUM,       \
                             ALSACtlElemInfoEnumClass))
#define ALSACTL_IS_ELEM_INFO_ENUM_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_ENUM))
#define ALSACTL_ELEM_INFO_ENUM_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSACTL_TYPE_ELEM_INFO_ENUM,     \
                               ALSACtlElemInfoEnumClass))

typedef struct _ALSACtlElemInfoEnum         ALSACtlElemInfoEnum;
typedef struct _ALSACtlElemInfoEnumClass    ALSACtlElemInfoEnumClass;

struct _ALSACtlElemInfoEnum {
    ALSACtlElemInfo parent_instance;
};

struct _ALSACtlElemInfoEnumClass {
    ALSACtlElemInfoClass parent_class;
};

GType alsactl_elem_info_enum_get_type(void) G_GNUC_CONST;
G_END_DECLS

#endif
