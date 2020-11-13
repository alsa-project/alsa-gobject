// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ENUM_TYPES__H__
#define __ALSA_GOBJECT_ALSACTL_ENUM_TYPES__H__

#include <sound/asound.h>

/**
 * ALSACtlElemType:
 * @ALSACTL_ELEM_TYPE_NONE:         Unudentified type.
 * @ALSACTL_ELEM_TYPE_BOOLEAN:      The element has boolean values.
 * @ALSACTL_ELEM_TYPE_INTEGER:      The element has integer values.
 * @ALSACTL_ELEM_TYPE_ENUMERATED:   The element has values for enumerated labels.
 * @ALSACTL_ELEM_TYPE_BYTES:        The element has byte values.
 * @ALSACTL_ELEM_TYPE_IEC60958:     The element has parameters of IEC 60958.
 * @ALSACTL_ELEM_TYPE_INTEGER64:    The element has 64 bit integer values.
 *
 * A set of enumerations for type of element.
 */
typedef enum {
    ALSACTL_ELEM_TYPE_NONE         = SNDRV_CTL_ELEM_TYPE_NONE,
    ALSACTL_ELEM_TYPE_BOOLEAN      = SNDRV_CTL_ELEM_TYPE_BOOLEAN,
    ALSACTL_ELEM_TYPE_INTEGER      = SNDRV_CTL_ELEM_TYPE_INTEGER,
    ALSACTL_ELEM_TYPE_ENUMERATED   = SNDRV_CTL_ELEM_TYPE_ENUMERATED,
    ALSACTL_ELEM_TYPE_BYTES        = SNDRV_CTL_ELEM_TYPE_BYTES,
    ALSACTL_ELEM_TYPE_IEC60958     = SNDRV_CTL_ELEM_TYPE_IEC958,
    ALSACTL_ELEM_TYPE_INTEGER64    = SNDRV_CTL_ELEM_TYPE_INTEGER64,
} ALSACtlElemType;

/**
 * ALSACtlElemIfaceType:
 * @ALSACTL_ELEM_IFACE_TYPE_CARD:       The element has effects to whole the sound card.
 * @ALSACTL_ELEM_IFACE_TYPE_HWDEP:      The element has effects to hwdep device.
 * @ALSACTL_ELEM_IFACE_TYPE_MIXER:      The element has effects to mixer device.
 * @ALSACTL_ELEM_IFACE_TYPE_PCM:        The element has effects to PCM device.
 * @ALSACTL_ELEM_IFACE_TYPE_RAWMIDI:    The element has effects to Rawmidi device.
 * @ALSACTL_ELEM_IFACE_TYPE_TIMER:      The element has effects to Timer device.
 * @ALSACTL_ELEM_IFACE_TYPE_SEQUENCER:  The element has effects to Sequencer device.
 *
 * A set of enumerations for interface of element.
 */
typedef enum {
    ALSACTL_ELEM_IFACE_TYPE_CARD        = SNDRV_CTL_ELEM_IFACE_CARD,
    ALSACTL_ELEM_IFACE_TYPE_HWDEP       = SNDRV_CTL_ELEM_IFACE_HWDEP,
    ALSACTL_ELEM_IFACE_TYPE_MIXER       = SNDRV_CTL_ELEM_IFACE_MIXER,
    ALSACTL_ELEM_IFACE_TYPE_PCM         = SNDRV_CTL_ELEM_IFACE_PCM,
    ALSACTL_ELEM_IFACE_TYPE_RAWMIDI     = SNDRV_CTL_ELEM_IFACE_RAWMIDI,
    ALSACTL_ELEM_IFACE_TYPE_TIMER       = SNDRV_CTL_ELEM_IFACE_TIMER,
    ALSACTL_ELEM_IFACE_TYPE_SEQUENCER   = SNDRV_CTL_ELEM_IFACE_SEQUENCER,
} ALSACtlElemIfaceType;

/**
 * ALSACtlElemAccessFlag:
 * @ALSACTL_ELEM_ACCESS_FLAG_READ:          The element supports operation to get its values.
 * @ALSACTL_ELEM_ACCESS_FLAG_WRITE:         The element supports operation to change its values.
 * @ALSACTL_ELEM_ACCESS_FLAG_VOLATILE:      The state of element is mutable independent of operating system.
 * @ALSACTL_ELEM_ACCESS_FLAG_TLV_READ:      The element supports operation to get Type-Length-Value data.
 * @ALSACTL_ELEM_ACCESS_FLAG_TLV_WRITE:     The element supports operation to change Type-Length-Value data.
 * @ALSACTL_ELEM_ACCESS_FLAG_TLV_COMMAND:   The element supports operation to use Type-Length-Value data as any command.
 * @ALSACTL_ELEM_ACCESS_FLAG_INACTIVE:      The element is not activated yet.
 * @ALSACTL_ELEM_ACCESS_FLAG_LOCK:          The element is under locked state from any write operation.
 * @ALSACTL_ELEM_ACCESS_FLAG_OWNER:         The element is owned by the process.
 * @ALSACTL_ELEM_ACCESS_FLAG_TLV_CALLBACK:  The element supports specific operation to process Type-Length-Value data.
 * @ALSACTL_ELEM_ACCESS_FLAG_USER:          The element is maintained by any user application, instead of kernel driver.
 *
 * A set of flags for access information of the element.
 */
typedef enum /*< flags >*/
{
    ALSACTL_ELEM_ACCESS_FLAG_READ         = SNDRV_CTL_ELEM_ACCESS_READ,
    ALSACTL_ELEM_ACCESS_FLAG_WRITE        = SNDRV_CTL_ELEM_ACCESS_WRITE,
    ALSACTL_ELEM_ACCESS_FLAG_VOLATILE     = SNDRV_CTL_ELEM_ACCESS_VOLATILE,
    ALSACTL_ELEM_ACCESS_FLAG_TLV_READ     = SNDRV_CTL_ELEM_ACCESS_TLV_READ,
    ALSACTL_ELEM_ACCESS_FLAG_TLV_WRITE    = SNDRV_CTL_ELEM_ACCESS_TLV_WRITE,
    ALSACTL_ELEM_ACCESS_FLAG_TLV_COMMAND  = SNDRV_CTL_ELEM_ACCESS_TLV_COMMAND,
    ALSACTL_ELEM_ACCESS_FLAG_INACTIVE     = SNDRV_CTL_ELEM_ACCESS_INACTIVE,
    ALSACTL_ELEM_ACCESS_FLAG_LOCK         = SNDRV_CTL_ELEM_ACCESS_LOCK,
    ALSACTL_ELEM_ACCESS_FLAG_OWNER        = SNDRV_CTL_ELEM_ACCESS_OWNER,
    ALSACTL_ELEM_ACCESS_FLAG_TLV_CALLBACK = SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK,
    ALSACTL_ELEM_ACCESS_FLAG_USER         = SNDRV_CTL_ELEM_ACCESS_USER,
} ALSACtlElemAccessFlag;

/**
 * ALSACtlEventType:
 * @ALSACTL_EVENT_TYPE_ELEM:     The event is related to any element.
 *
 * A set of enumerations for the type of event.
 */
typedef enum
{
  ALSACTL_EVENT_TYPE_ELEM  = SNDRV_CTL_EVENT_ELEM,
} ALSACtlEventType;

/**
 * ALSACtlElemEventMask:
 * @ALSACTL_ELEM_EVENT_MASK_VALUE:  The event notifies any change of value for the element.
 * @ALSACTL_ELEM_EVENT_MASK_INFO:   The event notifies any change of information for the element.
 * @ALSACTL_ELEM_EVENT_MASK_ADD:    The element notifies addition of the element.
 * @ALSACTL_ELEM_EVENT_MASK_TLV:    The element notifies any change of Type-Length-Value data for the element.
 * @ALSACTL_ELEM_EVENT_MASK_REMOVE: The element notifies removal of the element.
 *
 * A set of flags for the content of event for the element.
 */
typedef enum /*< flags >*/
{
    ALSACTL_ELEM_EVENT_MASK_VALUE   = SNDRV_CTL_EVENT_MASK_VALUE,
    ALSACTL_ELEM_EVENT_MASK_INFO    = SNDRV_CTL_EVENT_MASK_INFO,
    ALSACTL_ELEM_EVENT_MASK_ADD     = SNDRV_CTL_EVENT_MASK_ADD,
    ALSACTL_ELEM_EVENT_MASK_TLV     = SNDRV_CTL_EVENT_MASK_TLV,
    ALSACTL_ELEM_EVENT_MASK_REMOVE  = SNDRV_CTL_EVENT_MASK_TLV << 1,
} ALSACtlElemEventMask;

/**
 * ALSACtlCardError:
 * @ALSACTL_CARD_ERROR_FAILED:              The system call failed.
 *
 * A set of error code for GError with domain which equals to #alsactl_card_error_quark()
 */
typedef enum {
    ALSACTL_CARD_ERROR_FAILED,
} ALSACtlCardError;

#endif
