// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_CARD_INFO__H__
#define __ALSA_GOBJECT_ALSACTL_CARD_INFO__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_CARD_INFO      (alsactl_card_info_get_type())

#define ALSACTL_CARD_INFO(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSACTL_TYPE_CARD_INFO,     \
                                ALSACtlCardInfo))
#define ALSACTL_IS_CARD_INFO(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSACTL_TYPE_CARD_INFO))

#define ALSACTL_CARD_INFO_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSACTL_TYPE_CARD_INFO,        \
                             ALSACtlCardInfoClass))
#define ALSACTL_IS_CARD_INFO_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSACTL_TYPE_CARD_INFO))
#define ALSACTL_CARD_INFO_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSACTL_TYPE_CARD_INFO,      \
                               ALSACtlCardInfoClass))

typedef struct _ALSACtlCardInfo         ALSACtlCardInfo;
typedef struct _ALSACtlCardInfoClass    ALSACtlCardInfoClass;
typedef struct _ALSACtlCardInfoPrivate  ALSACtlCardInfoPrivate;

struct _ALSACtlCardInfo {
    GObject parent_instance;

    ALSACtlCardInfo *priv;
};

struct _ALSACtlCardInfoClass {
    GObjectClass parent_class;
};

GType alsactl_card_info_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
