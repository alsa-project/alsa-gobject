// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_PRIVATES__H__
#define __ALSA_GOBJECT_ALSASEQ_PRIVATES__H__

#include <string.h>

#include <glib.h>
#include <glib-object.h>

#include "system-info.h"

#include <sound/asequencer.h>

G_BEGIN_DECLS

GQuark alsaseq_error_quark(void);

#define generate_error(err, errno)                      \
    g_set_error(err, alsaseq_error_quark(), errno,      \
                __FILE__ ":%d: %s", __LINE__, strerror(errno))

void seq_system_info_refer_private(ALSASeqSystemInfo *self,
                                   struct snd_seq_system_info **info);

G_END_DECLS

#endif
