// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_PARAMS_H__
#define __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_PARAMS_H__

#include <alsarawmidi.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS (alsarawmidi_substream_params_get_type())

#define ALSARAWMIDI_SUBSTREAM_PARAMS(obj)                           \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                              \
                                ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS,  \
                                ALSARawmidiSubstreamParams))
#define ALSARAWMIDI_IS_SUBSTREAM_PARAMS(obj)                        \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                              \
                                ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS))

#define ALSARAWMIDI_SUBSTREAM_PARAMS_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_CAST((klass),                               \
                             ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS,     \
                             ALSARawmidiSubstreamParamsClass))
#define ALSARAWMIDI_IS_SUBSTREAM_PARAMS_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                               \
                             ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS))
#define ALSARAWMIDI_SUBSTREAM_PARAMS_GET_CLASS(obj)                 \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                               \
                               ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS,   \
                               ALSARawmidiSubstreamParamsClass))

typedef struct _ALSARawmidiSubstreamParams          ALSARawmidiSubstreamParams;
typedef struct _ALSARawmidiSubstreamParamsClass     ALSARawmidiSubstreamParamsClass;
typedef struct _ALSARawmidiSubstreamParamsPrivate   ALSARawmidiSubstreamParamsPrivate;

struct _ALSARawmidiSubstreamParams {
    GObject parent_instance;

    ALSARawmidiSubstreamParamsPrivate *priv;
};

struct _ALSARawmidiSubstreamParamsClass {
    GObjectClass parent_class;
};

GType alsarawmidi_substream_params_get_type(void) G_GNUC_CONST;

ALSARawmidiSubstreamParams *alsarawmidi_substream_params_new();

G_END_DECLS

#endif
