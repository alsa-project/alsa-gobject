// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_INFO_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_INFO_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_INFO      (alsactl_elem_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemInfo, alsactl_elem_info, ALSACTL, ELEM_INFO, GObject);

struct _ALSACtlElemInfoClass {
    GObjectClass parent_class;
};

ALSACtlElemInfo *alsactl_elem_info_new(ALSACtlElemType elem_type, GError **error);

void alsactl_elem_info_get_int_data(ALSACtlElemInfo *self,
                                    const gint32 *data[3], GError **error);
void alsactl_elem_info_set_int_data(ALSACtlElemInfo *self,
                                    const gint32 data[3], GError **error);

void alsactl_elem_info_get_int64_data(ALSACtlElemInfo *self,
                                      const gint64 *data[3], GError **error);
void alsactl_elem_info_set_int64_data(ALSACtlElemInfo *self,
                                      const gint64 data[3], GError **error);

void alsactl_elem_info_get_enum_data(ALSACtlElemInfo *self,
                                     const gchar ***data, GError **error);
void alsactl_elem_info_set_enum_data(ALSACtlElemInfo *self,
                                     const gchar **data, GError **error);

G_END_DECLS

#endif
