/**************************************************************************
 * Name: eaction_item.c  
 * Project: ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: Implementation for editing actions in e17
 *************************************************************************/
#include"eaction_item.h"

eaction_item *
eaction_item_new(void)
{
   eaction_item *result;

   result = (eaction_item *) malloc(sizeof(eaction_item));
   memset(result, 0, sizeof(eaction_item));

   return result;
}

void
eaction_item_free(eaction_item * e)
{
   if (!e)
      return;

   IF_FREE(e->name);
   IF_FREE(e->action);
   IF_FREE(e->params);
   IF_FREE(e->key);

   free(e);
}

void
_eaction_item_free(void *data)
{
   eaction_item_free((eaction_item *) data);
}

void
action_container_init(void)
{
   action_container.keys = NULL;
   action_container.focus = NULL;
}

void
action_container_free(void)
{
   Evas_List *l;

   for (l = action_container.keys; l; l = l->next)
   {
      _eaction_item_free(l->data);
   }
   evas_list_free(action_container.keys);
   for (l = action_container.focus; l; l = l->next)
   {
      _eaction_item_free(l->data);
   }
   evas_list_free(action_container.focus);
}

void
action_container_keys_reinit(void)
{
   Evas_List *l;

   for (l = action_container.keys; l; l = l->next)
   {
      _eaction_item_free(l->data);
   }
   evas_list_free(action_container.keys);
   action_container.keys = NULL;
}

void
action_container_focus_reinit(void)
{
   Evas_List *l;

   for (l = action_container.focus; l; l = l->next)
   {
      _eaction_item_free(l->data);
   }
   evas_list_free(action_container.focus);
   action_container.focus = NULL;
}
