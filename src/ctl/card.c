// SPDX-License-Identifier: LGPL-3.0-or-later
#include "card.h"
#include "query.h"
#include "privates.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <sound/asound.h>

struct _ALSACtlCardPrivate {
    int fd;
    char *devnode;
};
G_DEFINE_TYPE_WITH_PRIVATE(ALSACtlCard, alsactl_card, G_TYPE_OBJECT)

enum ctl_card_prop_type {
    CTL_CARD_PROP_DEVNODE = 1,
    CTL_CARD_PROP_COUNT,
};
static GParamSpec *ctl_card_props[CTL_CARD_PROP_COUNT] = { NULL, };

static void ctl_card_get_property(GObject *obj, guint id, GValue *val,
                                  GParamSpec *spec)
{
    ALSACtlCard *self = ALSACTL_CARD(obj);
    ALSACtlCardPrivate *priv = alsactl_card_get_instance_private(self);

    switch (id) {
    case CTL_CARD_PROP_DEVNODE:
        g_value_set_string(val, priv->devnode);
        break;
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

    g_object_class_install_properties(gobject_class, CTL_CARD_PROP_COUNT,
                                      ctl_card_props);
}

static void alsactl_card_init(ALSACtlCard *self)
{
    ALSACtlCardPrivate *priv = alsactl_card_get_instance_private(self);

    priv->fd = -1;
}

/**
 * alsactl_card_new:
 *
 * Allocate and return an instance of ALSACtlCard class.
 */
ALSACtlCard *alsactl_card_new()
{
    return g_object_new(ALSACTL_TYPE_CARD, NULL);
}

/**
 * alsactl_card_open:
 * @self: A #ALSACtlCard.
 * @card_id: The numerical ID of sound card.
 * @error: A #GError.
 *
 * Open ALSA control character device for the sound card.
 */
void alsactl_card_open(ALSACtlCard *self, guint card_id, GError **error)
{
    ALSACtlCardPrivate *priv;
    char *devnode;

    g_return_if_fail(ALSACTL_IS_CARD(self));
    priv = alsactl_card_get_instance_private(self);

    alsactl_get_control_devnode(card_id, &devnode, error);
    if (*error != NULL)
        return;

    priv->fd = open(devnode, O_RDONLY | O_NONBLOCK);
    if (priv->fd < 0) {
        generate_error(error, errno);
        g_free(devnode);
        return;
    }

    priv->devnode = devnode;
}
