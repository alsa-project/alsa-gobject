// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_CARD__H__
#define __ALSA_GOBJECT_ALSACTL_CARD__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/card-info.h>
#include <ctl/elem-id.h>
#include <ctl/elem-info.h>
#include <ctl/elem-value.h>

#include <ctl/alsactl-sigs-marshal.h>

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

#define ALSACTL_CARD_ERROR	alsactl_card_error_quark()

GQuark alsactl_card_error_quark();

typedef struct _ALSACtlCard         ALSACtlCard;
typedef struct _ALSACtlCardClass    ALSACtlCardClass;
typedef struct _ALSACtlCardPrivate  ALSACtlCardPrivate;

struct _ALSACtlCard {
    GObject parent_instance;

    ALSACtlCardPrivate *priv;
};

struct _ALSACtlCardClass {
    GObjectClass parent_class;

    /**
     * ALSACtlCardClass::handle_elem_event:
     * @self: A #ALSACtlCard.
     * @elem_id: (transfer none): A #ALSACtlElemId.
     * @events: A set of #ALSACtlElemEventMask.
     *
     * When event occurs for any element, this signal is emit.
     */
    void (*handle_elem_event)(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                              ALSACtlElemEventMask events);

    /**
     * ALSACtlCardClass::handle_disconnection:
     * @self: A #ALSACtlCard.
     *
     * When the sound card is not available anymore due to unbinding driver or
     * hot unplugging, this signal is emit. The owner of this object should
     * call g_object_free() as quickly as possible to be going to release ALSA
     * control character device.
     */
    void (*handle_disconnection)(ALSACtlCard *self);
};

GType alsactl_card_get_type() G_GNUC_CONST;

ALSACtlCard *alsactl_card_new();

void alsactl_card_open(ALSACtlCard *self, guint card_id, gint open_flag,
                       GError **error);

void alsactl_card_get_protocol_version(ALSACtlCard *self,
                                       const guint16 *proto_ver_triplet[3],
                                       GError **error);

void alsactl_card_get_info(ALSACtlCard *self, ALSACtlCardInfo **card_info,
                           GError **error);

void alsactl_card_get_elem_id_list(ALSACtlCard *self, GList **entries,
                                   GError **error);

void alsactl_card_lock_elem(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                            gboolean lock, GError **error);

void alsactl_card_get_elem_info(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                ALSACtlElemInfo **elem_info, GError **error);

void alsactl_card_write_elem_tlv(ALSACtlCard *self,
                            const ALSACtlElemId *elem_id,
                            const gint32 *container, gsize container_count,
                            GError **error);
void alsactl_card_read_elem_tlv(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                            gint32 *const *container, gsize *container_count,
                            GError **error);
void alsactl_card_command_elem_tlv(ALSACtlCard *self,
                            const ALSACtlElemId *elem_id,
                            gint32 *const *container, gsize *container_count,
                            GError **error);

void alsactl_card_add_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                            guint elem_count, ALSACtlElemInfo *elem_info,
                            GList **entries, GError **error);
void alsactl_card_replace_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                            guint elem_count, ALSACtlElemInfo *elem_info,
                            GList **entries, GError **error);
void alsactl_card_remove_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                               GError **error);

void alsactl_card_write_elem_value(ALSACtlCard *self,
                                   const ALSACtlElemId *elem_id,
                                   const ALSACtlElemValue *elem_value,
                                   GError **error);
void alsactl_card_read_elem_value(ALSACtlCard *self,
                                  const ALSACtlElemId *elem_id,
                                  ALSACtlElemValue *const *elem_value,
                                  GError **error);

void alsactl_card_create_source(ALSACtlCard *self, GSource **gsrc,
                                GError **error);

G_END_DECLS

#endif
