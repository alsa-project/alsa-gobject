// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_PORT_INFO_H__
#define __ALSA_GOBJECT_ALSASEQ_PORT_INFO_H__

#include <alsaseq.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_PORT_INFO      (alsaseq_port_info_get_type())

G_DECLARE_DERIVABLE_TYPE(ALSASeqPortInfo, alsaseq_port_info, ALSASEQ, PORT_INFO, GObject);

struct _ALSASeqPortInfoClass {
    GObjectClass parent_class;
};

ALSASeqPortInfo *alsaseq_port_info_new();

#endif
