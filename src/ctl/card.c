// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * ALSACtlCard:
 * An GObject-derived object to represent sound card.
 *
 * A [class@Card] is a GObject-derived object to represent sound card. Applications use the
 * instance of object to manipulate functionalities on sound card. After the call of
 * [method@Card.open] for the numeric ID of sound card, the object maintains file descriptor till
 * object destruction.
 */
typedef struct {
    int fd;
    char *devnode;
    gint subscribers;
    guint16 proto_ver_triplet[3];
} ALSACtlCardPrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSACtlCard, alsactl_card, G_TYPE_OBJECT)

/**
 * alsactl_card_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] with code in [enum@CardError] enumerations.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(alsactl-card-error-quark, alsactl_card_error)

static const char *const err_msgs[] = {
    [ALSACTL_CARD_ERROR_DISCONNECTED] = "The card associated to the instance is in disconnect state",
    [ALSACTL_CARD_ERROR_ELEM_NOT_FOUND] = "The control element not found in the card",
    [ALSACTL_CARD_ERROR_ELEM_NOT_SUPPORTED] = "The operation is not supported by the control element.",
    [ALSACTL_CARD_ERROR_ELEM_OWNED] = "The control element is owned by the other process.",
    [ALSACTL_CARD_ERROR_ELEM_EXIST] = "The control element already exists.",
};

#define generate_local_error(exception, code) \
    g_set_error_literal(exception, ALSACTL_CARD_ERROR, code, err_msgs[code])

#define generate_syscall_error(exception, errno, fmt, arg)                  \
    g_set_error(exception, ALSACTL_CARD_ERROR, ALSACTL_CARD_ERROR_FAILED,   \
                fmt" %d(%s)", arg, errno, strerror(errno))

typedef struct {
    GSource src;
    ALSACtlCard *self;
    gpointer tag;
    void *buf;
    unsigned int buf_len;
} CtlCardSource;

enum ctl_card_prop_type {
    CTL_CARD_PROP_DEVNODE = 1,
    CTL_CARD_PROP_SUBSCRIBED,
    CTL_CARD_PROP_COUNT,
};
static GParamSpec *ctl_card_props[CTL_CARD_PROP_COUNT] = { NULL, };

enum ctl_card_sig_type {
    CTL_CARD_SIG_HANDLE_ELEM_EVENT = 0,
    CTL_CARD_SIG_HANDLE_DISCONNECTION,
    CTL_CARD_SIG_COUNT,
};
static guint ctl_card_sigs[CTL_CARD_SIG_COUNT] = { 0 };

static void ctl_card_get_property(GObject *obj, guint id, GValue *val,
                                  GParamSpec *spec)
{
    ALSACtlCard *self = ALSACTL_CARD(obj);
    ALSACtlCardPrivate *priv = alsactl_card_get_instance_private(self);

    switch (id) {
    case CTL_CARD_PROP_DEVNODE:
        g_value_set_string(val, priv->devnode);
        break;
    case CTL_CARD_PROP_SUBSCRIBED:
    {
        gboolean subscribed = g_atomic_int_get(&priv->subscribers) > 0;
        g_value_set_boolean(val, subscribed);
        break;
    }
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, spec);
        break;
    }
}

static void ctl_card_finalize(GObject *obj)
{
    ALSACtlCard *self = ALSACTL_CARD(obj);
    ALSACtlCardPrivate *priv = alsactl_card_get_instance_private(self);

    if (priv->fd >= 0) {
        close(priv->fd);
        g_free(priv->devnode);
    }

    G_OBJECT_CLASS(alsactl_card_parent_class)->finalize(obj);
}

static void alsactl_card_class_init(ALSACtlCardClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = ctl_card_finalize;
    gobject_class->get_property = ctl_card_get_property;

    ctl_card_props[CTL_CARD_PROP_DEVNODE] =
        g_param_spec_string("devnode", "devnode",
                            "The full path of control character device.",
                            "",
                            G_PARAM_READABLE);

    ctl_card_props[CTL_CARD_PROP_SUBSCRIBED] =
        g_param_spec_boolean("subscribed", "subscribed",
                             "Whether to be subscribed for event.",
                             FALSE,
                             G_PARAM_READABLE);

    g_object_class_install_properties(gobject_class, CTL_CARD_PROP_COUNT,
                                      ctl_card_props);

    /**
     * ALSACtlCard::handle-elem-event:
     * @self: A [class@Card].
     * @elem_id: (transfer none): A [struct@ElemId].
     * @events: A set of [flags@ElemEventMask].
     *
     * Emitted when event occurs for any element.
     */
    ctl_card_sigs[CTL_CARD_SIG_HANDLE_ELEM_EVENT] =
        g_signal_new("handle-elem-event",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSACtlCardClass, handle_elem_event),
                     NULL, NULL,
                     alsactl_sigs_marshal_VOID__BOXED_FLAGS,
                     G_TYPE_NONE, 2, ALSACTL_TYPE_ELEM_ID,
                     ALSACTL_TYPE_ELEM_EVENT_MASK);

    /**
     * ALSACtlCard::handle-disconnection:
     * @self: A [class@Card].
     *
     * Emitted when the sound card is not available anymore due to unbinding driver or hot
     * unplugging. The owner of this object should call [method@GObject.Object.unref] as quickly
     * as possible to be going to release ALSA control character device.
     */
    ctl_card_sigs[CTL_CARD_SIG_HANDLE_DISCONNECTION] =
        g_signal_new("handle-disconnection",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSACtlCardClass, handle_disconnection),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0, G_TYPE_NONE, 0);
}

static void alsactl_card_init(ALSACtlCard *self)
{
    ALSACtlCardPrivate *priv = alsactl_card_get_instance_private(self);

    priv->fd = -1;
}

/**
 * alsactl_card_new:
 *
 * Allocate and return an instance of [class@Card].
 *
 * Returns: An instance of [class@Card].
 */
ALSACtlCard *alsactl_card_new()
{
    return g_object_new(ALSACTL_TYPE_CARD, NULL);
}

/**
 * alsactl_card_open:
 * @self: A [class@Card].
 * @card_id: The numeric ID of sound card.
 * @open_flag: The flag of `open(2)` system call. O_RDONLY is forced to fulfil internally.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSACtl.CardError`.
 *
 * Open ALSA control character device for the sound card.
 *
 * The call of function executes `open(2)` system call for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_open(ALSACtlCard *self, guint card_id, gint open_flag, GError **error)
{
    ALSACtlCardPrivate *priv;
    char *devnode;
    int proto_ver;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!alsactl_get_control_devnode(card_id, &devnode, error))
        return FALSE;

    open_flag |= O_RDONLY;
    priv->fd = open(devnode, open_flag);
    if (priv->fd < 0) {
        if (errno == ENODEV) {
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        } else {
            GFileError code = g_file_error_from_errno(errno);

            if (code != G_FILE_ERROR_FAILED)
                generate_file_error(error, code, "open(%s)", devnode);
            else
                generate_syscall_error(error, errno, "open(%s)", devnode);
        }

        g_free(devnode);
        return FALSE;
    }

    // Remember the version of protocol currently used.
    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_PVERSION, &proto_ver) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "PVERSION");
        close(priv->fd);
        priv->fd = -1;
        g_free(devnode);
        return FALSE;
    }

    priv->devnode = devnode;
    priv->proto_ver_triplet[0] = SNDRV_PROTOCOL_MAJOR(proto_ver);
    priv->proto_ver_triplet[1] = SNDRV_PROTOCOL_MINOR(proto_ver);
    priv->proto_ver_triplet[2] = SNDRV_PROTOCOL_MICRO(proto_ver);

    return TRUE;
}

/**
 * alsactl_card_get_protocol_version:
 * @self: A [class@Card].
 * @proto_ver_triplet: (array fixed-size=3)(out)(transfer none): The version of protocol currently
 *                     used.
 * @error: A [struct@GLib.Error].
 *
 * Get the version of control protocol currently used. The version is represented as the array with
 * three elements; major, minor, and micro version in the order. The length of major version is
 * 16 bit, the length of minor and micro version is 8 bit each.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_get_protocol_version(ALSACtlCard *self, const guint16 *proto_ver_triplet[3],
                                           GError **error)
{
    ALSACtlCardPrivate *priv;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(proto_ver_triplet != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *proto_ver_triplet = (const guint16 *)priv->proto_ver_triplet;

    return TRUE;
}

/**
 * alsactl_card_get_info:
 * @self: A [class@Card].
 * @card_info: (out): A [class@Card]Info for the sound card.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Get the information of sound card.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_CARD_INFO` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_get_info(ALSACtlCard *self, ALSACtlCardInfo **card_info, GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_card_info *info;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(card_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *card_info = g_object_new(ALSACTL_TYPE_CARD_INFO, NULL);

    ctl_card_info_refer_private(*card_info, &info);
    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_CARD_INFO, info) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "CARD_INFO");
        g_object_unref(*card_info);
        return FALSE;
    }

    return TRUE;
}

static gboolean allocate_elem_ids(int fd, struct snd_ctl_elem_list *list, GError **error)
{
    struct snd_ctl_elem_id *ids;

    // Help for deallocation.
    memset(list, 0, sizeof(*list));

    // Get the number of elements in this control device.
    if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_LIST, list) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "ELEM_LIST");
        return FALSE;
    }

    // No elements found.
    if (list->count == 0)
        return TRUE;

    // Allocate spaces for these elements.
    ids = g_malloc_n(list->count, sizeof(*ids));

    list->offset = 0;
    while (list->offset < list->count) {
        // ALSA middleware has limitation of one operation.
        // 1000 is enought less than the limitation.
        list->space = MIN(list->count - list->offset, 1000);
        list->pids = ids + list->offset;

        // Get the IDs of elements in this control device.
        if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_LIST, list) < 0) {
            if (errno == ENODEV)
                generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
            else
                generate_syscall_error(error, errno, "ioctl(%s)", "ELEM_LIST");
            free(ids);
            list->pids = NULL;
            return FALSE;
        }

        list->offset += list->space;
    }
    list->pids = ids;
    list->space = list->count;

    return TRUE;
}

static inline void deallocate_elem_ids(struct snd_ctl_elem_list *list)
{
    if (list->pids!= NULL)
        free(list->pids);
}

/**
 * alsactl_card_get_elem_id_list:
 * @self: A [class@Card].
 * @entries: (element-type ALSACtl.ElemId)(out): The list of entries for [struct@ElemId].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Generate a list of [struct@ElemId] for ALSA control character device associated to the sound
 * card.
 *
 * The call of function executes several `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_LIST`
 * command for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_get_elem_id_list(ALSACtlCard *self, GList **entries, GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_elem_list list = {0};
    int i;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(entries != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!allocate_elem_ids(priv->fd, &list, error))
        return FALSE;

    for (i = 0; i < list.count; ++i) {
        struct snd_ctl_elem_id *id = list.pids + i;
        ALSACtlElemId *elem_id = g_boxed_copy(ALSACTL_TYPE_ELEM_ID, id);
        *entries = g_list_append(*entries, (gpointer)elem_id);
    }

    deallocate_elem_ids(&list);

    return TRUE;
}

/**
 * alsactl_card_lock_elem:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @lock: whether to lock or unlock the element.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Lock/Unlock indicated element not to be written by the other processes.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_LOCK` and
 * `SNDRV_CTL_IOCTL_ELEM_UNLOCK` commands for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_lock_elem(ALSACtlCard *self, const ALSACtlElemId *elem_id, gboolean lock,
                                GError **error)
{
    ALSACtlCardPrivate *priv;
    unsigned long req;
    const char *req_name;
    int ret;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (lock) {
        req = SNDRV_CTL_IOCTL_ELEM_LOCK;
        req_name = "ELEM_LOCK";
    } else {
        req = SNDRV_CTL_IOCTL_ELEM_UNLOCK;
        req_name = "ELEM_UNLOCK";
    }

    ret = ioctl(priv->fd, req, elem_id);
    if (ret < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else if ((errno == EBUSY && lock) || (errno == EPERM && !lock))
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_OWNED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", req_name);
        return FALSE;
    }

    return TRUE;
}

static gboolean parse_enum_names(ALSACtlCardPrivate *priv, struct snd_ctl_elem_info *info,
                                 gchar ***labels, GError **error)
{
    gsize count = info->value.enumerated.items;
    int i;

    *labels = g_malloc0_n(count + 1, sizeof(**labels));

    for (i = 0; i < count; ++i) {
        info->value.enumerated.item = i;
        if (ioctl(priv->fd, SNDRV_CTL_IOCTL_ELEM_INFO, info)) {
            if (errno == ENODEV)
                generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
            else
                generate_syscall_error(error, errno, "ioctl(%s)", "ELEM_INFO");
            goto error;
        }

        (*labels)[i] = g_strdup(info->value.enumerated.name);
    }

    (*labels)[count] = NULL;
    return TRUE;
error:
    g_strfreev(*labels);
    return FALSE;
}

/**
 * alsactl_card_get_elem_info:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @elem_info: (out): A [class@ElemInfo].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Get information of element corresponding to given id.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_INFO` command
 * for ALSA control character device. For enumerated element, it executes the system call for
 * several times to retrieve all of enumeration labels.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_get_elem_info(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                    ALSACtlElemInfo **elem_info, GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_elem_info *info;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    g_return_val_if_fail(elem_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *elem_info = g_object_new(ALSACTL_TYPE_ELEM_INFO, NULL);
    ctl_elem_info_refer_private(*elem_info, &info);

    info->id = *elem_id;
    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_ELEM_INFO, info)) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "ELEM_INFO");
        return FALSE;
    }

    switch (info->type) {
    case SNDRV_CTL_ELEM_TYPE_BOOLEAN:
    case SNDRV_CTL_ELEM_TYPE_INTEGER:
    case SNDRV_CTL_ELEM_TYPE_BYTES:
    case SNDRV_CTL_ELEM_TYPE_IEC958:
    case SNDRV_CTL_ELEM_TYPE_INTEGER64:
        break;
    case SNDRV_CTL_ELEM_TYPE_ENUMERATED:
    {
        gchar **labels;
	gboolean result;

        if (!parse_enum_names(priv, info, &labels, error))
            return FALSE;

        result = alsactl_elem_info_set_enum_data(*elem_info, (const gchar **)labels, error);
        g_strfreev(labels);
	if (!result) {
            g_object_unref(*elem_info);
            return FALSE;
        }

        break;
    }
    default:
        g_return_val_if_reached(FALSE);
    }

    return TRUE;
}

/**
 * alsactl_card_write_elem_tlv:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @container: (array length=container_count): The array with qudalets for Type-Length-Value data.
 * @container_count: The number of quadlets in the container.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Write the given array of bytes as Type/Length/Value data for element pointed by the identifier.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_TLV_WRITE` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_write_elem_tlv(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                     const guint32 *container, gsize container_count,
                                     GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_tlv *packet;
    size_t container_size;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    // At least two quadlets should be included for type and length.
    g_return_val_if_fail(container != NULL, FALSE);
    g_return_val_if_fail(container_count >= 2, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    container_size = container_count * sizeof(*container);

    packet = g_malloc0(sizeof(*packet) + container_size);

    packet->numid = elem_id->numid;
    packet->length = container_size;
    memcpy(packet->tlv, container, container_size);

    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_TLV_WRITE, packet) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else if (errno == EPERM)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_OWNED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "TLV_WRITE");
        return FALSE;
    }

    g_free(packet);
    return TRUE;
}

/**
 * alsactl_card_read_elem_tlv:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @container: (array length=container_count)(inout): The array with qudalets for Type-Length-Value
 *             data.
 * @container_count: The number of quadlets in the container.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Read Type/Length/Value data from element pointed by the identifier and fulfil the given array of
 * bytes with the data.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_TLV_READ` command for
 * ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_read_elem_tlv(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                    guint32 *const *container, gsize *container_count,
                                    GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_tlv *packet;
    size_t container_size;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    // At least two quadlets should be included for type and length.
    g_return_val_if_fail(container != NULL, FALSE);
    g_return_val_if_fail(container_count != NULL && *container_count >= 2, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    container_size = *container_count * sizeof(**container);

    packet = g_malloc0(sizeof(*packet) + container_size);

    packet->numid = elem_id->numid;
    packet->length = container_size;

    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_TLV_READ, packet) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "TLV_READ");
        return FALSE;
    }

    memcpy(*container, packet->tlv, packet->length);
    *container_count = packet->length / sizeof(**container);

    g_free(packet);
    return TRUE;
}

/**
 * alsactl_card_command_elem_tlv:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @container: (array length=container_count)(inout): The array with qudalets for Type-Length-Value
 *             data.
 * @container_count: The number of quadlets in the container.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Command the given array of bytes as Type/Length/Value data for element pointed by the identifier.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_TLV_COMMAND` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_command_elem_tlv(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                       guint32 *const *container, gsize *container_count,
                                       GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_tlv *packet;
    size_t container_size;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    // At least two quadlets should be included for type and length.
    g_return_val_if_fail(container != NULL, FALSE);
    g_return_val_if_fail(container_count != NULL && *container_count >= 2, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    container_size = *container_count * sizeof(**container);

    packet = g_malloc0(sizeof(*packet) + container_size);

    packet->numid = elem_id->numid;
    packet->length = container_size;
    memcpy(packet->tlv, *container, container_size);

    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_TLV_COMMAND, packet) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else if (errno == EPERM)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_OWNED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "TLV_COMMAND");
        return FALSE;
    }

    memcpy(*container, packet->tlv, packet->length);
    *container_count = packet->length / sizeof(**container);

    g_free(packet);
    return TRUE;
}

static void prepare_enum_names(struct snd_ctl_elem_info *info, const gchar **labels)
{
    unsigned int count;
    unsigned int length;
    char *pos;

    length = 0;
    for (count = 0; labels[count] != NULL; ++count) {
        const gchar *label = labels[count];

        g_return_if_fail(strlen(label) < 64);

        length += strlen(label) + 1;
    }

    g_return_if_fail(length <= 64 * 1024);

    pos = g_malloc0(length);

    info->value.enumerated.names_ptr = (__u64)pos;
    info->value.enumerated.names_length = length;

    for (count = 0; labels[count] != NULL; ++count) {
        const gchar *label = labels[count];
        strcpy(pos, label);
        pos[strlen(label)] = '\0';
        pos += strlen(label) + 1;
    }
    info->value.enumerated.items = count;
}

static gboolean add_or_replace_elems(int fd, const ALSACtlElemId *elem_id, guint elem_count,
                                     ALSACtlElemInfo *elem_info, gboolean replace, GList **entries,
                                     GError **error)
{
    struct snd_ctl_elem_info *info;
    long request;
    const char *req_name;
    gboolean result;
    int i;

    ctl_elem_info_refer_private(elem_info, &info);

    switch (info->type) {
    case SNDRV_CTL_ELEM_TYPE_BOOLEAN:
    case SNDRV_CTL_ELEM_TYPE_INTEGER:
    case SNDRV_CTL_ELEM_TYPE_BYTES:
    case SNDRV_CTL_ELEM_TYPE_IEC958:
    case SNDRV_CTL_ELEM_TYPE_INTEGER64:
        break;
    case SNDRV_CTL_ELEM_TYPE_ENUMERATED:
    {
        const gchar **labels;

        if (!alsactl_elem_info_get_enum_data(elem_info, &labels, error))
            return FALSE;

        prepare_enum_names(info, labels);

        break;
    }
    default:
        g_return_val_if_reached(FALSE);
    }

    info->id = *elem_id;

    if (!replace) {
        request = SNDRV_CTL_IOCTL_ELEM_ADD;
        req_name = "ELEM_ADD";
    } else {
        request = SNDRV_CTL_IOCTL_ELEM_REPLACE;
        req_name = "ELEM_REPLACE";
    }

    info->owner = (__kernel_pid_t)elem_count;
    if (ioctl(fd, request, info) < 0) {
        if (errno == ENODEV) {
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        } else if (errno == ENOENT) {
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        } else if (errno == EBUSY) {
            if (replace)
                generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_OWNED);
            else
                generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_EXIST);
        } else {
            generate_syscall_error(error, errno, "ioctl(%s)", req_name);
        }
        result = FALSE;
    } else {
        result = TRUE;
    }
    g_free((void *)info->value.enumerated.names_ptr);
    if (!result)
        return FALSE;

    for (i = 0; i < elem_count; ++i) {
        ALSACtlElemId *entry = g_boxed_copy(ALSACTL_TYPE_ELEM_ID, &info->id);
        *entries = g_list_append(*entries, (gpointer)entry);

        ++info->id.numid;
        ++info->id.index;
    }

    return TRUE;
}

/**
 * alsactl_card_add_elems:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @elem_count: The number of elements going to be added.
 * @elem_info: A [class@ElemInfo].
 * @entries: (element-type ALSACtl.ElemId)(out): The list of added element identifiers.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Add the user-defined elements and return the list of their identifier.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_ADD` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_add_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id, guint elem_count,
                                ALSACtlElemInfo *elem_info, GList **entries, GError **error)
{
    ALSACtlCardPrivate *priv;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    g_return_val_if_fail(elem_count > 0, FALSE);
    g_return_val_if_fail(ALSACTL_IS_ELEM_INFO(elem_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return add_or_replace_elems(priv->fd, elem_id, elem_count, elem_info, FALSE, entries, error);
}

/**
 * alsactl_card_replace_elems:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @elem_count: The number of elements going to be added.
 * @elem_info: A [class@ElemInfo].
 * @entries: (element-type ALSACtl.ElemId)(out): The list of renewed element identifiers.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Add user-defined elements to replace the existent ones.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_REPLACE` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_replace_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                    guint elem_count, ALSACtlElemInfo *elem_info, GList **entries,
                                    GError **error)
{
    ALSACtlCardPrivate *priv;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    g_return_val_if_fail(elem_count > 0, FALSE);
    g_return_val_if_fail(ALSACTL_IS_ELEM_INFO(elem_info), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return add_or_replace_elems(priv->fd, elem_id, elem_count, elem_info, TRUE, entries, error);
}

/**
 * alsactl_card_remove_elems:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Remove user-defined elements pointed by the identifier.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_REMOVE` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_remove_elems(ALSACtlCard *self, const ALSACtlElemId *elem_id, GError **error)
{
    ALSACtlCardPrivate *priv;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_ELEM_REMOVE, elem_id) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else if (errno == EBUSY)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_OWNED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "ELEM_REMOVE");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsactl_card_write_elem_value:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @elem_value: A derivative of #ALSACtlElemValue.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Write given value to element indicated by the given identifier.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_WRITE` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_write_elem_value(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                       const ALSACtlElemValue *elem_value, GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_elem_value *value;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    g_return_val_if_fail(ALSACTL_IS_ELEM_VALUE((ALSACtlElemValue *)elem_value), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    ctl_elem_value_refer_private((ALSACtlElemValue *)elem_value, &value);
    value->id = *elem_id;

    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_ELEM_WRITE, value) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else if (errno == EPERM)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_SUPPORTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "ELEM_WRITE");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsactl_card_read_elem_value:
 * @self: A [class@Card].
 * @elem_id: A [struct@ElemId].
 * @elem_value: (inout): A derivative of #ALSACtlElemValue.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Read given value from element indicated by the given identifier.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_ELEM_READ` command
 * for ALSA control character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_read_elem_value(ALSACtlCard *self, const ALSACtlElemId *elem_id,
                                      ALSACtlElemValue *const *elem_value, GError **error)
{
    ALSACtlCardPrivate *priv;
    struct snd_ctl_elem_value *value;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);

    g_return_val_if_fail(elem_id != NULL, FALSE);
    g_return_val_if_fail(elem_value != NULL && ALSACTL_IS_ELEM_VALUE(*elem_value), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    ctl_elem_value_refer_private(*elem_value, &value);
    value->id = *elem_id;

    if (ioctl(priv->fd, SNDRV_CTL_IOCTL_ELEM_READ, value) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
        else if (errno == ENOENT)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_FOUND);
        else if (errno == EPERM)
            generate_local_error(error, ALSACTL_CARD_ERROR_ELEM_NOT_SUPPORTED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "ELEM_READ");
        return FALSE;
    }
    return TRUE;
}

static void handle_elem_event(CtlCardSource *src, struct snd_ctl_event *ev)
{
    ALSACtlCard *self = src->self;
    ALSACtlElemId *elem_id;
    ALSACtlElemEventMask mask;

    elem_id = &ev->data.elem.id;

    if (ev->data.elem.mask != SNDRV_CTL_EVENT_MASK_REMOVE)
        mask = ev->data.elem.mask;
    else
        mask = ALSACTL_ELEM_EVENT_MASK_REMOVE;

    g_signal_emit(self, ctl_card_sigs[CTL_CARD_SIG_HANDLE_ELEM_EVENT], 0,
                  elem_id, mask);
}

static gboolean ctl_card_check_src(GSource *gsrc)
{
    CtlCardSource *src = (CtlCardSource *)gsrc;
    GIOCondition condition;

    // Don't go to dispatch if nothing available. As an exception, return TRUE
    // for POLLERR to call .dispatch for internal destruction.
    condition = g_source_query_unix_fd(gsrc, src->tag);
    return !!(condition & (G_IO_IN | G_IO_ERR));
}

static gboolean ctl_card_dispatch_src(GSource *gsrc, GSourceFunc cb,
                                      gpointer user_data)
{
    CtlCardSource *src = (CtlCardSource *)gsrc;
    ALSACtlCard *self = src->self;
    ALSACtlCardPrivate *priv;
    GIOCondition condition;
    int len;
    struct snd_ctl_event *ev;

    priv = alsactl_card_get_instance_private(self);
    if (priv->fd < 0)
        return G_SOURCE_REMOVE;

    condition = g_source_query_unix_fd(gsrc, src->tag);
    if (condition & G_IO_ERR) {
        g_signal_emit(self,
                      ctl_card_sigs[CTL_CARD_SIG_HANDLE_DISCONNECTION],
                      0, NULL);
        return G_SOURCE_REMOVE;
    }

    len = read(priv->fd, src->buf, src->buf_len);
    if (len < 0) {
        if (errno == EAGAIN)
            return G_SOURCE_CONTINUE;

        return G_SOURCE_REMOVE;
    }

    ev = src->buf;
    while (len >= sizeof(*ev)) {
        if (ev->type == SNDRV_CTL_EVENT_ELEM)
            handle_elem_event(src, ev);

        len -= sizeof(*ev);
        ++ev;
    }

    // Just be sure to continue to process this source.
    return G_SOURCE_CONTINUE;
}

static void ctl_card_finalize_src(GSource *gsrc)
{
    CtlCardSource *src = (CtlCardSource *)gsrc;
    ALSACtlCardPrivate *priv = alsactl_card_get_instance_private(src->self);

    // Unsubscribe events.
    if (g_atomic_int_dec_and_test(&priv->subscribers)) {
        int subscribe = 0;
        ioctl(priv->fd, SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS, &subscribe);
    }

    g_free(src->buf);
    g_object_unref(src->self);
}

/**
 * alsactl_card_create_source:
 * @self: A [class@Card].
 * @gsrc: (out): A [struct@GLib.Source] to handle events from ALSA control character device.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSACtl.CardError`.
 *
 * Allocate [struct@GLib.Source] structure to handle events from ALSA control character device. In
 * each iteration of [struct@GLib.MainContext], the `read(2)` system call is executed to dispatch
 * control event for [signal@Card::handle-elem-event] signal, according to the result of `poll(2)`
 * system call.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsactl_card_create_source(ALSACtlCard *self, GSource **gsrc, GError **error)
{
    static GSourceFuncs funcs = {
            .check          = ctl_card_check_src,
            .dispatch       = ctl_card_dispatch_src,
            .finalize       = ctl_card_finalize_src,
    };
    ALSACtlCardPrivate *priv;
    CtlCardSource *src;
    long page_size = sysconf(_SC_PAGESIZE);
    void *buf;

    g_return_val_if_fail(ALSACTL_IS_CARD(self), FALSE);
    priv = alsactl_card_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(gsrc != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    buf = g_malloc0(page_size);

    *gsrc = g_source_new(&funcs, sizeof(CtlCardSource));
    src = (CtlCardSource *)(*gsrc);

    g_source_set_name(*gsrc, "ALSACtlCard");
    g_source_set_priority(*gsrc, G_PRIORITY_HIGH_IDLE);
    g_source_set_can_recurse(*gsrc, TRUE);

    src->self = g_object_ref(self);
    src->tag = g_source_add_unix_fd(*gsrc, priv->fd, G_IO_IN);
    src->buf = buf;
    src->buf_len = page_size;

    // Subscribe any event.
    {
        int subscribe = 1;

        g_atomic_int_inc(&priv->subscribers);

        if (ioctl(priv->fd, SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS, &subscribe)) {
            if (errno == ENODEV)
                generate_local_error(error, ALSACTL_CARD_ERROR_DISCONNECTED);
            else
                generate_syscall_error(error, errno, "ioctl(%s)", "SUBSCRIBE_EVENTS");
            g_source_unref(*gsrc);
            return FALSE;
        }
    }

    return TRUE;
}
