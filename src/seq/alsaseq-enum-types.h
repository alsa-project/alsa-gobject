// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_ENUM_TYPES__H__
#define __ALSA_GOBJECT_ALSASEQ_ENUM_TYPES__H__

#include <sound/asequencer.h>

/**
 * ALSASeqSpecificAddress:
 * @ALSASEQ_SPECIFIC_ADDRESS_UNKNOWN:       The address for unknown client/port/queue.
 * @ALSASEQ_SPECIFIC_ADDRESS_SUBSCRIBERS:   The client/port/queue address towards subscribers.
 * @ALSASEQ_SPECIFIC_ADDRESS_BROADCAST:     The client/port/queue address to broadcast.
 *
 * A set of enumerations for specific addresses.
 */
typedef enum {
    ALSASEQ_SPECIFIC_ADDRESS_UNKNOWN        = SNDRV_SEQ_ADDRESS_UNKNOWN,
    ALSASEQ_SPECIFIC_ADDRESS_SUBSCRIBERS    = SNDRV_SEQ_ADDRESS_SUBSCRIBERS,
    ALSASEQ_SPECIFIC_ADDRESS_BROADCAST      = SNDRV_SEQ_ADDRESS_BROADCAST,
} ALSASeqSpecificAddress;

/**
 * ALSASeqSpecificClientId:
 * @ALSASEQ_SPECIFIC_CLIENT_ID_SYSTEM:  The numerical ID to system client.
 * @ALSASEQ_SPECIFIC_CLIENT_ID_DUMMY:   The numerical ID to dummy client.
 * @ALSASEQ_SPECIFIC_CLIENT_ID_OSS:     The numerical ID to OSS client.
 *
 * A set of enumerations for the numerical IDs of specific clients.
 */
typedef enum {
    ALSASEQ_SPECIFIC_CLIENT_ID_SYSTEM       = SNDRV_SEQ_CLIENT_SYSTEM,
    ALSASEQ_SPECIFIC_CLIENT_ID_DUMMY        = SNDRV_SEQ_CLIENT_DUMMY,
    ALSASEQ_SPECIFIC_CLIENT_ID_OSS          = SNDRV_SEQ_CLIENT_OSS,
} ALSASeqSpecificClientId;

/**
 * ALSASeqClientType:
 * @ALSASEQ_CLIENT_TYPE_NONE:       The client is invalid.
 * @ALSASEQ_CLIENT_TYPE_USER:       The client is userspace application.
 * @ALSASEQ_CLIENT_TYPE_KERNEL:     The client is kernel driver.
 *
 * A set of enumerations for the type of client.
 */
typedef enum {
    ALSASEQ_CLIENT_TYPE_NONE    = NO_CLIENT,
    ALSASEQ_CLIENT_TYPE_USER    = USER_CLIENT,
    ALSASEQ_CLIENT_TYPE_KERNEL  = KERNEL_CLIENT,
} ALSASeqClientType;

/**
 * ALSASeqFilterAttrFlag:
 * @ALSASEQ_FILTER_ATTR_FLAG_BROADCAST: The client is going to handle broadcast events.
 * @ALSASEQ_FILTER_ATTR_FLAG_MULTICAST: The client is going to handle multicast events.
 * @ALSASEQ_FILTER_ATTR_FLAG_BOUNCE:    The client is going to handle bounce events.
 *
 * A set of flags for attributes of event filter.
 */
typedef enum /*< flags >*/
{
    ALSASEQ_FILTER_ATTR_FLAG_BROADCAST  = SNDRV_SEQ_FILTER_BROADCAST,
    ALSASEQ_FILTER_ATTR_FLAG_MULTICAST  = SNDRV_SEQ_FILTER_MULTICAST,
    ALSASEQ_FILTER_ATTR_FLAG_BOUNCE     = SNDRV_SEQ_FILTER_BOUNCE,
} ALSASeqFilterAttrFlag;

/**
 * ALSASeqSpecificPortId:
 * @ALSASEQ_SPECIFIC_PORT_ID_SYSTEM_TIMER:      The numerical ID of port for system timer.
 * @ALSASEQ_SPECIFIC_PORT_ID_SYSTEM_ANNOUNCE:   The numerical ID of port for system announce.
 *
 * A set of enumerations for the numerical IDs of port for specific purpose.
 */
typedef enum
{
    ALSASEQ_SPECIFIC_PORT_ID_SYSTEM_TIMER    = SNDRV_SEQ_PORT_SYSTEM_TIMER,
    ALSASEQ_SPECIFIC_PORT_ID_SYSTEM_ANNOUNCE = SNDRV_SEQ_PORT_SYSTEM_ANNOUNCE,
} ALSASeqSpecificPortId;

/**
 * ALSASeqPortCapFlag:
 * @ALSASEQ_PORT_CAP_FLAG_READ:         Any message can be read from the port.
 * @ALSASEQ_PORT_CAP_FLAG_WRITE:        Any message can be written to the port.
 * @ALSASEQ_PORT_CAP_FLAG_DUPLEX:       Any messages can be read from/written to the port simultaneously.
 * @ALSASEQ_PORT_CAP_FLAG_SUBS_READ:    The data of subscription can be read from the port.
 * @ALSASEQ_PORT_CAP_FLAG_SUBS_WRITE:   The data of subscription can be written to the port.
 * @ALSASEQ_PORT_CAP_FLAG_NO_EXPORT:    Only owners of the port can maintain subscription.
 *
 * A set of flags for the capabilities of port.
 */
typedef enum /*< flags >*/
{
    ALSASEQ_PORT_CAP_FLAG_READ          = SNDRV_SEQ_PORT_CAP_READ,
    ALSASEQ_PORT_CAP_FLAG_WRITE         = SNDRV_SEQ_PORT_CAP_WRITE,
    ALSASEQ_PORT_CAP_FLAG_DUPLEX        = SNDRV_SEQ_PORT_CAP_DUPLEX,
    ALSASEQ_PORT_CAP_FLAG_SUBS_READ     = SNDRV_SEQ_PORT_CAP_SUBS_READ,
    ALSASEQ_PORT_CAP_FLAG_SUBS_WRITE    = SNDRV_SEQ_PORT_CAP_SUBS_WRITE,
    ALSASEQ_PORT_CAP_FLAG_NO_EXPORT     = SNDRV_SEQ_PORT_CAP_NO_EXPORT,
} ALSASeqPortCapFlag;

/**
 * ALSASeqPortAttrFlag:
 * @ALSASEQ_PORT_ATTR_FLAG_SPECIFIC:        For hardware specific messages.
 * @ALSASEQ_PORT_ATTR_FLAG_MIDI_GENERIC:    For generic MIDI messages.
 * @ALSASEQ_PORT_ATTR_FLAG_MIDI_GM:         For General MIDI comatible messages.
 * @ALSASEQ_PORT_ATTR_FLAG_MIDI_GS:         For GS compatible messages.
 * @ALSASEQ_PORT_ATTR_FLAG_MIDI_XG:         For XG compatible messages.
 * @ALSASEQ_PORT_ATTR_FLAG_MIDI_MT32:       For MT-32 compatible messages.
 * @ALSASEQ_PORT_ATTR_FLAG_MIDI_GM2:        For General MIDI 2 compatible messages.
 * @ALSASEQ_PORT_ATTR_FLAG_HARDWARE:        The port on any hardware.
 * @ALSASEQ_PORT_ATTR_FLAG_SOFTWARE:        The port on any software.
 * @ALSASEQ_PORT_ATTR_FLAG_SYNTHESIZER:     The port to generate sound.
 * @ALSASEQ_PORT_ATTR_FLAG_PORT:            The port connects to the other devices.
 * @ALSASEQ_PORT_ATTR_FLAG_APPLICATION:     The port on any application such as sequencer and editor.
 *
 * A set of flags for the attributes of port.
 */
typedef enum /*< flags >*/
{
    ALSASEQ_PORT_ATTR_FLAG_SPECIFIC         = SNDRV_SEQ_PORT_TYPE_SPECIFIC,
    ALSASEQ_PORT_ATTR_FLAG_MIDI_GENERIC     = SNDRV_SEQ_PORT_TYPE_MIDI_GENERIC,
    ALSASEQ_PORT_ATTR_FLAG_MIDI_GM          = SNDRV_SEQ_PORT_TYPE_MIDI_GM,
    ALSASEQ_PORT_ATTR_FLAG_MIDI_GS          = SNDRV_SEQ_PORT_TYPE_MIDI_GS,
    ALSASEQ_PORT_ATTR_FLAG_MIDI_XG          = SNDRV_SEQ_PORT_TYPE_MIDI_XG,
    ALSASEQ_PORT_ATTR_FLAG_MIDI_MT32        = SNDRV_SEQ_PORT_TYPE_MIDI_MT32,
    ALSASEQ_PORT_ATTR_FLAG_MIDI_GM2         = SNDRV_SEQ_PORT_TYPE_MIDI_GM2,
    ALSASEQ_PORT_ATTR_FLAG_HARDWARE         = SNDRV_SEQ_PORT_TYPE_HARDWARE,
    ALSASEQ_PORT_ATTR_FLAG_SOFTWARE         = SNDRV_SEQ_PORT_TYPE_SOFTWARE,
    ALSASEQ_PORT_ATTR_FLAG_SYNTHESIZER      = SNDRV_SEQ_PORT_TYPE_SYNTHESIZER,
    ALSASEQ_PORT_ATTR_FLAG_PORT             = SNDRV_SEQ_PORT_TYPE_PORT,
    ALSASEQ_PORT_ATTR_FLAG_APPLICATION      = SNDRV_SEQ_PORT_TYPE_APPLICATION,
} ALSASeqPortAttrFlag;

/**
 * ALSASeqEventType:
 * @ALSASEQ_EVENT_TYPE_SYSTEM:              For system status.
 * @ALSASEQ_EVENT_TYPE_RESULT:              For result status.
 * @ALSASEQ_EVENT_TYPE_NOTE:                For note message with duration.
 * @ALSASEQ_EVENT_TYPE_NOTEON:              For note on message.
 * @ALSASEQ_EVENT_TYPE_NOTEOFF:             For note off message.
 * @ALSASEQ_EVENT_TYPE_KEYPRESS:            For keypress message.
 * @ALSASEQ_EVENT_TYPE_CONTROLLER:          For control change message.
 * @ALSASEQ_EVENT_TYPE_PGMCHANGE:           For program change message.
 * @ALSASEQ_EVENT_TYPE_CHANPRESS:           For channel pressure message.
 * @ALSASEQ_EVENT_TYPE_PITCHBEND:           For pitchbend message.
 * @ALSASEQ_EVENT_TYPE_CONTROL14:           For control message with 14 bit value.
 * @ALSASEQ_EVENT_TYPE_NONREGPARAM:         For 14 bit NRPN address and 14 bit unsigned value.
 * @ALSASEQ_EVENT_TYPE_REGPARAM:            For 14 bit RPN address and 14 bit unsigned value.
 * @ALSASEQ_EVENT_TYPE_SONGPOS:             For song position message with LSB and MSB values.
 * @ALSASEQ_EVENT_TYPE_SONGSEL:             For song select message with numerical ID of song.
 * @ALSASEQ_EVENT_TYPE_QFRAME:              For time code quarter frame message of MIDI.
 * @ALSASEQ_EVENT_TYPE_TIMESIGN:            For time signature message of Standard MIDi File.
 * @ALSASEQ_EVENT_TYPE_KEYSIGN:             For key signature message of Standard MIDI File.
 * @ALSASEQ_EVENT_TYPE_START:               For Real Time Start message of MIDI.
 * @ALSASEQ_EVENT_TYPE_CONTINUE:            For Real Time Continue message of MIDI.
 * @ALSASEQ_EVENT_TYPE_STOP:                For Real Time Stop message of MIDI.
 * @ALSASEQ_EVENT_TYPE_SETPOS_TICK:         For position setting of tick queue.
 * @ALSASEQ_EVENT_TYPE_SETPOS_TIME:         For position setting of realtime queue.
 * @ALSASEQ_EVENT_TYPE_TEMPO:               For tempo message of Standard MIDI File.
 * @ALSASEQ_EVENT_TYPE_CLOCK:               For Real Time Clock message of MIDI.
 * @ALSASEQ_EVENT_TYPE_TICK:                For Real Time Tick message of MIDI.
 * @ALSASEQ_EVENT_TYPE_QUEUE_SKEW:          For skew of tempo for queue.
 * @ALSASEQ_EVENT_TYPE_TUNE_REQUEST:        For requests to tune.
 * @ALSASEQ_EVENT_TYPE_RESET:               For reset to power-on state.
 * @ALSASEQ_EVENT_TYPE_SENSING:             For active sensing message.
 * @ALSASEQ_EVENT_TYPE_ECHO:                For echo message.
 * @ALSASEQ_EVENT_TYPE_OSS:                 For raw message from Open Sound System.
 * @ALSASEQ_EVENT_TYPE_CLIENT_START:        For appear of the port.
 * @ALSASEQ_EVENT_TYPE_CLIENT_EXIT:         For disappear of the client.
 * @ALSASEQ_EVENT_TYPE_CLIENT_CHANGE:       For change of information or status of the client.
 * @ALSASEQ_EVENT_TYPE_PORT_START:          For addition of the port.
 * @ALSASEQ_EVENT_TYPE_PORT_EXIT:           For removal of the port.
 * @ALSASEQ_EVENT_TYPE_PORT_CHANGE:         For change of information or status of the port.
 * @ALSASEQ_EVENT_TYPE_PORT_SUBSCRIBED:     For establishment of subscription about the port.
 * @ALSASEQ_EVENT_TYPE_PORT_UNSUBSCRIBED:   For break of subscription about the port.
 * @ALSASEQ_EVENT_TYPE_USR0:                For user-defined message 0.
 * @ALSASEQ_EVENT_TYPE_USR1:                For user-defined message 1.
 * @ALSASEQ_EVENT_TYPE_USR2:                For user-defined message 2.
 * @ALSASEQ_EVENT_TYPE_USR3:                For user-defined message 3.
 * @ALSASEQ_EVENT_TYPE_USR4:                For user-defined message 4.
 * @ALSASEQ_EVENT_TYPE_USR5:                For user-defined message 5.
 * @ALSASEQ_EVENT_TYPE_USR6:                For user-defined message 6.
 * @ALSASEQ_EVENT_TYPE_USR7:                For user-defined message 7.
 * @ALSASEQ_EVENT_TYPE_USR8:                For user-defined message 8.
 * @ALSASEQ_EVENT_TYPE_USR9:                For user-defined message 9.
 * @ALSASEQ_EVENT_TYPE_SYSEX:               For system exclisive message with variable length data.
 * @ALSASEQ_EVENT_TYPE_BOUNCE:              For error message.
 * @ALSASEQ_EVENT_TYPE_USR_VAR0:            For user-defined message 0 with variable length data.
 * @ALSASEQ_EVENT_TYPE_USR_VAR1:            For user-defined message 1 with variable length data.
 * @ALSASEQ_EVENT_TYPE_USR_VAR2:            For user-defined message 2 with variable length data.
 * @ALSASEQ_EVENT_TYPE_USR_VAR3:            For user-defined message 3 with variable length data.
 * @ALSASEQ_EVENT_TYPE_USR_VAR4:            For user-defined message 4 with variable length data.
 * @ALSASEQ_EVENT_TYPE_NONE:                For invalid or unknown message.
 *
 * The type of event.
 */
typedef enum {
    ALSASEQ_EVENT_TYPE_SYSTEM               = SNDRV_SEQ_EVENT_SYSTEM,
    ALSASEQ_EVENT_TYPE_RESULT               = SNDRV_SEQ_EVENT_RESULT,
    ALSASEQ_EVENT_TYPE_NOTE                 = SNDRV_SEQ_EVENT_NOTE,
    ALSASEQ_EVENT_TYPE_NOTEON               = SNDRV_SEQ_EVENT_NOTEON,
    ALSASEQ_EVENT_TYPE_NOTEOFF              = SNDRV_SEQ_EVENT_NOTEOFF,
    ALSASEQ_EVENT_TYPE_KEYPRESS             = SNDRV_SEQ_EVENT_KEYPRESS,
    ALSASEQ_EVENT_TYPE_CONTROLLER           = SNDRV_SEQ_EVENT_CONTROLLER,
    ALSASEQ_EVENT_TYPE_PGMCHANGE            = SNDRV_SEQ_EVENT_PGMCHANGE,
    ALSASEQ_EVENT_TYPE_CHANPRESS            = SNDRV_SEQ_EVENT_CHANPRESS,
    ALSASEQ_EVENT_TYPE_PITCHBEND            = SNDRV_SEQ_EVENT_PITCHBEND,
    ALSASEQ_EVENT_TYPE_CONTROL14            = SNDRV_SEQ_EVENT_CONTROL14,
    ALSASEQ_EVENT_TYPE_NONREGPARAM          = SNDRV_SEQ_EVENT_NONREGPARAM,
    ALSASEQ_EVENT_TYPE_REGPARAM             = SNDRV_SEQ_EVENT_REGPARAM,
    ALSASEQ_EVENT_TYPE_SONGPOS              = SNDRV_SEQ_EVENT_SONGPOS,
    ALSASEQ_EVENT_TYPE_SONGSEL              = SNDRV_SEQ_EVENT_SONGSEL,
    ALSASEQ_EVENT_TYPE_QFRAME               = SNDRV_SEQ_EVENT_QFRAME,
    ALSASEQ_EVENT_TYPE_TIMESIGN             = SNDRV_SEQ_EVENT_TIMESIGN,
    ALSASEQ_EVENT_TYPE_KEYSIGN              = SNDRV_SEQ_EVENT_KEYSIGN,
    ALSASEQ_EVENT_TYPE_START                = SNDRV_SEQ_EVENT_START,
    ALSASEQ_EVENT_TYPE_CONTINUE             = SNDRV_SEQ_EVENT_CONTINUE,
    ALSASEQ_EVENT_TYPE_STOP                 = SNDRV_SEQ_EVENT_STOP,
    ALSASEQ_EVENT_TYPE_SETPOS_TICK          = SNDRV_SEQ_EVENT_SETPOS_TICK,
    ALSASEQ_EVENT_TYPE_SETPOS_TIME          = SNDRV_SEQ_EVENT_SETPOS_TIME,
    ALSASEQ_EVENT_TYPE_TEMPO                = SNDRV_SEQ_EVENT_TEMPO,
    ALSASEQ_EVENT_TYPE_CLOCK                = SNDRV_SEQ_EVENT_CLOCK,
    ALSASEQ_EVENT_TYPE_TICK                 = SNDRV_SEQ_EVENT_TICK,
    ALSASEQ_EVENT_TYPE_QUEUE_SKEW           = SNDRV_SEQ_EVENT_QUEUE_SKEW,
    ALSASEQ_EVENT_TYPE_TUNE_REQUEST         = SNDRV_SEQ_EVENT_TUNE_REQUEST,
    ALSASEQ_EVENT_TYPE_RESET                = SNDRV_SEQ_EVENT_RESET,
    ALSASEQ_EVENT_TYPE_SENSING              = SNDRV_SEQ_EVENT_SENSING,
    ALSASEQ_EVENT_TYPE_ECHO                 = SNDRV_SEQ_EVENT_ECHO,
    ALSASEQ_EVENT_TYPE_OSS                  = SNDRV_SEQ_EVENT_OSS,
    ALSASEQ_EVENT_TYPE_CLIENT_START         = SNDRV_SEQ_EVENT_CLIENT_START,
    ALSASEQ_EVENT_TYPE_CLIENT_EXIT          = SNDRV_SEQ_EVENT_CLIENT_EXIT,
    ALSASEQ_EVENT_TYPE_CLIENT_CHANGE        = SNDRV_SEQ_EVENT_CLIENT_CHANGE,
    ALSASEQ_EVENT_TYPE_PORT_START           = SNDRV_SEQ_EVENT_PORT_START,
    ALSASEQ_EVENT_TYPE_PORT_EXIT            = SNDRV_SEQ_EVENT_PORT_EXIT,
    ALSASEQ_EVENT_TYPE_PORT_CHANGE          = SNDRV_SEQ_EVENT_PORT_CHANGE,
    ALSASEQ_EVENT_TYPE_PORT_SUBSCRIBED      = SNDRV_SEQ_EVENT_PORT_SUBSCRIBED,
    ALSASEQ_EVENT_TYPE_PORT_UNSUBSCRIBED    = SNDRV_SEQ_EVENT_PORT_UNSUBSCRIBED,
    ALSASEQ_EVENT_TYPE_USR0                 = SNDRV_SEQ_EVENT_USR0,
    ALSASEQ_EVENT_TYPE_USR1                 = SNDRV_SEQ_EVENT_USR1,
    ALSASEQ_EVENT_TYPE_USR2                 = SNDRV_SEQ_EVENT_USR2,
    ALSASEQ_EVENT_TYPE_USR3                 = SNDRV_SEQ_EVENT_USR3,
    ALSASEQ_EVENT_TYPE_USR4                 = SNDRV_SEQ_EVENT_USR4,
    ALSASEQ_EVENT_TYPE_USR5                 = SNDRV_SEQ_EVENT_USR5,
    ALSASEQ_EVENT_TYPE_USR6                 = SNDRV_SEQ_EVENT_USR6,
    ALSASEQ_EVENT_TYPE_USR7                 = SNDRV_SEQ_EVENT_USR7,
    ALSASEQ_EVENT_TYPE_USR8                 = SNDRV_SEQ_EVENT_USR8,
    ALSASEQ_EVENT_TYPE_USR9                 = SNDRV_SEQ_EVENT_USR9,
    ALSASEQ_EVENT_TYPE_SYSEX                = SNDRV_SEQ_EVENT_SYSEX,
    ALSASEQ_EVENT_TYPE_BOUNCE               = SNDRV_SEQ_EVENT_BOUNCE,
    ALSASEQ_EVENT_TYPE_USR_VAR0             = SNDRV_SEQ_EVENT_USR_VAR0,
    ALSASEQ_EVENT_TYPE_USR_VAR1             = SNDRV_SEQ_EVENT_USR_VAR1,
    ALSASEQ_EVENT_TYPE_USR_VAR2             = SNDRV_SEQ_EVENT_USR_VAR2,
    ALSASEQ_EVENT_TYPE_USR_VAR3             = SNDRV_SEQ_EVENT_USR_VAR3,
    ALSASEQ_EVENT_TYPE_USR_VAR4             = SNDRV_SEQ_EVENT_USR_VAR4,
    ALSASEQ_EVENT_TYPE_NONE                 = SNDRV_SEQ_EVENT_NONE,
} ALSASeqEventType;

/**
 * ALSASeqEventTimestampMode:
 * @ALSASEQ_EVENT_TIMESTAMP_MODE_TICK:      The timestamp includes tick count.
 * @ALSASEQ_EVENT_TIMESTAMP_MODE_REAL:      The timestamp includes real time.
 *
 * A set of enumeration for the mode of timestamp.
 */
typedef enum {
    ALSASEQ_EVENT_TIMESTAMP_MODE_TICK   = SNDRV_SEQ_TIME_STAMP_TICK,
    ALSASEQ_EVENT_TIMESTAMP_MODE_REAL   = SNDRV_SEQ_TIME_STAMP_REAL,
} ALSASeqEventTimestampMode;

/**
 * ALSASeqEventTimeMode:
 * @ALSASEQ_EVENT_TIME_MODE_ABS:    The time is absolute.
 * @ALSASEQ_EVENT_TIME_MODE_REL:    The time is relative.
 *
 * A set of enumerations for the mode of time.
 */
typedef enum {
    ALSASEQ_EVENT_TIME_MODE_ABS         = SNDRV_SEQ_TIME_MODE_ABS,
    ALSASEQ_EVENT_TIME_MODE_REL         = SNDRV_SEQ_TIME_MODE_REL,
} ALSASeqEventTimeMode;

/**
 * ALSASeqEventLengthMode:
 * @ALSASEQ_EVENT_LENGTH_MODE_FIXED:    The data is fixed length.
 * @ALSASEQ_EVENT_LENGTH_MODE_VARIABLE: The data is variable length.
 * @ALSASEQ_EVENT_LENGTH_MODE_POINTER:  The data is a pointer and its length in userspace.
 *
 * A set of enumeration for the mode of data length.
 */
typedef enum {
    ALSASEQ_EVENT_LENGTH_MODE_FIXED     = SNDRV_SEQ_EVENT_LENGTH_FIXED,
    ALSASEQ_EVENT_LENGTH_MODE_VARIABLE  = SNDRV_SEQ_EVENT_LENGTH_VARIABLE,
    ALSASEQ_EVENT_LENGTH_MODE_POINTER = SNDRV_SEQ_EVENT_LENGTH_VARUSR,
} ALSASeqEventLengthMode;

/**
 * ALSASeqEventPriorityMode:
 * @ALSASEQ_EVENT_PRIORITY_MODE_NORMAL:     For normal priority.
 * @ALSASEQ_EVENT_PRIORITY_MODE_HIGH:       For high priority.
 *
 * A set of enumerations for the mode of priority.
 */
typedef enum {
    ALSASEQ_EVENT_PRIORITY_MODE_NORMAL  = SNDRV_SEQ_PRIORITY_NORMAL,
    ALSASEQ_EVENT_PRIORITY_MODE_HIGH    = SNDRV_SEQ_PRIORITY_HIGH,
} ALSASeqEventPriorityMode;

/**
 * ALSASeqSpecificQueueId:
 * @ALSASEQ_SPECIFIC_QUEUE_ID_DIRECT:   The message is delivered immediately, instead of being queued.
 *
 * A set of enumerations for the numerical IDs of queue for specific purpose.
 */
typedef enum {
    ALSASEQ_SPECIFIC_QUEUE_ID_DIRECT = SNDRV_SEQ_QUEUE_DIRECT,
} ALSASeqSpecificQueueId;

/**
 * ALSASeqPortSubscribeFlag:
 * @ALSASEQ_PORT_SUBSCRIBE_FLAG_EXCLUSIVE:  The subscription can be changed by originator.
 * @ALSASEQ_PORT_SUBSCRIBE_FLAG_TIMESTAMP:  Any event for this subscription has timestamp.
 * @ALSASEQ_PORT_SUBSCRIBE_FLAG_TIME_REAL:  The timestamp on the event from the port has real time.
 *
 * A set of flags for subscription conditions.
 */
typedef enum /*< flags >*/
{
    ALSASEQ_PORT_SUBSCRIBE_FLAG_EXCLUSIVE = SNDRV_SEQ_PORT_SUBS_EXCLUSIVE,
    ALSASEQ_PORT_SUBSCRIBE_FLAG_TIMESTAMP = SNDRV_SEQ_PORT_SUBS_TIMESTAMP,
    ALSASEQ_PORT_SUBSCRIBE_FLAG_TIME_REAL = SNDRV_SEQ_PORT_SUBS_TIME_REAL,
} ALSASeqPortSubscribeFlag;

/**
 * ALSASeqQuerySubscribeType
 * @ALSASEQ_QUERY_SUBSCRIBE_TYPE_READ:  To query subscribers to read from the port.
 * @ALSASEQ_QUERY_SUBSCRIBE_TYPE_WRITE: To query subscribers to write to the port.
 *
 * A set of enumerations for the type to query subscription.
 */
typedef enum {
    ALSASEQ_QUERY_SUBSCRIBE_TYPE_READ   = SNDRV_SEQ_QUERY_SUBS_READ,
    ALSASEQ_QUERY_SUBSCRIBE_TYPE_WRITE  = SNDRV_SEQ_QUERY_SUBS_WRITE,
} ALSASeqQuerySubscribeType;

/**
 * ALSASeqQueueTimerType:
 * @ALSASEQ_QUEUE_TIMER_TYPE_ALSA:          Any ALSA timer device.
 *
 * A set of enumerations for the type of timer source for the queue.
 */
typedef enum {
    ALSASEQ_QUEUE_TIMER_TYPE_ALSA       = SNDRV_SEQ_TIMER_ALSA,
} ALSASeqQueueTimerType;

/**
 * ALSASeqRemoveFilterFlag:
 * @ALSASEQ_REMOVE_FILTER_FLAG_INPUT:   Flush of input queues.
 * @ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT:  Flush of output queues.
 *
 * A set of flags of the mode of removal event.
 */
typedef enum /*< flags >*/
{
    ALSASEQ_REMOVE_FILTER_FLAG_INPUT    = SNDRV_SEQ_REMOVE_INPUT,
    ALSASEQ_REMOVE_FILTER_FLAG_OUTPUT   = SNDRV_SEQ_REMOVE_OUTPUT,
} ALSASeqRemoveFilterFlag;

/**
 * ALSASeqUserClientError:
 * @ALSASEQ_USER_CLIENT_ERROR_FAILED:	The system call failed.
 *
 * A set of error code for GError with domain which equals to #alsaseq_user_client_error_quark()
 */
typedef enum {
    ALSASEQ_USER_CLIENT_ERROR_FAILED,
} ALSASeqUserClientError;

#endif
