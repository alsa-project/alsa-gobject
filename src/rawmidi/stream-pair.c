// SPDX-License-Identifier: LGPL-3.0-or-later
#include "stream-pair.h"

G_DEFINE_TYPE(ALSARawmidiStreamPair, alsarawmidi_stream_pair, G_TYPE_OBJECT)

static void alsarawmidi_stream_pair_class_init(ALSARawmidiStreamPairClass *klass)
{
    return;
}

static void alsarawmidi_stream_pair_init(ALSARawmidiStreamPair *self)
{
    return;
}

/**
 * alsarawmidi_stream_pair_new:
 *
 * Allocate and return an instance of ALSARawmidiStreamPair class.
 */
ALSARawmidiStreamPair *alsarawmidi_stream_pair_new()
{
    return g_object_new(ALSARAWMIDI_TYPE_STREAM_PAIR, NULL);
}
