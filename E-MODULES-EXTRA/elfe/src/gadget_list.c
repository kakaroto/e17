#include <e.h>
#include <Elementary.h>

#include "elfe_config.h"
#include "utils.h"

static void
_list_longpress(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *ic;
   E_Gadcon_Client_Class *gcc = NULL;
   const char *name;
   Elm_List_Item *it = event_info;

   printf("List longpressed\n");

   name = elm_list_item_data_get(it);
  
   evas_object_smart_callback_call(data,
				   "list,longpressed", name);
}

Evas_Object *
elfe_gadget_list_add(Evas_Object *obj)
{
   Evas_Object *list;
   E_Gadcon_Client_Class *gcc = NULL;
   Eina_List *l;
   Evas_Object *icon = NULL, *end;
   const char *lbl = NULL;

   list = elm_list_add(obj);
   evas_object_smart_callback_add(list, "longpressed", _list_longpress, list);
   EINA_LIST_FOREACH(e_gadcon_provider_list(), l, gcc)
     {
	if (gcc->func.label) lbl = gcc->func.label(gcc);
	if (!lbl) lbl = gcc->name;
	if (gcc->func.icon) icon = gcc->func.icon(gcc, evas_object_evas_get(obj));
	evas_object_size_hint_min_set(icon, 48, 48);
	elm_list_item_append(list, lbl, icon, NULL, NULL, gcc->name);
     }
   return list;
}
