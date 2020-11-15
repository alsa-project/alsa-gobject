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

/**
 * SECTION: query
 * @Title: Global functions in ALSASeq
 * @Short_description: Global functions available without holding any file
 *                     descriptor
 */

#define SEQ_SYSNAME   "seq"

#define generate_file_error(exception, errno, msg) \
        g_set_error_literal(exception, G_FILE_ERROR, g_file_error_from_errno(errno), msg)

#define generate_file_error_fmt(exception, errno, fmt, msg) \
        g_set_error(exception, G_FILE_ERROR, g_file_error_from_errno(errno), fmt, msg)

/**
 * alsaseq_get_seq_sysname:
 * @sysname: (out): The sysname of ALSA Sequencer.
 * @error: A #GError.
 *
 * Allocate sysname string for ALSA sequencer and return it when exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsaseq_get_seq_sysname(gchar **sysname, GError **error)
{
    struct udev *ctx;
    struct udev_device *dev;
    const char *name;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    ctx = udev_new();
    if (ctx == NULL) {
        generate_file_error(error, errno, "udev_new()");
        return;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", SEQ_SYSNAME);
    if (dev == NULL) {
        generate_file_error(error, errno, "udev_enumerate_new()");
        udev_unref(ctx);
        return;
    }

    name = udev_device_get_sysname(dev);
    if (name == NULL) {
        generate_file_error(error, errno, "udev_enumerate_add_match_subsystem()");
        udev_device_unref(dev);
        udev_unref(ctx);
        return;
    }

    *sysname = g_strdup(name);

    udev_device_unref(dev);
    udev_unref(ctx);
}

/**
 * alsaseq_get_seq_devnode:
 * @devnode: (out): The devnode of ALSA Sequencer.
 * @error: A #GError.
 *
 * Allocate devnode string for ALSA Sequencer and return it when exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsaseq_get_seq_devnode(gchar **devnode, GError **error)
{
    struct udev *ctx;
    struct udev_device *dev;
    const char *node;

    g_return_if_fail(devnode != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    ctx = udev_new();
    if (ctx == NULL) {
        generate_file_error(error, errno, "udev_new()");
        return;
    }

    dev = udev_device_new_from_subsystem_sysname(ctx, "sound", SEQ_SYSNAME);
    if (dev == NULL) {
        generate_file_error(error, errno, "udev_device_new_from_subsystem_sysname()");
        udev_unref(ctx);
        return;
    }

    node = udev_device_get_devnode(dev);
    if (node == NULL) {
        generate_file_error(error, ENODEV, "udev_device_get_devnode()");
        udev_device_unref(dev);
        udev_unref(ctx);
        return;
    }

    *devnode = g_strdup(node);

    udev_device_unref(dev);
    udev_unref(ctx);
}

static int open_fd(GError **error)
{
    char *devnode;
    int fd;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return -1;

    fd = open(devnode, O_RDONLY);
    if (fd < 0) {
        generate_file_error_fmt(error, errno, "open(%s)", devnode);
        g_free(devnode);
        return -1;
    }
    g_free(devnode);

    return fd;
}

/**
 * alsaseq_get_system_info:
 * @system_info: (out): The information of ALSA Sequencer.
 * @error: A #GError.
 *
 * Get information of ALSA Sequencer.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_SYSTEM_INFO command for ALSA sequencer character device.
 */
void alsaseq_get_system_info(ALSASeqSystemInfo **system_info, GError **error)
{
    struct snd_seq_system_info *info;
    int fd;

    g_return_if_fail(system_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    *system_info = g_object_new(ALSASEQ_TYPE_SYSTEM_INFO, NULL);
    seq_system_info_refer_private(*system_info, &info);

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(SYSTEM_INFO)");
        g_object_unref(*system_info);
        *system_info = NULL;
        goto end;
    }

    // Decrement count for the above connection.
    --info->cur_clients;
end:
    close(fd);
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
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_CLIENT_ID, SNDRV_SEQ_IOCTL_SYSTEM_INFO, and
 * SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT command for ALSA sequencer character
 * device.
 */
void alsaseq_get_client_id_list(guint8 **entries, gsize *entry_count,
                                GError **error)
{
    int my_id;
    struct snd_seq_system_info system_info = {0};
    unsigned int count;
    guint8 *list;
    unsigned int index;
    struct snd_seq_client_info client_info = {0};
    int fd;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    if (ioctl(fd, SNDRV_SEQ_IOCTL_CLIENT_ID, &my_id) < 0) {
        generate_file_error(error, errno, "ioctl(CLIENT_ID)");
        goto end;
    }

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, &system_info) < 0) {
        generate_file_error(error, errno, "ioctl(SYSTEM_INFO)");
        goto end;
    }

    // Exclude myself.
    count = system_info.cur_clients - 1;
    if (count == 0)  {
        *entry_count = 0;
        goto end;
    }

    list = g_malloc0_n(count, sizeof(guint));

    index = 0;

    client_info.client = -1;
    while (index < count) {
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT, &client_info) < 0) {
            if (errno != ENOENT)
                generate_file_error(error, errno, "ioctl(QUERY_NEXT_CLIENT)");
            break;
        }

        if (client_info.client != my_id) {
            list[index] = client_info.client;
            ++index;
        }
    }

    if (*error != NULL) {
        g_free(list);
        goto end;
    }

    g_warn_if_fail(index == count);

    *entries = list;
    *entry_count = count;
end:
    close(fd);
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
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_CLIENT_INFO command for ALSA sequencer character
 * device.
 */
void alsaseq_get_client_info(guint8 client_id, ALSASeqClientInfo **client_info,
                             GError **error)
{
    struct snd_seq_client_info *info;
    int fd;

    g_return_if_fail(client_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    *client_info = g_object_new(ALSASEQ_TYPE_CLIENT_INFO, NULL);
    seq_client_info_refer_private(*client_info, &info);

    info->client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_CLIENT_INFO)");
        g_object_unref(*client_info);
        *client_info = NULL;
    }

    close(fd);
}

/**
 * alsaseq_get_port_id_list:
 * @client_id: The numerical ID of client to query. One of
 *             ALSASeqSpecificClientId is available as well as any numerical
 *             value.
 * @entries: (array length=entry_count)(out): The array with elements for
 *           numerical ID of port. One of ALSASeqSpecificPortId is available as
 *           well as any numerical value.
 * @entry_count: The number of entries in the array.
 * @error: A #GError.
 *
 * Get the list of numerical IDs for port added by the client.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_CLIENT_INFO and SNDRV_SEQ_IOCTL_QUERY_NEXT_PORT
 * commands for ALSA sequencer character device.
 */
void alsaseq_get_port_id_list(guint8 client_id, guint8 **entries,
                              gsize *entry_count, GError **error)
{
    struct snd_seq_client_info client_info = {0};
    unsigned int count;
    guint8 *list;
    unsigned int index;
    struct snd_seq_port_info port_info = {0};
    int fd;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    client_info.client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, &client_info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_CLIENT_INFO)");
        goto end;
    }

    count = client_info.num_ports;
    list = g_malloc0_n(count, sizeof(*list));

    index = 0;

    port_info.addr.client = client_id;
    port_info.addr.port = -1;
    while (index < count) {
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_NEXT_PORT, &port_info) < 0) {
            if (errno != ENOENT)
                generate_file_error(error, errno, "ioctl(QUERY_NEXT_PORT)");
            break;
        }

        list[index] = port_info.addr.port;
        ++index;
    }
    close(fd);

    if (*error != NULL) {
        g_free(list);
        goto end;
    }

    g_warn_if_fail(index == count);

    *entries = list;
    *entry_count = count;
end:
    close(fd);
}

/**
 * alsaseq_get_port_info:
 * @client_id: The numerical ID of client to query. One of
 *             ALSASeqSpecificClientId is available as well as any numerica
 *             value.
 * @port_id: The numerical ID of port in the client. One of
 *           ALSASeqSpecificPortId is available as well as any numerical value.
 * @port_info: (out): A #ALSASeqPortInfo for the port.
 * @error: A #GError.
 *
 * Get the information of port in client.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_PORT_INFO command for ALSA sequencer character
 * device.
 */
void alsaseq_get_port_info(guint8 client_id, guint8 port_id,
                           ALSASeqPortInfo **port_info, GError **error)
{
    struct snd_seq_port_info *info;
    int fd;

    g_return_if_fail(port_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    *port_info = g_object_new(ALSASEQ_TYPE_PORT_INFO, NULL);
    seq_port_info_refer_private(*port_info, &info);

    info->addr.client = client_id;
    info->addr.port = port_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_PORT_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_PORT_INFO)");
        g_object_unref(*port_info);
        *port_info = NULL;
    }

    close(fd);
}

/**
 * alsaseq_get_client_pool:
 * @client_id: The numerical ID of client to query. One of
 *             ALSASeqSpecificClientId is available as well as any numerical
 *             value.
 * @client_pool: (out): The information of memory pool for the client.
 * @error: A #GError.
 *
 * Get statistical information of memory pool for the given client.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_CLIENT_POOL command for ALSA sequencer character
 * device.
 */
void alsaseq_get_client_pool(guint8 client_id, ALSASeqClientPool **client_pool,
                             GError **error)
{
    int fd;
    struct snd_seq_client_pool *pool;

    g_return_if_fail(client_pool != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    *client_pool = g_object_new(ALSASEQ_TYPE_CLIENT_POOL, NULL);
    seq_client_pool_refer_private(*client_pool, &pool);

    pool->client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_POOL, pool) < 0) {
        generate_file_error(error, errno, "ioctl(GET_CLIENT_POOL)");
        g_object_unref(*client_pool);
    }

    close(fd);
}

static void fill_data_with_result(struct snd_seq_port_subscribe *data,
                                  struct snd_seq_query_subs *query)
{
    if (query->type == SNDRV_SEQ_QUERY_SUBS_READ) {
        data->sender = query->root;
        data->dest = query->addr;
    } else {
        data->sender = query->addr;
        data->dest = query->root;
    }
    data->queue = query->queue;
    data->flags = query->flags;
}

/**
 * alsaseq_get_subscription_list:
 * @addr: A #ALSASeqAddr to query.
 * @query_type: The type of query, one of #ALSASeqQuerySubscribeType.
 * @entries: (element-type ALSASeq.SubscribeData)(out): The array with element
 *           for subscription data.
 * @error: A #GError.
 *
 * Get the list of subscription for given address and query type.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_QUERY_SUBS command for ALSA sequencer character device.
 */
void alsaseq_get_subscription_list(const ALSASeqAddr *addr,
                                   ALSASeqQuerySubscribeType query_type,
                                   GList **entries, GError **error)
{
    int fd;
    struct snd_seq_query_subs query = {0};
    unsigned int count;
    unsigned int index;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    g_object_get((gpointer)addr, "client-id", &query.root.client,
                 "port-id", &query.root.port, NULL);
    query.type = query_type;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_SUBS, &query) < 0) {
        if (errno != ENOENT)
            generate_file_error(error, errno, "ioctl(QUERY_SUBS)");
        goto end;
    }
    count = query.num_subs;

    index = 0;
    while (index < count) {
        ALSASeqSubscribeData *subs_data =
                                g_object_new(ALSASEQ_TYPE_SUBSCRIBE_DATA, NULL);
        struct snd_seq_port_subscribe *data;

        seq_subscribe_data_refer_private(subs_data, &data);
        fill_data_with_result(data, &query);

        *entries = g_list_append(*entries, (gpointer)subs_data);
        ++index;

        ++query.index;
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_SUBS, &query) < 0) {
            if (errno != ENOENT)
                generate_file_error(error, errno, "ioctl(QUERY_SUBS)");
            break;
        }
    }

    if (*error != NULL) {
        g_list_free_full(*entries, g_object_unref);
        goto end;
    }

    g_warn_if_fail(index == count);
end:
    close(fd);
}

/**
 * alsaseq_get_queue_id_list:
 * @entries: (array length=entry_count)(out): The array of elements for
 *           numerical ID of queue.
 * @entry_count: The number of entries.
 * @error: A #GError.
 *
 * Get the list of queue in ALSA Sequencer.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_SYSTEM_INFO and SNDRV_SEQ_IOCTL_GET_QUEUE_INFO commands
 * for ALSA sequencer character device.
 */
void alsaseq_get_queue_id_list(guint8 **entries, gsize *entry_count,
                               GError **error)
{
    int fd;
    struct snd_seq_system_info info = {0};
    unsigned int maximum_count;
    unsigned int count;
    guint8 *list;
    unsigned int index;
    int i;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, &info) < 0) {
        generate_file_error(error, errno, "ioctl(SYSTEM_INFO)");
        goto end;
    }
    maximum_count = info.queues;
    count = info.cur_queues;

    if (count == 0)
        goto end;

    list = g_malloc0_n(count, sizeof(*entries));

    index = 0;
    for (i = 0; i < maximum_count; ++i) {
        struct snd_seq_queue_info info;

        info.queue = i;
        if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_QUEUE_INFO, &info) < 0)
            continue;

        list[index] = i;
        if (++index >= count)
            break;
    }

    g_warn_if_fail(index == count);

    *entries = list;
    *entry_count = count;
end:
    close(fd);
}

/**
 * alsaseq_get_queue_info_by_id:
 * @queue_id: The numerical ID of queue, except for one of
 *            ALSASeqSpecificQueueId.
 * @queue_info: (out): The information of queue.
 * @error: A #GError.
 *
 * Get the information of queue, according to the numerical ID.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_QUEUE_INFO command for ALSA sequencer character
 * device.
 */
void alsaseq_get_queue_info_by_id(guint8 queue_id, ALSASeqQueueInfo **queue_info,
                                  GError **error)
{
    struct snd_seq_queue_info *info;
    int fd;

    g_return_if_fail(queue_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    *queue_info = g_object_new(ALSASEQ_TYPE_QUEUE_INFO, NULL);
    seq_queue_info_refer_private(*queue_info, &info);

    info->queue = queue_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_QUEUE_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_QUEUE_INFO)");
        g_object_unref(*queue_info);
    }

    close(fd);
}

/**
 * alsaseq_get_queue_info_by_name:
 * @name: The name string of queue to query.
 * @queue_info: (out): The information of queue.
 * @error: A #GError.
 *
 * Get the information of queue, according to the name string.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_NAMED_QUEUE command for ALSA sequencer character
 * device.
 */
void alsaseq_get_queue_info_by_name(const gchar *name,
                                    ALSASeqQueueInfo **queue_info,
                                    GError **error)
{
    struct snd_seq_queue_info *info;
    int fd;

    g_return_if_fail(queue_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    *queue_info = g_object_new(ALSASEQ_TYPE_QUEUE_INFO, NULL);
    seq_queue_info_refer_private(*queue_info, &info);

    strncpy(info->name, name, sizeof(info->name));
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_NAMED_QUEUE, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_NAMED_QUEUE)");
        g_object_unref(*queue_info);
    }

    close(fd);
}

/**
 * alsaseq_get_queue_status:
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @queue_status: (inout): The current status of queue.
 * @error: A #GError.
 *
 * Get current status of queue.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_QUEUE_STATUS command for ALSA sequencer character
 * device.
 */
void alsaseq_get_queue_status(guint8 queue_id,
                              ALSASeqQueueStatus *const *queue_status,
                              GError **error)
{
    struct snd_seq_queue_status *status;
    int fd;

    g_return_if_fail(queue_status != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    fd = open_fd(error);
    if (fd < 0)
        return;

    g_return_if_fail(ALSASEQ_IS_QUEUE_STATUS(*queue_status));
    seq_queue_status_refer_private(*queue_status, &status);

    status->queue = (int)queue_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_QUEUE_STATUS, status) < 0)
        generate_file_error(error, errno, "ioctl(GET_QUEUE_STATUS)");

    close(fd);
}
