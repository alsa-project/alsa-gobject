// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/ioctl.h>

/**
 * alsahwdep_get_device_id_list:
 * @card_id: The numeric ID of sound card.
 * @entries: (array length=entry_count)(out): The list of numeric ID for hwdep device.
 * @entry_count: The number of entries.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the list of numeric ID for available hwdep devices of sound card.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsahwdep_get_device_id_list(guint card_id, guint **entries,
                                  gsize *entry_count, GError **error)
{
    int err;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = generate_hwdep_sysnum_list(entries, entry_count, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate list of hwdep sysnum");
}

/**
 * alsahwdep_get_hwdep_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @device_id: The numeric ID of hwdep device for the sound card.
 * @sysname: (out): The string for sysname of hwdep device.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Allocate sysname for hwdep device and return it when it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsahwdep_get_hwdep_sysname(guint card_id, guint device_id,
                                 char **sysname, GError **error)
{
    int err;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_hwdep_sysname(sysname, card_id, device_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate hwdep sysname");
}

/**
 * alsahwdep_get_hwdep_devnode:
 * @card_id: The numeridcal ID of sound card.
 * @device_id: The numeric ID of hwdep device for the sound card.
 * @devnode: (out): The string for devnode of hwdep device.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Allocate devnode string for hwdep device and return it when exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsahwdep_get_hwdep_devnode(guint card_id, guint device_id,
                                 char **devnode, GError **error)
{
    int err;

    g_return_if_fail(devnode != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_hwdep_devname(devnode, card_id, device_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate hwdep devname");
}

/**
 * alsahwdep_get_device_info:
 * @card_id: The numeric value for sound card to query.
 * @device_id: The numeric value of hwdep device to query.
 * @device_info: (out): The information of the device.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the information according to given numeric IDs for card and device.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system call
 * with `SNDRV_CTL_IOCTL_HWDEP_INFO` command for ALSA control character device.
 */
void alsahwdep_get_device_info(guint card_id, guint device_id,
                               ALSAHwdepDeviceInfo **device_info,
                               GError **error)
{
    struct snd_hwdep_info *info;
    int err;

    g_return_if_fail(device_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    *device_info = g_object_new(ALSAHWDEP_TYPE_DEVICE_INFO, NULL);
    hwdep_device_info_refer_private(*device_info, &info);

    info->device = device_id;
    info->card = card_id;
    err = request_ctl_ioctl(card_id, SNDRV_CTL_IOCTL_HWDEP_INFO, info);
    if (err < 0) {
        generate_file_error(error, -err, "ioctl(HWDEP_INFO)");
        g_object_unref(*device_info);
    }
}
