// SPDX-License-Identifier: LGPL-3.0-or-later
#include "user-instance.h"
#include "query.h"
#include "privates.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

struct _ALSATimerUserInstancePrivate {
    int fd;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSATimerUserInstance, alsatimer_user_instance, G_TYPE_OBJECT)

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
