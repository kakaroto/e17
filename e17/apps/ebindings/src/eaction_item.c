/**************************************************************************
 * Name: eaction_item.c  
 * Project: ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: Implementation for editing actions in e17
 *************************************************************************/
#include"eaction_item.h"

eaction_item*
eaction_item_new(void)
{
	eaction_item *result;

	result = (eaction_item*) malloc(sizeof(eaction_item));

	result->name = result->action = result->params = result->key = NULL;
	result->button = result->event = result->modifiers = 0;

	return result;
}

void
eaction_item_free(eaction_item *e)
{
	if(!e) return;
	
	IF_FREE(e->name);
	IF_FREE(e->action);
	IF_FREE(e->params);
	IF_FREE(e->key);

	free(e);
}
void
eaction_item_print(eaction_item* e)
{
	if(!e) return;
	printf("Name: %s\nAction: %s\nParams: %s\nKey: %s\nButton: %d\n" 
		   "Event: %d\nModifiers: %d\n\n", e->name, e->action, e->params,
			e->key, e->button, e->event, e->modifiers);
}
void
_eaction_item_free(void *data)
{
	eaction_item_free((eaction_item*)data);
}

void
action_container_init(void)
{
	action_container.keys = ewd_list_new();
	action_container.focus = ewd_list_new();

	ewd_list_set_free_cb(action_container.keys, _eaction_item_free);
	ewd_list_set_free_cb(action_container.focus, _eaction_item_free);
}
void
action_container_free(void)
{
	ewd_list_destroy(action_container.keys);
	ewd_list_destroy(action_container.focus);
}
void
action_container_keys_reinit(void)
{
	ewd_list_destroy(action_container.keys);
	action_container.keys = ewd_list_new();
	ewd_list_set_free_cb(action_container.keys, _eaction_item_free);
}
void
action_container_focus_reinit(void)
{
	ewd_list_destroy(action_container.focus);
	action_container.focus = ewd_list_new();
	ewd_list_set_free_cb(action_container.focus, _eaction_item_free);
}
