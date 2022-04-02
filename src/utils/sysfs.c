// SPDX-License-Identifier: LGPL-3.0-or-later
#include "utils.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define SOUND_SUBSYSTEM     "sound"

int lookup_and_allocate_string_by_sysname(char **name, const char *sysname,
                                          const char *(*func)(struct udev_device *))
{
    struct udev *ctx;
    struct udev_device *device;
    const char *n;
    int err = 0;

    if (name == NULL || sysname == NULL || func == NULL)
        return -EINVAL;

    ctx = udev_new();
    if (ctx == NULL)
        return -errno;

    device = udev_device_new_from_subsystem_sysname(ctx, SOUND_SUBSYSTEM, sysname);
    if (device == NULL) {
        err = -errno;
        goto err_ctx;
    }

    n = func(device);
    if (n == NULL) {
        err = -errno;
        goto err_dev;
    }

    *name = strdup(n);
    if (*name == NULL)
        err = -ENOMEM;
err_dev:
    udev_device_unref(device);
err_ctx:
    udev_unref(ctx);
    return err;
}

static int detect_device(struct udev_device **device, struct udev *ctx,
                         struct udev_list_entry *entry, const char *prefix)
{
    const char *syspath;
    struct udev_device *dev;
    const char *sysname;

    syspath = udev_list_entry_get_name(entry);
    if (syspath == NULL)
        return -errno;

    dev = udev_device_new_from_syspath(ctx, syspath);
    if (dev == NULL)
        return -errno;

    sysname = udev_device_get_sysname(dev);
    if (sysname == NULL) {
        udev_device_unref(dev);
        return -errno;
    }

    if (strstr(sysname, prefix) != sysname) {
        udev_device_unref(dev);
        return -ENODEV;
    }

    *device = dev;

    return 0;
}

static int compare_u32(const void *l, const void *r)
{
    const unsigned int *x = l;
    const unsigned int *y = r;

    return *x > *y;
}

int generate_sysnum_list_by_sysname_prefix(unsigned int **entries, unsigned long *entry_count,
                                           const char *prefix)
{
    struct udev *ctx;
    struct udev_enumerate *enumerator;
    unsigned int count;
    struct udev_list_entry *entry, *entry_list;
    unsigned int index;
    int err;

    ctx = udev_new();
    if (ctx == NULL)
        return -errno;

    enumerator = udev_enumerate_new(ctx);
    if (enumerator == NULL) {
        err = -errno;
        goto err_ctx;
    }

    err = udev_enumerate_add_match_subsystem(enumerator, SOUND_SUBSYSTEM);
    if (err < 0) {
        goto err_enum;
    }

    err = udev_enumerate_scan_devices(enumerator);
    if (err < 0)
        goto err_enum;

    count = 0;
    entry_list = udev_enumerate_get_list_entry(enumerator);
    udev_list_entry_foreach(entry, entry_list) {
        struct udev_device *dev;
        int err;

        err = detect_device(&dev, ctx, entry, prefix);
        if (err < 0)
            continue;

         ++count;
         udev_device_unref(dev);
    }

    // Nothing available.
    if (count == 0)
        goto err_enum;

    *entries = calloc(count, sizeof(**entries));
    if (*entries == NULL) {
        err = -ENOMEM;
        goto err_enum;
    }

    index = 0;
    udev_list_entry_foreach(entry, entry_list) {
        struct udev_device *dev;
        const char *sysnum;
        long val;
        int err;

        err = detect_device(&dev, ctx, entry, prefix);
        if (err < 0)
            continue;

        sysnum = udev_device_get_sysnum(dev);
        if (sysnum != NULL && !long_from_string(sysnum, &val)) {
            (*entries)[index] = (unsigned int)val;
            ++index;
        }

        udev_device_unref(dev);
    }

    if (index != count) {
        err = -ENODATA;
        goto err_enum;
    }

    *entry_count = count;

    qsort(*entries, count, sizeof(unsigned int), compare_u32);
err_enum:
    udev_enumerate_unref(enumerator);
err_ctx:
    udev_unref(ctx);

    return err;
}
