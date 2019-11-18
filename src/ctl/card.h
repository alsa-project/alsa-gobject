// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_CARD__H__
#define __ALSA_GOBJECT_ALSACTL_CARD__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_CARD   (alsactl_card_get_type())

#define ALSACTL_CARD(obj)                           \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),              \
                                ALSACTL_TYPE_CARD,  \
                                ALSACtlCard))
#define ALSACTL_IS_CARD(obj)                        \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),              \
                                ALSACTL_TYPE_CARD))

#define ALSACTL_CARD_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_CAST((klass),               \
                             ALSACTL_TYPE_CARD,     \
                             ALSACtlCardClass))
#define ALSACTL_IS_CARD_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_TYPE((klass),               \
                             ALSACTL_TYPE_CARD))
#define ALSACTL_CARD_GET_CLASS(obj)                 \
    (G_TYPE_INSTANCE_GET_CLASS((obj),               \
                               ALSACTL_TYPE_CARD,   \
                               ALSACtlCardClass))

typedef struct _ALSACtlCard         ALSACtlCard;
typedef struct _ALSACtlCardClass    ALSACtlCardClass;

struct _ALSACtlCard {
    GObject parent_instance;
};

struct _ALSACtlCardClass {
    GObjectClass parent_class;
};

GType alsactl_card_get_type() G_GNUC_CONST;

ALSACtlCard *alsactl_card_new();

G_END_DECLS

#endif
