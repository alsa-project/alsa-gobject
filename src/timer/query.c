// SPDX-License-Identifier: LGPL-3.0-or-later
#include "query.h"
#include "privates.h"

#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#include <sound/asound.h>

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

/**
 * alsatimer_get_devnode:
 * @devnode: (out): The string for devnode of ALSA Timer.
 * @error: A #GError.
 *
 * Allocate string of devnode for ALSA Timer and return it if exists.
 */
void alsatimer_get_devnode(char **devnode, GError **error)
{
    struct udev *ctx;
    struct udev_device *dev;
    const char *node;

    g_return_if_fail(devnode != NULL);

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        return;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound",
                                                 TIMER_SYSNAME_TEMPLATE);
    if (dev == NULL) {
        generate_error(error, ENODEV);
        udev_unref(ctx);
        return;
    }

    node = udev_device_get_devnode(dev);
    if (node != NULL)
        *devnode = strdup(node);
    else
        generate_error(error, ENODEV);

    udev_device_unref(dev);
    udev_unref(ctx);
}

/**
 * alsatimer_get_device_id_list:
 * @entries: (element-type ALSATimer.DeviceId)(out): The array with
 *           entries of ALSATimerId.
 * @error: A #GError.
 *
 */
void alsatimer_get_device_id_list(GList **entries, GError **error)
{
    struct snd_timer_id id = {
        .dev_class = -1,
    };
    char *devnode;
    int fd;

    g_return_if_fail(entries != NULL);

    alsatimer_get_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    while (true) {
        ALSATimerDeviceId *entry;

        if (ioctl(fd, SNDRV_TIMER_IOCTL_NEXT_DEVICE, &id) < 0)
            break;
        if (id.dev_class == SNDRV_TIMER_CLASS_NONE)
            break;

        entry = g_boxed_copy(ALSATIMER_TYPE_DEVICE_ID, &id);
        *entries = g_list_append(*entries, entry);
    }

    close(fd);
}
