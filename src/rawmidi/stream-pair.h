// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_STREAM_PAIR__H__
#define __ALSA_GOBJECT_ALSARAWMIDI_STREAM_PAIR__H__

#include <glib.h>
#include <glib-object.h>

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

struct _ALSARawmidiStreamPair {
    GObject parent_instance;
};

struct _ALSARawmidiStreamPairClass {
    GObjectClass parent_class;
};

GType alsarawmidi_stream_pair_get_type(void) G_GNUC_CONST;

ALSARawmidiStreamPair *alsarawmidi_stream_pair_new();

G_END_DECLS

#endif
