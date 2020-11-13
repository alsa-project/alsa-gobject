// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_PRIVATES__H__
#define __ALSA_GOBJECT_ALSACTL_PRIVATES__H__

#include <string.h>

#include <glib.h>
#include <glib-object.h>

#include "card-info.h"
#include "elem-id.h"
#include "elem-info.h"
#include "elem-value.h"

#include <sound/asound.h>

G_BEGIN_DECLS

void ctl_card_info_refer_private(ALSACtlCardInfo *self,
                                 struct snd_ctl_card_info **info);

void ctl_elem_info_refer_private(ALSACtlElemInfo *self,
                                 struct snd_ctl_elem_info **info);

void ctl_elem_value_refer_private(ALSACtlElemValue *self,
                                  struct snd_ctl_elem_value **value);

G_END_DECLS

#endif
