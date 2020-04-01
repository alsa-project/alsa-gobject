// SPDX-License-Identifier: LGPL-3.0-or-later
#include "query.h"
#include "privates.h"

#include <errno.h>

#include <libudev.h>

// For error handling.
G_DEFINE_QUARK("alsaseq-error", alsaseq_error)

#define SEQ_SYSNAME   "seq"

/**
 * alsaseq_get_seq_sysname:
 * @sysname: (out): The sysname of ALSA Sequencer.
 * @error: A #GError.
 *
 * Allocate sysname string for ALSA sequencer and return it when exists.
 */
void alsaseq_get_seq_sysname(gchar **sysname, GError **error)
{
    struct udev *ctx;
    struct udev_device *dev;
    const char *name;

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        return;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", SEQ_SYSNAME);
    if (dev == NULL) {
        generate_error(error, ENODEV);
        udev_unref(ctx);
        return;
    }

    name = udev_device_get_sysname(dev);
    if (name == NULL) {
        generate_error(error, ENOENT);
        udev_device_unref(dev);
        udev_unref(ctx);
        return;
    }

    *sysname = strdup(name);
    if (*sysname == NULL)
        generate_error(error, ENOMEM);

    udev_device_unref(dev);
    udev_unref(ctx);
}

/**
 * alsaseq_get_seq_devnode:
 * @devnode: (out): The devnode of ALSA Sequencer.
 * @error: A #GError.
 *
 * Allocate devnode string for ALSA Sequencer and return it when exists.
 */
void alsaseq_get_seq_devnode(gchar **devnode, GError **error)
{
    struct udev *ctx;
    struct udev_device *dev;
    const char *node;

    ctx = udev_new();
    if (ctx == NULL) {
        generate_error(error, errno);
        return;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", SEQ_SYSNAME);
    if (dev == NULL) {
        generate_error(error, ENODEV);
        udev_unref(ctx);
        return;
    }

    node = udev_device_get_devnode(dev);
    if (node == NULL) {
        generate_error(error, ENODEV);
        udev_device_unref(dev);
        udev_unref(ctx);
        return;
    }

    *devnode = strdup(node);
    if (*devnode == NULL)
        generate_error(error, ENOMEM);

    udev_device_unref(dev);
    udev_unref(ctx);
}
