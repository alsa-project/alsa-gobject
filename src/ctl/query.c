// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

#include <libudev.h>

/**
 * SECTION: query
 * @Title: Global functions in ALSACtl
 * @Short_description: Global functions available without holding any file
 *                     descriptor
 */

#define generate_file_error(exception, errno, msg) \
        g_set_error_literal(exception, G_FILE_ERROR, g_file_error_from_errno(errno), msg)

static void prepare_udev_enum(struct udev_enumerate **enumerator,
                              GError **error)
{
    struct udev *ctx;
    int err;

    ctx = udev_new();
    if (ctx == NULL) {
        generate_file_error(error, errno, "udev_new()");
        return;
    }

    *enumerator = udev_enumerate_new(ctx);
    if (*enumerator == NULL) {
        generate_file_error(error, errno, "udev_enumerate_new()");
        udev_unref(ctx);
        return;
    }

    err = udev_enumerate_add_match_subsystem(*enumerator, "sound");
    if (err < 0) {
        generate_file_error(error, -err, "udev_enumerate_add_match_subsystem()");
        udev_enumerate_unref(*enumerator);
        udev_unref(ctx);
        return;
    }

    err = udev_enumerate_scan_devices(*enumerator);
    if (err < 0) {
        generate_file_error(error, -err, "udev_enumerate_scan_devices()");
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

/**
 * alsactl_get_card_id_list:
 * @entries: (array length=entry_count)(out): The list of numerical ID for sound
 *           cards.
 * @entry_count: The number of entries.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Get the list of numerical ID for available sound cards.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_card_id_list(guint **entries, gsize *entry_count,
                              GError **error)
{
    struct udev_enumerate *enumerator = NULL;
    struct udev_list_entry *entry, *entry_list;
    unsigned int count;
    unsigned int index;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    prepare_udev_enum(&enumerator, error);
    if (*error != NULL)
        return;

    entry_list = udev_enumerate_get_list_entry(enumerator);

    count = 0;
    udev_list_entry_foreach(entry, entry_list) {
        struct udev_device *dev = detect_dev(enumerator, entry, "card");
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
        struct udev_device *dev = detect_dev(enumerator, entry, "card");
        if (dev != NULL) {
            const char *sysnum = udev_device_get_sysnum(dev);
            long val;

            if (!long_from_string(sysnum, &val)) {
                (*entries)[index] = val;
                ++index;
            }
            udev_device_unref(dev);
        }
    }

    g_warn_if_fail(index == count);

    *entry_count = count;

    qsort(*entries, count, sizeof(guint), compare_guint);
end:
    release_udev_enum(enumerator);
}

/**
 * alsactl_get_card_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @sysname: (out): The string for sysname of the sound card.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate sysname for the sound card and return it when it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_card_sysname(guint card_id, char **sysname, GError **error)
{
    int err;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_card_sysname(sysname, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate card sysname");
}

/**
 * alsactl_get_control_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @sysname: (out): The string for sysname of control device for the sound card.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate sysname of control device for the sound card and return it when
 * it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_control_sysname(guint card_id, char **sysname, GError **error)
{
    int err;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_control_sysname(sysname, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate control sysname");
}

/**
 * alsactl_get_control_devnode:
 * @card_id: The numerical ID of sound card.
 * @devnode: (out): The string for devnode of control device for the sound card.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate string of devnode for control device of the sound card and return it
 * if exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_control_devnode(guint card_id, char **devnode, GError **error)
{
    int err;

    g_return_if_fail(devnode != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_control_devname(devnode, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate control devname");
}
