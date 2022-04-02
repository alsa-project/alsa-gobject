// SPDX-License-Identifier: LGPL-3.0-or-later
#include "utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

int request_ctl_ioctl_opened(int *fd, unsigned int card_id, long request, void *data)
{
    char *devname;
    int err;

    if (fd == NULL)
        return -EINVAL;

    err = lookup_and_allocate_control_devname(&devname, card_id);
    if (err < 0)
        return err;

    *fd = open(devname, O_RDONLY | O_NONBLOCK);
    free(devname);
    if (*fd < 0)
        return -errno;

    if (ioctl(*fd, request, data) < 0)
        return -errno;

    return 0;
}

int request_ctl_ioctl(unsigned int card_id, long request, void *data)
{
    int fd;
    int err;

    err = request_ctl_ioctl_opened(&fd, card_id, request, data);
    close(fd);

    return err;
}
