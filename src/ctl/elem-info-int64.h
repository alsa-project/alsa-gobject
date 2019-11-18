// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INT64__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_INT64__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-info.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_INT64        (alsactl_elem_info_int64_get_type())

#define ALSACTL_ELEM_INFO_INT64(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_INT64,   \
                                ALSACtlElemInfoInt64))
#define ALSACTL_IS_ELEM_INFO_INT64(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_INT64))

#define ALSACTL_ELEM_INFO_INT64_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_INT64,      \
                             ALSACtlElemInfoInt64Class))
#define ALSACTL_IS_ELEM_INFO_INT64_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_INT64))
#define ALSACTL_ELEM_INFO_INT64_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSACTL_TYPE_ELEM_INFO_INT64,    \
                               ALSACtlElemInfoInt64Class))

typedef struct _ALSACtlElemInfoInt64        ALSACtlElemInfoInt64;
typedef struct _ALSACtlElemInfoInt64Class   ALSACtlElemInfoInt64Class;

struct _ALSACtlElemInfoInt64 {
    ALSACtlElemInfo parent_instance;
};

struct _ALSACtlElemInfoInt64Class {
    ALSACtlElemInfoClass parent_class;
};

GType alsactl_elem_info_int64_get_type(void) G_GNUC_CONST;

ALSACtlElemInfoInt64 *alsactl_elem_info_int64_new();

G_END_DECLS

#endif
