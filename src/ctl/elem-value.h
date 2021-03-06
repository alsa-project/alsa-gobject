// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_ELEM_VALUE__H__
#define __ALSA_GOBJECT_ALSACTL_ELEM_VALUE__H__

#include <glib.h>
#include <glib-object.h>

#include <ctl/elem-id.h>

G_BEGIN_DECLS

#define ALSACTL_TYPE_ELEM_VALUE     (alsactl_elem_value_get_type())

#define ALSACTL_ELEM_VALUE(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSACTL_TYPE_ELEM_VALUE,    \
                                ALSACtlElemValue))
#define ALSACTL_IS_ELEM_VALUE(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSACTL_TYPE_ELEM_VALUE))

#define ALSACTL_ELEM_VALUE_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSACTL_TYPE_ELEM_VALUE,       \
                             ALSACtlElemValueClass))
#define ALSACTL_IS_ELEM_VALUE_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSACTL_TYPE_ELEM_VALUE))
#define ALSACTL_ELEM_VALUE_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSACTL_TYPE_ELEM_VALUE,     \
                               ALSACtlElemValueClass))

typedef struct _ALSACtlElemValue         ALSACtlElemValue;
typedef struct _ALSACtlElemValueClass    ALSACtlElemValueClass;
typedef struct _ALSACtlElemValuePrivate  ALSACtlElemValuePrivate;

struct _ALSACtlElemValue {
    GObject parent_instance;

    ALSACtlElemValuePrivate *priv;
};

struct _ALSACtlElemValueClass {
    GObjectClass parent_class;
};

GType alsactl_elem_value_get_type() G_GNUC_CONST;

ALSACtlElemValue *alsactl_elem_value_new();

void alsactl_elem_value_set_bool(ALSACtlElemValue *self,
                                 const gboolean *values, gsize value_count);
void alsactl_elem_value_get_bool(ALSACtlElemValue *self,
                                 gboolean *const *values, gsize *value_count);

void alsactl_elem_value_set_int(ALSACtlElemValue *self, const gint32 *values,
                                gsize value_count);
void alsactl_elem_value_get_int(ALSACtlElemValue *self, gint32 *const *values,
                                gsize *value_count);

void alsactl_elem_value_set_enum(ALSACtlElemValue *self,
                                 const guint32 *values, gsize value_count);
void alsactl_elem_value_get_enum(ALSACtlElemValue *self,
                                 guint32 *const *values, gsize *value_count);

void alsactl_elem_value_set_bytes(ALSACtlElemValue *self,
                                  const guint8 *values, gsize value_count);
void alsactl_elem_value_get_bytes(ALSACtlElemValue *self,
                                  guint8 *const *values, gsize *value_count);

void alsactl_elem_value_set_iec60958_channel_status(ALSACtlElemValue *self,
                                        const guint8 *status, gsize length);
void alsactl_elem_value_get_iec60958_channel_status(ALSACtlElemValue *self,
                                        guint8 *const *status, gsize *length);

void alsactl_elem_value_set_iec60958_user_data(ALSACtlElemValue *self,
                                        const guint8 *data, gsize length);
void alsactl_elem_value_get_iec60958_user_data(ALSACtlElemValue *self,
                                        guint8 *const *data, gsize *length);

void alsactl_elem_value_set_int64(ALSACtlElemValue *self, const gint64 *values,
                                  gsize value_count);
void alsactl_elem_value_get_int64(ALSACtlElemValue *self,
                                  gint64 *const *values, gsize *value_count);

gboolean alsactl_elem_value_equal(const ALSACtlElemValue *self,
                                  const ALSACtlElemValue *target);

G_END_DECLS

#endif
