// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

/**
 * ALSASeqEvent:
 * A boxed structure for sequencer event.
 *
 * A [struct@Event] expresses any type of event in ALSA Sequencer. The event consists of some
 * product-type propertiess and two variant-type properties.
 *
 * The instance should be released by call of [func@GObject.boxed_free] since it can point to the
 * other allocated object for blob data, therefore it's preferable to use [func@GObject.boxed_copy]
 * to duplicate the instance so that the blob data is going to be duplicated as well.
 *
 * This is the list of product-type properties:
 *
 * - the type of event
 * - the mode of time stamp
 * - the mode of time
 * - the mode of length
 * - the mode of priority
 * - the numeric value of tag associated to the event
 * - the numeric identifier of queue to schedule the event
 * - destination address
 * - source address
 *
 * One of variant-type property is for time stamp.
 *
 * - tick count as time stamp of event
 * - real time as time stamp of event
 *
 * Another variant-type property is for data of event.
 *
 * - note
 * - control
 * - 12 bytes
 * - 3 quadlets
 * - blob as variable length of bytes
 * - pointer in VMA of user process
 * - queue control
 * - tick count as arbitrary time stamp
 * - real time as arbitrary time stamp
 * - arbitrary address
 * - connection between source and destination addresses
 * - result
 *
 * The type of time stamp is associated to the mode of time stamp, while the type of data is
 * associated to the type of event loosely. Each of the variant type property has single storage
 * internally, thus an event can includes the sole variant.
 *
 * The object wraps `struct snd_seq_event` in UAPI of Linux sound subsystem.
 */

static ALSASeqEvent *seq_event_copy(const ALSASeqEvent *self)
{
    ALSASeqEvent *ev;

    ev = g_malloc0(sizeof(*ev));
    memcpy(ev, self, sizeof(*ev));

    // NOTE: The instance of boxed structure should have the same size, thus variable length of
    // event with flattened layout is unavailable. The event is converted to the one pointing to
    // heap buffer for data.
    switch (ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
    {
        const guint8 *src;
        guint8 *dst;
        gsize length;

        src = (const guint8 *)self;
        src += sizeof(*ev);

        length = ev->data.ext.len;
        dst = g_malloc(length);
        memcpy(dst, src, length);

        ev->data.ext.ptr = dst;
        break;
    }
    case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
    case SNDRV_SEQ_EVENT_LENGTH_FIXED:
    default:
        break;
    }

    return ev;
}

static void free_blob_data(ALSASeqEvent *ev)
{
    // NOTE: free the allocated object when it follows to instance object.
    switch (ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
    {
        guint8 *ptr = (guint8 *)ev;
        ptr += sizeof(*ev);
        if (ptr != ev->data.ext.ptr)
            g_free(ev->data.ext.ptr);
        break;
    }
    case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
    case SNDRV_SEQ_EVENT_LENGTH_FIXED:
    default:
        break;
    }
}

static void seq_event_free(ALSASeqEvent *ev)
{
    free_blob_data(ev);
    g_free(ev);
}

G_DEFINE_BOXED_TYPE(ALSASeqEvent, alsaseq_event, seq_event_copy, seq_event_free);

/**
 * alsaseq_event_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] which has code of [enum@EventError]
 * enumerations.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(alsaseq-event-error-quark, alsaseq_event_error)

/**
 * alsaseq_event_new:
 * @event_type: A [enum@EventType].
 *
 * Allocate and return an instance of [struct@Event].
 *
 * Returns: An instance of [struct@Event].
 */
ALSASeqEvent *alsaseq_event_new(ALSASeqEventType event_type)
{
    struct snd_seq_event src = { 0 };

    src.type = event_type;

    return g_boxed_copy(ALSASEQ_TYPE_EVENT, &src);
}

/**
 * alsaseq_event_get_event_type:
 * @self: A [struct@Event].
 * @event_type: (out): A [enum@EventType].
 *
 * Get the type of event.
 */
void alsaseq_event_get_event_type(const ALSASeqEvent *self, ALSASeqEventType *event_type)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(event_type != NULL);

    *event_type = self->type;
}

/**
 * alsaseq_event_get_length_mode:
 * @self: A [struct@Event].
 * @length_mode: (out): A [enum@EventLengthMode] for the mode of data length.
 *
 * Get the mode of data length for the event.
 */
void alsaseq_event_get_length_mode(const ALSASeqEvent *self, ALSASeqEventLengthMode *length_mode)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(length_mode != NULL);

    *length_mode = self->flags & SNDRV_SEQ_EVENT_LENGTH_MASK;
}

/**
 * alsaseq_event_get_tstamp_mode:
 * @self: A [struct@Event].
 * @tstamp_mode: (out): A [enum@EventTstampMode] for the mode of time stamp.
 *
 * Get the mode of time stamp for the event.
 */
void alsaseq_event_get_tstamp_mode(const ALSASeqEvent *self, ALSASeqEventTstampMode *tstamp_mode)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(tstamp_mode != NULL);

    *tstamp_mode = self->flags & SNDRV_SEQ_TIME_STAMP_MASK;
}

/**
 * alsaseq_event_get_time_mode:
 * @self: A [struct@Event].
 * @time_mode: (out): A [enum@EventTimeMode] for the mode of time.
 *
 * Get the mode of time for the event.
 */
void alsaseq_event_get_time_mode(const ALSASeqEvent *self, ALSASeqEventTimeMode *time_mode)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(time_mode != NULL);

    *time_mode = self->flags & SNDRV_SEQ_TIME_MODE_MASK;
}

/**
 * alsaseq_event_set_time_mode:
 * @self: A [struct@Event].
 * @time_mode: A [enum@EventTimeMode] for the mode of time.
 *
 * Set the mode of time for the event.
 */
void alsaseq_event_set_time_mode(ALSASeqEvent *self, ALSASeqEventTimeMode time_mode)
{
    g_return_if_fail(self != NULL);

    self->flags &= ~SNDRV_SEQ_TIME_MODE_MASK;
    self->flags |= time_mode;
}

/**
 * alsaseq_event_get_priority_mode:
 * @self: A [struct@Event].
 * @priority_mode: (out): A [enum@EventPriorityMode] The mode of priority.
 *
 * Get the mode of priority for the event.
 */
void alsaseq_event_get_priority_mode(const ALSASeqEvent *self,
                                     ALSASeqEventPriorityMode *priority_mode)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(priority_mode != NULL);

    *priority_mode = self->flags & SNDRV_SEQ_PRIORITY_MASK;
}

/**
 * alsaseq_event_set_priority_mode:
 * @self: A [struct@Event].
 * @priority_mode: A [enum@EventPriorityMode] for the mode of priority.
 *
 * Set the mode of priority for the event.
 */
void alsaseq_event_set_priority_mode(ALSASeqEvent *self, ALSASeqEventPriorityMode priority_mode)
{
    g_return_if_fail(self != NULL);

    self->flags &= ~SNDRV_SEQ_PRIORITY_MASK;
    self->flags |= priority_mode;
}

/**
 * alsaseq_event_get_tag:
 * @self: A [struct@Event].
 * @tag: (out): The numeric value of tag.
 *
 * Get the numeric value of tag associated to the event.
 */
void alsaseq_event_get_tag(const ALSASeqEvent *self, gint8 *tag)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(tag != NULL);

    *tag = self->tag;
}

/**
 * alsaseq_event_set_tag:
 * @self: A [struct@Event].
 * @tag: The numeric value of tag.
 *
 * Set the numeric value of tag associated to the event.
 */
void alsaseq_event_set_tag(ALSASeqEvent *self, gint8 tag)
{
    g_return_if_fail(self != NULL);

    self->tag = tag;
}

/**
 * alsaseq_event_get_queue_id:
 * @self: A [struct@Event].
 * @queue_id: (out): The numeric identifier of queue.
 *
 * Get the numeric identifier of queue to schedule the event.
 */
void alsaseq_event_get_queue_id(const ALSASeqEvent *self, guint8 *queue_id)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(queue_id != NULL);

    *queue_id = self->queue;
}

/**
 * alsaseq_event_set_queue_id:
 * @self: A [struct@Event].
 * @queue_id: The numeric identifier of queue.
 *
 * Set the numeric identifier of queue to schedule the event.
 */
void alsaseq_event_set_queue_id(ALSASeqEvent *self, guint8 queue_id)
{
    g_return_if_fail(self != NULL);

    self->queue = queue_id;
}

/**
 * alsaseq_event_get_source:
 * @self: A [struct@Event].
 * @addr: (out) (transfer none): A [struct@Addr] for source address.
 *
 * Refer to the source address of event.
 */
void alsaseq_event_get_source(const ALSASeqEvent *self, const ALSASeqAddr **addr)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(addr != NULL);

    *addr = &self->source;
}

/**
 * alsaseq_event_set_source:
 * @self: A [struct@Event].
 * @addr: A [struct@Addr] for source address.
 *
 * Copy the address as source of event.
 */
void alsaseq_event_set_source(ALSASeqEvent *self, const ALSASeqAddr *addr)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(addr != NULL);

    self->source = *addr;
}

/**
 * alsaseq_event_get_destination:
 * @self: A [struct@Event].
 * @addr: (out) (transfer none): A [struct@Addr] for event destination.
 *
 * Refer to the destination address of event.
 */
void alsaseq_event_get_destination(const ALSASeqEvent *self, const ALSASeqAddr **addr)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(addr != NULL);

    *addr = &self->dest;
}

/**
 * alsaseq_event_set_destination:
 * @self: A [struct@Event].
 * @addr: A [struct@Addr] for event destination.
 *
 * Copy the address as destination of event.
 */
void alsaseq_event_set_destination(ALSASeqEvent *self, const ALSASeqAddr *addr)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(addr != NULL);

    self->dest = *addr;
}

static gboolean validate_event_tstamp_mode(const struct snd_seq_event *ev,
                                           ALSASeqEventTstampMode mode, GError **error)
{
    int val = ev->flags & SNDRV_SEQ_TIME_STAMP_MASK;

    if (val != mode) {
        return FALSE;
    }

    return TRUE;
}

/**
 * alsaseq_event_get_tick_time:
 * @self: A [struct@Event].
 * @tick_time: (out) (transfer none): The tick time of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the tick time of event, available only when [enum@EventTstampMode].TICK is retrieved by
 * [method@Event.get_tstamp_mode].
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_tick_time(const ALSASeqEvent *self, guint *tick_time, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(tick_time != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_TICK, error))
        return FALSE;

    *tick_time = self->time.tick;

    return TRUE;
}

/**
 * alsaseq_event_set_tick_time:
 * @self: A [struct@Event].
 * @tick_time: The tick time of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the real time to the event and set [enum@EventTstampMode].TICK.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_tick_time(ALSASeqEvent *self, guint tick_time, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_TICK, error))
        return FALSE;

    self->time.tick = tick_time;

    return TRUE;
}

/**
 * alsaseq_event_get_real_time:
 * @self: A [struct@Event].
 * @real_time: (array fixed-size=2) (out) (transfer none): The real time of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the real time to the event, available only when [enum@EventTstampMode].REAL is retrieved by
 * [method@Event.get_tstamp_mode].
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_real_time(const ALSASeqEvent *self, const guint32 *real_time[2],
                                     GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(real_time != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_REAL, error))
        return FALSE;

    *real_time = (const guint32 *)&self->time.time;

    return TRUE;
}

/**
 * alsaseq_event_set_real_time:
 * @self: A [struct@Event].
 * @real_time: (array fixed-size=2): The real time of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the real time to the event and set [enum@EventTstampMode].REAL.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_real_time(ALSASeqEvent *self, const guint32 real_time[2], GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(real_time != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_REAL, error))
        return FALSE;

    self->time.time.tv_sec = real_time[0];
    self->time.time.tv_nsec = real_time[1];

    return TRUE;
}

// NOTE: Some events for System V IPC was added in alsa-driver v0.5.0 and removed in alsa-driver
// v0.9.0. The event was designed to deliver key and length in `struct snd_seq_ev_ipc`.
//
// - 140: IPCSHM
// - 145: USR_VARIPC0
// - 146: USR_VARIPC1
// - 147: USR_VARIPC2
// - 148: USR_VARIPC3
// - 149: USR_VARIPC4
//
// Some events for instrument memory bank was added in alsa-driver v0.3.0 and removed in
// Linux kernel v2.6.25. They were specific to some hardware electronic musical synthesizers.
// The data was delivered by two structures; `struct snd_seq_ev_inst_begin` and
// `struct snd_seq_ev_ext` with LENGTH_VARUSR flag for direct dispatch.
//
// - 100: INSTR_BEGIN
// - 101: INSTR_END
// - 102: INSTR_INFO
// - 103: INSTR_INFO_RESULT
// - 104: INSTR_FINFO
// - 105: INSTR_FINFO_RESULT
// - 106: INSTR_RESET
// - 107: INSTR_STATUS
// - 108: INSTR_STATUS_RESULT
// - 109: INSTR_PUT
// - 110: INSTR_GET
// - 111: INSTR_GET_RESULT
// - 112: INSTR_FREE
// - 113: INSTR_LIST
// - 114: INSTR_LIST_RESULT
// - 115: INSTR_CLUSTER
// - 116: INSTR_CLUSTER_GET
// - 117: INSTR_CLUSTER_RESULT
// - 118: INSTR_CHANGE
//
// Some events for hardware sampler function was added in alsa-driver v0.5.0 and removed in Linux
// kernel v2.6.25. The data was delivered by `struct snd_seq_ev_sample_control`.
//
// - 70: SAMPLE
// - 71: SAMPLE_CLUSTER
// - 72: SAMPLE_START
// - 73: SAMPLE_STOP
// - 74: SAMPLE_FREQ
// - 75: SAMPLE_VOLUME
// - 76: SAMPLE_LOOP
// - 77: SAMPLE_POSITION
// - 78: SAMPLE_PRIVATE1
//

enum seq_event_data_flag {
    // The userspace application can decide the association between type and data voluntarily.
    SEQ_EVENT_DATA_FLAG_USER                = 0x00100000,

    // The event doesn't have explicit data.
    SEQ_EVENT_DATA_FLAG_BLANK               = 0x01000000,

    // This is not delivered to user space. It's jsut ignored to write by user space application.
    SEQ_EVENT_DATA_FLAG_NONE                = 0x02000000,

    // Fixed length of data.
    SEQ_EVENT_DATA_FLAG_RESULT              = 0x00000001,
    SEQ_EVENT_DATA_FLAG_NOTE                = 0x00000002,
    SEQ_EVENT_DATA_FLAG_CONTROL             = 0x00000004,
    SEQ_EVENT_DATA_FLAG_QUEUE               = 0x00000008,
    SEQ_EVENT_DATA_FLAG_ADDR                = 0x00000010,
    SEQ_EVENT_DATA_FLAG_CONNECT             = 0x00000020,
    SEQ_EVENT_DATA_FLAG_BYTE                = 0x00000040,
    SEQ_EVENT_DATA_FLAG_QUADLET             = 0x00000080,
    SEQ_EVENT_DATA_FLAG_TSTAMP              = 0x00000100,
    SEQ_EVENT_DATA_FLAG_FIXED_ANY           = 0x00000fff,

    // Variable length of data.
    SEQ_EVENT_DATA_FLAG_BLOB                = 0x00001000,
    SEQ_EVENT_DATA_FLAG_VARIABLE_ANY        = 0x0000f000,

    // Pointer to arbitrary data in user space.
    SEQ_EVENT_DATA_FLAG_POINTER             = 0x00010000,
    SEQ_EVENT_DATA_FLAG_VARUSR_ANY          = 0x000f0000,

    SEQ_EVENT_DATA_FLAG_ANY                 = 0x000fffff,
};

static const enum seq_event_data_flag seq_event_data_flags[] = {
    [0 ... 4]       = SEQ_EVENT_DATA_FLAG_RESULT,
    [5 ... 9]       = SEQ_EVENT_DATA_FLAG_NOTE,
    [10 ... 24]     = SEQ_EVENT_DATA_FLAG_CONTROL,
    [25 ... 29]     = SEQ_EVENT_DATA_FLAG_FIXED_ANY,
    [30 ... 39]     = SEQ_EVENT_DATA_FLAG_QUEUE,
    [40 ... 49]     = SEQ_EVENT_DATA_FLAG_BLANK,
    [50 ... 59]     = SEQ_EVENT_DATA_FLAG_ANY,          // Used by kernel clients.
    [60 ... 65]     = SEQ_EVENT_DATA_FLAG_ADDR,
    [66 ... 69]     = SEQ_EVENT_DATA_FLAG_CONNECT,
    [70 ... 79]     = SEQ_EVENT_DATA_FLAG_FIXED_ANY,    // For ancient sample type.
    [80 ... 89]     = SEQ_EVENT_DATA_FLAG_FIXED_ANY,    // For ancient sample type.
    [90 ... 99]     = SEQ_EVENT_DATA_FLAG_FIXED_ANY | SEQ_EVENT_DATA_FLAG_USER,
    [100 ... 109]   = SEQ_EVENT_DATA_FLAG_FIXED_ANY,    // For ancient instrument type.
    [110 ... 119]   = SEQ_EVENT_DATA_FLAG_FIXED_ANY,    // For ancient instrument type.
    [120 ... 129]   = SEQ_EVENT_DATA_FLAG_FIXED_ANY,
    [130 ... 134]   = SEQ_EVENT_DATA_FLAG_BLOB,
    [135 ... 139]   = SEQ_EVENT_DATA_FLAG_VARIABLE_ANY | SEQ_EVENT_DATA_FLAG_USER,
    [140 ... 149]   = SEQ_EVENT_DATA_FLAG_VARIABLE_ANY, // For ancient IPC type.
    [150 ... 254]   = SEQ_EVENT_DATA_FLAG_ANY,          // Used by kernel clients.
    [255]           = SEQ_EVENT_DATA_FLAG_NONE,
};

static gboolean validate_event_type(const struct snd_seq_event *ev, enum seq_event_data_flag flag,
                                    GError **error)
{
    if ((seq_event_data_flags[ev->type] & flag) == 0) {
        g_set_error_literal(error, ALSASEQ_EVENT_ERROR, ALSASEQ_EVENT_ERROR_INVALID_DATA_TYPE,
                            "The type of requested data is invalid in the event");
        return FALSE;
    }

    return TRUE;
}

static gboolean validate_event_type_and_length(const struct snd_seq_event *ev,
                                               enum seq_event_data_flag flag,
                                               GError **error)
{
    int expected, val = ev->flags & SNDRV_SEQ_EVENT_LENGTH_MASK;

    if (!validate_event_type(ev, flag, error))
            return FALSE;

    switch (flag) {
    case SEQ_EVENT_DATA_FLAG_RESULT:
    case SEQ_EVENT_DATA_FLAG_NOTE:
    case SEQ_EVENT_DATA_FLAG_CONTROL:
    case SEQ_EVENT_DATA_FLAG_QUEUE:
    case SEQ_EVENT_DATA_FLAG_ADDR:
    case SEQ_EVENT_DATA_FLAG_CONNECT:
    case SEQ_EVENT_DATA_FLAG_BYTE:
    case SEQ_EVENT_DATA_FLAG_QUADLET:
    case SEQ_EVENT_DATA_FLAG_TSTAMP:
        expected = SNDRV_SEQ_EVENT_LENGTH_FIXED;
        break;
    case SEQ_EVENT_DATA_FLAG_BLOB:
        expected = SNDRV_SEQ_EVENT_LENGTH_VARIABLE;
        break;
    case SEQ_EVENT_DATA_FLAG_POINTER:
        expected = SNDRV_SEQ_EVENT_LENGTH_VARUSR;
        break;
    default:
        expected = 0xffffffff;
        break;
    }

    if (val != expected) {
        g_set_error_literal(error, ALSASEQ_EVENT_ERROR, ALSASEQ_EVENT_ERROR_INVALID_LENGTH_MODE,
                            "The mode of length for requested data is invalid in the event");
        return FALSE;
    }

    return TRUE;
}

static void clear_data_fields(struct snd_seq_event *ev)
{
    free_blob_data(ev);
    memset(&ev->data, 0, sizeof(ev->data));
}

static void ensure_fixed_length_event(struct snd_seq_event *ev)
{
    clear_data_fields(ev);

    ev->flags &= ~SNDRV_SEQ_EVENT_LENGTH_MASK;
    ev->flags |= SNDRV_SEQ_EVENT_LENGTH_FIXED;
}

static void ensure_variable_length_event(struct snd_seq_event *ev, const guint8 *buf, gsize length)
{
    guint8 *ptr;

    clear_data_fields(ev);

    ptr = g_malloc(length);
    memcpy(ptr, buf, length);

    ev->data.ext.ptr = ptr;
    ev->data.ext.len = length;

    ev->flags &= ~SNDRV_SEQ_EVENT_LENGTH_MASK;
    ev->flags |= SNDRV_SEQ_EVENT_LENGTH_VARIABLE;
}

static void ensure_pointer_length_event(struct snd_seq_event *ev)
{
    clear_data_fields(ev);

    ev->flags &= ~SNDRV_SEQ_EVENT_LENGTH_MASK;
    ev->flags |= SNDRV_SEQ_EVENT_LENGTH_VARUSR;
}

/**
 * alsaseq_event_get_note_data:
 * @self: A [struct@Event].
 * @data: (out) (transfer none): The note data of event.
 * @error: A [struct@GLib.Error].
 *
 * Refer to the note data, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].NOTE
 * - [enum@EventType].NOTEON
 * - [enum@EventType].NOTEOFF
 * - [enum@EventType].KEYPRESS
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_note_data(const ALSASeqEvent *self, const ALSASeqEventDataNote **data,
                                     GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_NOTE, error))
        return FALSE;

    *data = (const ALSASeqEventDataNote *)&self->data.note;

    return TRUE;
}

/**
 * alsaseq_event_set_note_data:
 * @self: A [struct@Event].
 * @data: The note data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the note data, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].NOTE
 * - [enum@EventType].NOTEON
 * - [enum@EventType].NOTEOFF
 * - [enum@EventType].KEYPRESS
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_note_data(ALSASeqEvent *self, const ALSASeqEventDataNote *data,
                                     GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_NOTE, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.note = *(struct snd_seq_ev_note *)data;

    return TRUE;
}

/**
 * alsaseq_event_get_ctl_data:
 * @self: A [struct@Event].
 * @data: (out) (transfer none): The control data of event.
 * @error: A [struct@GLib.Error].
 *
 * Refer to the control data, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].CONTROLLER
 * - [enum@EventType].PGMCHANGE
 * - [enum@EventType].CHANPRESS
 * - [enum@EventType].PITCHBEND
 * - [enum@EventType].CONTROL14
 * - [enum@EventType].NONREGPARAM
 * - [enum@EventType].REGPARAM
 * - [enum@EventType].SONGPOS
 * - [enum@EventType].SONGSEL
 * - [enum@EventType].QFRAME
 * - [enum@EventType].TIMESIGN
 * - [enum@EventType].KEYSIGN
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_ctl_data(const ALSASeqEvent *self, const ALSASeqEventDataCtl **data,
                                    GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_CONTROL, error))
        return FALSE;

    *data = (const ALSASeqEventDataCtl *)&self->data.control;

    return TRUE;
}

/**
 * alsaseq_event_set_ctl_data:
 * @self: A [struct@Event].
 * @data: The control data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the control data, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].CONTROLLER
 * - [enum@EventType].PGMCHANGE
 * - [enum@EventType].CHANPRESS
 * - [enum@EventType].PITCHBEND
 * - [enum@EventType].CONTROL14
 * - [enum@EventType].NONREGPARAM
 * - [enum@EventType].REGPARAM
 * - [enum@EventType].SONGPOS
 * - [enum@EventType].SONGSEL
 * - [enum@EventType].QFRAME
 * - [enum@EventType].TIMESIGN
 * - [enum@EventType].KEYSIGN
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_ctl_data(ALSASeqEvent *self, const ALSASeqEventDataCtl *data,
                                    GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_CONTROL, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.control = *(struct snd_seq_ev_ctrl *)data;

    return TRUE;
}

/**
 * alsaseq_event_get_byte_data:
 * @self: A [struct@Event].
 * @data: (array fixed-size=12) (out) (transfer none): The byte data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the byte data, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_byte_data(const ALSASeqEvent *self, const guint8 *data[12],
                                     GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_BYTE, error))
        return FALSE;

    // NOTE: PyGObject v3.42 aborts with NULL pointer to fixed array...
    *data = self->data.raw8.d;

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_BYTE, error))
        return FALSE;

    return TRUE;
}

/**
 * alsaseq_event_set_byte_data:
 * @self: A [struct@Event].
 * @data: (array fixed-size=12): The byte data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the byte data, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_byte_data(ALSASeqEvent *self, const guint8 data[12], GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_BYTE, error))
        return FALSE;

    ensure_fixed_length_event(self);
    memcpy(self->data.raw8.d, data, sizeof(self->data.raw8.d));

    return TRUE;
}

/**
 * alsaseq_event_get_quadlet_data:
 * @self: A [struct@Event].
 * @data: (array fixed-size=3) (out) (transfer none): The quadlet data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the quadlet data of event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_quadlet_data(const ALSASeqEvent *self, const guint32 *data[3],
                                        GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_QUADLET, error))
        return FALSE;

    // NOTE: PyGObject v3.42 aborts with NULL pointer to fixed array...
    *data = self->data.raw32.d;

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_QUADLET, error))
        return FALSE;

    return TRUE;
}

/**
 * alsaseq_event_set_quadlet_data:
 * @self: A [struct@Event].
 * @data: (array fixed-size=3): The quadlet data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the quadlet data to the event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_quadlet_data(ALSASeqEvent *self, const guint32 data[3], GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_QUADLET, error))
        return FALSE;

    ensure_fixed_length_event(self);
    memcpy(self->data.raw32.d, data, sizeof(self->data.raw32.d));

    return TRUE;
}

/**
 * alsaseq_event_get_blob_data:
 * @self: A [struct@Event].
 * @data: (array length=length) (out) (transfer none): The pointer to blob data.
 * @length: The length of data in byte unit.
 * @error: A [struct@GLib.Error].
 *
 * Refer to the blob data, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].SYSEX
 * - [enum@EventType].BOUNCE
 * - [enum@EventType].USR_VAR0
 * - [enum@EventType].USR_VAR1
 * - [enum@EventType].USR_VAR2
 * - [enum@EventType].USR_VAR3
 * - [enum@EventType].USR_VAR4
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_blob_data(const ALSASeqEvent *self, const guint8 **data, gsize *length,
                                     GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(length != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_BLOB, error))
        return FALSE;

    *data = self->data.ext.ptr;
    *length = self->data.ext.len;

    return TRUE;
}

/**
 * alsaseq_event_set_blob_data:
 * @self: A [struct@Event].
 * @data: (array length=length): The pointer to blob data for the event.
 * @length: The length of data.
 * @error: A [struct@GLib.Error].
 *
 * Copy the quadlet data to the event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].SYSEX
 * - [enum@EventType].BOUNCE
 * - [enum@EventType].USR_VAR0
 * - [enum@EventType].USR_VAR1
 * - [enum@EventType].USR_VAR2
 * - [enum@EventType].USR_VAR3
 * - [enum@EventType].USR_VAR4
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_blob_data(ALSASeqEvent *self, const guint8 *data, gsize length,
                                     GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_BLOB, error))
        return FALSE;

    ensure_variable_length_event(self, data, length);

    return TRUE;
}

/**
 * alsaseq_event_get_pointer_data:
 * @self: A [struct@Event].
 * @data: (out) (transfer none): A pointer to user data.
 * @length: (out): The length of user data.
 * @error: A [struct@GLib.Error].
 *
 * Get the pointer and length in data of event. It has no gurantee that the dereference of pointer
 * is safe in VMA of user process, thus users have to know the purpose of the pointer in advance;
 * e.g. inter-process communication between forked parent and child processes.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_pointer_data(const ALSASeqEvent *self, gconstpointer *data,
                                        gsize *length, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(length != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_POINTER, error))
        return FALSE;

    *data = self->data.ext.ptr;
    *length = self->data.ext.len;

    return TRUE;
}

/**
 * alsaseq_event_set_pointer_data:
 * @self: A [struct@Event].
 * @data: A pointer to user data.
 * @length: The length of user data.
 * @error: A [struct@GLib.Error].
 *
 * Copy the pointer and length to data of event. When using the type, the event should be deliverd
 * directly without scheduling to queue. The aim of data pointed by it is arbitrary, thus
 * programmer needs to decide protocol between transmitter and receiver in advance.
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_pointer_data(ALSASeqEvent *self, const gpointer data, gsize length,
                                        GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(length > 0, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_POINTER, error))
        return FALSE;

    ensure_pointer_length_event(self);
    self->data.ext.ptr = data;
    self->data.ext.len = length;

    return TRUE;
}

/**
 * alsaseq_event_get_queue_data:
 * @self: A [struct@Event].
 * @data: (out) (transfer none): The queue data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the queue data of event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].START
 * - [enum@EventType].CONTINUE
 * - [enum@EventType].STOP
 * - [enum@EventType].SETPOS_TICK
 * - [enum@EventType].SETPOS_TIME
 * - [enum@EventType].TEMPO
 * - [enum@EventType].CLOCK
 * - [enum@EventType].TICK
 * - [enum@EventType].QUEUE_SKEW
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_queue_data(const ALSASeqEvent *self, const ALSASeqEventDataQueue **data,
                                      GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_QUEUE, error))
        return FALSE;

    *data = (const ALSASeqEventDataQueue *)&self->data.queue;

    return TRUE;
}

/**
 * alsaseq_event_set_queue_data:
 * @self: A [struct@Event].
 * @data: The queue data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the queue data to the event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].START
 * - [enum@EventType].CONTINUE
 * - [enum@EventType].STOP
 * - [enum@EventType].SETPOS_TICK
 * - [enum@EventType].SETPOS_TIME
 * - [enum@EventType].TEMPO
 * - [enum@EventType].CLOCK
 * - [enum@EventType].TICK
 * - [enum@EventType].QUEUE_SKEW
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_queue_data(ALSASeqEvent *self, const ALSASeqEventDataQueue *data,
                                      GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_QUEUE, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.queue = *(struct snd_seq_ev_queue_control *)data;

    return TRUE;
}

/**
 * alsaseq_event_get_tick_time_data:
 * @self: A [struct@Event].
 * @tick_time: (out) (transfer none): The tick time data of event.
 *
 * Get the tick time data of event, available when [method@Event.get_tstamp_mode is
 * [enum@EventTstampMode].TICK and [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_tick_time_data(const ALSASeqEvent *self, guint *tick_time,
                                          GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(tick_time != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_TSTAMP, error))
        return FALSE;

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_TICK, error))
        return FALSE;

    *tick_time = self->data.time.tick;

    return TRUE;
}

/**
 * alsaseq_event_set_tick_time_data:
 * @self: A [struct@Event].
 * @tick_time: The tick time data of event.
 *
 * Copy the tick time data to the event, available when [method@Event.get_tstamp_mode is
 * [enum@EventTstampMode].TICK and [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_tick_time_data(ALSASeqEvent *self, guint tick_time, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_TSTAMP, error))
        return FALSE;

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_TICK, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.time.tick = tick_time;

    return TRUE;
}

/**
 * alsaseq_event_get_real_time_data:
 * @self: A [struct@Event].
 * @real_time: (array fixed-size=2) (out) (transfer none): The real time data of event.
 *
 * Get the real time data of event, available when [method@Event.get_tstamp_mode is
 * [enum@EventTstampMode].REAL and [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_real_time_data(const ALSASeqEvent *self, const guint32 *real_time[2],
                                          GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(real_time != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_TSTAMP, error))
        return FALSE;

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_REAL, error))
        return FALSE;

    *real_time = (const guint32 *)&self->data.time.time;

    return TRUE;
}

/**
 * alsaseq_event_set_real_time_data:
 * @self: A [struct@Event].
 * @real_time: (array fixed-size=2) : The real time data of event.
 *
 * Copy the real time data to the event, available [method@Event.get_tstamp_mode is
 * [enum@EventTstampMode].REAL and [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_real_time_data(ALSASeqEvent *self, const guint32 real_time[2],
                                          GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(real_time != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_TSTAMP, error))
        return FALSE;

    if (!validate_event_tstamp_mode(self, ALSASEQ_EVENT_TSTAMP_MODE_REAL, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.time.time.tv_sec = real_time[0];
    self->data.time.time.tv_nsec = real_time[1];

    return TRUE;
}

/**
 * alsaseq_event_get_addr_data:
 * @self: A [struct@Event].
 * @data: (out) (transfer none): The address data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the address data of event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].CLIENT_START
 * - [enum@EventType].CLIENT_EXIT
 * - [enum@EventType].CLIENT_CHANGE
 * - [enum@EventType].PORT_START
 * - [enum@EventType].PORT_EXIT
 * - [enum@EventType].PORT_CHANGE
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_addr_data(const ALSASeqEvent *self, const ALSASeqAddr **data,
                                     GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_ADDR, error))
        return FALSE;

    *data = &self->data.addr;

    return TRUE;
}

/**
 * alsaseq_event_set_addr_data:
 * @self: A [struct@Event].
 * @data: The address data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the address data of event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].CLIENT_START
 * - [enum@EventType].CLIENT_EXIT
 * - [enum@EventType].CLIENT_CHANGE
 * - [enum@EventType].PORT_START
 * - [enum@EventType].PORT_EXIT
 * - [enum@EventType].PORT_CHANGE
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_addr_data(ALSASeqEvent *self, const ALSASeqAddr *data, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_ADDR, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.addr = *data;

    return TRUE;
}

/**
 * alsaseq_event_get_connect_data:
 * @self: A [struct@Event].
 * @data: (out) (transfer none): The connect data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the connect data of event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].PORT_SUBSCRIBED
 * - [enum@EventType].PORT_UNSUBSCRIBED
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_connect_data(const ALSASeqEvent *self,
                                        const ALSASeqEventDataConnect **data, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_CONNECT, error))
        return FALSE;

    *data = (const ALSASeqEventDataConnect *)&self->data.connect;

    return TRUE;
}

/**
 * alsaseq_event_set_connect_data:
 * @self: A [struct@Event].
 * @data: The connect data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the connect data to the event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].PORT_SUBSCRIBED
 * - [enum@EventType].PORT_UNSUBSCRIBED
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_connect_data(ALSASeqEvent *self, const ALSASeqEventDataConnect *data,
                                        GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_CONNECT, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.connect = *(struct snd_seq_connect *)data;

    return TRUE;
}

/**
 * alsaseq_event_get_result_data:
 * @self: A [struct@Event].
 * @data: (out) (transfer none): The result data of event.
 * @error: A [struct@GLib.Error].
 *
 * Get the result data of event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].SYSTEM
 * - [enum@EventType].RESULT
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_get_result_data(const ALSASeqEvent *self,
                                       const ALSASeqEventDataResult **data, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type_and_length(self, SEQ_EVENT_DATA_FLAG_RESULT, error))
        return FALSE;

    *data = (const ALSASeqEventDataResult *)&self->data.result;

    return TRUE;
}

/**
 * alsaseq_event_set_result_data:
 * @self: A [struct@Event].
 * @data: The result data of event.
 * @error: A [struct@GLib.Error].
 *
 * Copy the result data to the event, available when [method@Event.get_event_type] results in one of:
 *
 * - [enum@EventType].SYSTEM
 * - [enum@EventType].RESULT
 * - [enum@EventType].USR0
 * - [enum@EventType].USR1
 * - [enum@EventType].USR2
 * - [enum@EventType].USR3
 * - [enum@EventType].USR4
 * - [enum@EventType].USR5
 * - [enum@EventType].USR6
 * - [enum@EventType].USR7
 * - [enum@EventType].USR8
 * - [enum@EventType].USR9
 *
 * Returns: %TRUE when the overall operation finishes successfully, else %FALSE.
 */
gboolean alsaseq_event_set_result_data(ALSASeqEvent *self, const ALSASeqEventDataResult *data,
                                       GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!validate_event_type(self, SEQ_EVENT_DATA_FLAG_RESULT, error))
        return FALSE;

    ensure_fixed_length_event(self);
    self->data.result = *(struct snd_seq_result *)data;

    return TRUE;
}

void seq_event_copy_flattened(const ALSASeqEvent *self, guint8 *buf, gsize length)
{
    memcpy(buf, self, sizeof(*self));

    switch (self->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
    {
        g_return_if_fail(sizeof(*self) + self->data.ext.len <= length);
        memcpy(buf + sizeof(*self), self->data.ext.ptr, self->data.ext.len);
        break;
    }
    case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
    case SNDRV_SEQ_EVENT_LENGTH_FIXED:
    default:
        break;
    }
}

// Calculate the length of event followed by allocated object for blob data at variable type. This
// is the default layout for buffer read from ALSA Sequencer core.
gsize seq_event_calculate_flattened_length(const ALSASeqEvent *self, gboolean aligned)
{
    gsize length = sizeof(*self);

    switch (self->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
        length += self->data.ext.len;
        break;
    case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
    case SNDRV_SEQ_EVENT_LENGTH_FIXED:
    default:
        break;
    }

    if (aligned)
        length = (length + sizeof(*self) - 1) / sizeof(*self) * sizeof(*self);

    return length;
}

gboolean seq_event_is_deliverable(const ALSASeqEvent *self)
{
    enum seq_event_data_flag flags = seq_event_data_flags[self->type];

    switch (self->flags & SNDRV_SEQ_EVENT_LENGTH_MASK) {
    case SNDRV_SEQ_EVENT_LENGTH_FIXED:
    {
        // NOTE: ALSA Sequencer core can emulates note on/off events by simple note event. The
        // emulation is in queue implementation, thus it should not be dispatched directly.
        if (self->type == ALSASEQ_EVENT_TYPE_NOTE)
            return self->queue != SNDRV_SEQ_QUEUE_DIRECT;
        else
            return (flags & SEQ_EVENT_DATA_FLAG_FIXED_ANY) > 0;
    }
    case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
        return (flags & SEQ_EVENT_DATA_FLAG_VARIABLE_ANY) > 0;
    case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
        // NOTE: The data pointed by the pointer should be delivered to receivers immediately so
        // that the user space application has no need to wait for completion of scheduled delivery.
        return ((flags & SEQ_EVENT_DATA_FLAG_VARUSR_ANY) > 0) &&
                (self->queue == SNDRV_SEQ_QUEUE_DIRECT);
    default:
        return FALSE;
    }
}
