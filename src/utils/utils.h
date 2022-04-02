// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_UTILS_H__
#define __ALSA_GOBJECT_UTILS_H__

#include <stdlib.h>
#include <stdarg.h>
#include <libudev.h>

#define CARD_SYSNAME_TEMPLATE           "card%u"
#define CONTROL_SYSNAME_TEMPLATE        "controlC%u"
#define RAWMIDI_SYSNAME_TEMPLATE        "midiC%uD%u"
#define HWDEP_SYSNAME_TEMPLATE          "hwC%uD%u"
#define TIMER_SYSNAME                   "timer"
#define SEQ_SYSNAME                     "seq"

long long_from_string(const char *literal, long *number);

int allocate_string(char **dst, const char *template, va_list ap);

int lookup_and_allocate_string_by_sysname(char **name, const char *sysname,
                                          const char *(*func)(struct udev_device *));

static inline int lookup_and_allocate_name_by_sysname(char **name,
                                                      const char *(*func)(struct udev_device *),
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
    err = lookup_and_allocate_name_by_sysname(sysname, udev_device_get_sysname, fmt, ap);
    va_end(ap);

    return err;
}

static inline int lookup_and_allocate_devname_by_sysname(char **devname, const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = lookup_and_allocate_name_by_sysname(devname, udev_device_get_devnode, fmt, ap);
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

#endif
