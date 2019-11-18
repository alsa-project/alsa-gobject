// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-id.h>

#include <ctl/alsactl-enums.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO      (alsactl_elem_info_get_type())

#define ALSACTL_ELEM_INFO(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSACTL_TYPE_ELEM_INFO,     \
                                ALSACtlElemInfo))
#define ALSACTL_IS_ELEM_INFO(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSACTL_TYPE_ELEM_INFO))

#define ALSACTL_ELEM_INFO_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSACTL_TYPE_ELEM_INFO,        \
                             ALSACtlElemInfoClass))
#define ALSACTL_IS_ELEM_INFO_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSACTL_TYPE_ELEM_INFO))
#define ALSACTL_ELEM_INFO_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSACTL_TYPE_ELEM_INFO,      \
                               ALSACtlElemInfoClass))

typedef struct _ALSACtlElemInfo         ALSACtlElemInfo;
typedef struct _ALSACtlElemInfoClass    ALSACtlElemInfoClass;
typedef struct _ALSACtlElemInfoPrivate  ALSACtlElemInfoPrivate;

struct _ALSACtlElemInfo {
    GObject parent_instance;

    ALSACtlElemInfoPrivate *priv;
};

struct _ALSACtlElemInfoClass {
    GObjectClass parent_class;
};

GType alsactl_elem_info_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
