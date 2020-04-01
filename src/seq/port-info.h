// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_PORT_INFO__H__
#define __ALSA_GOBJECT_ALSASEQ_PORT_INFO__H__

#include <glib.h>
#include <glib-object.h>

#include <seq/addr.h>
#include <seq/alsaseq-enums.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_PORT_INFO      (alsaseq_port_info_get_type())

#define ALSASEQ_PORT_INFO(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_PORT_INFO,     \
                                ALSASeqPortInfo))
#define ALSASEQ_IS_PORT_INFO(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_PORT_INFO))

#define ALSASEQ_PORT_INFO_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_PORT_INFO,        \
                             ALSASeqPortInfoClass))
#define ALSASEQ_IS_PORT_INFO_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_PORT_INFO))
#define ALSASEQ_PORT_INFO_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_PORT_INFO,      \
                               ALSASeqPortInfoClass))

typedef struct _ALSASeqPortInfo         ALSASeqPortInfo;
typedef struct _ALSASeqPortInfoClass    ALSASeqPortInfoClass;
typedef struct _ALSASeqPortInfoPrivate  ALSASeqPortInfoPrivate;

struct _ALSASeqPortInfo {
    GObject parent_instance;

    ALSASeqPortInfoPrivate *priv;
};

struct _ALSASeqPortInfoClass {
    GObjectClass parent_class;
};

GType alsaseq_port_info_get_type() G_GNUC_CONST;

ALSASeqPortInfo *alsaseq_port_info_new();

#endif
