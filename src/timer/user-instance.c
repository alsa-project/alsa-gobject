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

struct _ALSATimerUserInstancePrivate {
    int fd;
    ALSATimerEventDataType event_data_type;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerUserInstance, alsatimer_user_instance, G_TYPE_OBJECT)

typedef struct {
    GSource src;
    ALSATimerUserInstance *self;
    gpointer tag;
    void *buf;
    unsigned int buf_len;
    ALSATimerEventData *event_data;
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
     * @event_data: (transfer none): An object derived from #ALSATimerEventData.
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
                     G_TYPE_NONE, 1, ALSATIMER_TYPE_EVENT_DATA);

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
 * @error: A #GError.
 *
 * Open ALSA Timer character device to allocate queue.
 */
void alsatimer_user_instance_open(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    char *devnode;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    alsatimer_get_devnode(&devnode, error);
    if (*error != NULL)
        return;

    priv->fd = open(devnode, O_RDONLY);
    g_free(devnode);
    if (priv->fd < 0) {
        generate_error(error, errno);
    }
}

ALSATimerUserInstance *alsatimer_user_instance_new()
{
    return g_object_new(ALSATIMER_TYPE_USER_INSTANCE, NULL);
}

/**
 * alsatimer_user_instance_attach:
 * @self: A #ALSATimerUserInstance.
 * @device_id: A #ALSATimerDeviceId to which the instance is attached.
 * @event_data_type: The type of event data, one of ALSATimerEventDataType.
 * @error: A #GError.
 *
 * Attach the instance to the timer device.
 */
void alsatimer_user_instance_attach(ALSATimerUserInstance *self,
                                    ALSATimerDeviceId *device_id,
                                    ALSATimerEventDataType event_data_type,
                                    GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    int tread;
    struct snd_timer_select sel = {0};

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    g_return_if_fail(device_id != NULL);
    priv = alsatimer_user_instance_get_instance_private(self);

    tread = (int)event_data_type;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_TREAD, &tread) < 0) {
        generate_error(error, errno);
        return;
    }
    priv->event_data_type = event_data_type;

    sel.id = *device_id;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_SELECT, &sel) < 0)
        generate_error(error, errno);
}

/**
 * alsatimer_user_instance_attach_as_slave:
 * @self: A #ALSATimerUserInstance.
 * @slave_class: The class identifier of master instance, one of
 *               #ALSATimerSlaveClass.
 * @slave_id: The numerical identifier of master instance.
 * @event_data_type: The type of event data, one of ALSATimerEventDataType.
 * @error: A #GError.
 *
 * Attach the instance to timer device as an slave to another instance indicated
 * by a pair of slave_class and slave_id. If the slave_class is for application
 * (=ALSATIMER_SLAVE_CLASS_APPLICATION), the slave_id is for the PID of
 * application process which owns the instance of timer. If the slave_class is
 * for ALSA sequencer (=ALSATIMER_SLAVE_CLASS_SEQUENCER), the slave_id is the
 * numerical ID of queue bound for timer device.
 */
void alsatimer_user_instance_attach_as_slave(ALSATimerUserInstance *self,
                                        ALSATimerSlaveClass slave_class,
                                        int slave_id,
                                        ALSATimerEventDataType event_data_type,
                                        GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    int tread;
    struct snd_timer_select sel = {0};

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    tread = (int)event_data_type;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_TREAD, &tread) < 0) {
        generate_error(error, errno);
        return;
    }
    priv->event_data_type = event_data_type;

    sel.id.dev_class = SNDRV_TIMER_CLASS_SLAVE;
    sel.id.dev_sclass = slave_class;
    sel.id.device = slave_id;
    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_SELECT, &sel) < 0)
        generate_error(error, errno);
}

/**
 * alsatimer_user_instance_get_info:
 * @self: A #ALSATimerUserInstance.
 * @instance_info: (out): A #ALSATimerInstanceInfo.
 * @error: A #GError.
 *
 * Return the information of device if attached to the instance.
 */
void alsatimer_user_instance_get_info(ALSATimerUserInstance *self,
                                      ALSATimerInstanceInfo **instance_info,
                                      GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_info *info;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    *instance_info = g_object_new(ALSATIMER_TYPE_INSTANCE_INFO, NULL);
    timer_instance_info_refer_private(*instance_info, &info);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_INFO, info) < 0) {
        generate_error(error, errno);
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
 */
void alsatimer_user_instance_set_params(ALSATimerUserInstance *self,
                                ALSATimerInstanceParams *const *instance_params,
                                GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_params *params;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    timer_instance_params_refer_private(*instance_params, &params);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PARAMS, params) < 0)
        generate_error(error, errno);
}

/**
 * alsatimer_user_instance_get_status:
 * @self: A #ALSATimerUserInstance.
 * @instance_status: (out): A #ALSATimerInstanceStatus.
 * @error: A #GError.
 *
 * Get the latest status of instance.
 */
void alsatimer_user_instance_get_status(ALSATimerUserInstance *self,
                                    ALSATimerInstanceStatus **instance_status,
                                    GError **error)
{
    ALSATimerUserInstancePrivate *priv;
    struct snd_timer_status *status;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    *instance_status = g_object_new(ALSATIMER_TYPE_INSTANCE_STATUS, NULL);
    timer_instance_status_refer_private(*instance_status, &status);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_STATUS, status) < 0) {
        generate_error(error, errno);
        g_object_unref(*instance_status);
    }
}

static void handle_tick_events(TimerUserInstanceSource *src, int len)
{
    struct snd_timer_read *ev = src->buf;

    while (len >= sizeof(*ev)) {
        ALSATimerEventDataTick *data = ALSATIMER_EVENT_DATA_TICK(src->event_data);

        timer_event_data_tick_set_data(data, ev);
        g_signal_emit(src->self,
                timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_EVENT],
                0, src->event_data);

        len -= sizeof(*ev);
        ++ev;
    }
}

static void handle_timestamp_events(TimerUserInstanceSource *src, int len)
{
    struct snd_timer_tread *ev = src->buf;

    while (len >= sizeof(*ev)) {
        ALSATimerEventDataTimestamp *data = ALSATIMER_EVENT_DATA_TIMESTAMP(src->event_data);

        timer_event_data_timestamp_set_data(data, ev);
        g_signal_emit(src->self,
                timer_user_instance_sigs[TIMER_USER_INSTANCE_SIG_HANDLE_EVENT],
                0, src->event_data);

        len -= sizeof(*ev);
        ++ev;
    }
}

static gboolean timer_user_instance_prepare_src(GSource *src, gint *timeout)
{
    *timeout = 500;

    // This source is not ready, let's poll(2).
    return FALSE;
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
    int len;

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

    if (priv->event_data_type == ALSATIMER_EVENT_DATA_TYPE_TICK)
        handle_tick_events(src, len);
    else
        handle_timestamp_events(src, len);

    // Just be sure to continue to process this source.
    return G_SOURCE_CONTINUE;
}

static void timer_user_instance_finalize_src(GSource *gsrc)
{
    TimerUserInstanceSource *src = (TimerUserInstanceSource *)gsrc;

    g_free(src->buf);
    g_object_unref(src->event_data);
    g_object_unref(src->self);
}

/**
 * alsatimer_user_instance_create_source:
 * @self: A #ALSATimerUserInstance.
 * @gsrc: (out): A #GSource to handle events from ALSA timer character device.
 * @error: A #GError.
 *
 * Allocate GSource structure to handle events from ALSA timer character
 * device.
 */
void alsatimer_user_instance_create_source(ALSATimerUserInstance *self,
                                         GSource **gsrc, GError **error)
{
    static GSourceFuncs funcs = {
            .prepare        = timer_user_instance_prepare_src,
            .check          = timer_user_instance_check_src,
            .dispatch       = timer_user_instance_dispatch_src,
            .finalize       = timer_user_instance_finalize_src,
    };
    ALSATimerUserInstancePrivate *priv;
    GType event_data_class;
    TimerUserInstanceSource *src;
    long page_size = sysconf(_SC_PAGESIZE);
    void *buf;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    if (priv->fd < 0) {
        generate_error(error, ENXIO);
        return;
    }

    switch (priv->event_data_type) {
    case ALSATIMER_EVENT_DATA_TYPE_TICK:
        event_data_class = ALSATIMER_TYPE_EVENT_DATA_TICK;
        break;
    case ALSATIMER_EVENT_DATA_TYPE_TIMESTAMP:
        event_data_class = ALSATIMER_TYPE_EVENT_DATA_TIMESTAMP;
        break;
    default:
        generate_error(error, EINVAL);
        return;
    }

    buf = g_try_malloc0(page_size);
    if (buf == NULL) {
        generate_error(error, ENOMEM);
        return;
    }

    *gsrc = g_source_new(&funcs, sizeof(TimerUserInstanceSource));
    src = (TimerUserInstanceSource *)(*gsrc);

    g_source_set_name(*gsrc, "ALSATimerUserInstance");
    g_source_set_priority(*gsrc, G_PRIORITY_HIGH_IDLE);
    g_source_set_can_recurse(*gsrc, TRUE);

    src->self = g_object_ref(self);
    src->tag = g_source_add_unix_fd(*gsrc, priv->fd, G_IO_IN);
    src->buf = buf;
    src->buf_len = page_size;

    src->event_data = g_object_new(event_data_class,
		                   "type", priv->event_data_type, NULL);
}

/**
 * alsatimer_user_instance_start:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Start event emission.
 */
void alsatimer_user_instance_start(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_START) < 0)
        generate_error(error, errno);
}

/**
 * alsatimer_user_instance_stop:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Stop event emission.
 */
void alsatimer_user_instance_stop(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_STOP) < 0)
        generate_error(error, errno);
}

/**
 * alsatimer_user_instance_pause:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Stop event emission.
 */
void alsatimer_user_instance_pause(ALSATimerUserInstance *self, GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_PAUSE) < 0)
        generate_error(error, errno);
}

/**
 * alsatimer_user_instance_continue:
 * @self: A #ALSATimerUserInstance.
 * @error: A #GError.
 *
 * Stop event emission.
 */
void alsatimer_user_instance_continue(ALSATimerUserInstance *self,
                                      GError **error)
{
    ALSATimerUserInstancePrivate *priv;

    g_return_if_fail(ALSATIMER_IS_USER_INSTANCE(self));
    priv = alsatimer_user_instance_get_instance_private(self);

    if (ioctl(priv->fd, SNDRV_TIMER_IOCTL_CONTINUE) < 0)
        generate_error(error, errno);
}
