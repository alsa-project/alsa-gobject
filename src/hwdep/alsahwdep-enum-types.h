// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSAHWDEP_ENUM_TYPES__H__
#define __ALSA_GOBJECT_ALSAHWDEP_ENUM_TYPES__H__

#include <sound/asound.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0)
#ifndef __GTK_DOC_IGNORE__
#define SNDRV_HWDEP_IFACE_LINE6         (SNDRV_HWDEP_IFACE_FW_TASCAM + 1)
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,12,0)
#ifndef __GTK_DOC_IGNORE__
#define SNDRV_HWDEP_IFACE_FW_MOTU       (SNDRV_HWDEP_IFACE_LINE6 + 1)
#define SNDRV_HWDEP_IFACE_FW_FIREFACE   (SNDRV_HWDEP_IFACE_FW_MOTU + 1)
#endif
#endif

/**
 *ALSAHwdepIfaceType:
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
 * A set of enumerators for the interface of hwdep device.
 */
typedef enum {
    ALSAHWDEP_IFACE_TYPE_OPL2           = SNDRV_HWDEP_IFACE_OPL2,
    ALSAHWDEP_IFACE_TYPE_OPL3           = SNDRV_HWDEP_IFACE_OPL3,
    ALSAHWDEP_IFACE_TYPE_OPL4           = SNDRV_HWDEP_IFACE_OPL4,
    ALSAHWDEP_IFACE_TYPE_SB16CSP        = SNDRV_HWDEP_IFACE_SB16CSP,
    ALSAHWDEP_IFACE_TYPE_EMU10K1        = SNDRV_HWDEP_IFACE_EMU10K1,
    ALSAHWDEP_IFACE_TYPE_YSS225         = SNDRV_HWDEP_IFACE_YSS225,
    ALSAHWDEP_IFACE_TYPE_ICS2115        = SNDRV_HWDEP_IFACE_ICS2115,
    ALSAHWDEP_IFACE_TYPE_SSCAPE         = SNDRV_HWDEP_IFACE_SSCAPE,
    ALSAHWDEP_IFACE_TYPE_VX             = SNDRV_HWDEP_IFACE_VX,
    ALSAHWDEP_IFACE_TYPE_MIXART         = SNDRV_HWDEP_IFACE_MIXART,
    ALSAHWDEP_IFACE_TYPE_USX2Y          = SNDRV_HWDEP_IFACE_USX2Y,
    ALSAHWDEP_IFACE_TYPE_EMUX_WAVETABLE = SNDRV_HWDEP_IFACE_EMUX_WAVETABLE,
    ALSAHWDEP_IFACE_TYPE_BLUETOOTH      = SNDRV_HWDEP_IFACE_BLUETOOTH,
    ALSAHWDEP_IFACE_TYPE_USX2Y_PCM      = SNDRV_HWDEP_IFACE_USX2Y_PCM,
    ALSAHWDEP_IFACE_TYPE_PCXHR          = SNDRV_HWDEP_IFACE_PCXHR,
    ALSAHWDEP_IFACE_TYPE_SB_RC          = SNDRV_HWDEP_IFACE_SB_RC,
    ALSAHWDEP_IFACE_TYPE_HDA            = SNDRV_HWDEP_IFACE_HDA,
    ALSAHWDEP_IFACE_TYPE_USB_STREAM     = SNDRV_HWDEP_IFACE_USB_STREAM,
    ALSAHWDEP_IFACE_TYPE_FW_DICE        = SNDRV_HWDEP_IFACE_FW_DICE,
    ALSAHWDEP_IFACE_TYPE_FW_FIREWORKS   = SNDRV_HWDEP_IFACE_FW_FIREWORKS,
    ALSAHWDEP_IFACE_TYPE_FW_BEBOB       = SNDRV_HWDEP_IFACE_FW_BEBOB,
    ALSAHWDEP_IFACE_TYPE_FW_OXFW        = SNDRV_HWDEP_IFACE_FW_OXFW,
    ALSAHWDEP_IFACE_TYPE_FW_DIGI00X     = SNDRV_HWDEP_IFACE_FW_DIGI00X,
    ALSAHWDEP_IFACE_TYPE_FW_TASCAM      = SNDRV_HWDEP_IFACE_FW_TASCAM,
    ALSAHWDEP_IFACE_TYPE_LINE6          = SNDRV_HWDEP_IFACE_LINE6,
    ALSAHWDEP_IFACE_TYPE_FW_MOTU        = SNDRV_HWDEP_IFACE_FW_MOTU,
    ALSAHWDEP_IFACE_TYPE_FW_FIREFACE    = SNDRV_HWDEP_IFACE_FW_FIREFACE,
} ALSAHwdepIfaceType;

#endif
