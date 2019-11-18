// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_STREAM_PAIR__H__
#define __ALSA_GOBJECT_ALSARAWMIDI_STREAM_PAIR__H__

#include <glib.h>
#include <glib-object.h>

#include <rawmidi/alsarawmidi-enums.h>
#include <rawmidi/substream-info.h>
#include <rawmidi/substream-params.h>
#include <rawmidi/substream-status.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_STREAM_PAIR    (alsarawmidi_stream_pair_get_type())

#define ALSARAWMIDI_STREAM_PAIR(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                ALSARAWMIDI_TYPE_STREAM_PAIR,   \
                                ALSARawmidiStreamPair))
#define ALSARAWMIDI_IS_STREAM_PAIR(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                ALSARAWMIDI_TYPE_STREAM_PAIR))

#define ALSARAWMIDI_STREAM_PAIR_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             ALSARAWMIDI_TYPE_STREAM_PAIR,      \
                             ALSARawmidiStreamPairClass))
#define ALSARAWMIDI_IS_STREAM_PAIR_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             ALSARAWMIDI_TYPE_STREAM_PAIR))
#define ALSARAWMIDI_STREAM_PAIR_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               ALSARAWMIDI_TYPE_STREAM_PAIR,    \
                               ALSARawmidiStreamPairClass))

typedef struct _ALSARawmidiStreamPair           ALSARawmidiStreamPair;
typedef struct _ALSARawmidiStreamPairClass      ALSARawmidiStreamPairClass;
typedef struct _ALSARawmidiStreamPairPrivate    ALSARawmidiStreamPairPrivate;

struct _ALSARawmidiStreamPair {
    GObject parent_instance;

    ALSARawmidiStreamPairPrivate *priv;
};

struct _ALSARawmidiStreamPairClass {
    GObjectClass parent_class;
};

GType alsarawmidi_stream_pair_get_type(void) G_GNUC_CONST;

ALSARawmidiStreamPair *alsarawmidi_stream_pair_new();

void alsarawmidi_stream_pair_open(ALSARawmidiStreamPair *self, guint card_id,
                                  guint device_id, guint subdevice_id,
                                  ALSARawmidiStreamPairInfoFlag access_modes,
				  gint open_flag, GError **error);

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

G_END_DECLS

#endif
