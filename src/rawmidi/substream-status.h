// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_STATUS_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_STATUS_H__

#include <alsarawmidi.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_SUBSTREAM_STATUS   (alsarawmidi_substream_status_get_type())

#define ALSARAWMIDI_SUBSTREAM_STATUS(obj)                           \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                              \
                                ALSARAWMIDI_TYPE_SUBSTREAM_STATUS,  \
                                ALSARawmidiSubstreamStatus))
#define ALSARAWMIDI_IS_SUBSTREAM_STATUS(obj)                        \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                              \
                                ALSARAWMIDI_TYPE_SUBSTREAM_STATUS))

#define ALSARAWMIDI_SUBSTREAM_STATUS_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_CAST((klass),                               \
                             ALSARAWMIDI_TYPE_SUBSTREAM_STATUS,     \
                             ALSARawmidiSubstreamStatusClass))
#define ALSARAWMIDI_IS_SUBSTREAM_STATUS_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                               \
                             ALSARAWMIDI_TYPE_SUBSTREAM_STATUS))
#define ALSARAWMIDI_SUBSTREAM_STATUS_GET_CLASS(obj)                 \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                               \
                               ALSARAWMIDI_TYPE_SUBSTREAM_STATUS,   \
                               ALSARawmidiSubstreamStatusClass))

typedef struct _ALSARawmidiSubstreamStatus          ALSARawmidiSubstreamStatus;
typedef struct _ALSARawmidiSubstreamStatusClass     ALSARawmidiSubstreamStatusClass;
typedef struct _ALSARawmidiSubstreamStatusPrivate   ALSARawmidiSubstreamStatusPrivate;

struct _ALSARawmidiSubstreamStatus {
    GObject parent_instance;

    ALSARawmidiSubstreamStatusPrivate *priv;
};

struct _ALSARawmidiSubstreamStatusClass {
    GObjectClass parent_class;
};

GType alsarawmidi_substream_status_get_type() G_GNUC_CONST;

ALSARawmidiSubstreamStatus *alsarawmidi_substream_status_new();

G_END_DECLS

#endif
