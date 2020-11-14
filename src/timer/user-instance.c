// SPDX-License-Identifier: LGPL-3.0-or-later
#include "user-instance.h"
#include "query.h"
#include "privates.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

/**
 * SECTION: user-instance
 * @Title: ALSATimerUserInstance
 * @Short_description: A GObject-derived object to represent user instance
 *
 * A #ALSATimerUserInstance is a GObject-derived object to represent information
 * of user instance attached to any timer device or the other instance as slave.
 * After calling alsatimer_user_instance_open(), the object maintains file
 * descriptor till object destruction. After calling
 * alsatimer_user_instance_attach() or alsatimer_user_instance_attach_as_slave(),
 * the user instance is attached to any timer device or the other instance as
 * slave.
 */
struct _ALSATimerUserInstancePrivate {
    int fd;
    ALSATimerEventDataType event_data_type;
    guint16 proto_ver_triplet[3];
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerUserInstance, alsatimer_user_instance, G_TYPE_OBJECT)

/**
 * alsatimer_user_instance_error_quark:
 *
 * Return the GQuark for error domain of GError which has code in #ALSATimerUserInstanceError enumerations.
 *
 * Returns: A #GQuark.
 */
G_DEFINE_QUARK(alsatimer-user-instance-error-quark, alsatimer_user_instance_error)

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
    TIMER_USER_INSTANCE_SIG_HANDLE_EVENT = 0,
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
     * ALSATimerUserInstance::handle-event:
     * @self: A #ALSATimerUserInstance.
     * @event: (transfer none): The instance of #ALSATimerEvent.
     *
     * When event occurs for any element, this signal is emit.
     */
    timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_EVENT] =
        g_signal_new("handle-event",
                     G_OBJECT_CLASS_TYPE(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ALSATimerUserInstanceClass, handle_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, ALSATIMER_TYPE_EVENT);

    /**
     * ALSATimerUserInstance::handle-disconnection:
     * @self: A #ALSATimerUserInstance.
     *
     * When the attached timer device is not available anymore due to unbinding
     * driver or hot unplugging, this signal is emit. The owner of this object
     * should call g_object_free() as quickly as possible to release ALSA timer
     * character device.
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
 * @self: A #ALSATimerUserInstance.
 * @open_flag: The flag of open(2) system call. O_RDONLY is forced to fulfil internally.
 * @error: A #GError.
 *
 * Open ALSA Timer character device to allocate queue.
 *
 * The call of function executes open(2) system call for ALSA timer character
 * device.
 */
void alsatimer_user_instance_open(ALSATimerUserInstance *self, gint open_flag,
                                  GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    char *devnode;
    int proto_ver;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(error == NULL || *error == NULL);

    alsatimer_get_devnode(&devnode, error);
    if (*error != NULL)
        return;

    open_flag |= O_RDONLY;
    priv->fd = open(devnode, open_flag);
    g_free(devnode);
    if (priv->fd < 0) {
        generate_error(error, errno);
        return;
    }

    // Remember the version of protocol currently used.
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PVERSION, &proto_ver) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "PVERSION");
        close(priv->fd);
        priv->fd = -1;
        return;
    }

    priv->proto_ver_triplet[0] = SNDRV_PROTOCOL_MAJOR(proto_ver);
    priv->proto_ver_triplet[1] = SNDRV_PROTOCOL_MINOR(proto_ver);
    priv->proto_ver_triplet[2] = SNDRV_PROTOCOL_MICRO(proto_ver);
}

ALSATimerUserInstance *alsatimer_user_instance_new()
{
    return g_object_new(ALSATIMER_TYPE_USER_INSTANCE, NULL);
}

/**
 * alsatimer_user_instance_get_protocol_version:
 * @self: A #ALSATimerUserInstance.
 * @proto_ver_triplet: (array fixed-size=3)(out)(transfer none): The version of
 *                     protocol currently used.
 * @error: A #GError.
 *
 * Get the version of timer protocol currently used. The version is
 * represented as the array with three elements; major, minor, and micro version
 * in the order. The length of major version is 16 bit, the length of minor
 * and micro version is 8 bit each.
 */
void alsatimer_user_instance_get_protocol_version(ALSATimerUserInstance *self,
                                        const guint16 *proto_ver_triplet[3],
                                        GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);
    g_return_if_fail(priv->fd >= 0);

    g_return_if_fail(proto_ver_triplet != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    *proto_ver_triplet = (const guint16 *)priv->proto_ver_triplet;
}

/**
 * alsatimer_user_instance_choose_event_data_type:
 * @self: A #ALSATimerUserInstance.
 * @event_data_type: The type of event data, one of ALSATimerEventDataType.
 * @error: A #GError.
 *
 * Choose the type of event data to receive.
 *
 * The call of function is successful just before the instance is not attached
 * yet. ALSATIMER_EVENT_DATA_TYPE_TICK is used as a default if the function is
 * not called for ALSATIMER_EVENT_DATA_TYPE_TSTAMP explicitly.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_TREAD command for ALSA timer character device.
 */
void alsatimer_user_instance_choose_event_data_type(ALSATimerUserInstance *self,
                                        ALSATimerEventDataType event_data_type,
                                        GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    int tread;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(error == NULL || *error == NULL);

    tread = (int)event_data_type;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_TREAD, &tread) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "TREAD");
    else
        priv->event_data_type = event_data_type;
}

/**
 * alsatimer_user_instance_attach:
 * @self: A #ALSATimerUserInstance.
 * @device_id: A #ALSATimerDeviceId to which the instance is attached.
 * @error: A #GError.
 *
 * Attach the instance to the timer device. If the given device_id is for
 * absent timer device, the instance can be detached with error.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_SELECT command for ALSA timer character device.
 */
void alsatimer_user_instance_attach(ALSATimerUserInstance *self,
                                    ALSATimerDeviceId *device_id,
                                    GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_select sel = {0};

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(device_id != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    sel.id = *device_id;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_SELECT, &sel) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "SELECT");
}

/**
 * alsatimer_user_instance_attach_as_slave:
 * @self: A #ALSATimerUserInstance.
 * @slave_class: The class identifier of master instance, one of
 *               #ALSATimerSlaveClass.
 * @slave_id: The numerical identifier of master instance.
 * @error: A #GError.
 *
 * Attach the instance as an slave to another instance indicated by a pair of
 * slave_class and slave_id. If the slave_class is for application
 * (=ALSATIMER_SLAVE_CLASS_APPLICATION), the slave_id is for the PID of
 * application process which owns the instance of timer. If the slave_class is
 * for ALSA sequencer (=ALSATIMER_SLAVE_CLASS_SEQUENCER), the slave_id is the
 * numerical ID of queue bound for timer device.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_SELECT command for ALSA timer character device.
 */
void alsatimer_user_instance_attach_as_slave(ALSATimerUserInstance *self,
                                        ALSATimerSlaveClass slave_class,
                                        int slave_id,
                                        GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_select sel = {0};

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(error == NULL || *error == NULL);

    sel.id.dev_class = SNDRV_TIMER_CLASS_SLAVE;
    sel.id.dev_sclass = slave_class;
    sel.id.device = slave_id;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_SELECT, &sel) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "SELECT");
}

/**
 * alsatimer_user_instance_get_info:
 * @self: A #ALSATimerUserInstance.
 * @instance_info: (out): A #ALSATimerInstanceInfo.
 * @error: A #GError.
 *
 * Return the information of device if attached to the instance.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_INFO command for ALSA timer character device.
 */
void alsatimer_user_instance_get_info(ALSATimerUserInstance *self,
                                      ALSATimerInstanceInfo **instance_info,
                                      GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_info *info;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(instance_info != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    *instance_info = g_object_new(ALSATIMER_TYPE_INSTANCE_INFO, NULL);
    timer_instance_info_refer_private(*instance_info, &info);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_INFO, info) < 0) {
        generate_syscall_error(error, errno, "ioctl(%s)", "INFO");
        g_object_unref(*instance_info);
    }
}

/**
 * alsatimer_user_instance_set_params:
 * @self: A #ALSATimerUserInstance.
 * @instance_params: (inout): A #ALSATimerInstanceParams.
 * @error: A #GError.
 *
 * Configure the instance with the parameters and return the latest parameters.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_PARAMS command for ALSA timer character device.
 */
void alsatimer_user_instance_set_params(ALSATimerUserInstance *self,
                                ALSATimerInstanceParams *const *instance_params,
                                GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_params *params;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(instance_params != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    timer_instance_params_refer_private(*instance_params, &params);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PARAMS, params) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "PARAMS");
}

/**
 * alsatimer_user_instance_get_status:
 * @self: A #ALSATimerUserInstance.
 * @instance_status: (inout): A #ALSATimerInstanceStatus.
 * @error: A #GError.
 *
 * Get the latest status of instance.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_STATUS command for ALSA timer character device.
 */
void alsatimer_user_instance_get_status(ALSATimerUserInstance *self,
                                ALSATimerInstanceStatus *const *instance_status,
                                GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_status *status;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(instance_status != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    g_return_if_fail(ALSATIMER_IS_INSTANCE_STATUS(*instance_status));
    timer_instance_status_refer_private(*instance_status, &status);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_STATUS, status) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "STATUS");
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

static gboolean timer_user_instance_dispatch_src(GSource *gsrc, GSourceFunc cb,
                                      gpointer user_data)
{
    TimerUserInstanceSource *src = (TimerUserInstanceSource *)gsrc;
    ALSATimerUserInstance *self = src->self;
    ALSATimerUserInstancePrivate *priv;
    GIOCondition condition;
    size_t event_size;
    int len;
    guint8 *buf;

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

    switch (priv->event_data_type) {
    case ALSATIMER_EVENT_DATA_TYPE_TICK:
        event_size = sizeof(struct snd_timer_read);
        break;
    case ALSATIMER_EVENT_DATA_TYPE_TSTAMP:
        event_size = sizeof(struct snd_timer_tread);
        break;
    default:
        return G_SOURCE_CONTINUE;
    }

    buf = src->buf;
    while (len > 0) {
        ALSATimerEvent *ev = (ALSATimerEvent *)buf;

        g_signal_emit(src->self,
                      timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_EVENT],
                      0, ev);

        buf += event_size;
        len -= event_size;
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
 * @self: A #ALSATimerUserInstance.
 * @gsrc: (out): A #GSource to handle events from ALSA timer character device.
 * @error: A #GError.
 *
 * Allocate GSource structure to handle events from ALSA timer character
 * device. In each iteration of GMainContext, the read(2) system call is
 * executed to dispatch timer event for 'handle-event' signal, according to
 * the result of poll(2) system call.
 */
void alsatimer_user_instance_create_source(ALSATimerUserInstance *self,
                                         GSource **gsrc, GError **error)
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

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);
    g_return_if_fail(priv->fd >= 0);

    g_return_if_fail(gsrc != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

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
}

/**
 * alsatimer_user_instance_start:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Start timer event emission.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_START command for ALSA timer character device.
 */
void alsatimer_user_instance_start(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(error == NULL || *error == NULL);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_START) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "START");
}

/**
 * alsatimer_user_instance_stop:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Stop timer event emission.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_STOP command for ALSA timer character device.
 */
void alsatimer_user_instance_stop(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(error == NULL || *error == NULL);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_STOP) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "STOP");
}

/**
 * alsatimer_user_instance_pause:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Pause timer event emission.
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_PAUSE command for ALSA timer character device.
 */
void alsatimer_user_instance_pause(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(error == NULL || *error == NULL);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PAUSE) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "PAUSE");
}

/**
 * alsatimer_user_instance_continue:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Continue timer event emission paused by alsatimer_user_instance_pause().
 *
 * The call of function executes ioctl(2) system call with
 * SNDRV_TIMER_IOCTL_CONTINUE command for ALSA timer character device.
 */
void alsatimer_user_instance_continue(ALSATimerUserInstance *self,
                                      GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    g_return_if_fail(error == NULL || *error == NULL);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_CONTINUE) < 0)
        generate_syscall_error(error, errno, "ioctl(%s)", "CONTINUE");
}
