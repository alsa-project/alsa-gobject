// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_CARD_H__
#define __ALSA_GOBJECT_ALSACTL_CARD_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_CARD   (alsactl_card_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlCard, alsactl_card, ALSACTL, CARD, GObject);

#define ALSACTL_CARD_ERROR  alsactl_card_error_quark()

GQuark alsactl_card_error_quark();

struct _ALSACtlCardClass {
    GObjectClass parent_class;

    /**
     * ALSACtlCardClass::handle_elem_event:
     * @self: A [class@Card].
     * @elem_id: (transfer none): A [struct@ElemId].
     * @events: A set of [flags@ElemEventMask].
     *
     * Class closure for the [signal@Card::handle-elem-event] signal.
     */
    void (*handle_elem_event)(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                              ALSACtlElemEventMask events);

    /**
     * ALSACtlCardClass::handle_disconnection:
     * @self: A [class@Card].
     *
     * Class closure for the [signal@Card::handle-disconnection] signal.
     */
    void (*handle_disconnection)(ALSACtlCard *self);
};

ALSACtlCard *alsactl_card_new();

gboolean alsactl_card_open(ALSACtlCard *self, guint card_id, gint open_flag, GError **error);

gboolean alsactl_card_get_protocol_version(ALSACtlCard *self, const guint16 *proto_ver_triplet[3],
                                           GError **error);

gboolean alsactl_card_get_info(ALSACtlCard *self, ALSACtlCardInfo **card_info, GError **error);

gboolean alsactl_card_get_elem_id_list(ALSACtlCard *self, GList **entries, GError **error);

gboolean alsactl_card_lock_elem(ALSACtlCard *self, const ALSACtlElemId *elem_id, gboolean lock,
                                GError **error);

gboolean alsactl_card_get_elem_info(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                    ALSACtlElemInfoCommon **elem_info, GError **error);

gboolean alsactl_card_write_elem_tlv(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                     const guint32 *container, gsize container_count,
                                     GError **error);
gboolean alsactl_card_read_elem_tlv(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                    guint32 *const *container, gsize *container_count,
                                    GError **error);
gboolean alsactl_card_command_elem_tlv(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                       guint32 *const *container, gsize *container_count,
                                       GError **error);

gboolean alsactl_card_add_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id, guint elem_count,
                                ALSACtlElemInfoCommon *elem_info, GList **entries, GError **error);
gboolean alsactl_card_replace_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id, guint elem_count,
                                    ALSACtlElemInfoCommon *elem_info, GList **entries, GError **error);
gboolean alsactl_card_remove_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id, GError **error);

gboolean alsactl_card_write_elem_value(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                       const ALSACtlElemValue *elem_value, GError **error);
gboolean alsactl_card_read_elem_value(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                      ALSACtlElemValue *const *elem_value, GError **error);

gboolean alsactl_card_create_source(ALSACtlCard *self, GSource **gsrc, GError **error);

G_END_DECLS

#endif
