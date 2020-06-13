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

// For error handling.
G_DEFINE_QUARK("alsaseq-error", alsaseq_error)

#define SEQ_SYSNAME   "seq"

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
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
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
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_SYSTEM_INFO command for ALSA sequencer character device.
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
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_CLIENT_ID, SNDRV_SEQ_IOCTL_SYSTEM_INFO, and
 * SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT command for ALSA sequencer character
 * device.
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
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_CLIENT_INFO command for ALSA sequencer character
 * device.
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
void alsaseq_get_port_id_list(guint client_id, guint **entries,
                              gsize *entry_count, GError **error)
{
    char *devnode;
    struct snd_seq_client_info client_info = {0};
    unsigned int count;
    guint *list;
    unsigned int index;
    struct snd_seq_port_info port_info = {0};
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

    client_info.client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, &client_info) < 0) {
        generate_error(error, errno);
        close(fd);
        return;
    }

    count = client_info.num_ports;
    list = g_try_malloc0_n(count, sizeof(*list));
    if (list == NULL) {
        generate_error(error, ENOMEM);
        close(fd);
        return;
    }
    index = 0;

    port_info.addr.client = client_id;
    port_info.addr.port = -1;
    while (index < count) {
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_NEXT_PORT, &port_info) < 0) {
            break;
        }

        list[index] = port_info.addr.port;
        ++index;
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
void alsaseq_get_port_info(guint client_id, guint port_id,
                           ALSASeqPortInfo **port_info, GError **error)
{
    char *devnode;
    struct snd_seq_port_info *info;
    int fd;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    *port_info = g_object_new(ALSASEQ_TYPE_PORT_INFO, NULL);
    seq_port_info_refer_private(*port_info, &info);

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        g_object_unref(*port_info);
        *port_info = NULL;
        return;
    }

    info->addr.client = client_id;
    info->addr.port = port_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_PORT_INFO, info) < 0)
        generate_error(error, errno);
    close(fd);
    if (*error != NULL) {
        g_object_unref(*port_info);
        *port_info = NULL;
    }
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
void alsaseq_get_client_pool(gint client_id, ALSASeqClientPool **client_pool,
                             GError **error)
{
    char *devnode;
    int fd;
    struct snd_seq_client_pool *pool;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    *client_pool = g_object_new(ALSASEQ_TYPE_CLIENT_POOL, NULL);
    seq_client_pool_refer_private(*client_pool, &pool);

    pool->client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_POOL, pool) < 0) {
        generate_error(error, errno);
        close(fd);
        g_object_unref(*client_pool);
        return;
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
    char *devnode;
    int fd;
    struct snd_seq_query_subs query = {0};
    unsigned int count;
    unsigned int index;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    g_object_get((gpointer)addr, "client-id", &query.root.client,
                 "port-id", &query.root.port, NULL);
    query.type = query_type;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_SUBS, &query) < 0) {
        if (errno != ENOENT)
            generate_error(error, errno);
        close(fd);
        return;
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
                generate_error(error, errno);
            break;
        }
    }

    close(fd);

    if (index != count)
        generate_error(error, ENXIO);
    if (*error != NULL) {
        g_list_free_full(*entries, g_object_unref);
        return;
    }
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
void alsaseq_get_queue_id_list(guint **entries, gsize *entry_count,
                               GError **error)
{
    char *devnode;
    int fd;
    struct snd_seq_system_info info = {0};
    unsigned int maximum_count;
    unsigned int count;
    guint *list;
    unsigned int index;
    int i;

    alsaseq_get_seq_devnode(&devnode, error);
    if (*error != NULL)
        return;

    fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (fd < 0) {
        generate_error(error, errno);
        return;
    }

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, &info) < 0) {
        generate_error(error, errno);
        close(fd);
        return;
    }
    maximum_count = info.queues;
    count = info.cur_queues;

    if (count == 0) {
        close(fd);
        return;
    }

    list = g_try_malloc0_n(count, sizeof(*entries));
    if (list == NULL) {
        generate_error(error, ENOMEM);
        close(fd);
        return;
    }

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
void alsaseq_get_queue_info_by_id(guint queue_id, ALSASeqQueueInfo **queue_info,
                                  GError **error)
{
    struct snd_seq_queue_info *info;
    char *devnode;
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

    *queue_info = g_object_new(ALSASEQ_TYPE_QUEUE_INFO, NULL);
    seq_queue_info_refer_private(*queue_info, &info);

    info->queue = queue_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_QUEUE_INFO, info) < 0) {
        generate_error(error, errno);
        close(fd);
        g_object_unref(*queue_info);
        return;
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
    char *devnode;
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

    *queue_info = g_object_new(ALSASEQ_TYPE_QUEUE_INFO, NULL);
    seq_queue_info_refer_private(*queue_info, &info);

    strncpy(info->name, name, sizeof(info->name));
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_NAMED_QUEUE, info) < 0) {
        generate_error(error, errno);
        close(fd);
        g_object_unref(*queue_info);
        return;
    }

    close(fd);
}

/**
 * alsaseq_get_queue_status:
 * @queue_id: The numerical ID of queue, except for entries in
 *            ALSASeqSpecificQueueId.
 * @queue_status: (out): The current status of queue.
 * @error: A #GError.
 *
 * Get current status of queue.
 *
 * The call of function executes open(2), close(2), and ioctl(2) system calls
 * with SNDRV_SEQ_IOCTL_GET_QUEUE_STATUS command for ALSA sequencer character
 * device.
 */
void alsaseq_get_queue_status(guint queue_id, ALSASeqQueueStatus **queue_status,
                              GError **error)
{
    struct snd_seq_queue_status *status;
    char *devnode;
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

    *queue_status = g_object_new(ALSASEQ_TYPE_QUEUE_STATUS, NULL);
    seq_queue_status_refer_private(*queue_status, &status);

    status->queue = (int)queue_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_QUEUE_STATUS, status) < 0) {
        generate_error(error, errno);
        close(fd);
        g_object_unref(*queue_status);
        return;
    }

    close(fd);
}
