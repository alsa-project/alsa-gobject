// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSAHwdepDeviceCommon:
 * An interface to express common features of ALSA HwDep device.
 *
 * A [iface@DeviceCommon] should be implemented by any type of ALSA HwDep device.
 *
 * Since: 0.3.
 */

static void alsahwdep_device_common_default_init(ALSAHwdepDeviceCommonInterface *iface);

G_DEFINE_INTERFACE(ALSAHwdepDeviceCommon, alsahwdep_device_common, G_TYPE_OBJECT)

/**
 * alsahwdep_device_common_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] with code in ALSAHwdep.DeviceCommonError
 * enumerations.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(alsahwdep-device-common-error-quark, alsahwdep_device_common_error)

/**
 * alsahwdep_device_common_error_to_label:
 * @code: A ALSAHwdep.DeviceCommonError.
 * @label: (out) (transfer none): The string label of error.
 *
 * Retrieve the string label of error from code.
 */
void alsahwdep_device_common_error_to_label(ALSAHwdepDeviceCommonError code, const char **label)
{
    static const char *const labels[] = {
        [ALSAHWDEP_DEVICE_COMMON_ERROR_FAILED] =
                            "The operation failed due to unspecified reason.",
        [ALSAHWDEP_DEVICE_COMMON_ERROR_IS_OPENED] =
                            "The instance is already associated to character device.",
        [ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_OPENED] =
                            "The instance is not associated to character device yet.",
        [ALSAHWDEP_DEVICE_COMMON_ERROR_IS_USED] =
                            "The character device is already used.",
        [ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_SUPPORTED] =
                            "The HwDep device associated to the character device is not supported.",
        [ALSAHWDEP_DEVICE_COMMON_ERROR_IS_DISCONNECTED] =
                            "The sound card is under disconnected state.",
    };

    switch (code) {
    case ALSAHWDEP_DEVICE_COMMON_ERROR_FAILED:
    case ALSAHWDEP_DEVICE_COMMON_ERROR_IS_OPENED:
    case ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_OPENED:
    case ALSAHWDEP_DEVICE_COMMON_ERROR_IS_USED:
    case ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_SUPPORTED:
    case ALSAHWDEP_DEVICE_COMMON_ERROR_IS_DISCONNECTED:
        break;
    default:
        code = ALSAHWDEP_DEVICE_COMMON_ERROR_FAILED;
        break;
    }

    *label = labels[code];
}

static void alsahwdep_device_common_default_init(ALSAHwdepDeviceCommonInterface *iface)
{
    /**
     * ALSAHwdepDeviceCommon::handle-disconnection:
     * @self: A [iface@DeviceCommon].
     *
     * Emitted when the sound card is not available anymore due to unbinding driver or hot
     * unplugging. The owner of the object should prepare to call [method@GObject.Object.unref]
     * so that ALSA HwDep character device is going to be closed and sound card is released.
     */
    g_signal_new("handle-disconnection",
                 G_TYPE_FROM_INTERFACE(iface),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(ALSAHwdepDeviceCommonInterface, handle_disconnection),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE, 0, G_TYPE_NONE, 0);
}

/**
 * alsahwdep_device_common_open:
 * @self: A [iface@DeviceCommon].
 * @card_id: The numeric identifier of sound card.
 * @device_id: The numerid identifier of device.
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
gboolean alsahwdep_device_common_open(ALSAHwdepDeviceCommon *self, guint card_id, guint device_id,
                                      gint open_flag, GError **error)
{
    char *devname;
    gboolean result;

    g_return_val_if_fail(ALSAHWDEP_IS_DEVICE_COMMON(self), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!alsahwdep_get_hwdep_devnode(card_id, device_id, &devname, error))
        return FALSE;

    result = ALSAHWDEP_DEVICE_COMMON_GET_IFACE(self)->open(self, devname, open_flag, error);
    g_free(devname);

    return result;
}

/**
 * alsahwdep_device_common_get_protocol_version:
 * @self: A [iface@DeviceCommon].
 * @proto_ver_triplet: (array fixed-size=3) (inout) (transfer none): The version of protocol used
 *                     currently.
 * @error: A [struct@GLib.Error].
 *
 * Get the version of hwdep protocol used currently. The version is expressed as the array with
 * three elements; major, minor, and micro version in the order. The length of major version is
 * 16 bit, the length of minor and micro version is 8 bit each.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsahwdep_device_common_get_protocol_version(ALSAHwdepDeviceCommon *self,
                                                      guint16 *const proto_ver_triplet[3],
                                                      GError **error)
{
    g_return_val_if_fail(ALSAHWDEP_IS_DEVICE_COMMON(self), FALSE);
    g_return_val_if_fail(proto_ver_triplet != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return ALSAHWDEP_DEVICE_COMMON_GET_IFACE(self)->get_protocol_version(self, proto_ver_triplet,
                                                                         error);
}

/**
 * alsahwdep_device_common_get_device_info:
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
gboolean alsahwdep_device_common_get_device_info(ALSAHwdepDeviceCommon *self,
                                                 ALSAHwdepDeviceInfo **device_info, GError **error)
{
    g_return_val_if_fail(ALSAHWDEP_IS_DEVICE_COMMON(self), FALSE);
    g_return_val_if_fail(device_info != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return ALSAHWDEP_DEVICE_COMMON_GET_IFACE(self)->get_device_info(self, device_info, error);
}

/**
 * alsahwdep_device_common_create_source:
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
gboolean alsahwdep_device_common_create_source(ALSAHwdepDeviceCommon *self, GSource **source,
                                               GError **error)
{
    g_return_val_if_fail(ALSAHWDEP_IS_DEVICE_COMMON(self), FALSE);
    g_return_val_if_fail(source != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return ALSAHWDEP_DEVICE_COMMON_GET_IFACE(self)->create_source(self, source, error);
}
