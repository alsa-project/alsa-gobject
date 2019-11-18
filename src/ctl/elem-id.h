// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_ID__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_ID__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/alsactl-enums.h>

#include <sound/asound.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_ID    (alsactl_elem_id_get_type())

typedef struct snd_ctl_elem_id ALSACtlElemId;

GType alsactl_elem_id_get_type() G_GNUC_CONST;

ALSACtlElemId *alsactl_elem_id_new_by_numid(guint numid);
ALSACtlElemId *alsactl_elem_id_new_by_name(ALSACtlElemIfaceType iface,
                                           guint device_id, guint subdevice_id,
                                           const gchar *name, guint index);

void alsactl_elem_id_get_numid(const ALSACtlElemId *self, guint *numid);
void alsactl_elem_id_get_iface(const ALSACtlElemId *self,
                               ALSACtlElemIfaceType *iface);
void alsactl_elem_id_get_device_id(const ALSACtlElemId *self, guint *device_id);
void alsactl_elem_id_get_subdevice_id(const ALSACtlElemId *self,
                                      guint *subdevice_id);
void alsactl_elem_id_get_name(const ALSACtlElemId *self, const gchar **name);
void alsactl_elem_id_get_index(const ALSACtlElemId *self, guint *index);

G_END_DECLS

#endif
