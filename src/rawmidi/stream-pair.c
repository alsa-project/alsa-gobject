// SPDX-License-Identifier: LGPL-3.0-or-later
#include "stream-pair.h"
#include "query.h"
#include "privates.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct _ALSARawmidiStreamPairPrivate {
    int fd;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSARawmidiStreamPair, alsarawmidi_stream_pair, G_TYPE_OBJECT)

static void rawmidi_stream_pair_finalize(GObject *obj)
{
    ALSARawmidiStreamPair *self = ALSARAWMIDI_STREAM_PAIR(obj);
    ALSARawmidiStreamPairPrivate *priv =
                            alsarawmidi_stream_pair_get_instance_private(self);

    if (priv->fd >= 0)
        close(priv->fd);

    G_OBJECT_CLASS(alsarawmidi_stream_pair_parent_class)->finalize(obj);
}

static void alsarawmidi_stream_pair_class_init(ALSARawmidiStreamPairClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = rawmidi_stream_pair_finalize;
}

static void alsarawmidi_stream_pair_init(ALSARawmidiStreamPair *self)
{
    ALSARawmidiStreamPairPrivate *priv =
                            alsarawmidi_stream_pair_get_instance_private(self);

    priv->fd = -1;
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

/**
 * alsarawmidi_stream_pair_open:
 * @self: A #ALSARawmidiStreamPair.
 * @card_id: The numerical ID of sound card.
 * @device_id: The numerical ID of rawmidi device for the sound card.
 * @subdevice_id: The numerical ID of subdevice for the rawmidi device.
 * @access_modes: Access flags for stream direction.
 * @open_flag: The flag of open(2) system call. O_RDWR, O_WRONLY and O_RDONLY
 *             are forced to fulfil internally according to the access_modes.
 * @error: A #GError.
 *
 * Open file descriptor for a pair of streams to attach input/output substreams
 * corresponding to the given subdevice.
 */
void alsarawmidi_stream_pair_open(ALSARawmidiStreamPair *self, guint card_id,
                                  guint device_id, guint subdevice_id,
                                  ALSARawmidiStreamPairInfoFlag access_modes,
                                  gint open_flag, GError **error)
{
    ALSARawmidiStreamPairPrivate *priv;
    char *devnode;

    g_return_if_fail(ALSARAWMIDI_IS_STREAM_PAIR(self));
    priv = alsarawmidi_stream_pair_get_instance_private(self);

    // The flag is used to attach substreams for each direction.
    if (access_modes & ~(ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT |
                         ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT)) {
        generate_error(error, EINVAL);
        return;
    }

    if ((access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT) &&
        (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT)) {
        open_flag = O_RDWR;
    } else if (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_OUTPUT) {
        open_flag = O_WRONLY;
    } else if (access_modes & ALSARAWMIDI_STREAM_PAIR_INFO_FLAG_INPUT) {
        open_flag = O_RDONLY;
    } else {
        generate_error(error, EINVAL);
        return;
    }

    alsarawmidi_get_rawmidi_devnode(card_id, device_id, &devnode, error);
    if (*error != NULL)
        return;

    rawmidi_select_subdevice(card_id, subdevice_id, error);
    if (*error != NULL) {
        g_free(devnode);
        return;
    }

    priv->fd = open(devnode, open_flag);
    if (priv->fd < 0)
        generate_error(error, errno);

    g_free(devnode);
}
