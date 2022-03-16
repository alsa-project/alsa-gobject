// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_SUBSCRIBE_DATA__H__
#define __ALSA_GOBJECT_ALSASEQ_SUBSCRIBE_DATA__H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_SUBSCRIBE_DATA     (alsaseq_subscribe_data_get_type())

#define ALSASEQ_SUBSCRIBE_DATA(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSASEQ_TYPE_SUBSCRIBE_DATA,    \
                                ALSASeqSubscribeData))
#define ALSASEQ_IS_SUBSCRIBE_DATA(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSASEQ_TYPE_SUBSCRIBE_DATA))

#define ALSASEQ_SUBSCRIBE_DATA_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSASEQ_TYPE_SUBSCRIBE_DATA,       \
                             ALSASeqSubscribeDataClass))
#define ALSASEQ_IS_SUBSCRIBE_DATA_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSASEQ_TYPE_SUBSCRIBE_DATA))
#define ALSASEQ_SUBSCRIBE_DATA_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSASEQ_TYPE_SUBSCRIBE_DATA,     \
                               ALSASeqSubscribeDataClass))

typedef struct _ALSASeqSubscribeData            ALSASeqSubscribeData;
typedef struct _ALSASeqSubscribeDataClass       ALSASeqSubscribeDataClass;
typedef struct _ALSASeqSubscribeDataPrivate     ALSASeqSubscribeDataPrivate;

struct _ALSASeqSubscribeData {
    GObject parent_instance;

    ALSASeqSubscribeData *priv;
};

struct _ALSASeqSubscribeDataClass {
    GObjectClass parent_class;
};

GType alsaseq_subscribe_data_get_type() G_GNUC_CONST;

ALSASeqSubscribeData *alsaseq_subscribe_data_new();

G_END_DECLS

#endif
