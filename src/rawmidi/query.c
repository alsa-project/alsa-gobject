// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/ioctl.h>

/**
 * SECTION: query
 * @Title: Global functions in ALSARawmidi
 * @Short_description: Global functions available without holding any file
 *                     descriptor
 */

/**
 * alsarawmidi_get_device_id_list:
 * @card_id: The numerical ID of sound card.
 * @entries: (array length=entry_count)(out): The list of numerical ID for
 *           rawmidi device.
 * @entry_count: The number of entries.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Get the list of numerical ID for available rawmidi devices of sound card.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsarawmidi_get_device_id_list(guint card_id, guint **entries,
                                    gsize *entry_count, GError **error)
{
    int err;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = generate_rawmidi_sysnum_list(entries, entry_count, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate list of rawmidi sysnum");
}

/**
 * alsarawmidi_get_rawmidi_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @device_id: The numerical ID of rawmidi device for the sound card.
 * @sysname: (out): The string for sysname of rawmidi device.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate sysname for rawmidi device and return it when it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsarawmidi_get_rawmidi_sysname(guint card_id, guint device_id,
                                     char **sysname, GError **error)
{
    int err;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_rawmidi_sysname(sysname, card_id, device_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate rawmidi sysname");
}

/**
 * alsarawmidi_get_rawmidi_devnode:
 * @card_id: The numeridcal ID of sound card.
 * @device_id: The numerical ID of rawmidi device for the sound card.
 * @devnode: (out): The string for devnode of rawmidi device.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate devnode string for rawmidi device and return it when exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsarawmidi_get_rawmidi_devnode(guint card_id, guint device_id,
                                     char **devnode, GError **error)
{
    int err;

    g_return_if_fail(devnode != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_rawmidi_devname(devnode, card_id, device_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate rawmidi devname");
}

/**
 * alsarawmidi_get_subdevice_id_list:
 * @card_id: The numberical value for sound card to query.
 * @device_id: The numerical value of rawmidi device to query.
 * @direction: The direction of stream to query, one of
 *             ALSARawmidiStreamDirection.
 * @entries: (array length=entry_count)(out): The list of card.
 * @entry_count: The number of entries.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Get the list of numerical IDs for subdevices belongs to the numerical ID of
 * card, device, and the direction.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_CTL_IOCTL_RAWMIDI_INFO command for ALSA control character device.
 */
void alsarawmidi_get_subdevice_id_list(guint card_id, guint device_id,
                                       ALSARawmidiStreamDirection direction,
                                       guint **entries, gsize *entry_count,
                                       GError **error)
{
    struct snd_rawmidi_info info = {
        .card = card_id,
        .device = device_id,
        .stream = direction,
        .subdevice = 0,
    };
    int i;
    int err;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = request_ctl_ioctl(card_id, SNDRV_CTL_IOCTL_RAWMIDI_INFO, &info);
    if (err < 0) {
        generate_file_error(error, -err, "RAWMIDI_INFO");
        return;
    }

    *entries = g_malloc0_n(info.subdevices_count, sizeof(guint));

    for (i = 0; i < info.subdevices_count; ++i)
        (*entries)[i] = i;
    *entry_count = info.subdevices_count;
}

/**
 * alsarawmidi_get_substream_info:
 * @card_id: The numberical value for sound card to query.
 * @device_id: The numerical value of rawmidi device to query.
 * @direction: The direction of stream, one of ALSARawmidiStreamDirection.
 * @subdevice_id: The numerical value of subdevice in rawmidi device.
 * @substream_info: (out): The information of substream for the subdevice.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Get the information of substream pointed by the numerical ID of card, device,
 * subdevice, and the direction.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_CTL_IOCTL_RAWMIDI_INFO command for ALSA control character device.
 */
void alsarawmidi_get_substream_info(guint card_id, guint device_id,
                                    ALSARawmidiStreamDirection direction,
                                    guint subdevice_id,
                                    ALSARawmidiSubstreamInfo **substream_info,
                                    GError **error)
{
    struct snd_rawmidi_info *info;
    int err;

    g_return_if_fail(substream_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    *substream_info = g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_INFO, NULL);

    rawmidi_substream_info_refer_private(*substream_info, &info);
    info->device = device_id;
    info->subdevice = subdevice_id;
    info->stream = direction;
    info->card = card_id;

    err = request_ctl_ioctl(card_id, SNDRV_CTL_IOCTL_RAWMIDI_INFO, info);
    if (err < 0) {
        g_object_unref(*substream_info);
        generate_file_error(error, -err, "RAWMIDI_INFO");
    }
}

void rawmidi_select_subdevice(guint card_id, guint subdevice_id, int *ctl_fd, GError **error)
{
    guint data = subdevice_id;
    int err;

    err = request_ctl_ioctl_opened(ctl_fd, card_id, SNDRV_CTL_IOCTL_RAWMIDI_PREFER_SUBDEVICE, &data);
    if (err < 0)
        generate_file_error(error, -err, "RAWMIDI_PREFER_SUBDEVICE");
}
