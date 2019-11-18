// SPDX-License-Identifier: LGPL-3.0-or-later
#include "substream-params.h"

G_DEFINE_TYPE(ALSARawmidiSubstreamParams, alsarawmidi_substream_params, G_TYPE_OBJECT)

static void alsarawmidi_substream_params_class_init(ALSARawmidiSubstreamParamsClass *klass)
{
    return;
}

static void alsarawmidi_substream_params_init(ALSARawmidiSubstreamParams *self)
{
    return;
}

/**
 * alsarawmidi_substream_params_new:
 *
 * Allocate and return an instance of ALSARawmidiSubstreamParams class.
 */
ALSARawmidiSubstreamParams *alsarawmidi_substream_params_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_PARAMS, NULL);
}
