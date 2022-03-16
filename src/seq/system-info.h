// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_SYSTEM_INFO__H__
#define __ALSA_GOBJECT_ALSASEQ_SYSTEM_INFO__H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_SYSTEM_INFO     (alsaseq_system_info_get_type())

#define ALSASEQ_SYSTEM_INFO(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_SYSTEM_INFO,   \
                                ALSASeqSystemInfo))
#define ALSASEQ_IS_SYSTEM_INFO(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_SYSTEM_INFO))

#define ALSASEQ_SYSTEM_INFO_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_SYSTEM_INFO,      \
                             ALSASeqSystemInfoClass))
#define ALSASEQ_IS_SYSTEM_INFO_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_SYSTEM_INFO))
#define ALSASEQ_SYSTEM_INFO_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_SYSTEM_INFO,    \
                               ALSASeqSystemInfoClass))

typedef struct _ALSASeqSystemInfo           ALSASeqSystemInfo;
typedef struct _ALSASeqSystemInfoClass      ALSASeqSystemInfoClass;
typedef struct _ALSASeqSystemInfoPrivate    ALSASeqSystemInfoPrivate;

struct _ALSASeqSystemInfo {
    GObject parent_instance;

    ALSASeqSystemInfoPrivate *priv;
};

struct _ALSASeqSystemInfoClass {
    GObjectClass parent_class;
};

GType alsaseq_system_info_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
