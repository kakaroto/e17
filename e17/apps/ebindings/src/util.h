/**************************************************************************
 * Name: util.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: Utility functions.  Parsing of menu dbs to a and Evas_List
 *  of defined structure, emenu_item,  from emenu_item.h.  Parsing is also
 *  done of the user's actions db.  And an Evas_List with nodes of a defined
 *  type, eaction_item, from ekeybindings.h. Reading and Writing to the
 *  actions and menu dbs is done here
 *************************************************************************/

#ifndef __E_DB_PARSING_ROUTINES_
#define __E_DB_PARSING_ROUTINES_

#include <Evas.h>
#include <unistd.h>
#include "emenu_item.h"
#include "eaction_item.h"

/* 
 * Parsing user dbs populates action_container, Keybinds go specifically
 * into the keys list, while ANYTHING other then keybinds go into others,
 * this is the only sane way i can see managing all the actions without
 * attempting to keep track of the value in the db, so when written the db
 * is cleared, then rewritten.
 */
int parse_user_actions_db(void);

/* 
 * Write the changes back to the db, for consistency we clean out the old
 * one completely then write all the stuff back 
 */
int write_user_actions_db(void);

int write_user_menu_db_with_evas_list(Evas_List *);
Evas_List *read_user_menu_db_into_evas_list(Evas_List *);

/* wrapper to open and start recursive call */

#endif
