// SPDX-License-Identifier: LGPL-3.0-or-later
#include "device-id.h"

/**
 * SECTION: device-id
 * @Title: ALSATimerDeviceId
 * @Short_description: A boxed object to represent the identifier of timer
 *                     device.
 *
 * A #ALSATimerDeviceId is a boxed object to represent the identifier of timer
 * device. The identifier mainly consists of the class of timer device. The
 * other members; the numerical ID of card, device, and subdevice are optional
 * according to the class of timer device.
 *
 * The object wraps 'struct snd_timer_id' in UAPI of Linux sound subsystem.
 */
ALSATimerDeviceId *timer_device_id_copy(const ALSATimerDeviceId *self)
{
    return g_memdup(self, sizeof(*self));
}

G_DEFINE_BOXED_TYPE(ALSATimerDeviceId, alsatimer_device_id, timer_device_id_copy, g_free)

/**
 * alsatimer_device_id_new:
 * @class: The class of device, one of #ALSATImerClass.
 * @card_id: The numerical ID of relevant sound card.
 * @device_id: The numerical ID of relevant device.
 * @subdevice_id: The numerical ID of relevant subdevice.
 *
 * Allocate and return an instance of ALSATimerDeviceId.
 *
 * Returns: A #ALSATimerDeviceId.
 */
ALSATimerDeviceId *alsatimer_device_id_new(ALSATimerClass class,
                                           gint card_id, gint device_id,
                                           gint subdevice_id)
{
    ALSATimerDeviceId *self = g_malloc0(sizeof(*self));

    self->dev_class= class;
    self->card = card_id;
    self->device = device_id;
    self->subdevice = subdevice_id;

    return self;
}

/**
 * alsatimer_device_id_get_class:
 * @self: A #ALSATimerDeviceId.
 * @class: (out): The class of timer, one of #ALSATimerClass.
 *
 * Get the class of timer.
 */
void alsatimer_device_id_get_class(const ALSATimerDeviceId *self,
                                   ALSATimerClass *class)
{
    *class = self->dev_class;
}

/**
 * alsatimer_device_id_get_card_id:
 * @self: A #ALSATimerDeviceId.
 * @card_id: (out): The numerical ID of sound card to which the timer belongs.
 *
 * Get the numerical ID of sound card to which the device belongs.
 */
void alsatimer_device_id_get_card_id(const ALSATimerDeviceId *self,
                                     gint *card_id)
{
    *card_id = self->card;
}

/**
 * alsatimer_device_id_get_device_id:
 * @self: A #ALSATimerDeviceId.
 * @device_id: (out): The numerical ID of device to which the timer belongs.
 *
 * Get the numerical ID of device to which the timer belongs.
 */
void alsatimer_device_id_get_device_id(const ALSATimerDeviceId *self,
                                       gint *device_id)
{
    *device_id = self->device;
}

/**
 * alsatimer_device_id_get_subdevice_id:
 * @self: A #ALSATimerDeviceId.
 * @subdevice_id: (out): The numerical ID of subdevice to which the timer belongs.
 *
 * Get the numerical ID of subdevice to which the timer belongs.
 */
void alsatimer_device_id_get_subdevice_id(const ALSATimerDeviceId *self,
                                          gint *subdevice_id)
{
    *subdevice_id = self->subdevice;
}
