// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_ID__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_ID__H__

#include <glib.h>
#include <glib-object.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_ID    (alsactl_elem_id_get_type())

typedef struct snd_ctl_elem_id ALSACtlElemId;

GType alsactl_elem_id_get_type() G_GNUC_CONST;

G_END_DECLS

#endif
