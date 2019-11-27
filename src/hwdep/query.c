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

#include <libudev.h>

// For error handling.
G_DEFINE_QUARK("alsahwdep-error", alsahwdep_error)

// 'C' is required apart from emulation of Open Sound System.
#define PREFIX_SYSNAME_TEMPLATE     "hwC%u"
#define HWDEP_SYSNAME_TEMPLATE      "hwC%uD%u"

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
 * alsahwdep_get_device_id_list:
 * @card_id: The numerical ID of sound card.
 * @entries: (array length=entry_count)(out): The list of numerical ID for
 *           hwdep device.
 * @entry_count: The number of entries.
 * @error: A #GError.
 *
 * Get the list of numerical ID for available hwdep devices of sound card.
 */
void alsahwdep_get_device_id_list(guint card_id, guint **entries,
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

    prepare_udev_enum(&enumerator, error);
    if (*error != NULL)
        goto end;

    length = strlen(PREFIX_SYSNAME_TEMPLATE) + calculate_digits(card_id) + 1;
    prefix = g_try_malloc0(length);
    if (prefix == NULL) {
        generate_error(error, ENOMEM);
        goto end;
    }
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

    *entries = g_try_malloc0_n(count, sizeof(**entries));
    if (*entries == NULL) {
        generate_error(error, ENOMEM);
        goto end;
    }

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
        generate_error(error, ENOENT);
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
 * alsahwdep_get_hwdep_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @device_id: The numerical ID of hwdep device for the sound card.
 * @sysname: (out): The string for sysname of hwdep device.
 * @error: A #GError.
 *
 * Allocate sysname for hwdep device and return it when it exists.
 */
void alsahwdep_get_hwdep_sysname(guint card_id, guint device_id,
                                 char **sysname, GError **error)
{
    unsigned int length;
    char *name;
    struct udev *ctx;
    struct udev_device *dev;

    length = strlen(HWDEP_SYSNAME_TEMPLATE) + calculate_digits(card_id) +
             calculate_digits(device_id) + 1;
    name = g_try_malloc0(length);
    if (name == NULL) {
        generate_error(error, ENOMEM);
        return;
    }
    snprintf(name, length, HWDEP_SYSNAME_TEMPLATE, card_id, device_id);

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
