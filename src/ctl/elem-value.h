// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_VALUE_H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_VALUE_H__

#include <alsactl.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_VALUE     (alsactl_elem_value_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSACtlElemValue, alsactl_elem_value, ALSACTL, ELEM_VALUE, GObject);

struct _ALSACtlElemValueClass {
    GObjectClass parent_class;
};

ALSACtlElemValue *alsactl_elem_value_new();

void alsactl_elem_value_set_bool(ALSACtlElemValue *self,
                                 const gboolean *values, gsize value_count);
void alsactl_elem_value_get_bool(ALSACtlElemValue *self,
                                 const gboolean **values, gsize *value_count);

void alsactl_elem_value_set_int(ALSACtlElemValue *self, const gint32 *values,
                                gsize value_count);
void alsactl_elem_value_get_int(ALSACtlElemValue *self, const gint32 **values,
                                gsize *value_count);

void alsactl_elem_value_set_enum(ALSACtlElemValue *self,
                                 const guint32 *values, gsize value_count);
void alsactl_elem_value_get_enum(ALSACtlElemValue *self,
                                 const guint32 **values, gsize *value_count);

void alsactl_elem_value_set_bytes(ALSACtlElemValue *self,
                                  const guint8 *values, gsize value_count);
void alsactl_elem_value_get_bytes(ALSACtlElemValue *self,
                                  const guint8 **values, gsize *value_count);

void alsactl_elem_value_set_iec60958_channel_status(ALSACtlElemValue *self,
                                        const guint8 *status, gsize length);
void alsactl_elem_value_get_iec60958_channel_status(ALSACtlElemValue *self,
                                        const guint8 **status, gsize *length);

void alsactl_elem_value_set_iec60958_user_data(ALSACtlElemValue *self,
                                        const guint8 *data, gsize length);
void alsactl_elem_value_get_iec60958_user_data(ALSACtlElemValue *self,
                                        const guint8 **data, gsize *length);

void alsactl_elem_value_set_int64(ALSACtlElemValue *self, const gint64 *values,
                                  gsize value_count);
void alsactl_elem_value_get_int64(ALSACtlElemValue *self,
                                  const gint64 **values, gsize *value_count);

gboolean alsactl_elem_value_equal(const ALSACtlElemValue *self,
                                  const ALSACtlElemValue *target);

G_END_DECLS

#endif
