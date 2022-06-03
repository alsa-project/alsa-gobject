// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_ENUM_TYPES_H__
#define __ALSA_GOBJECT_ALSAHWDEP_ENUM_TYPES_H__

#include <linux/version.h>

G_BEGIN_DECLS

/**
 * ALSAHwdepIfaceType:
 * @ALSAHWDEP_IFACE_TYPE_OPL2:              For OPL2 sound chip.
 * @ALSAHWDEP_IFACE_TYPE_OPL3:              For OPL3 sound chip.
 * @ALSAHWDEP_IFACE_TYPE_OPL4:              For OPL4 sound chip.
 * @ALSAHWDEP_IFACE_TYPE_SB16CSP:           For Creative Signal Processor.
 * @ALSAHWDEP_IFACE_TYPE_EMU10K1:           For FX8010 processor in EMU10K1 chip.
 * @ALSAHWDEP_IFACE_TYPE_YSS225:            For Yamaha FX processor.
 * @ALSAHWDEP_IFACE_TYPE_ICS2115:           For Wavetable synth.
 * @ALSAHWDEP_IFACE_TYPE_SSCAPE:            For Ensoniq SoundScape ISA card (MC68EC000).
 * @ALSAHWDEP_IFACE_TYPE_VX:                For Digigram VX cards.
 * @ALSAHWDEP_IFACE_TYPE_MIXART:            For Digigram miXart cards.
 * @ALSAHWDEP_IFACE_TYPE_USX2Y:             For Tascam US122, US224 & US428 usb.
 * @ALSAHWDEP_IFACE_TYPE_EMUX_WAVETABLE:    For EmuX wavetable.
 * @ALSAHWDEP_IFACE_TYPE_BLUETOOTH:         For Bluetooth audio.
 * @ALSAHWDEP_IFACE_TYPE_USX2Y_PCM:         For Tascam US122, US224 & US428 rawusb pcm.
 * @ALSAHWDEP_IFACE_TYPE_PCXHR:             For Digigram PCXHR.
 * @ALSAHWDEP_IFACE_TYPE_SB_RC:             For SB Extigy/Audigy2NX remote control.
 * @ALSAHWDEP_IFACE_TYPE_HDA:               For HD-audio.
 * @ALSAHWDEP_IFACE_TYPE_USB_STREAM:        For direct access to usb stream.
 * @ALSAHWDEP_IFACE_TYPE_FW_DICE:           For TC DICE FireWire device.
 * @ALSAHWDEP_IFACE_TYPE_FW_FIREWORKS:      For Echo Audio Fireworks based devices.
 * @ALSAHWDEP_IFACE_TYPE_FW_BEBOB:          For BridgeCo BeBoB based device.
 * @ALSAHWDEP_IFACE_TYPE_FW_OXFW:           For Oxford OXFW970/971 based devices.
 * @ALSAHWDEP_IFACE_TYPE_FW_DIGI00X:        For Digidesign Digi 002/003 family.
 * @ALSAHWDEP_IFACE_TYPE_FW_TASCAM:         For TASCAM FireWire series.
 * @ALSAHWDEP_IFACE_TYPE_LINE6:             For Line6 USB processors. Available in Linux kernel 4.9.0 or later.
 * @ALSAHWDEP_IFACE_TYPE_FW_MOTU:           For MOTU FireWire series. Available in Linux kernel 4.12.0 or later.
 * @ALSAHWDEP_IFACE_TYPE_FW_FIREFACE:       For RME Fireface series. Available in Linux kernel 4.12.0 or later.
 *
 * A set of enumerations for the interface of hwdep device.
 */
typedef enum {
    ALSAHWDEP_IFACE_TYPE_OPL2 = 0,
    ALSAHWDEP_IFACE_TYPE_OPL3,
    ALSAHWDEP_IFACE_TYPE_OPL4,
    ALSAHWDEP_IFACE_TYPE_SB16CSP,
    ALSAHWDEP_IFACE_TYPE_EMU10K1,
    ALSAHWDEP_IFACE_TYPE_YSS225,
    ALSAHWDEP_IFACE_TYPE_ICS2115,
    ALSAHWDEP_IFACE_TYPE_SSCAPE,
    ALSAHWDEP_IFACE_TYPE_VX,
    ALSAHWDEP_IFACE_TYPE_MIXART,
    ALSAHWDEP_IFACE_TYPE_USX2Y,
    ALSAHWDEP_IFACE_TYPE_EMUX_WAVETABLE,
    ALSAHWDEP_IFACE_TYPE_BLUETOOTH,
    ALSAHWDEP_IFACE_TYPE_USX2Y_PCM,
    ALSAHWDEP_IFACE_TYPE_PCXHR,
    ALSAHWDEP_IFACE_TYPE_SB_RC,
    ALSAHWDEP_IFACE_TYPE_HDA,
    ALSAHWDEP_IFACE_TYPE_USB_STREAM,
    ALSAHWDEP_IFACE_TYPE_FW_DICE,
    ALSAHWDEP_IFACE_TYPE_FW_FIREWORKS,
    ALSAHWDEP_IFACE_TYPE_FW_BEBOB,
    ALSAHWDEP_IFACE_TYPE_FW_OXFW,
    ALSAHWDEP_IFACE_TYPE_FW_DIGI00X,
    ALSAHWDEP_IFACE_TYPE_FW_TASCAM,
    ALSAHWDEP_IFACE_TYPE_LINE6,
    ALSAHWDEP_IFACE_TYPE_FW_MOTU,
    ALSAHWDEP_IFACE_TYPE_FW_FIREFACE,
} ALSAHwdepIfaceType;

/**
 * ALSAHwdepDeviceCommonError:
 *  @ALSAHWDEP_DEVICE_COMMON_ERROR_FAILED:              The operation failed due to unspecified reason.
 *  @ALSAHWDEP_DEVICE_COMMON_ERROR_IS_OPENED:           The instance is already associated to character device.
 *  @ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_OPENED:       The instance is not associated to character device yet.
 *  @ALSAHWDEP_DEVICE_COMMON_ERROR_IS_USED:             The character device is already used.
 *  @ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_SUPPORTED:    The HwDep device associated to the character device is not supported.
 *  @ALSAHWDEP_DEVICE_COMMON_ERROR_IS_DISCONNECTED:     The sound card is under disconnected state.
 *
 * A set of enumerations for code of ALSAHwDep.DeviceCommonError error domain.
 */
typedef enum {
    ALSAHWDEP_DEVICE_COMMON_ERROR_FAILED = 1,
    ALSAHWDEP_DEVICE_COMMON_ERROR_IS_OPENED,
    ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_OPENED,
    ALSAHWDEP_DEVICE_COMMON_ERROR_IS_USED,
    ALSAHWDEP_DEVICE_COMMON_ERROR_IS_NOT_SUPPORTED,
    ALSAHWDEP_DEVICE_COMMON_ERROR_IS_DISCONNECTED,
} ALSAHwdepDeviceCommonError;

G_END_DECLS

#endif
