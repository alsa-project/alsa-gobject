// SPDX-License-Identifier: LGPL-3.0-or-later
#include "query.h"
#include "privates.h"

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#include <sound/asound.h>
#include <time.h>

#include <libudev.h>

#define TIMER_SYSNAME_TEMPLATE  "timer"
#define SYSFS_SND_TIMER_NODE    "/sys/module/snd_timer/"

/**
 * SECTION: query
 * @Title: Global functions in ALSATimer
 * @Short_description: Global functions available without holding any
 *                     file descriptor
 */

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
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsatimer_get_sysname(char **sysname, GError **error)
{
    char *name;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    name = g_strdup(TIMER_SYSNAME_TEMPLATE);

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
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsatimer_get_devnode(char **devnode, GError **error)
{
    struct udev *ctx;
    struct udev_device *dev;
    const char *node;

    g_return_if_fail(devnode != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

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
        *devnode = g_strdup(node);
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
 * Get the list of existent timer device.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_TIMER_IOCTL_NEXT_DEVICE command for ALSA timer character device.
 */
void alsatimer_get_device_id_list(GList **entries, GError **error)
{
    struct snd_timer_id id = {
        .dev_class = -1,
    };
    char *devnode;
    int fd;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

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

/**
 * alsatimer_get_device_info:
 * @device_id: A #ALSATimerDeviceId to identify the timer device.
 * @device_info: (out): The information of timer device.
 * @error: A #GError.
 *
 * Get the information of timer device.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_TIMER_IOCTL_GINFO command for ALSA timer character device.
 */
void alsatimer_get_device_info(ALSATimerDeviceId *device_id,
                               ALSATimerDeviceInfo **device_info,
                               GError **error)
{
    char *devnode;
    struct snd_timer_ginfo *info;
    int fd;

    g_return_if_fail(device_id != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    alsatimer_get_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    *device_info = g_object_new(ALSATIMER_TYPE_DEVICE_INFO, NULL);
    timer_device_info_refer_private(*device_info, &info);

    info->tid = *device_id;
    if (ioctl(fd, SNDRV_TIMER_IOCTL_GINFO, info) < 0) {
        generate_error(error, errno);
        g_object_unref(*device_info);
    }

    close(fd);
}

/**
 * alsatimer_get_device_status:
 * @device_id: A #ALSATimerDeviceId to identify the timer device.
 * @device_status: (inout): The status of timer device.
 * @error: A #GError.
 *
 * Get the status of timer device.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_TIMER_IOCTL_GSTATUS command for ALSA timer character device.
 */
void alsatimer_get_device_status(ALSATimerDeviceId *device_id,
                                 ALSATimerDeviceStatus *const *device_status,
                                 GError **error)
{
    char *devnode;
    struct snd_timer_gstatus *status;
    int fd;

    g_return_if_fail(device_id != NULL);
    g_return_if_fail(ALSATIMER_IS_DEVICE_STATUS(*device_status));
    g_return_if_fail(error == NULL || *error == NULL);

    alsatimer_get_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    timer_device_status_refer_private(*device_status, &status);

    status->tid = *device_id;
    if (ioctl(fd, SNDRV_TIMER_IOCTL_GSTATUS, status) < 0) {
        generate_error(error, errno);
        g_object_unref(*device_status);
    }

    close(fd);
}

/**
 * alsatimer_set_device_params:
 * @device_id: A #ALSATimerDeviceId to identify the timer device.
 * @device_params: The parameters of timer device.
 * @error: A #GError.
 *
 * Set the given parameters to the timer indicated by the identifier.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system call
 * with SNDRV_TIMER_IOCTL_GPARAMS command for ALSA timer character device.
 */
void alsatimer_set_device_params(ALSATimerDeviceId *device_id,
                                 const ALSATimerDeviceParams *device_params,
                                 GError **error)
{
    char *devnode;
    struct snd_timer_gparams *params;
    int fd;

    g_return_if_fail(device_id != NULL);
    g_return_if_fail(device_params != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    alsatimer_get_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    timer_device_params_refer_private((ALSATimerDeviceParams *)device_params,
                                      &params);
    params->tid = *device_id;
    if (ioctl(fd, SNDRV_TIMER_IOCTL_GPARAMS, params) < 0)
        generate_error(error, errno);

    close(fd);
}

static void timer_get_node_param_value(const char *param_name, char *buf,
                                       gsize size, int *val, GError **error)
{
    char literal[64];
    int fd;
    ssize_t len;
    long v;
    char *term;

    snprintf(literal, sizeof(literal), SYSFS_SND_TIMER_NODE "parameters/%s",
             param_name);

    fd = open(literal, O_RDONLY);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    len = read(fd, buf, size);
    if (len < 0) {
        generate_error(error, errno);
        goto end;
    }

    v = strtol(buf, &term, 10);
    if (errno > 0)
        generate_error(error, errno);
    else if (*term != '\n')
        generate_error(error, EIO);
    else
        *val = (int)v;
end:
    close(fd);
}

/**
 * alsatimer_get_tstamp_source:
 * @clock_id: (out): The clock source for timestamp. The value of CLOCK_XXX in
 *                   UAPI of Linux kernel.
 * @error: A #GError.
 *
 * Get the clock source for timestamp when #ALSATimerUserInstance is configured
 * to receive event with timestamp. The source is selected according to
 * parameter of 'snd-timer' kernel module, and the call of function is just to
 * refer to it.
 *
 * 0 means CLOCK_REALTIME is used. 1 means CLOCK_MONOTONIC is used.
 *
 * The call of function executes open(2), read(2), close(2) system calls for
 * the sysfs node corresponding to 'snd-timer' kernel module.
 */
void alsatimer_get_tstamp_source(int *clock_id, GError **error)
{
    int val;
    gsize size;
    char *buf;

    g_return_if_fail(error == NULL || *error == NULL);

    // Count required digits.
    val = INT_MAX;
    size = 0;
    while (val > 0) {
        val /= 10;
        ++size;
    }

    // For codes of sign and new line.
    size += 2;

    buf = g_malloc0(size);

    timer_get_node_param_value("timer_tstamp_monotonic", buf, size, &val,
                               error);
    if (*error != NULL)
        goto end;

    switch (val) {
    case 0:
        *clock_id = CLOCK_REALTIME;
        break;
    case 1:
        *clock_id = CLOCK_MONOTONIC;
        break;
    default:
        generate_error(error, EPROTO);
        break;
    }
end:
    g_free(buf);
}
