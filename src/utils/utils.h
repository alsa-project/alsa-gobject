// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_UTILS_H__
#define __ALSA_GOBJECT_UTILS_H__

#include <stdlib.h>
#include <stdarg.h>
#include <systemd/sd-device.h>

#define generate_file_error(exception, errno, ...) \
        g_set_error(exception, G_FILE_ERROR, g_file_error_from_errno(errno), __VA_ARGS__)

#define CARD_SYSNAME_PREFIX             "card"
#define CARD_SYSNAME_TEMPLATE           CARD_SYSNAME_PREFIX "%u"
#define CONTROL_SYSNAME_TEMPLATE        "controlC%u"
// 'C' is required apart from emulation of Open Sound System.
#define RAWMIDI_SYSNAME_PREFIX_TEMPLATE "midiC%u"
#define RAWMIDI_SYSNAME_TEMPLATE        RAWMIDI_SYSNAME_PREFIX_TEMPLATE "D%u"
// 'C' is required apart from emulation of Open Sound System.
#define HWDEP_SYSNAME_PREFIX_TEMPLATE   "hwC%u"
#define HWDEP_SYSNAME_TEMPLATE          HWDEP_SYSNAME_PREFIX_TEMPLATE "D%u"
#define TIMER_SYSNAME                   "timer"
#define SEQ_SYSNAME                     "seq"

long long_from_string(const char *literal, long *number);

int allocate_string(char **dst, const char *template, va_list ap);

int lookup_and_allocate_string_by_sysname(char **name, const char *sysname,
                                          int (*func)(sd_device *, const char **));

int generate_sysnum_list_by_sysname_prefix(unsigned int **entries, unsigned long *entry_count,
                                           const char *prefix);

int request_ctl_ioctl_opened(int *fd, unsigned int card_id, long request, void *data);
int request_ctl_ioctl(unsigned int card_id, long request, void *data);

static inline int lookup_and_allocate_name_by_sysname(char **name,
                                                      int (*func)(sd_device *, const char **),
                                                      const char *fmt, va_list ap)
{
    char *sysname;
    int err;

    err = allocate_string(&sysname, fmt, ap);
    if (err >= 0)
        err = lookup_and_allocate_string_by_sysname(name, sysname, func);
    free(sysname);
    return err;
}

static inline int lookup_and_allocate_sysname_by_sysname(char **sysname, const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = lookup_and_allocate_name_by_sysname(sysname, sd_device_get_sysname, fmt, ap);
    va_end(ap);

    return err;
}

static inline int lookup_and_allocate_devname_by_sysname(char **devname, const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = lookup_and_allocate_name_by_sysname(devname, sd_device_get_devname, fmt, ap);
    va_end(ap);

    return err;
}

static inline int lookup_and_allocate_card_sysname(char **sysname, unsigned int card_id)
{
    return lookup_and_allocate_sysname_by_sysname(sysname, CARD_SYSNAME_TEMPLATE, card_id);
}

static inline int lookup_and_allocate_control_sysname(char **sysname, unsigned int card_id)
{
    return lookup_and_allocate_sysname_by_sysname(sysname, CONTROL_SYSNAME_TEMPLATE, card_id);
}

static inline int lookup_and_allocate_control_devname(char **devname, unsigned int card_id)
{
    return lookup_and_allocate_devname_by_sysname(devname, CONTROL_SYSNAME_TEMPLATE, card_id);
}

static inline int lookup_and_allocate_hwdep_sysname(char **sysname, unsigned int card_id,
                                                    unsigned int device_id)
{
    return lookup_and_allocate_sysname_by_sysname(sysname, HWDEP_SYSNAME_TEMPLATE, card_id,
                                                  device_id);
}

static inline int lookup_and_allocate_hwdep_devname(char **devname, unsigned int card_id,
                                                    unsigned int device_id)
{
    return lookup_and_allocate_devname_by_sysname(devname, HWDEP_SYSNAME_TEMPLATE, card_id,
                                                  device_id);
}

static inline int lookup_and_allocate_rawmidi_sysname(char **sysname, unsigned int card_id,
                                                      unsigned int device_id)
{
    return lookup_and_allocate_sysname_by_sysname(sysname, RAWMIDI_SYSNAME_TEMPLATE, card_id,
                                                  device_id);
}

static inline int lookup_and_allocate_rawmidi_devname(char **devname, unsigned int card_id,
                                                      unsigned int device_id)
{
    return lookup_and_allocate_devname_by_sysname(devname, RAWMIDI_SYSNAME_TEMPLATE, card_id,
                                                  device_id);
}

static inline int lookup_and_allocate_timer_sysname(char **sysname)
{
    return lookup_and_allocate_sysname_by_sysname(sysname, TIMER_SYSNAME);
}

static inline int lookup_and_allocate_timer_devname(char **devname)
{
    return lookup_and_allocate_devname_by_sysname(devname, TIMER_SYSNAME);
}

static inline int lookup_and_allocate_seq_sysname(char **sysname)
{
    return lookup_and_allocate_sysname_by_sysname(sysname, SEQ_SYSNAME);
}

static inline int lookup_and_allocate_seq_devname(char **devname)
{
    return lookup_and_allocate_devname_by_sysname(devname, SEQ_SYSNAME);
}

static inline int generate_sysnum_list(unsigned int **entries, unsigned long *entry_count,
                                       const char *fmt, ...)
{
    char *prefix;
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = allocate_string(&prefix, fmt, ap);
    va_end(ap);

    if (err >= 0)
        err = generate_sysnum_list_by_sysname_prefix(entries, entry_count, prefix);
    free(prefix);

    return err;
}

static inline int generate_card_sysnum_list(unsigned int **entries, unsigned long *entry_count)
{
    return generate_sysnum_list(entries, entry_count, CARD_SYSNAME_PREFIX);
}

static inline int generate_hwdep_sysnum_list(unsigned int **entries, unsigned long *entry_count,
                                             unsigned int card_id)
{
    return generate_sysnum_list(entries, entry_count, HWDEP_SYSNAME_PREFIX_TEMPLATE, card_id);
}

static inline int generate_rawmidi_sysnum_list(unsigned int **entries, unsigned long *entry_count,
                                               unsigned int card_id)
{
    return generate_sysnum_list(entries, entry_count, RAWMIDI_SYSNAME_PREFIX_TEMPLATE, card_id);
}

#endif
