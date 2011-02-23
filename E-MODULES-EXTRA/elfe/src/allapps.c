#include <Elementary.h>

#include "utils.h"

typedef struct _Elfe_Grid_Item Elfe_Grid_Item;

struct _Elfe_Grid_Item
{
    const char *icon_path;
    Efreet_Menu *menu;
};

static Elm_Gengrid_Item_Class app_itc;
static Efreet_Menu *menus;
static Eina_List *grid_items = NULL;

static void _populate_items_cb(void *data, Evas_Object *obj, void *event_info);

static char *
_label_get(void *data, Evas_Object *obj, const char *part)
{

   Elfe_Grid_Item *gitem = data;

   if (gitem->menu->name)
     return strdup(gitem->menu->name);
   else
     return NULL;

}

static Evas_Object *
_icon_get(void *data, Evas_Object *obj, const char *part)
{
   const char *path;
   Evas_Object *ic = NULL;
   Elfe_Grid_Item *gitem = data;

   if (!gitem)
     return NULL;

   /* Fixme use elfe_util_fdo_icon_add */
   if (!strcmp(part, "elm.swallow.icon"))
     {
	ic = elm_icon_add(obj);
	elm_icon_file_set(ic, gitem->icon_path, NULL);
     }

   return ic;
}



static void
_gl_longpress(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *popup;
   Evas_Object *ic;
   Elfe_Grid_Item *gitem;
   Elm_Ctxpopup_Item *it;

   gitem = elm_gengrid_item_data_get(event_info);

   if (gitem->menu->type != EFREET_MENU_ENTRY_DESKTOP)
     return;

   evas_object_smart_callback_call(data,
				   "entry,longpressed", gitem->menu);
}

static void
_item_selected(void *data, Evas_Object *obj, void *event_info)
{
   Elfe_Grid_Item *gitem = data;
   printf("Selected\n");
   evas_object_smart_callback_call(obj, "item,selected", gitem->menu);
}


static void
_add_items(Evas_Object *parent, Efreet_Menu *entry)
{
   Efreet_Menu *menu;
   Efreet_Menu *it;
   Eina_List *l;
   Elfe_Grid_Item *gitem;

   if (!entry)
     menu = efreet_menu_get();
   else
     menu = entry;

   EINA_LIST_FOREACH(menu->entries, l, it)
     {
	switch (it->type)
	  {
	   case EFREET_MENU_ENTRY_MENU :
	      _add_items(parent, it);
	      break;
	   case EFREET_MENU_ENTRY_DESKTOP :
	      gitem = calloc(1, sizeof(Elfe_Grid_Item));
	      gitem->icon_path = elfe_utils_fdo_icon_path_get(it, 72);
	      gitem->menu = it;
	      grid_items = eina_list_append(grid_items, gitem);
	      elm_gengrid_item_append(parent, &app_itc, gitem, _item_selected, gitem);
	   default:
	      break;
	  }
     }
}

static Eina_Bool
_desktop_cache_update(void *data, int type, void *event)
{

   return ECORE_CALLBACK_PASS_ON;
}

Evas_Object *
elfe_allapps_add(Evas_Object *parent)
{
   Evas_Object *list;
   char *path;
   Evas_Object *ic;
   Eina_List *l;


   /* FIXME use dynamic item size */
   list = elm_gengrid_add(parent);
   elm_gengrid_item_size_set(list, 128, 128);
   elm_gengrid_multi_select_set(list, EINA_FALSE);
   elm_gengrid_bounce_set(list, EINA_FALSE, EINA_TRUE);

   evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);

   evas_object_smart_callback_add(list, "longpressed", _gl_longpress, list);
   evas_object_show(list);

   app_itc.item_style     = "default";
   app_itc.func.label_get = _label_get;
   app_itc.func.icon_get  = _icon_get;
   app_itc.func.state_get = NULL;
   app_itc.func.del       = NULL;

   _add_items(list, NULL);
   evas_object_show(list);

   ecore_event_handler_add(EFREET_EVENT_DESKTOP_CACHE_UPDATE,
                           _desktop_cache_update,
                           NULL);

   return list;
}

