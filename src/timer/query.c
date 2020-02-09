// SPDX-License-Identifier: LGPL-3.0-or-later
#include "query.h"
#include "privates.h"

#include <stdbool.h>
#include <errno.h>

#include <libudev.h>

#define TIMER_SYSNAME_TEMPLATE  "timer"

// For error handling.
G_DEFINE_QUARK("alsatimer-error", alsatimer_error)

static bool check_existence(char *sysname, GError **error)
{
    struct udev *ctx;
    struct udev_device *dev;
    bool result;

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        return false;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", sysname);
    if (dev == NULL) {
        generate_error(error, errno);
        result = false;
    } else {
        result = true;
    }
    udev_device_unref(dev);

    udev_unref(ctx);

    return result;
}

/**
 * alsatimer_get_sysname:
 * @sysname: (out): The string for sysname of ALSA Timer.
 * @error: A #GError.
 *
 * Allocate sysname for ALSA Timer and return it when it exists.
 */
void alsatimer_get_sysname(char **sysname, GError **error)
{
    char *name;

    g_return_if_fail(sysname != NULL);

    name = strdup(TIMER_SYSNAME_TEMPLATE);
    if (name == NULL) {
        generate_error(error, ENOMEM);
        return;
    }

    if (!check_existence(name, error)) {
        g_free(name);
        return;
    }

    *sysname = name;
}
