// SPDX-License-Identifier: LGPL-3.0-or-later
#include "utils.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define SOUND_SUBSYSTEM     "sound"

int lookup_and_allocate_string_by_sysname(char **name, const char *sysname,
                                          int (*func)(sd_device *, const char **))
{
    sd_device *device;
    const char *n;
    int err;

    if (name == NULL || sysname == NULL || func == NULL)
        return -EINVAL;

    err = sd_device_new_from_subsystem_sysname(&device, SOUND_SUBSYSTEM, sysname);
    if (err < 0)
        return err;

    err = func(device, &n);
    if (err < 0)
        goto end;

    *name = strdup(n);
    if (*name == NULL)
        err = -ENOMEM;
end:
    sd_device_unref(device);
    return err;
}

static int detect_device(sd_device *device, const char *prefix)
{
    const char *sysname;
    int err;

    err = sd_device_get_sysname(device, &sysname);
    if (err < 0)
        return err;

    if (strstr(sysname, prefix) != sysname)
        return -ENODEV;

    return 0;
}

static int compare_u32(const void *l, const void *r)
{
    const unsigned int *x = l;
    const unsigned int *y = r;

    return *x > *y;
}

int generate_sysnum_list_by_sysname_prefix(unsigned int **entries, unsigned long *entry_count,
                                           const char *prefix)
{
    sd_device_enumerator *enumerator;
    unsigned int count;
    sd_device *device;
    unsigned int index;
    int err;

    err = sd_device_enumerator_new(&enumerator);
    if (err < 0)
        return err;

    err = sd_device_enumerator_add_match_subsystem(enumerator, SOUND_SUBSYSTEM, 1);
    if (err < 0)
        return err;

    count = 0;
    device = sd_device_enumerator_get_device_first(enumerator);
    do {
        int err;

        err = detect_device(device, prefix);
        if (err >= 0)
            ++count;
    } while ((device = sd_device_enumerator_get_device_next(enumerator)));

    // Nothing available.
    if (count == 0)
        goto end;

    *entries = calloc(count, sizeof(**entries));
    if (*entries == NULL) {
        err = -ENOMEM;
        goto end;
    }

    index = 0;
    device = sd_device_enumerator_get_device_first(enumerator);
    do {
        const char *sysnum;
        int err;

        err = detect_device(device, prefix);
        if (err < 0)
            continue;

        err = sd_device_get_sysnum(device, &sysnum);
        if (err >= 0) {
            long val;

            if (!long_from_string(sysnum, &val)) {
                (*entries)[index] = (unsigned int)val;
                ++index;
            }
        }
    } while ((device = sd_device_enumerator_get_device_next(enumerator)));

    if (index != count) {
        err = -ENODATA;
        goto end;
    }

    *entry_count = count;

    qsort(*entries, count, sizeof(unsigned int), compare_u32);
end:
    sd_device_enumerator_unref(enumerator);
    return err;
}
