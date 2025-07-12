// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

/**
 * ALSATimerUserInstance:
 * A GObject-derived object to express user instance.
 *
 * A [class@UserInstance] is a GObject-derived object to express information of user instance
 * attached to any timer device or the other instance as slave. After calling
 * [method@UserInstance.open], the object maintains file descriptor till object destruction. After
 * calling [method@UserInstance.attach] or [method@UserInstance.attach_as_slave], the user instance
 * is attached to any timer device or the other instance as slave.
 */
typedef struct {
    int fd;
    ALSATimerEventType event_type;
    guint16 proto_ver_triplet[3];
} ALSATimerUserInstancePrivate;
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerUserInstance, alsatimer_user_instance, G_TYPE_OBJECT)

/**
 * alsatimer_user_instance_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] which has code in
 * `ALSATimer.UserInstanceError`.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(alsatimer-user-instance-error-quark, alsatimer_user_instance_error)

static const char *const err_msgs[] = {
    [ALSATIMER_USER_INSTANCE_ERROR_TIMER_NOT_FOUND] = "The timer instance is not found",
    [ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED] = "The timer instance is not attached to any timer device or the other instance",
    [ALSATIMER_USER_INSTANCE_ERROR_ATTACHED] = "The timer instance is already attached to timer device or the other instance",
};

#define generate_local_error(exception, code) \
    g_set_error_literal(exception, ALSATIMER_USER_INSTANCE_ERROR, code, err_msgs[code])

#define generate_syscall_error(exception, errno, fmt, arg)                                      \
    g_set_error(exception, ALSATIMER_USER_INSTANCE_ERROR, ALSATIMER_USER_INSTANCE_ERROR_FAILED, \
                fmt" %d(%s)", arg, errno, strerror(errno))

typedef struct {
    GSource src;
    ALSATimerUserInstance *self;
    gpointer tag;
    void *buf;
    unsigned int buf_len;
} TimerUserInstanceSource;

enum timer_user_instance_sig_type {
    TIMER_USER_INSTANCE_SIG_HANDLE_TICK_TIME_EVENT = 0,
    TIMER_USER_INSTANCE_SIG_HANDLE_REAL_TIME_EVENT,
    TIMER_USER_INSTANCE_SIG_HANDLE_DISCONNECTION,
    TIMER_USER_INSTANCE_SIG_COUNT,
};
static guint timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_COUNT] = { 0 };

static void timer_user_instance_finalize(GObject *obj)
{
    ALSATimerUserInstance *self = ALSATIMER_USER_INSTANCE(obj);
    ALSATimerUserInstancePrivate *priv =
                            alsatimer_user_instance_get_instance_private(self);

    if (priv->fd >= 0)
        close(priv->fd);

    G_OBJECT_CLASS(alsatimer_user_instance_parent_class)->finalize(obj);
}

static void alsatimer_user_instance_class_init(ALSATimerUserInstanceClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = timer_user_instance_finalize;

    /**
     * ALSATimerUserInstance::handle-tick-time-event:
     * @self: A [class@UserInstance].
     * @event: (transfer none): The instance of [struct@TickTimeEvent].
     *
     * Emitted when event occurs to notify tick time.
     */
    timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_TICK_TIME_EVENT] =
        g_signal_new("handle-tick-time-event",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSATimerUserInstanceClass, handle_tick_time_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOXED,
                     G_TYPE_NONE, 1, ALSATIMER_TYPE_TICK_TIME_EVENT);

    /**
     * ALSATimerUserInstance::handle-real-time-event:
     * @self: A [class@UserInstance].
     * @event: (transfer none): The instance of [struct@RealTimeEvent].
     *
     * Emitted when event occurs to notify real time.
     */
    timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_REAL_TIME_EVENT] =
        g_signal_new("handle-real-time-event",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSATimerUserInstanceClass, handle_real_time_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOXED,
                     G_TYPE_NONE, 1, ALSATIMER_TYPE_REAL_TIME_EVENT);

    /**
     * ALSATimerUserInstance::handle-disconnection:
     * @self: A [class@UserInstance].
     *
     * Emitted when the attached timer device is not available anymore due to unbinding driver or
     * hot unplugging. The owner of this object should call [method@GObject.Object.unref] as quickly
     * as possible to release ALSA timer character device.
     */
    timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_DISCONNECTION] =
        g_signal_new("handle-disconnection",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSATimerUserInstanceClass, handle_disconnection),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0, G_TYPE_NONE, 0);
}

static void alsatimer_user_instance_init(ALSATimerUserInstance *self)
{
    ALSATimerUserInstancePrivate *priv =
                            alsatimer_user_instance_get_instance_private(self);

    priv->fd = -1;
}

/**
 * alsatimer_user_instance_open:
 * @self: A [class@UserInstance].
 * @open_flag: The flag of `open(2)` system call. `O_RDONLY` is forced to fulfil internally.
 * @error: A [struct@GLib.Error]. Error is generated with two domains; `GLib.FileError` and
 *         `ALSATimer.UserInstanceError`.
 *
 * Open ALSA Timer character device to allocate queue.
 *
 * The call of function executes `open(2)` system call for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_open(ALSATimerUserInstance *self, gint open_flag, GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    char *devnode;
    int proto_ver;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!alsatimer_get_devnode(&devnode, error))
        return FALSE;

    open_flag |= O_RDONLY;
    priv->fd = open(devnode, open_flag);
    if (priv->fd < 0) {
        GFileError code = g_file_error_from_errno(errno);

        if (code != G_FILE_ERROR_FAILED)
            generate_file_error(error, code, "open(%s)", devnode);
        else
            generate_syscall_error(error, errno, "open(%s)", devnode);

        g_free(devnode);
        return FALSE;
    }
    g_free(devnode);

    // Remember the version of protocol currently used.
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PVERSION, &proto_ver) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "PVERSION");
        close(priv->fd);
        priv->fd = -1;
        return FALSE;
    }

    priv->proto_ver_triplet[0] = SNDRV_PROTOCOL_MAJOR(proto_ver);
    priv->proto_ver_triplet[1] = SNDRV_PROTOCOL_MINOR(proto_ver);
    priv->proto_ver_triplet[2] = SNDRV_PROTOCOL_MICRO(proto_ver);

    return TRUE;
}

/**
 * alsatimer_user_instance_new:
 *
 * Allocate and return an instance of [class@UserInstance].
 *
 * Returns: An instance of [class@UserInstance].
 */
ALSATimerUserInstance *alsatimer_user_instance_new()
{
    return g_object_new(ALSATIMER_TYPE_USER_INSTANCE, NULL);
}

/**
 * alsatimer_user_instance_get_protocol_version:
 * @self: A [class@UserInstance].
 * @proto_ver_triplet: (array fixed-size=3)(out)(transfer none): The version of protocol currently
 *                     used.
 * @error: A [struct@GLib.Error].
 *
 * Get the version of timer protocol currently used. The version is expressed as the array with
 * three elements; major, minor, and micro version in the order. The length of major version is
 * 16 bit, the length of minor and micro version is 8 bit each.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_get_protocol_version(ALSATimerUserInstance *self,
                                        const guint16 *proto_ver_triplet[3],
                                        GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(proto_ver_triplet != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *proto_ver_triplet = (const guint16 *)priv->proto_ver_triplet;

    return TRUE;
}

/**
 * alsatimer_user_instance_choose_event_type:
 * @self: A [class@UserInstance].
 * @event_type: The type of event data, one of [enum@EventType].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Choose the type of event data to receive.
 *
 * The call of function is successful just before call of [method@UserInstance.attach].
 * [enum@EventType].TICK_TIME is used as a default if the function is not called for
 * [enum@EventType].REAL_TIME explicitly. When the former is configured, event for tick time is
 * available for [signal@UserInstance::handle_tick_time_event]. When the latter is configured,
 * event for real time is available for [signal@UserInstance::handle_real_time_event].
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_TREAD` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_choose_event_type(ALSATimerUserInstance *self,
                                        ALSATimerEventType event_type,
                                        GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    int tread;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    tread = (int)event_type;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_TREAD, &tread) < 0) {
        if (errno == EBUSY)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "TREAD");
        return FALSE;
    }

    priv->event_type = event_type;
    return TRUE;
}

/**
 * alsatimer_user_instance_attach:
 * @self: A [class@UserInstance].
 * @device_id: A [struct@DeviceId] to which the instance is attached.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Attach the instance to the timer device. If the given device_id is for absent timer device, the
 * instance can be detached with error.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_SELECT` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_attach(ALSATimerUserInstance *self, ALSATimerDeviceId *device_id,
                                        GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_select sel = {0};

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(device_id != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    sel.id = *device_id;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_SELECT, &sel) < 0) {
        if (errno == ENODEV)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_TIMER_NOT_FOUND);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "SELECT");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_user_instance_attach_as_slave:
 * @self: A [class@UserInstance].
 * @slave_class: The class identifier of master instance, one of [enum@SlaveClass].
 * @slave_id: The numeric identifier of master instance.
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Attach the instance as an slave to another instance indicated by a pair of slave_class and
 * slave_id. If the slave_class is [enum@SlaveClass].APPLICATION, the slave_id is for the PID of
 * application process which owns the instance of timer. If the slave_class is
 * [enum@SlaveClass].SEQUENCER, the slave_id is the numeric ID of queue bound for timer device.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_SELECT` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_attach_as_slave(ALSATimerUserInstance *self,
                                                 ALSATimerSlaveClass slave_class,
                                                 int slave_id,
                                                 GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_select sel = {0};

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    sel.id.dev_class = SNDRV_TIMER_CLASS_SLAVE;
    sel.id.dev_sclass = slave_class;
    sel.id.device = slave_id;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_SELECT, &sel) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "SELECT");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_user_instance_get_info:
 * @self: A [class@UserInstance].
 * @instance_info: (out): A [class@InstanceInfo].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Return the information of device if attached to the instance.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_INFO` command for
 * ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_get_info(ALSATimerUserInstance *self,
                                          ALSATimerInstanceInfo **instance_info,
                                          GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_info *info;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(instance_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    *instance_info = g_object_new(ALSATIMER_TYPE_INSTANCE_INFO, NULL);
    timer_instance_info_refer_private(*instance_info, &info);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_INFO, info) < 0) {
        if (errno == EBADFD)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "INFO");
        g_object_unref(*instance_info);
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_user_instance_set_params:
 * @self: A [class@UserInstance].
 * @instance_params: (inout): A [class@InstanceParams].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Configure the instance with the parameters and return the latest parameters.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_PARAMS` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_set_params(ALSATimerUserInstance *self,
                                ALSATimerInstanceParams *const *instance_params,
                                GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_params *params;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(instance_params != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    timer_instance_params_refer_private(*instance_params, &params);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PARAMS, params) < 0) {
        if (errno == EBADFD)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "PARAMS");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_user_instance_get_status:
 * @self: A [class@UserInstance].
 * @instance_status: (inout): A [class@InstanceStatus].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Get the latest status of instance.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_STATUS` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_get_status(ALSATimerUserInstance *self,
                                ALSATimerInstanceStatus *const *instance_status,
                                GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_status *status;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(instance_status != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    g_return_val_if_fail(ALSATIMER_IS_INSTANCE_STATUS(*instance_status), FALSE);
    timer_instance_status_refer_private(*instance_status, &status);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_STATUS, status) < 0) {
        if (errno == EBADFD)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "STATUS");
        return FALSE;
    }

    return TRUE;
}

static gboolean timer_user_instance_check_src(GSource *gsrc)
{
    TimerUserInstanceSource *src = (TimerUserInstanceSource *)gsrc;
    GIOCondition condition;

    // Don't go to dispatch if nothing available. As an exception, return TRUE
    // for POLLERR to call .dispatch for internal destruction.
    condition = g_source_query_unix_fd(gsrc, src->tag);
    return !!(condition & (G_IO_IN | G_IO_ERR));
}

static void dispatch_tick_time_events(ALSATimerUserInstance *self, const guint8 *buf, gsize length)
{
    const struct snd_timer_read *ev;

    while (length >= sizeof(*ev)) {
        ev = (const struct snd_timer_read *)buf;

        g_signal_emit(self,
                      timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_TICK_TIME_EVENT],
                      0, ev);

        length -= sizeof(*ev);
        buf += sizeof(*ev);
    }
}

static void dispatch_real_time_events(ALSATimerUserInstance *self, const guint8 *buf, gsize length)
{
    const struct snd_timer_tread *ev;

    while (length >= sizeof(*ev)) {
        const struct snd_timer_tread *ev = (const struct snd_timer_tread *)buf;

        g_signal_emit(self,
                      timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_REAL_TIME_EVENT],
                      0, ev);

        length -= sizeof(*ev);
        buf += sizeof(*ev);
    }
}

static gboolean timer_user_instance_dispatch_src(GSource *gsrc, GSourceFunc cb,
                                      gpointer user_data)
{
    TimerUserInstanceSource *src = (TimerUserInstanceSource *)gsrc;
    ALSATimerUserInstance *self = src->self;
    ALSATimerUserInstancePrivate *priv;
    GIOCondition condition;
    ssize_t len;

    priv = alsatimer_user_instance_get_instance_private(self);
    if (priv->fd < 0)
        return G_SOURCE_REMOVE;

    condition = g_source_query_unix_fd(gsrc, src->tag);
    if (condition & G_IO_ERR) {
        g_signal_emit(self,
            timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_DISCONNECTION],
            0, NULL);
        return G_SOURCE_REMOVE;
    }

    len = read(priv->fd, src->buf, src->buf_len);
    if (len < 0) {
        if (errno == EAGAIN)
            return G_SOURCE_CONTINUE;

        return G_SOURCE_REMOVE;
    }

    switch (priv->event_type) {
    case ALSATIMER_EVENT_TYPE_TICK_TIME:
        dispatch_tick_time_events(self, src->buf, (size_t)len);
        break;
    case ALSATIMER_EVENT_TYPE_REAL_TIME:
        dispatch_real_time_events(self, src->buf, (size_t)len);
        break;
    default:
        break;
    }

    // Just be sure to continue to process this source.
    return G_SOURCE_CONTINUE;
}

static void timer_user_instance_finalize_src(GSource *gsrc)
{
    TimerUserInstanceSource *src = (TimerUserInstanceSource *)gsrc;

    g_free(src->buf);
    g_object_unref(src->self);
}

/**
 * alsatimer_user_instance_create_source:
 * @self: A [class@UserInstance].
 * @gsrc: (out): A [struct@GLib.Source] to handle events from ALSA timer character device.
 * @error: A [struct@GLib.Error].
 *
 * Allocate [struct@GLib.Source] structure to handle events from ALSA timer character device. In
 * each iteration of [struct@GLib.MainContext], the `read(2)` system call is executed to dispatch
 * timer event for either [signal@UserInstance::handle-tick-time-event] or
 * [signal@UserInstance::handle-real-time-event] signals, according to the result of `poll(2)`
 * system call.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_create_source(ALSATimerUserInstance *self, GSource **gsrc,
                                               GError **error)
{
    static GSourceFuncs funcs = {
            .check          = timer_user_instance_check_src,
            .dispatch       = timer_user_instance_dispatch_src,
            .finalize       = timer_user_instance_finalize_src,
    };
    ALSATimerUserInstancePrivate *priv;
    TimerUserInstanceSource *src;
    long page_size = sysconf(_SC_PAGESIZE);
    void *buf;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);
    g_return_val_if_fail(priv->fd >= 0, FALSE);

    g_return_val_if_fail(gsrc != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    buf = g_malloc0(page_size);

    *gsrc = g_source_new(&funcs, sizeof(TimerUserInstanceSource));
    src = (TimerUserInstanceSource *)(*gsrc);

    g_source_set_name(*gsrc, "ALSATimerUserInstance");
    g_source_set_priority(*gsrc, G_PRIORITY_HIGH_IDLE);
    g_source_set_can_recurse(*gsrc, TRUE);

    src->self = g_object_ref(self);
    src->tag = g_source_add_unix_fd(*gsrc, priv->fd, G_IO_IN);
    src->buf = buf;
    src->buf_len = page_size;

    return TRUE;
}

/**
 * alsatimer_user_instance_start:
 * @self: A [class@UserInstance].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Start timer event emission.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_START` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_start(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_START) < 0) {
        if (errno == EBADFD)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "START");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_user_instance_stop:
 * @self: A [class@UserInstance].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Stop timer event emission.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_STOP` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_stop(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_STOP) < 0) {
        if (errno == EBADFD)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "STOP");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_user_instance_pause:
 * @self: A [class@UserInstance].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Pause timer event emission.
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_PAUSE` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_pause(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PAUSE) < 0) {
        if (errno == EBADFD)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "PAUSE");
        return FALSE;
    }

    return TRUE;
}

/**
 * alsatimer_user_instance_continue:
 * @self: A [class@UserInstance].
 * @error: A [struct@GLib.Error]. Error is generated with domain of `ALSATimer.UserInstanceError`.
 *
 * Continue timer event emission paused by [method@UserInstance.pause].
 *
 * The call of function executes `ioctl(2)` system call with `SNDRV_TIMER_IOCTL_CONTINUE` command
 * for ALSA timer character device.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsatimer_user_instance_continue(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_val_if_fail(ALSATIMER_IS_USER_INSTANCE(self), FALSE);
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_CONTINUE) < 0) {
        if (errno == EBADFD)
            generate_local_error(error, ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED);
        else
            generate_syscall_error(error, errno, "ioctl(%s)", "CONTINUE");
        return FALSE;
    }

    return TRUE;
}
