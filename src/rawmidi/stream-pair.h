// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_STREAM_PAIR_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_STREAM_PAIR_H__

#include <alsarawmidi.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_STREAM_PAIR    (alsarawmidi_stream_pair_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSARawmidiStreamPair, alsarawmidi_stream_pair, ALSARAWMIDI, STREAM_PAIR, GObject);

#define ALSARAWMIDI_STREAM_PAIR_ERROR           alsarawmidi_stream_pair_error_quark()

GQuark alsarawmidi_stream_pair_error_quark();

struct _ALSARawmidiStreamPairClass {
    GObjectClass parent_class;

    /**
     * ALSARawmidiStreamPairClass::handle_messages:
     * @self: A #ALSARawmidiStreamPair.
     *
     * When any input message in available, this event is emit.
     */
    void (*handle_messages)(ALSARawmidiStreamPair *self);

    /**
     * ALSARawmidiStreamPairClass::handle_disconnection:
     * @self: A #ALSARawmidiStreamPair.
     *
     * When the sound card is not available anymore due to unbinding driver or
     * hot unplugging, this signal is emit. The owner of this object should
     * call g_object_free() as quickly as possible to release ALSA rawmidi
     * character device.
     */
    void (*handle_disconnection)(ALSARawmidiStreamPair *self);
};

ALSARawmidiStreamPair *alsarawmidi_stream_pair_new();

void alsarawmidi_stream_pair_open(ALSARawmidiStreamPair *self, guint card_id,
                                  guint device_id, guint subdevice_id,
                                  ALSARawmidiStreamPairInfoFlag access_modes,
                                  gint open_flag, GError **error);

void alsarawmidi_stream_pair_get_protocol_version(ALSARawmidiStreamPair *self,
                                       const guint16 *proto_ver_triplet[3],
                                       GError **error);

void alsarawmidi_stream_pair_get_substream_info(ALSARawmidiStreamPair *self,
                                ALSARawmidiStreamDirection direction,
                                ALSARawmidiSubstreamInfo **substream_info,
                                GError **error);

void alsarawmidi_stream_pair_set_substream_params(ALSARawmidiStreamPair *self,
                                ALSARawmidiStreamDirection direction,
                                ALSARawmidiSubstreamParams *substream_params,
                                GError **error);

void alsarawmidi_stream_pair_get_substream_status(ALSARawmidiStreamPair *self,
                            ALSARawmidiStreamDirection direction,
                            ALSARawmidiSubstreamStatus *const *substream_status,
                            GError **error);

void alsarawmidi_stream_pair_read_from_substream(ALSARawmidiStreamPair *self,
                                        guint8 *const *buf, gsize *buf_size,
                                        GError **error);
void alsarawmidi_stream_pair_write_to_substream(ALSARawmidiStreamPair *self,
                                        const guint8 *buf, gsize buf_size,
                                        GError **error);

void alsarawmidi_stream_pair_drain_substream(ALSARawmidiStreamPair *self,
                                        ALSARawmidiStreamDirection direction,
                                        GError **error);

void alsarawmidi_stream_pair_drop_substream(ALSARawmidiStreamPair *self,
                                        ALSARawmidiStreamDirection direction,
                                        GError **error);

void alsarawmidi_stream_pair_create_source(ALSARawmidiStreamPair *self,
                                           GSource **gsrc, GError **error);

G_END_DECLS

#endif
