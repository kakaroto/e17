/**************************************************************************
 * Name: emenu_item.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: data representation for items in an emenu, create and free
 *************************************************************************/
#include"emenu_item.h"

/* Creates an emenu_item: sets everything to null or 0, and initializes the
 * children list 
 */
emenu_item*
emenu_item_new(void)	
{
	emenu_item *result;

	result = malloc(sizeof(emenu_item));
	result->text = result->icon = result->exec = NULL;
	
	result->children = ewd_list_new();
	ewd_list_set_free_cb(result->children, _emenu_item_free);
	
	result->type = E_MENU_EXECUTABLE;

	return result;
}
/* used by ewd, it wants a void pointer blah blah blah */
void
_emenu_item_free(void *data)	{
	emenu_item_free((emenu_item*)data);
}

/* destroy an allocated memory if there was any.  free the children list */
void
emenu_item_free(emenu_item *m)	{
	if(!m) return;

	IF_FREE(m->exec);
	IF_FREE(m->icon);
	IF_FREE(m->text);
	/* we get IF_FREE from Ewd Macros =) */
	
	ewd_list_destroy(m->children);
	free(m);
}

/* Lameness ! */
void
emenu_item_print(emenu_item *e)	{
	emenu_item *_e;

	switch(e->type)	{
		case E_MENU_SCRIPT:
			printf("SCRIPT:\t");
			printf("%s is the description\n\t%s is the icon\n\t%s is the
					exec\n\n", e->text, e->icon, e->exec);
			break;
		case E_MENU_SUBMENU:
			printf("SUBMENU:\t");
			printf("%s is the description\n\t%s is the icon\n\t%s is the
					exec\n\n", e->text, e->icon, e->exec);
			ewd_list_goto_first(e->children);
			while((_e = (emenu_item*)ewd_list_next(e->children)))
				emenu_item_print(_e);
			break;
		case E_MENU_SEPARATOR:
			printf("SEPARATOR:");
			break;
		default:
			printf("EXECUTABLE:");
			printf("%s is the description\n\t%s is the icon\n\t%s is the
					exec\n\n", e->text, e->icon, e->exec);
			break;
	}
}	
