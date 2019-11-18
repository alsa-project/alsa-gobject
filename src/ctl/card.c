// SPDX-License-Identifier: LGPL-3.0-or-later
#include "card.h"

G_DEFINE_TYPE(ALSACtlCard, alsactl_card, G_TYPE_OBJECT)

static void alsactl_card_class_init(ALSACtlCardClass *klass)
{
    return;
}

static void alsactl_card_init(ALSACtlCard *self)
{
    return;
}

/**
 * alsactl_card_new:
 *
 * Allocate and return an instance of ALSACtlCard class.
 */
ALSACtlCard *alsactl_card_new()
{
    return g_object_new(ALSACTL_TYPE_CARD, NULL);
}
