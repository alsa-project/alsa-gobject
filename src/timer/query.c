// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include <time.h>

#define SYSFS_SND_TIMER_NODE    "/sys/module/snd_timer/"

/**
 * alsatimer_get_sysname:
 * @sysname: (out): The string for sysname of ALSA Timer.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Allocate sysname for ALSA Timer and return it when it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_get_sysname(char **sysname, GError **error)
{
    int err;

    g_return_val_if_fail(sysname != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    err = lookup_and_allocate_timer_sysname(sysname);
    if (err < 0) {
        generate_file_error(error, -err, "Fail to generate timer sysname");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_get_devnode:
 * @devnode: (out): The string for devnode of ALSA Timer.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Allocate string of devnode for ALSA Timer and return it if exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_get_devnode(char **devnode, GError **error)
{
    int err;

    g_return_val_if_fail(devnode != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    err = lookup_and_allocate_timer_devname(devnode);
    if (err < 0) {
        generate_file_error(error, -err, "Fail to generate timer devname");
        return FALSE;
    }

    return TRUE;
}

static gboolean open_fd(int *fd, GError **error)
{
    char *devname;
    gboolean result;

    if (!alsatimer_get_devnode(&devname, error))
        return -1;

    result = TRUE;
    *fd = open(devname, O_RDONLY);
    if (*fd < 0) {
        generate_file_error(error, errno, "open(%s)", devname);
        result = FALSE;
    }
    g_free(devname);

    return result;
}

/**
 * alsatimer_get_device_id_list:
 * @entries: (element-type ALSATimer.DeviceId)(out): The array with entries of [struct@DeviceId].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the list of existent timer device.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system call with
 * `SNDRV_TIMER_IOCTL_NEXT_DEVICE` command for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_get_device_id_list(GList **entries, GError **error)
{
    struct snd_timer_id id = {
        .dev_class = -1,
    };
    int fd;
    gboolean result;

    g_return_val_if_fail(entries != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    result = TRUE;
    while (true) {
        ALSATimerDeviceId *entry;

        if (ioctl(fd, SNDRV_TIMER_IOCTL_NEXT_DEVICE, &id) < 0) {
            generate_file_error(error, errno, "ioctl(SNDRV_TIMER_IOCTL_NEXT_DEVICE)");
            result = FALSE;
            break;
        }
        if (id.dev_class == SNDRV_TIMER_CLASS_NONE)
            break;

        entry = g_boxed_copy(ALSATIMER_TYPE_DEVICE_ID, &id);
        *entries = g_list_append(*entries, entry);
    }

    close(fd);

    return result;
}

/**
 * alsatimer_get_device_info:
 * @device_id: A [struct@DeviceId] to identify the timer device.
 * @device_info: (out): The information of timer device.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the information of timer device.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system call with
 * `SNDRV_TIMER_IOCTL_GINFO` command for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_get_device_info(ALSATimerDeviceId *device_id,
                               ALSATimerDeviceInfo **device_info,
                               GError **error)
{
    struct snd_timer_ginfo *info;
    int fd;
    gboolean result;

    g_return_val_if_fail(device_id != NULL, FALSE);
    g_return_val_if_fail(device_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    *device_info = g_object_new(ALSATIMER_TYPE_DEVICE_INFO, NULL);
    timer_device_info_refer_private(*device_info, &info);

    result = TRUE;
    info->tid = *device_id;
    if (ioctl(fd, SNDRV_TIMER_IOCTL_GINFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(SNDRV_TIMER_IOCTL_GINFO)");
        g_object_unref(*device_info);
        result = FALSE;
    }

    close(fd);

    return result;
}

/**
 * alsatimer_get_device_status:
 * @device_id: A [struct@DeviceId] to identify the timer device.
 * @device_status: (inout): The status of timer device.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the status of timer device.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system call with
 * `SNDRV_TIMER_IOCTL_GSTATUS` command for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_get_device_status(ALSATimerDeviceId *device_id,
                                 ALSATimerDeviceStatus *const *device_status,
                                 GError **error)
{
    struct snd_timer_gstatus *status;
    int fd;
    gboolean result;

    g_return_val_if_fail(device_id != NULL, FALSE);
    g_return_val_if_fail(ALSATIMER_IS_DEVICE_STATUS(*device_status), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    timer_device_status_refer_private(*device_status, &status);

    result = TRUE;
    status->tid = *device_id;
    if (ioctl(fd, SNDRV_TIMER_IOCTL_GSTATUS, status) < 0) {
        generate_file_error(error, errno, "ioctl(SNDRV_TIMER_IOCTL_GSTATUS)");
        g_object_unref(*device_status);
        result = FALSE;
    }

    close(fd);

    return result;
}

/**
 * alsatimer_set_device_params:
 * @device_id: A [struct@DeviceId] to identify the timer device.
 * @device_params: The parameters of timer device.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Set the given parameters to the timer indicated by the identifier.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system call with
 * `SNDRV_TIMER_IOCTL_GPARAMS` command for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_set_device_params(ALSATimerDeviceId *device_id,
                                 const ALSATimerDeviceParams *device_params,
                                 GError **error)
{
    struct snd_timer_gparams *params;
    int fd;
    gboolean result;

    g_return_val_if_fail(device_id != NULL, FALSE);
    g_return_val_if_fail(device_params != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    result = TRUE;
    timer_device_params_refer_private((ALSATimerDeviceParams *)device_params,
                                      &params);
    params->tid = *device_id;
    if (ioctl(fd, SNDRV_TIMER_IOCTL_GPARAMS, params) < 0) {
        generate_file_error(error, errno, "ioctl(SNDRV_TIMER_IOCTL_GPARAMS)");
        result = FALSE;
    }

    close(fd);

    return result;
}

static gboolean timer_get_node_param_value(const char *param_name, char *buf, gsize size, int *val,
                                           GError **error)
{
    char literal[64];
    int fd;
    ssize_t len;
    long v;
    int err;
    gboolean result;

    snprintf(literal, sizeof(literal), SYSFS_SND_TIMER_NODE "parameters/%s",
             param_name);

    fd = open(literal, O_RDONLY);
    if (fd < 0) {
        generate_file_error(error, errno, "open(%s)", literal);
        return FALSE;
    }

    len = read(fd, buf, size);
    if (len < 0) {
        generate_file_error(error, errno, "read()");
        result = FALSE;
        goto end;
    }

    err = long_from_string(buf, &v);
    if (err < 0) {
        generate_file_error(error, -err, "Fail to parse snd_timer module parameter as integer value");
        result = FALSE;
    } else {
        *val = (int)v;
        result = TRUE;
    }
end:
    close(fd);

    return result;
}

/**
 * alsatimer_get_real_time_clock_id:
 * @clock_id: (out): The clock_id for real time. The value of `CLOCK_XXX` in UAPI of Linux
 *            kernel.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get `clock_id` for real time. The `clock_id` governs real time retrieved by both
 * [method@RealTimeEvent.get_time] and [method@InstanceStatus.get_time].
 *
 * The call of function is just to refer to parameter of `snd-timer` kernel module. `0` means
 * `CLOCK_REALTIME` is used. `1` means `CLOCK_MONOTONIC` is used.
 *
 * The call of function executes `open(2)`, `read(2)`, `close(2)` system calls for the sysfs node
 * corresponding to the parameter.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_get_real_time_clock_id(int *clock_id, GError **error)
{
    int val;
    gsize size;
    char *buf;
    gboolean result;

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

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

    result = timer_get_node_param_value("timer_tstamp_monotonic", buf, size, &val, error);
    g_free(buf);
    if (!result)
        return FALSE;

    switch (val) {
    case 0:
        *clock_id = CLOCK_REALTIME;
        break;
    case 1:
        *clock_id = CLOCK_MONOTONIC;
        break;
    default:
        generate_file_error(error, EPROTO, "timer_tstamp_monotonic");
        return FALSE;
    }

    return TRUE;
}
