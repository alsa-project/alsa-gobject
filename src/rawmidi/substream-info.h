// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_INFO__H__
#define __ALSA_GOBJECT_ALSARAWMIDI_SUBSTREAM_INFO__H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ALSARAWMIDI_TYPE_SUBSTREAM_INFO (alsarawmidi_substream_info_get_type())

#define ALSARAWMIDI_SUBSTREAM_INFO(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                              \
                                ALSARAWMIDI_TYPE_SUBSTREAM_INFO,    \
                                ALSARawmidiSubstreamInfo))
#define ALSARAWMIDI_IS_SUBSTREAM_INFO(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                              \
                                ALSARAWMIDI_TYPE_SUBSTREAM_INFO))

#define ALSARAWMIDI_SUBSTREAM_INFO_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                               \
                             ALSARAWMIDI_TYPE_SUBSTREAM_INFO,       \
                             ALSARawmidiSubstreamInfoClass))
#define ALSARAWMIDI_IS_SUBSTREAM_INFO_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                               \
                             ALSARAWMIDI_TYPE_SUBSTREAM_INFO))
#define ALSARAWMIDI_SUBSTREAM_INFO_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                               \
                               ALSARAWMIDI_TYPE_SUBSTREAM_INFO,     \
                               ALSARawmidiSubstreamInfoClass))

typedef struct _ALSARawmidiSubstreamInfo        ALSARawmidiSubstreamInfo;
typedef struct _ALSARawmidiSubstreamInfoClass   ALSARawmidiSubstreamInfoClass;

struct _ALSARawmidiSubstreamInfo {
    GObject parent_instance;
};

struct _ALSARawmidiSubstreamInfoClass {
    GObjectClass parent_class;
};

GType alsarawmidi_substream_info_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
