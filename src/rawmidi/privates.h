// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSARAWMIDI_PRIVATES__H__
#define __ALSA_GOBJECT_ALSARAWMIDI_PRIVATES__H__

#include <string.h>

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

GQuark alsarawmidi_error_quark(void);

#define generate_error(err, errno)                              \
    g_set_error(err, alsarawmidi_error_quark(), errno,          \
                __FILE__ ":%d: %s", __LINE__, strerror(errno))

G_END_DECLS

#endif
