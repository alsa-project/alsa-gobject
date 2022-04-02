// SPDX-License-Identifier: LGPL-3.0-or-later
#include <stdlib.h>
#include <errno.h>

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
