/**************************************************************************
 * Name: eaction_item.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Desctions: Structures for reading and writing users actions in e17, plus
 * implementation. 
 * Contains: 
 * - functions to modify the global action_container
 *- eaction_item modifier functions
 * September 21, 2002 - Migrated away from Ewd Lists in favor of evas lists
 *************************************************************************/
#ifndef __E_ACTINOS_DB__
#define __E_ACTINOS_DB__

#include<Edb.h>
#include<Evas.h>
#include"ebindings.h"

/* data representation of what e17 needs to know for an action */
struct _eaction_item
{
   char *name, *action, *params, *key;
   int button, event, modifiers;

};
typedef struct _eaction_item eaction_item;

/* create a new action every char* is NULL, and int vals are 0 */
eaction_item *eaction_item_new(void);

/* free the eaction_item passed to it */
void eaction_item_free(eaction_item *);

/* convenience container, keys are modifiable, others should not be, for the
 * sake of keeping the db consistent we keep track of all the info the db
 * contained at parse time so we can nuke it before we write */
struct
{
   Evas_List *keys;
   Evas_List *focus;
}
action_container;

/* wrapper function for destroying the data in the list nodes */
void _eaction_item_free(void *);

/* gest the two lists in action_container erady to be used */
void action_container_init(void);

/* destroys the two lists in action_container */
void action_container_free(void);

/* destroys the key list and reinitializes it */
void action_container_keys_reinit(void);

/* destroys the focus list and reinitializes it */
void action_container_focus_reinit(void);

#endif
