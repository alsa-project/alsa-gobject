// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privates.h"

#include <utils.h>

/**
 * SECTION: query
 * @Title: Global functions in ALSACtl
 * @Short_description: Global functions available without holding any file
 *                     descriptor
 */

#define generate_file_error(exception, errno, msg) \
        g_set_error_literal(exception, G_FILE_ERROR, g_file_error_from_errno(errno), msg)

/**
 * alsactl_get_card_id_list:
 * @entries: (array length=entry_count)(out): The list of numerical ID for sound
 *           cards.
 * @entry_count: The number of entries.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Get the list of numerical ID for available sound cards.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_card_id_list(guint **entries, gsize *entry_count,
                              GError **error)
{
    int err;

    g_return_if_fail(entries != NULL);
    g_return_if_fail(entry_count != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = generate_card_sysnum_list(entries, entry_count);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate list of card sysnum");
}

/**
 * alsactl_get_card_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @sysname: (out): The string for sysname of the sound card.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate sysname for the sound card and return it when it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_card_sysname(guint card_id, char **sysname, GError **error)
{
    int err;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_card_sysname(sysname, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate card sysname");
}

/**
 * alsactl_get_control_sysname:
 * @card_id: The numeridcal ID of sound card.
 * @sysname: (out): The string for sysname of control device for the sound card.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate sysname of control device for the sound card and return it when
 * it exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_control_sysname(guint card_id, char **sysname, GError **error)
{
    int err;

    g_return_if_fail(sysname != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_control_sysname(sysname, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate control sysname");
}

/**
 * alsactl_get_control_devnode:
 * @card_id: The numerical ID of sound card.
 * @devnode: (out): The string for devnode of control device for the sound card.
 * @error: A #GError. Error is generated with domain of #g_file_error_quark().
 *
 * Allocate string of devnode for control device of the sound card and return it
 * if exists.
 *
 * Nodes under sound subsystem in sysfs are used to gather the information.
 */
void alsactl_get_control_devnode(guint card_id, char **devnode, GError **error)
{
    int err;

    g_return_if_fail(devnode != NULL);
    g_return_if_fail(error == NULL || *error == NULL);

    err = lookup_and_allocate_control_devname(devnode, card_id);
    if (err < 0)
        generate_file_error(error, -err, "Fail to generate control devname");
}
