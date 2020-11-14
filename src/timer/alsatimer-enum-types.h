#ifndef __ALSA_GOBJECT_ALSATIMER_ENUM_TYPES__H__
#define __ALSA_GOBJECT_ALSATIMER_ENUM_TYPES__H__

#include <sound/asound.h>

/**
 * ALSATimerClass:
 * @ALSATIMER_CLASS_NONE:   Invalid.
 * @ALSATIMER_CLASS_GLOBAL: The timer device is relevant to system.
 * @ALSATIMER_CLASS_CARD:   The timer device is relevant to sound card.
 * @ALSATIMER_CLASS_PCM:    The timer device is relevant to PCM device.
 *
 * A set of enumerations for the class of timer device.
 */
typedef enum {
    ALSATIMER_CLASS_NONE     = SNDRV_TIMER_CLASS_NONE,
    ALSATIMER_CLASS_GLOBAL   = SNDRV_TIMER_CLASS_GLOBAL,
    ALSATIMER_CLASS_CARD     = SNDRV_TIMER_CLASS_CARD,
    ALSATIMER_CLASS_PCM      = SNDRV_TIMER_CLASS_PCM,
} ALSATimerClass;

/**
 * ALSATimerSlaveClass:
 * @ALSATIMER_SLAVE_CLASS_NONE:             The timer instance is not slave.
 * @ALSATIMER_SLAVE_CLASS_APPLICATION:      The timer instance is slave to any application.
 * @ALSATIMER_SLAVE_CLASS_SEQUENCER:        The timer instance is slave to ALSA sequencer.
 *
 * A set of enumerations for the slave class of timer instance (not timer device).
 */
typedef enum {
    ALSATIMER_SLAVE_CLASS_NONE          = SNDRV_TIMER_SCLASS_NONE,
    ALSATIMER_SLAVE_CLASS_APPLICATION   = SNDRV_TIMER_SCLASS_APPLICATION,
    ALSATIMER_SLAVE_CLASS_SEQUENCER     = SNDRV_TIMER_SCLASS_SEQUENCER,
} ALSATimerSlaveClass;

/**
 * ALSATimerSpecificGlobalDevice:
 * @ALSATIMER_SPECIFIC_GLOBAL_DEVICE_SYSTEM:    The timer device is system timer wheel.
 * @ALSATIMER_SPECIFIC_GLOBAL_DEVICE_HRTIMER:   The timer device is system hrtimer.
 *
 * A set of enumerations for the kind of global timer device.
 */
typedef enum {
    ALSATIMER_SPECIFIC_GLOBAL_DEVICE_SYSTEM    = SNDRV_TIMER_GLOBAL_SYSTEM,
    ALSATIMER_SPECIFIC_GLOBAL_DEVICE_HRTIMER   = SNDRV_TIMER_GLOBAL_HRTIMER,
} ALSATimerSpecificGlobalDevice;

/**
 * ALSATimerDeviceInfoFlag:
 * @ALSATIMER_DEVICE_INFO_FLAG_SLAVE:       The timer device is slave to any timer device.
 *
 * A set of flags for the information of timer device.
 */
typedef enum /*< flags >*/
{
    ALSATIMER_DEVICE_INFO_FLAG_SLAVE =  SNDRV_TIMER_FLG_SLAVE,
} ALSATimerDeviceInfoFlag;

/**
 * ALSATimerInstanceParamFlag:
 * @ALSATIMER_INSTANCE_PARAM_FLAG_AUTO:         The instance receives any events after configured.
 * @ALSATIMER_INSTANCE_PARAM_FLAG_EXCLUSIVE:    The instance occupies the attached timer device.
 * @ALSATIMER_INSTANCE_PARAM_FLAG_EARLY_EVENT:  The instance receives early event after configured even if not started.
 *
 * A set of flags for the parameters of user client.
 */
typedef enum /*< flags >*/
{
    ALSATIMER_INSTANCE_PARAM_FLAG_AUTO          = SNDRV_TIMER_PSFLG_AUTO,
    ALSATIMER_INSTANCE_PARAM_FLAG_EXCLUSIVE     = SNDRV_TIMER_PSFLG_EXCLUSIVE,
    ALSATIMER_INSTANCE_PARAM_FLAG_EARLY_EVENT   = SNDRV_TIMER_PSFLG_EARLY_EVENT,
} ALSATimerInstanceParamFlag;

/**
 * ALSATimerEventType:
 * @ALSATIMER_EVENT_TYPE_RESOLUTION:    The event for timer resolution.
 * @ALSATIMER_EVENT_TYPE_TICK:          The event for timer tick.
 * @ALSATIMER_EVENT_TYPE_START:         The event for timer start.
 * @ALSATIMER_EVENT_TYPE_STOP:          The event for timer stop.
 * @ALSATIMER_EVENT_TYPE_CONTINUE:      The event for timer continue.
 * @ALSATIMER_EVENT_TYPE_PAUSE:         The event for timer pause.
 * @ALSATIMER_EVENT_TYPE_EARLY:         The event before timer start.
 * @ALSATIMER_EVENT_TYPE_SUSPEND:       The event for timer suspend.
 * @ALSATIMER_EVENT_TYPE_RESUME:        The event for timer resume.
 * @ALSATIMER_EVENT_TYPE_MSTART:        The event for master timer start.
 * @ALSATIMER_EVENT_TYPE_MSTOP:         The event for master timer stop.
 * @ALSATIMER_EVENT_TYPE_MCONTINUE:     The event for master timer continue.
 * @ALSATIMER_EVENT_TYPE_MPAUSE:        The event for master timer pause.
 * @ALSATIMER_EVENT_TYPE_MSUSPEND:      The event for master timer suspend.
 * @ALSATIMER_EVENT_TYPE_MRESUME:       The event for master timer resume.
 *
 * A set of enumerations for timer event.
 */
typedef enum
{
    ALSATIMER_EVENT_TYPE_RESOLUTION = SNDRV_TIMER_EVENT_RESOLUTION,
    ALSATIMER_EVENT_TYPE_TICK       = SNDRV_TIMER_EVENT_TICK,
    ALSATIMER_EVENT_TYPE_START      = SNDRV_TIMER_EVENT_START,
    ALSATIMER_EVENT_TYPE_STOP       = SNDRV_TIMER_EVENT_STOP,
    ALSATIMER_EVENT_TYPE_CONTINUE   = SNDRV_TIMER_EVENT_CONTINUE,
    ALSATIMER_EVENT_TYPE_PAUSE      = SNDRV_TIMER_EVENT_PAUSE,
    ALSATIMER_EVENT_TYPE_EARLY      = SNDRV_TIMER_EVENT_EARLY,
    ALSATIMER_EVENT_TYPE_SUSPEND    = SNDRV_TIMER_EVENT_SUSPEND,
    ALSATIMER_EVENT_TYPE_RESUME     = SNDRV_TIMER_EVENT_RESUME,
    ALSATIMER_EVENT_TYPE_MSTART     = SNDRV_TIMER_EVENT_MSTART,
    ALSATIMER_EVENT_TYPE_MSTOP      = SNDRV_TIMER_EVENT_MSTOP,
    ALSATIMER_EVENT_TYPE_MCONTINUE  = SNDRV_TIMER_EVENT_MCONTINUE,
    ALSATIMER_EVENT_TYPE_MPAUSE     = SNDRV_TIMER_EVENT_MPAUSE,
    ALSATIMER_EVENT_TYPE_MSUSPEND   = SNDRV_TIMER_EVENT_MSUSPEND,
    ALSATIMER_EVENT_TYPE_MRESUME    = SNDRV_TIMER_EVENT_MRESUME,
} ALSATimerEventType;

/**
 * ALSATimerEventDataType
 * @ALSATIMER_EVENT_DATA_TYPE_TICK:     The data for tick event.
 * @ALSATIMER_EVENT_DATA_TYPE_TSTAMP:   The data for timestamp event.
 *
 * A set of enumerations for the type of event data.
 */
typedef enum {
    ALSATIMER_EVENT_DATA_TYPE_TICK,
    ALSATIMER_EVENT_DATA_TYPE_TSTAMP,
} ALSATimerEventDataType;

/**
 * ALSATimerUserInstanceError:
 * @ALSATIMER_USER_INSTANCE_ERROR_FAILED:           The system call failed.
 * @ALSATIMER_USER_INSTANCE_ERROR_TIMER_NOT_FOUND:  The timer instance is not found.
 * @ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED:     The timer instance is not attached to any timer
 *                                                  device or the other instance.
 * @ALSATIMER_USER_INSTANCE_ERROR_ATTACHED:         The timer instance is already attached to timer
 *                                                  device or the other instance.
 *
 * A set of error code for GError with domain which equals to #alsatimer_user_instance_error_quark()
 */
typedef enum {
    ALSATIMER_USER_INSTANCE_ERROR_FAILED,
    ALSATIMER_USER_INSTANCE_ERROR_TIMER_NOT_FOUND,
    ALSATIMER_USER_INSTANCE_ERROR_NOT_ATTACHED,
    ALSATIMER_USER_INSTANCE_ERROR_ATTACHED,
} ALSATimerUserInstanceError;

#endif
