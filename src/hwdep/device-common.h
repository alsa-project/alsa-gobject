// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_DEVICE_COMMON_H__
#define __ALSA_GOBJECT_ALSAHWDEP_DEVICE_COMMON_H__

#include <alsahwdep.h>

G_BEGIN_DECLS

#define ALSAHWDEP_TYPE_DEVICE_COMMON        (alsahwdep_device_common_get_type())

G_DECLARE_INTERFACE(ALSAHwdepDeviceCommon, alsahwdep_device_common, ALSAHWDEP, DEVICE_COMMON, GObject)

struct _ALSAHwdepDeviceCommonInterface {
    GTypeInterface parent_iface;

    /**
     * ALSAHwdepDeviceCommonInterface::open:
     * @self: A [iface@DeviceCommon].
     * @path: The path to special file for ALSA HwDep character device.
     * @open_flag: The flag of `open(2)` system call.
     * @error: A [struct@GLib.Error].
     *
     * Open one of ALSA hwdep character devices for the sound card.
     *
     * The call of function is expected to execute `open(2)` system call for ALSA hwdep character
     * device.
     *
     * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
     */
    gboolean (*open)(ALSAHwdepDeviceCommon *self, const gchar *path, gint open_flag, GError **error);

    /**
     * ALSAHwdepDeviceCommonInterface::get_protocol_version:
     * @self: A [iface@DeviceCommon].
     * @proto_ver_triplet: (array fixed-size=3) (inout) (transfer none): The version of protocol
     *                     used currently.
     * @error: A [struct@GLib.Error].
     *
     * Virtual function to retrieve the version of hwdep protocol used currently. The version is
     * expressed as the array with three elements; major, minor, and micro version in the order.
     * The length of major version is 16 bit, the length of minor and micro version is 8 bit each.
     *
     * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
     */
    gboolean (*get_protocol_version)(ALSAHwdepDeviceCommon *self, guint16 *const proto_ver_triplet[3],
                                     GError **error);

    /**
     * ALSAHwdepDeviceCommonInterface::get_device_info:
     * @self: A [iface@DeviceCommon].
     * @device_info: (out): The information of device.
     * @error: A [struct@GLib.Error].
     *
     * Get the information according to given numeric IDs for card and device. The call of function is
     * expected to executes `ioctl(2)` system call with `SNDRV_CTL_IOCTL_HWDEP_INFO` command to the
     * character device.
     *
     * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
     */
    gboolean (*get_device_info)(ALSAHwdepDeviceCommon *self, ALSAHwdepDeviceInfo **device_info,
                                GError **error);

    /**
     * ALSAHwdepDeviceCommonInterface::create_source:
     * @self: A [iface@DeviceCommon].
     * @source: (out): A [struct@GLib.Source] to handle events from ALSA hwdep character device.
     * @error: A [struct@GLib.Error].
     *
     * Allocate [struct@GLib.Source] structure to handle events from ALSA hwdep character device. In
     * each iteration of [struct@GLib.MainContext], the `read(2)` system call is executed to dispatch
     * hwdep event, according to the result of `poll(2)` system call.
     *
     * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
     */
    gboolean (*create_source)(ALSAHwdepDeviceCommon *self, GSource **source, GError **error);

    /**
     * ALSAHwdepDeviceCommonInterface::handle_disconnection:
     * @self: A [iface@DeviceCommon].
     *
     * Closure for the [signal@DeviceCommon::handle-disconnection] signal.
     */
    void (*handle_disconnection)(ALSAHwdepDeviceCommon *self);
};

gboolean alsahwdep_device_common_open(ALSAHwdepDeviceCommon *self, guint card_id, guint device_id,
                                      gint open_flag, GError **error);

gboolean alsahwdep_device_common_get_protocol_version(ALSAHwdepDeviceCommon *self,
                                                      guint16 *const proto_ver_triplet[3],
                                                      GError **error);

gboolean alsahwdep_device_common_get_device_info(ALSAHwdepDeviceCommon *self,
                                                 ALSAHwdepDeviceInfo **device_info, GError **error);

gboolean alsahwdep_device_common_create_source(ALSAHwdepDeviceCommon *self, GSource **source,
                                               GError **error);
G_END_DECLS

#endif
