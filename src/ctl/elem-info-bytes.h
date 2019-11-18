// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BYTES__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_BYTES__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-info.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_BYTES     (alsactl_elem_info_bytes_get_type())

#define ALSACTL_ELEM_INFO_BYTES(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_BYTES,   \
                                ALSACtlElemInfoBytes))
#define ALSACTL_IS_ELEM_INFO_BYTES(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSACTL_TYPE_ELEM_INFO_BYTES))

#define ALSACTL_ELEM_INFO_BYTES_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_BYTES,      \
                             ALSACtlElemInfoBytesClass))
#define ALSACTL_IS_ELEM_INFO_BYTES_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSACTL_TYPE_ELEM_INFO_BYTES))
#define ALSACTL_ELEM_INFO_BYTES_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSACTL_TYPE_ELEM_INFO_BYTES,    \
                               ALSACtlElemInfoBytesClass))

typedef struct _ALSACtlElemInfoBytes         ALSACtlElemInfoBytes;
typedef struct _ALSACtlElemInfoBytesClass    ALSACtlElemInfoBytesClass;

struct _ALSACtlElemInfoBytes {
    ALSACtlElemInfo parent_instance;
};

struct _ALSACtlElemInfoBytesClass {
    ALSACtlElemInfoClass parent_class;
};

GType alsactl_elem_info_bytes_get_type(void) G_GNUC_CONST;

ALSACtlElemInfoBytes *alsactl_elem_info_bytes_new();

G_END_DECLS

#endif
