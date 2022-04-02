// SPDX-License-Identifier: LGPL-3.0-or-later
#include "utils.h"

#include <errno.h>
#include <stdio.h>

long long_from_string(const char *literal, long *number)
{
    long val;
    char *endptr;

    errno = 0;
    val = strtol(literal, &endptr, 10);
    if (errno > 0)
        return -errno;
    if (!endptr || endptr == literal || *endptr != 0)
        return -EINVAL;
    *number = val;
    return 0;
}

int allocate_string(char **dst, const char *template, va_list ap)
{
    char *label;
    int size;
    va_list aq;

    va_copy(aq, ap);
    size = vsnprintf(NULL, 0, template, aq);
    va_end(aq);

    if (size < 0)
        return -EINVAL;
    size += 1;

    label = malloc(size);
    if (label == NULL)
        return -ENOMEM;

    va_copy(aq, ap);
    vsnprintf(label, size, template, aq);
    va_end(aq);

    *dst = label;

    return 0;
}
