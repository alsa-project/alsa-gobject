// SPDX-License-Identifier: LGPL-3.0-or-later
#include "query.h"
#include "privates.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

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

/**
 * alsaseq_get_system_info:
 * @system_info: (out): The information of ALSA Sequencer.
 * @error: A #GError.
 *
 * Get information of ALSA Sequencer.
 */
void alsaseq_get_system_info(ALSASeqSystemInfo **system_info, GError **error)
{
    char *devnode;
    struct snd_seq_system_info *info;
    int fd;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    *system_info = g_object_new(ALSASEQ_TYPE_SYSTEM_INFO, NULL);
    seq_system_info_refer_private(*system_info, &info);

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        g_object_unref(*system_info);
        *system_info = NULL;
        return;
    }

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, info) < 0)
        generate_error(error, errno);
    close(fd);
    if (*error != NULL) {
        g_object_unref(*system_info);
        *system_info = NULL;
        return;
    }

    // Decrement count for the above connection.
    --info->cur_clients;
}

/**
 * alsaseq_get_client_id_list:
 * @entries: (array length=entry_count)(out): The array with elements for
 *           numerical ID of client. One of ALSASeqSpecificClientId can be
 *           included in result as well as any numerical value.
 * @entry_count: The number of entries.
 * @error: A #GError.
 *
 * Get the list of clients as the numerical ID.
 */
void alsaseq_get_client_id_list(guint **entries, gsize *entry_count,
                                GError **error)
{
    char *devnode;
    int my_id;
    struct snd_seq_system_info system_info = {0};
    unsigned int count;
    guint *list;
    unsigned int index;
    struct snd_seq_client_info client_info = {0};
    int fd;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    if (ioctl(fd, SNDRV_SEQ_IOCTL_CLIENT_ID, &my_id) < 0) {
        generate_error(error, errno);
        close(fd);
        return;
    }

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, &system_info) < 0) {
        generate_error(error, errno);
        close(fd);
        return;
    }

    // Exclude myself.
    count = system_info.cur_clients - 1;
    if (count == 0)  {
        *entry_count = 0;
        close(fd);
        return;
    }

    list = g_try_malloc0_n(count, sizeof(guint));
    if (list == NULL) {
        *entry_count = 0;
        close(fd);
        return;
    }
    index = 0;

    client_info.client = -1;
    while (index < count) {
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT, &client_info) < 0)
            break;

        if (client_info.client != my_id) {
            list[index] = client_info.client;
            ++index;
        }
    }
    close(fd);

    if (index != count) {
        generate_error(error, ENXIO);
        g_free(list);
        return;
    }

    *entries = list;
    *entry_count = count;
}

/**
 * alsaseq_get_client_info:
 * @client_id: The numerical ID of client to query. One of
 *             ALSASeqSpecificClientId is available as well as any numerical
 *             value.
 * @client_info: (out): A #ALSASeqClientInfo for the client.
 * @error: A #GError.
 *
 * Get the information of client according to the numerical ID.
 */
void alsaseq_get_client_info(guint client_id, ALSASeqClientInfo **client_info,
                             GError **error)
{
    char *devnode;
    struct snd_seq_client_info *info;
    int fd;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    *client_info = g_object_new(ALSASEQ_TYPE_CLIENT_INFO, NULL);
    seq_client_info_refer_private(*client_info, &info);

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        g_object_unref(*client_info);
        *client_info = NULL;
        return;
    }

    info->client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, info) < 0)
        generate_error(error, errno);
    close(fd);
    if (*error != NULL) {
        g_object_unref(*client_info);
        *client_info = NULL;
    }
}
