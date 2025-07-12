// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

/**
 * alsaseq_get_seq_sysname:
 * @sysname: (out): The sysname of ALSA Sequencer.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Allocate sysname string for ALSA sequencer and return it when exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_seq_sysname(gchar **sysname, GError **error)
{
    int err;

    g_return_val_if_fail(sysname != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    err = lookup_and_allocate_seq_sysname(sysname);
    if (err < 0) {
        generate_file_error(error, -err, "Fail to generate seq sysname");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_get_seq_devnode:
 * @devnode: (out): The devnode of ALSA Sequencer.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Allocate devnode string for ALSA Sequencer and return it when exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_seq_devnode(gchar **devnode, GError **error)
{
    int err;

    g_return_val_if_fail(devnode != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    err = lookup_and_allocate_seq_devname(devnode);
    if (err < 0) {
        generate_file_error(error, -err, "Fail to generate seq devname");
        return FALSE;
    }

    return TRUE;
}

static gboolean open_fd(int *fd, GError **error)
{
    char *devname;
    gboolean result;

    if (!alsaseq_get_seq_devnode(&devname, error))
        return FALSE;

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
 * alsaseq_get_system_info:
 * @system_info: (out): The information of ALSA Sequencer.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get information of ALSA Sequencer.
 *
 * The call of function executes `open(2)``, ``close(2)``, and ``ioctl(2)`` system calls with
 * `SNDRV_SEQ_IOCTL_SYSTEM_INFO` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_system_info(ALSASeqSystemInfo **system_info, GError **error)
{
    struct snd_seq_system_info *info;
    int fd;
    gboolean result;

    g_return_val_if_fail(system_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    *system_info = g_object_new(ALSASEQ_TYPE_SYSTEM_INFO, NULL);
    seq_system_info_refer_private(*system_info, &info);

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(SYSTEM_INFO)");
        g_object_unref(*system_info);
        *system_info = NULL;
        result = FALSE;
        goto end;
    }

    // Decrement count for the above connection.
    --info->cur_clients;

    result = TRUE;
end:
    close(fd);
    return result;
}

/**
 * alsaseq_get_client_id_list:
 * @entries: (array length=entry_count)(out): The array with elements for numeric identified of
 *           client. One of [enum@SpecificClientId] can be included in result as well as any
 *           numeric value.
 * @entry_count: The number of entries.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the list of clients as the numeric identifier.
 *
 * The call of function executes `open(2)``, ``close(2)``, and ``ioctl(2)`` system calls with
 * `SNDRV_SEQ_IOCTL_CLIENT_ID`, `SNDRV_SEQ_IOCTL_SYSTEM_INFO`, and
 * `SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_client_id_list(guint8 **entries, gsize *entry_count, GError **error)
{
    int my_id;
    struct snd_seq_system_info system_info = {0};
    unsigned int count;
    guint8 *list;
    unsigned int index;
    struct snd_seq_client_info client_info = {0};
    int fd;
    gboolean result;

    g_return_val_if_fail(entries != NULL, FALSE);
    g_return_val_if_fail(entry_count != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    if (ioctl(fd, SNDRV_SEQ_IOCTL_CLIENT_ID, &my_id) < 0) {
        generate_file_error(error, errno, "ioctl(CLIENT_ID)");
        result = FALSE;
        goto end;
    }

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, &system_info) < 0) {
        generate_file_error(error, errno, "ioctl(SYSTEM_INFO)");
        result = FALSE;
        goto end;
    }

    // Exclude myself.
    count = system_info.cur_clients - 1;
    if (count == 0)  {
        *entry_count = 0;
        result = TRUE;
        goto end;
    }

    list = g_malloc0_n(count, sizeof(guint));

    index = 0;

    result = TRUE;
    client_info.client = -1;
    while (index < count) {
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT, &client_info) < 0) {
            if (errno != ENOENT) {
                generate_file_error(error, errno, "ioctl(QUERY_NEXT_CLIENT)");
                result = FALSE;
            }
            break;
        }

        if (client_info.client != my_id) {
            list[index] = client_info.client;
            ++index;
        }
    }

    if (!result) {
        g_free(list);
        goto end;
    }

    g_warn_if_fail(index == count);

    *entries = list;
    *entry_count = count;
end:
    close(fd);
    return result;
}

/**
 * alsaseq_get_client_info:
 * @client_id: The numeric identifier of client to query. One of [enum@SpecificClientId] is
 *             available as well as any numeric value.
 * @client_info: (out): A [class@ClientInfo] for the client.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the information of client according to the numeric ID.
 *
 * The call of function executes `open(2)``, ``close(2)``, and ``ioctl(2)`` system calls with
 * `SNDRV_SEQ_IOCTL_GET_CLIENT_INFO` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_client_info(guint8 client_id, ALSASeqClientInfo **client_info, GError **error)
{
    struct snd_seq_client_info *info;
    int fd;
    gboolean result;

    g_return_val_if_fail(client_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    *client_info = g_object_new(ALSASEQ_TYPE_CLIENT_INFO, NULL);
    seq_client_info_refer_private(*client_info, &info);

    info->client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_CLIENT_INFO)");
        g_object_unref(*client_info);
        *client_info = NULL;
        result = FALSE;
    } else {
        result = TRUE;
    }

    close(fd);

    return result;
}

/**
 * alsaseq_get_port_id_list:
 * @client_id: The numeric ID of client to query. One of [enum@SpecificClientId] is available as
 *             well as any numeric value.
 * @entries: (array length=entry_count)(out): The array with elements for numeric identifier of
 *           port. One of [enum@SpecificPortId] is available as well as any numeric value.
 * @entry_count: The number of entries in the array.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the list of numeric identifiers for port added by the client.
 *
 * The call of function executes `open(2)``, ``close(2)``, and ``ioctl(2)`` system calls with
 * `SNDRV_SEQ_IOCTL_GET_CLIENT_INFO` and `SNDRV_SEQ_IOCTL_QUERY_NEXT_PORT` commands for ALSA
 * sequencer character device.
 */
gboolean alsaseq_get_port_id_list(guint8 client_id, guint8 **entries, gsize *entry_count,
                                  GError **error)
{
    struct snd_seq_client_info client_info = {0};
    unsigned int count;
    guint8 *list;
    unsigned int index;
    struct snd_seq_port_info port_info = {0};
    int fd;
    gboolean result;

    g_return_val_if_fail(entries != NULL, FALSE);
    g_return_val_if_fail(entry_count != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    client_info.client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_INFO, &client_info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_CLIENT_INFO)");
        result = FALSE;
        goto end;
    }

    count = client_info.num_ports;
    list = g_malloc0_n(count, sizeof(*list));

    index = 0;

    result = TRUE;
    port_info.addr.client = client_id;
    port_info.addr.port = -1;
    while (index < count) {
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_NEXT_PORT, &port_info) < 0) {
            if (errno != ENOENT) {
                generate_file_error(error, errno, "ioctl(QUERY_NEXT_PORT)");
                result = FALSE;
            }
            break;
        }

        list[index] = port_info.addr.port;
        ++index;
    }
    close(fd);

    if (!result) {
        g_free(list);
        goto end;
    }

    g_warn_if_fail(index == count);

    *entries = list;
    *entry_count = count;
end:
    close(fd);
    return result;
}

/**
 * alsaseq_get_port_info:
 * @client_id: The numeric identifier of client to query. One of [enum@SpecificClientId] is
 *             available as well as any numerica value.
 * @port_id: The numeric identifier of port in the client. One of [enum@SpecificPortId] is
 *           available as well as any numeric value.
 * @port_info: (out): A [class@PortInfo] for the port.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the information of port in client.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system calls with
 * `SNDRV_SEQ_IOCTL_GET_PORT_INFO` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_port_info(guint8 client_id, guint8 port_id, ALSASeqPortInfo **port_info,
                               GError **error)
{
    struct snd_seq_port_info *info;
    int fd;
    gboolean result;

    g_return_val_if_fail(port_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    *port_info = g_object_new(ALSASEQ_TYPE_PORT_INFO, NULL);
    seq_port_info_refer_private(*port_info, &info);

    info->addr.client = client_id;
    info->addr.port = port_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_PORT_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_PORT_INFO)");
        g_object_unref(*port_info);
        *port_info = NULL;
        result = FALSE;
    } else {
        result = TRUE;
    }

    close(fd);

    return result;
}

/**
 * alsaseq_get_client_pool:
 * @client_id: The numeric ID of client to query. One of [enum@SpecificClientId] is available as
 *             well as any numeric value.
 * @client_pool: (out): The information of memory pool for the client.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get statistical information of memory pool for the given client.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system calls with
 * `SNDRV_SEQ_IOCTL_GET_CLIENT_POOL` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_client_pool(guint8 client_id, ALSASeqClientPool **client_pool, GError **error)
{
    int fd;
    struct snd_seq_client_pool *pool;
    gboolean result;

    g_return_val_if_fail(client_pool != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    *client_pool = g_object_new(ALSASEQ_TYPE_CLIENT_POOL, NULL);
    seq_client_pool_refer_private(*client_pool, &pool);

    pool->client = client_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_CLIENT_POOL, pool) < 0) {
        generate_file_error(error, errno, "ioctl(GET_CLIENT_POOL)");
        g_object_unref(*client_pool);
        result = FALSE;
    } else {
        result = TRUE;
    }

    close(fd);

    return result;
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
 * @addr: A [struct@Addr] to query.
 * @query_type: The type of query, one of [enum@QuerySubscribeType].
 * @entries: (element-type ALSASeq.SubscribeData)(out): The array with element for subscription
 *           data.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the list of subscription for given address and query type.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system calls with
 * `SNDRV_SEQ_IOCTL_QUERY_SUBS` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_subscription_list(const ALSASeqAddr *addr,
                                       ALSASeqQuerySubscribeType query_type, GList **entries,
                                       GError **error)
{
    int fd;
    struct snd_seq_query_subs query = {0};
    unsigned int count;
    unsigned int index;
    gboolean result;

    g_return_val_if_fail(entries != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    g_object_get((gpointer)addr, "client-id", &query.root.client,
                 "port-id", &query.root.port, NULL);
    query.type = query_type;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_SUBS, &query) < 0) {
        if (errno != ENOENT)
            generate_file_error(error, errno, "ioctl(QUERY_SUBS)");
        result = FALSE;
        goto end;
    }
    count = query.num_subs;

    index = 0;
    while (index < count) {
        ALSASeqSubscribeData *subs_data = g_object_new(ALSASEQ_TYPE_SUBSCRIBE_DATA, NULL);
        struct snd_seq_port_subscribe *data;

        seq_subscribe_data_refer_private(subs_data, &data);
        fill_data_with_result(data, &query);

        *entries = g_list_append(*entries, (gpointer)subs_data);
        ++index;

        ++query.index;
        if (ioctl(fd, SNDRV_SEQ_IOCTL_QUERY_SUBS, &query) < 0) {
            if (errno != ENOENT)
                generate_file_error(error, errno, "ioctl(QUERY_SUBS)");
            result = FALSE;
            break;
        }
    }

    if (!result) {
        g_list_free_full(*entries, g_object_unref);
        goto end;
    }

    g_warn_if_fail(index == count);
end:
    close(fd);
    return result;
}

/**
 * alsaseq_get_queue_id_list:
 * @entries: (array length=entry_count)(out): The array of elements for numeric identifier of queue.
 * @entry_count: The number of entries.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the list of queue in ALSA Sequencer.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system calls with
 * `SNDRV_SEQ_IOCTL_SYSTEM_INFO` and `SNDRV_SEQ_IOCTL_GET_QUEUE_INFO` commands for ALSA
 * sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_queue_id_list(guint8 **entries, gsize *entry_count, GError **error)
{
    int fd;
    struct snd_seq_system_info info = {0};
    unsigned int maximum_count;
    unsigned int count;
    guint8 *list;
    unsigned int index;
    gboolean result;
    int i;

    g_return_val_if_fail(entries != NULL, FALSE);
    g_return_val_if_fail(entry_count != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    if (ioctl(fd, SNDRV_SEQ_IOCTL_SYSTEM_INFO, &info) < 0) {
        generate_file_error(error, errno, "ioctl(SYSTEM_INFO)");
        result = FALSE;
        goto end;
    }
    maximum_count = info.queues;
    count = info.cur_queues;

    if (count == 0) {
        result = TRUE;
        goto end;
    }

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

    result = TRUE;
end:
    close(fd);

    return result;
}

/**
 * alsaseq_get_queue_info_by_id:
 * @queue_id: The numeric ID of queue. One of [enum@SpecificQueueId] is available as well.
 * @queue_info: (out): The information of queue.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the information of queue, according to the numeric ID.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system calls with
 * `SNDRV_SEQ_IOCTL_GET_QUEUE_INFO` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_queue_info_by_id(guint8 queue_id, ALSASeqQueueInfo **queue_info,
                                      GError **error)
{
    struct snd_seq_queue_info *info;
    int fd;
    gboolean result;

    g_return_val_if_fail(queue_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    *queue_info = g_object_new(ALSASEQ_TYPE_QUEUE_INFO, NULL);
    seq_queue_info_refer_private(*queue_info, &info);

    info->queue = queue_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_QUEUE_INFO, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_QUEUE_INFO)");
        g_object_unref(*queue_info);
        result = FALSE;
    } else {
        result = TRUE;
    }

    close(fd);

    return result;
}

/**
 * alsaseq_get_queue_info_by_name:
 * @name: The name string of queue to query.
 * @queue_info: (out): The information of queue.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get the information of queue, according to the name string.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system calls with
 * `SNDRV_SEQ_IOCTL_GET_NAMED_QUEUE` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_queue_info_by_name(const gchar *name, ALSASeqQueueInfo **queue_info,
                                        GError **error)
{
    struct snd_seq_queue_info *info;
    int fd;
    gboolean result;

    g_return_val_if_fail(queue_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    *queue_info = g_object_new(ALSASEQ_TYPE_QUEUE_INFO, NULL);
    seq_queue_info_refer_private(*queue_info, &info);

    g_strlcpy(info->name, name, sizeof(info->name));
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_NAMED_QUEUE, info) < 0) {
        generate_file_error(error, errno, "ioctl(GET_NAMED_QUEUE)");
        g_object_unref(*queue_info);
        result = FALSE;
    } else {
        result = TRUE;
    }

    close(fd);

    return result;
}

/**
 * alsaseq_get_queue_status:
 * @queue_id: The numeric ID of queue. One of [enum@SpecificQueueId] is available as well.
 * @queue_status: (inout): The current status of queue.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `GLib.FileError`.
 *
 * Get current status of queue.
 *
 * The call of function executes `open(2)`, `close(2)`, and `ioctl(2)` system calls with
 * `SNDRV_SEQ_IOCTL_GET_QUEUE_STATUS` command for ALSA sequencer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_get_queue_status(guint8 queue_id, ALSASeqQueueStatus *const *queue_status,
                                  GError **error)
{
    struct snd_seq_queue_status *status;
    int fd;
    gboolean result;

    g_return_val_if_fail(queue_status != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!open_fd(&fd, error))
        return FALSE;

    g_return_val_if_fail(ALSASEQ_IS_QUEUE_STATUS(*queue_status), FALSE);
    seq_queue_status_refer_private(*queue_status, &status);

    status->queue = (int)queue_id;
    if (ioctl(fd, SNDRV_SEQ_IOCTL_GET_QUEUE_STATUS, status) < 0) {
        generate_file_error(error, errno, "ioctl(GET_QUEUE_STATUS)");
        result = FALSE;
    } else {
        result = TRUE;
    }

    close(fd);

    return result;
}
