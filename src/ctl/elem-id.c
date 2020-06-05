// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * SECTION: elem-id
 * @Title: ALSACtlElemId
 * @Short_description: A boxed object to represent the identifier of element.
 *
 * A #ALSACtlElemId is a boxed object to represent the identifier of element.
 * It points to a element by two ways; by the numerical ID, or by the
 * combination of the type of interface, the numerical ID of device, the
 * numerical ID of subdevice, the name, and the index.
 *
 * The object wraps 'struct snd_ctl_elem_id' in UAPI of Linux sound subsystem.
 */
ALSACtlElemId *ctl_elem_id_copy(const ALSACtlElemId *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSACtlElemId, alsactl_elem_id, ctl_elem_id_copy, g_free);

/**
 * alsactl_elem_id_new_by_numid:
 * @numid: The numerical ID of the element.
 *
 * Allocates and return an instance of ALSACtlElemId by the numerical ID.
 *
 * Returns: A #ALSACtlElemId.
 */
ALSACtlElemId *alsactl_elem_id_new_by_numid(guint numid)
{
    struct snd_ctl_elem_id *id;

    id = g_malloc0(sizeof(*id));
    id->numid = numid;

    return (ALSACtlElemId *)id;
}

/**
 * alsactl_elem_id_new_by_name:
 * @iface:          The interface of element, one of ALSACtlElemIfaceType.
 * @device_id:      The numerical ID of device to which the element belongs.
 * @subdevice_id:   The numerical ID of subdevice to which the element belongs.
 * @name:           The name of element, up to 44 byte
 *                  (=SNDRV_CTL_ELEM_ID_NAME_MAXLEN) including terminator.
 * @index:          The index of element in a set of elements with the same name.
 *
 * Allocates and return an instance of ALSACtlElemId by the name.
 *
 * Returns: A #ALSACtlElemId.
 */
ALSACtlElemId *alsactl_elem_id_new_by_name(ALSACtlElemIfaceType iface,
                                           guint device_id, guint subdevice_id,
                                           const gchar *name, guint index)
{
    struct snd_ctl_elem_id *id;

    id = g_malloc0(sizeof(*id));

    id->iface = iface;
    id->device = device_id;
    id->subdevice = subdevice_id;
    g_strlcpy((char *)id->name, name, sizeof(id->name));
    id->index = index;

    return (ALSACtlElemId *)id;
}

/**
 * alsactl_elem_id_get_numid:
 * @self: A #ALSACtlElemId.
 * @numid: (out): The numerical ID of element.
 *
 * Get the numerical ID of element.
 */
void alsactl_elem_id_get_numid(const ALSACtlElemId *self, guint *numid)
{
    *numid = self->numid;
}

/**
 * alsactl_elem_id_get_iface:
 * @self: A #ALSACtlElemId.
 * @iface: (out): The interface of element.
 *
 * Get the interface of element.
 */
void alsactl_elem_id_get_iface(const ALSACtlElemId *self,
                               ALSACtlElemIfaceType *iface)
{
    *iface = self->iface;
}

/**
 * alsactl_elem_id_get_device_id:
 * @self: A #ALSACtlElemId.
 * @device_id: (out): The numerical ID of device to which the element belongs.
 *
 * Get the numerical ID of device to which the element belongs.
 */
void alsactl_elem_id_get_device_id(const ALSACtlElemId *self, guint *device_id)
{
    *device_id = self->device;
}

/**
 * alsactl_elem_id_get_subdevice_id:
 * @self: A #ALSACtlElemId.
 * @subdevice_id: (out): The numerical ID of subdevice to which the element belongs.
 *
 * Get the numerical ID of element.
 */
void alsactl_elem_id_get_subdevice_id(const ALSACtlElemId *self,
                                      guint *subdevice_id)
{
    *subdevice_id = self->subdevice;
}

/**
 * alsactl_elem_id_get_name:
 * @self: A #ALSACtlElemId.
 * @name: (transfer none)(out): The name of element.
 *
 * Get the name of element.
 */
void alsactl_elem_id_get_name(const ALSACtlElemId *self, const gchar **name)
{
    *name = (const gchar *)self->name;
}

/**
 * alsactl_elem_id_get_index:
 * @self: A #ALSACtlElemId.
 * @index: (out): The index of element.
 *
 * Get the index of element.
 */
void alsactl_elem_id_get_index(const ALSACtlElemId *self, guint *index)
{
    *index = self->index;
}

/**
 * alsactl_elem_id_equal:
 * @self: A #ALSACtlElemId.
 * @target: A #ALSACtlElemId to compare.
 *
 * Returns: whether the given object indicates the same element.
 */
gboolean alsactl_elem_id_equal(const ALSACtlElemId *self,
                               const ALSACtlElemId *target) {
    const struct snd_ctl_elem_id *lhs, *rhs;

    lhs = (const struct snd_ctl_elem_id *)self;
    rhs = (const struct snd_ctl_elem_id *)target;

    return lhs->numid == rhs->numid ||
           (lhs->iface == rhs->iface &&
            lhs->device == rhs->device &&
            lhs->subdevice == rhs->subdevice &&
            !strcmp((const char *)lhs->name, (const char *)rhs->name) &&
            lhs->index == rhs->index);
}
