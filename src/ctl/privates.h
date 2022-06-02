// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSACTL_PRIVATES_H__
#define __ALSA_GOBJECT_ALSACTL_PRIVATES_H__

#include "alsactl.h"

G_BEGIN_DECLS

void ctl_card_info_refer_private(ALSACtlCardInfo *self,
                                 struct snd_ctl_card_info **info);

void ctl_elem_info_refer_private(ALSACtlElemInfo *self,
                                 struct snd_ctl_elem_info **info);

void ctl_elem_value_refer_private(ALSACtlElemValue *self,
                                  struct snd_ctl_elem_value **value);

#define ELEM_ID_PROP_NAME       "elem-id"
#define ELEM_TYPE_PROP_NAME     "elem-type"
#define ACCESS_PROP_NAME        "access"
#define OWNER_PROP_NAME         "owner"

enum elem_info_common_prop_type {
    ELEM_INFO_COMMON_PROP_ELEM_ID = 1,
    ELEM_INFO_COMMON_PROP_ELEM_TYPE,
    ELEM_INFO_COMMON_PROP_ACCESS,
    ELEM_INFO_COMMON_PROP_OWNER,
    ELEM_INFO_COMMON_PROP_COUNT,
};

void elem_info_common_class_override_properties(GObjectClass *gobject_class);

void elem_info_common_set_property(struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                   const GValue *val, GParamSpec *spec);

void elem_info_common_get_property(const struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                   GValue *val, GParamSpec *spec);

#define VALUE_COUNT_PROP_NAME   "value-count"

enum elem_info_single_array_prop_type {
    ELEM_INFO_SINGLE_ARRAY_PROP_VALUE_COUNT = ELEM_INFO_COMMON_PROP_COUNT,
    ELEM_INFO_SINGLE_ARRAY_PROP_COUNT,
};

void elem_info_single_array_class_override_properties(GObjectClass *gobject_class);

void elem_info_single_array_set_property(struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                      const GValue *val, GParamSpec *spec);

void elem_info_single_array_get_property(const struct snd_ctl_elem_info *data, GObject *obj, guint id,
                                      GValue *val, GParamSpec *spec);

G_END_DECLS

#endif
