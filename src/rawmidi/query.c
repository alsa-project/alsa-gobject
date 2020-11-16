// SPDX-License-Identifier: LGPL-3.0-or-later
#include "query.h"
#include "privates.h"

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <libudev.h>

/**
 * SECTION: query
 * @Title: Global functions in ALSARawmidi
 * @Short_description: Global functions available without holding any file
 *                     descriptor
 */

// For error reporting.
G_DEFINE_QUARK("alsarawmidi-error", alsarawmidi_error)

// 'C' is required apart from emulation of Open Sound System.
#define PREFIX_SYSNAME_TEMPLATE     "midiC%u"
#define RAWMIDI_SYSNAME_TEMPLATE    "midiC%uD%u"
#define CTL_SYSNAME_TEMPLATE        "controlC%u"

static void prepare_udev_enum(struct udev_enumerate **enumerator, GError **error)
{
    struct udev *ctx;
    int err;

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        return;
    }

    *enumerator = udev_enumerate_new(ctx);
    if (*enumerator == NULL) {
        generate_error(error, errno);
        udev_unref(ctx);
        return;
    }

    err = udev_enumerate_add_match_subsystem(*enumerator, "sound");
    if (err < 0) {
        generate_error(error, -err);
        udev_enumerate_unref(*enumerator);
        udev_unref(ctx);
        return;
    }

    err = udev_enumerate_scan_devices(*enumerator);
    if (err < 0) {
        generate_error(error, -err);
        udev_enumerate_unref(*enumerator);
        udev_unref(ctx);
    }
}

static struct udev_device *detect_dev(struct udev_enumerate *enumerator,
                                      struct udev_list_entry *entry,
                                      const char *prefix)
{
    struct udev *ctx = udev_enumerate_get_udev(enumerator);
    const char *syspath;
    struct udev_device *dev;
    const char *sysname;

    syspath = udev_list_entry_get_name(entry);
    if (syspath == NULL)
        return NULL;

    dev = udev_device_new_from_syspath(ctx, syspath);
    if (dev == NULL)
        return NULL;

    sysname = udev_device_get_sysname(dev);
    if (sysname == NULL) {
        udev_device_unref(dev);
        return NULL;
    }

    if (strstr(sysname, prefix) != sysname) {
        udev_device_unref(dev);
        return NULL;
    }

    return dev;
}

static void release_udev_enum(struct udev_enumerate *enumerator)
{
    struct udev *ctx = udev_enumerate_get_udev(enumerator);

    udev_enumerate_unref(enumerator);
    udev_unref(ctx);
}

static int compare_guint(const void *l, const void *r)
{
    const guint *x = l;
    const guint *y = r;

    return *x > *y;
}

static unsigned int calculate_digits(unsigned int number)
{
    unsigned int digits;

    while (true) {
        number /= 10;
        ++digits;
        if (number == 0)
            break;
    }

    return digits;
}

/**
 * alsarawmidi_get_device_id_list:
 * @card_id: The numerical ID of sound card.
 * @entries: (array length=entry_count)(out): The list of numerical ID for
 *           rawmidi device.
 * @entry_count: The number of entries.
 * @error: A #GError.
 *
 * Get the list of numerical ID for available rawmidi devices of sound card.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsarawmidi_get_device_id_list(guint card_id, guint **entries,
                                    gsize *entry_count, GError **error)
{
    struct udev_enumerate *enumerator;
    unsigned int length;
    char *prefix;
    struct udev_list_entry *entry, *entry_list;
    unsigned int count;
    unsigned int index;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    prepare_udev_enum(&enumerator, error);
    if (*error != NULL)
        goto end;

    length = strlen(PREFIX_SYSNAME_TEMPLATE) + calculate_digits(card_id) + 1;
    prefix = g_malloc0(length);

    snprintf(prefix, length, PREFIX_SYSNAME_TEMPLATE, card_id);

    entry_list = udev_enumerate_get_list_entry(enumerator);

    count = 0;
    udev_list_entry_foreach(entry, entry_list) {
        struct udev_device *dev = detect_dev(enumerator, entry, prefix);
        if (dev != NULL) {
            ++count;
            udev_device_unref(dev);
        }
    }

    // Nothing available.
    if (count == 0)
        goto end;

    *entries = g_malloc0_n(count, sizeof(**entries));

    index = 0;
    udev_list_entry_foreach(entry, entry_list) {
        struct udev_device *dev = detect_dev(enumerator, entry, prefix);
        if (dev != NULL) {
            const char *sysnum = udev_device_get_sysnum(dev);
            long val;
            char *endptr;

            errno = 0;
            val = strtol(sysnum, &endptr, 10);
            if (errno == 0 && *endptr == '\0' && val >= 0) {
                (*entries)[index] = (guint)val;
                ++index;
            }
            udev_device_unref(dev);
        }
    }
    if (index != count) {
        g_warn_if_reached();
        g_free(*entries);
        *entries = NULL;
        goto end;
    }
    *entry_count = count;

    qsort(*entries, count, sizeof(guint), compare_guint);
end:
    g_free(prefix);
    release_udev_enum(enumerator);
}

/**
 * alsarawmidi_get_rawmidi_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @device_id: The numerical ID of rawmidi device for the sound card.
 * @sysname: (out): The string for sysname of rawmidi device.
 * @error: A #GError.
 *
 * Allocate sysname for rawmidi device and return it when it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsarawmidi_get_rawmidi_sysname(guint card_id, guint device_id,
                                     char **sysname, GError **error)
{
    unsigned int length;
    char *name;
    struct udev *ctx;
    struct udev_device *dev;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    length = strlen(RAWMIDI_SYSNAME_TEMPLATE) + calculate_digits(card_id) +
             calculate_digits(device_id) + 1;
    name = g_malloc0(length);

    snprintf(name, length, RAWMIDI_SYSNAME_TEMPLATE, card_id, device_id);

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        g_free(name);
        return;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", name);
    if (dev == NULL) {
        generate_error(error, ENODEV);
        g_free(name);
    } else {
        *sysname = name;
        udev_device_unref(dev);
    }

    udev_unref(ctx);
}

/**
 * alsarawmidi_get_rawmidi_devnode:
 * @card_id: The numeridcal ID of sound card.
 * @device_id: The numerical ID of rawmidi device for the sound card.
 * @devnode: (out): The string for devnode of rawmidi device.
 * @error: A #GError.
 *
 * Allocate devnode string for rawmidi device and return it when exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsarawmidi_get_rawmidi_devnode(guint card_id, guint device_id,
                                     char **devnode, GError **error)
{
    unsigned int length;
    char *name;
    struct udev *ctx;
    struct udev_device *dev;
    const char *node;

    g_return_if_fail(devnode != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    length = strlen(RAWMIDI_SYSNAME_TEMPLATE) + calculate_digits(card_id) +
             calculate_digits(device_id) + 1;
    name = g_malloc0(length);

    snprintf(name, length, RAWMIDI_SYSNAME_TEMPLATE, card_id, device_id);

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        g_free(name);
        return;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", name);
    g_free(name);
    if (dev == NULL) {
        generate_error(error, ENODEV);
        udev_unref(ctx);
        return;
    }

    node = udev_device_get_devnode(dev);
    if (node == NULL)
        generate_error(error, ENOENT);
    else
        *devnode = g_strdup(node);

    udev_device_unref(dev);
    udev_unref(ctx);
}

static void rawmidi_perform_ctl_ioctl(guint card_id, long request, void *data,
                                      GError **error)
{
    unsigned int length;
    char *sysname;
    struct udev *ctx;
    struct udev_device *dev;
    const char *devnode;
    int fd;

    length = strlen(CTL_SYSNAME_TEMPLATE) + calculate_digits(card_id) + 1;
    sysname = g_malloc0(length);

    snprintf(sysname, length, CTL_SYSNAME_TEMPLATE, card_id);

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        goto err_sysname;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", sysname);
    if (dev == NULL) {
        generate_error(error, errno);
        goto err_ctx;
    }

    devnode = udev_device_get_devnode(dev);
    if (devnode == NULL) {
        generate_error(error, ENODEV);
        goto err_device;
    }

    fd = open(devnode, O_RDONLY);
    if (fd < 0) {
        generate_error(error, errno);
        goto err_device;
    }

    if (ioctl(fd, request, data) < 0)
        generate_error(error, errno);

    close(fd);
err_device:
    udev_device_unref(dev);
err_ctx:
    udev_unref(ctx);
err_sysname:
    g_free(sysname);
}

/**
 * alsarawmidi_get_subdevice_id_list:
 * @card: The numberical value for sound card to query.
 * @device: The numerical value of rawmidi device to query.
 * @direction: The direction of stream to query, one of
 *             ALSARawmidiStreamDirection.
 * @entries: (array length=entry_count)(out): The list of card.
 * @entry_count: The number of entries.
 * @error: A #GError.
 *
 * Get the list of numerical IDs for subdevices belongs to the numerical ID of
 * card, device, and the direction.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_CTL_IOCTL_RAWMIDI_INFO command for ALSA control character device.
 */
void alsarawmidi_get_subdevice_id_list(guint card, guint device,
                                       ALSARawmidiStreamDirection direction,
                                       guint **entries, gsize *entry_count,
                                       GError **error)
{
    struct snd_rawmidi_info info = {
        .card = card,
        .device = device,
        .stream = direction,
        .subdevice = 0,
    };
    int i;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    rawmidi_perform_ctl_ioctl(card, SNDRV_CTL_IOCTL_RAWMIDI_INFO, &info, error);
    if (*error != NULL)
        return;

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
 * @error: A #GError.
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

    g_return_if_fail(substream_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    *substream_info = g_object_new(ALSARAWMIDI_TYPE_SUBSTREAM_INFO, NULL);

    rawmidi_substream_info_refer_private(*substream_info, &info);
    info->device = device_id;
    info->subdevice = subdevice_id;
    info->stream = direction;
    info->card = card_id;

    rawmidi_perform_ctl_ioctl(card_id, SNDRV_CTL_IOCTL_RAWMIDI_INFO, info, error);
    if (*error != NULL)
        g_object_unref(*substream_info);
}

void rawmidi_select_subdevice(guint card_id, guint subdevice_id, GError **error)
{
    guint data = subdevice_id;
    rawmidi_perform_ctl_ioctl(card_id, SNDRV_CTL_IOCTL_RAWMIDI_PREFER_SUBDEVICE,
                              &data, error);
}
