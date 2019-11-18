// SPDX-License-Identifier: LGPL-3.0-or-later
#include "substream-status.h"

G_DEFINE_TYPE(ALSARawmidiSubstreamStatus, alsarawmidi_substream_status, G_TYPE_OBJECT)

static void alsarawmidi_substream_status_class_init(ALSARawmidiSubstreamStatusClass *klass)
{
    return;
}

static void alsarawmidi_substream_status_init(ALSARawmidiSubstreamStatus *self)
{
    return;
}

/**
 * alsarawmidi_substream_status_new:
 *
 * Allocate and return an instance of ALSARawmidiSubstreamStatus class.
 */
ALSARawmidiSubstreamStatus *alsarawmidi_substream_status_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_STATUS, NULL);
}
