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
emenu_item *
emenu_item_new(void)
{
   emenu_item *result;

   result = malloc(sizeof(emenu_item));
   memset(result, 0, sizeof(emenu_item));

   result->type = E_MENU_EXECUTABLE;

   return result;
}

/* used by ewd, it wants a void pointer blah blah blah */
void
_emenu_item_free(void *data)
{
   emenu_item_free((emenu_item *) data);
}

/* destroy an allocated memory if there was any.  free the children list */
void
emenu_item_free(emenu_item * m)
{
   Evas_List *l;

   if (!m)
      return;

   IF_FREE(m->exec);
   IF_FREE(m->icon);
   IF_FREE(m->text);

   for (l = m->children; l; l = l->next)
      emenu_item_free((emenu_item *) l->data);

   free(m);
}
