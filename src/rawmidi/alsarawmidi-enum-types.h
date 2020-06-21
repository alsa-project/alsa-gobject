// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_ENUM_TYPES__H__
#define __ALSA_GOBJECT_ALSARAWMIDI_ENUM_TYPES__H__

#include <sound/asound.h>

/**
 * ALSARawmidiStreamDirection:
 * @ALSARAWMIDI_STREAM_DIRECTION_OUTPUT:        Output direction of stream.
 * @ALSARAWMIDI_STREAM_DIRECTION_INPUT:         Input direction of stream.
 *
 * A set of enumerators for the direction of stream.
 */
typedef enum {
    ALSARAWMIDI_STREAM_DIRECTION_OUTPUT = SNDRV_RAWMIDI_STREAM_OUTPUT,
    ALSARAWMIDI_STREAM_DIRECTION_INPUT = SNDRV_RAWMIDI_STREAM_INPUT,
} ALSARawmidiStreamDirection;

/**
 * ALSARawmidiStreamPairInfoFlag:
 * @ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT:   The pair of stream supports output substream.
 * @ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT:    The pair of stream supports input substream.
 * @ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_DUPLEX:   Both directions of stream are available at the same time.
 *
 * A set of flags for information of the pair of streams.
 */
typedef enum /*< flags >*/
{
    ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT = SNDRV_RAWMIDI_INFO_OUTPUT,
    ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT  = SNDRV_RAWMIDI_INFO_INPUT,
    ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_DUPLEX = SNDRV_RAWMIDI_INFO_DUPLEX,
} ALSARawmidiStreamPairInfoFlag;

#endif
