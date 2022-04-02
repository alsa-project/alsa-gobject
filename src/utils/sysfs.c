// SPDX-License-Identifier: LGPL-3.0-or-later
#include "utils.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

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
