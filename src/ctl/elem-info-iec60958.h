// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_IEC60958__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_IEC60958__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-info.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO_IEC60958 (alsactl_elem_info_iec60958_get_type())

#define ALSACTL_ELEM_INFO_IEC60958(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                              \
                                ALSACTL_TYPE_ELEM_INFO_IEC60958,    \
                                ALSACtlElemInfoIec60958))
#define ALSACTL_IS_ELEM_INFO_IEC60958(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                              \
                                ALSACTL_TYPE_ELEM_INFO_IEC60958))

#define ALSACTL_ELEM_INFO_IEC60958_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                               \
                             ALSACTL_TYPE_ELEM_INFO_IEC60958,       \
                             ALSACtlElemInfoIec60958Class))
#define ALSACTL_IS_ELEM_INFO_IEC60958_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                               \
                             ALSACTL_TYPE_ELEM_INFO_IEC60958))
#define ALSACTL_ELEM_INFO_IEC60958_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                               \
                               ALSACTL_TYPE_ELEM_INFO_IEC60958,     \
                               ALSACtlElemInfoIec60958Class))

typedef struct _ALSACtlElemInfoIec60958         ALSACtlElemInfoIec60958;
typedef struct _ALSACtlElemInfoIec60958Class    ALSACtlElemInfoIec60958Class;

struct _ALSACtlElemInfoIec60958 {
    ALSACtlElemInfo parent_instance;
};

struct _ALSACtlElemInfoIec60958Class {
    ALSACtlElemInfoClass parent_class;
};

GType alsactl_elem_info_iec60958_get_type(void) G_GNUC_CONST;

G_END_DECLS

#endif
