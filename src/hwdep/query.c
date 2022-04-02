// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/ioctl.h>

/**
 * SECTION: query
 * @Title: Global functions in ALSAHwdep
 * @Short_description: Global functions available without holding any file
 *                     descriptor
 */

/**
 * alsahwdep_get_device_id_list:
 * @card_id: The numerical ID of sound card.
 * @entries: (array length=entry_count)(out): The list of numerical ID for
 *           hwdep device.
 * @entry_count: The number of entries.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Get the list of numerical ID for available hwdep devices of sound card.
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
 * @device_id: The numerical ID of hwdep device for the sound card.
 * @sysname: (out): The string for sysname of hwdep device.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
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
 * @device_id: The numerical ID of hwdep device for the sound card.
 * @devnode: (out): The string for devnode of hwdep device.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
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
 * @card_id: The numberical value for sound card to query.
 * @device_id: The numerical value of hwdep device to query.
 * @device_info: (out): The information of the device.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Get the information according to given numerical IDs for card and device.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_CTL_IOCTL_HWDEP_INFO command for ALSA control character device.
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
