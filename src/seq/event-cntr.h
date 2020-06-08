// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR__H__
#define __ALSA_GOBJECT_ALSASEQ_EVENT_CNTR__H__

#include <glib.h>
#include <glib-object.h>

#include <seq/alsaseq-enums.h>

G_BEGIN_DECLS

#define ALSASEQ_TYPE_EVENT_CNTR     (alsaseq_event_cntr_get_type())

#define ALSASEQ_EVENT_CNTR(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                      \
                                ALSASEQ_TYPE_EVENT_CNTR,    \
                                ALSASeqEventCntr))
#define ALSASEQ_IS_EVENT_CNTR(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                      \
                                ALSASEQ_TYPE_EVENT_CNTR))

#define ALSASEQ_EVENT_CNTR_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                       \
                             ALSASEQ_TYPE_EVENT_CNTR,       \
                             ALSASeqEventCntrClass))
#define ALSASEQ_IS_EVENT_CNTR_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                       \
                             ALSASEQ_TYPE_EVENT_CNTR))
#define ALSASEQ_EVENT_CNTR_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                       \
                               ALSASEQ_TYPE_EVENT_CNTR,     \
                               ALSASeqEventCntrClass))

typedef struct _ALSASeqEventCntr            ALSASeqEventCntr;
typedef struct _ALSASeqEventCntrClass       ALSASeqEventCntrClass;
typedef struct _ALSASeqEventCntrPrivate     ALSASeqEventCntrPrivate;

struct _ALSASeqEventCntr {
    GObject parent_instance;

    ALSASeqEventCntrPrivate *priv;
};

struct _ALSASeqEventCntrClass {
    GObjectClass parent_class;
};

GType alsaseq_event_cntr_get_type() G_GNUC_CONST;

ALSASeqEventCntr *alsaseq_event_cntr_new(guint count, GError **error);

void alsaseq_event_cntr_count_events(ALSASeqEventCntr *self, gsize *count);

void alsaseq_event_cntr_calculate_pool_consumption(ALSASeqEventCntr *self,
				gsize count, gsize *cells, GError **error);

void alsaseq_event_cntr_get_event_type(ALSASeqEventCntr *self, gsize index,
                                    ALSASeqEventType *ev_type, GError **error);
void alsaseq_event_cntr_set_event_type(ALSASeqEventCntr *self,
                                         gsize index, ALSASeqEventType ev_type,
                                         GError **error);

void alsaseq_event_cntr_get_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimestampMode *mode,
                                          GError **error);
void alsaseq_event_cntr_set_tstamp_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventTimestampMode mode,
                                          GError **error);

void alsaseq_event_cntr_get_time_mode(ALSASeqEventCntr *self, gsize index,
                                        ALSASeqEventTimeMode *mode,
                                        GError **error);
void alsaseq_event_cntr_set_time_mode(ALSASeqEventCntr *self, gsize index,
                                        ALSASeqEventTimeMode mode,
                                        GError **error);

void alsaseq_event_cntr_get_length_mode(ALSASeqEventCntr *self, gsize index,
                                          ALSASeqEventLengthMode *mode,
                                          GError **error);

void alsaseq_event_cntr_get_priority_mode(
                                ALSASeqEventCntr *self, gsize index,
                                ALSASeqEventPriorityMode *mode, GError **error);
void alsaseq_event_cntr_set_priority_mode(
                                ALSASeqEventCntr *self, gsize index,
                                ALSASeqEventPriorityMode mode, GError **error);

void alsaseq_event_cntr_get_tag(ALSASeqEventCntr *self, gsize index,
                                  gint8 *tag, GError **error);
void alsaseq_event_cntr_set_tag(ALSASeqEventCntr *self, gsize index,
                                  gint8 tag, GError **error);

G_END_DECLS

#endif