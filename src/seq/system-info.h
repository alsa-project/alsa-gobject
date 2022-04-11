// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_SYSTEM_INFO_H__
#define __ALSA_GOBJECT_ALSASEQ_SYSTEM_INFO_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_SYSTEM_INFO     (alsaseq_system_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqSystemInfo, alsaseq_system_info, ALSASEQ, SYSTEM_INFO, GObject);

struct _ALSASeqSystemInfoClass {
    GObjectClass parent_class;
};

G_END_DECLS

#endif
