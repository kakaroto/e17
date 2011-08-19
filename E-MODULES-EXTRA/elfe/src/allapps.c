#include <e.h>
#include <Elementary.h>

#include "utils.h"
#include "elfe_config.h"
#include "desktop.h"
#include "gadget_list.h"

typedef struct _Elfe_Allapps Elfe_Allapps;
typedef struct _Elfe_Grid_Item Elfe_Grid_Item;

struct _Elfe_Allapps
{
   Evas_Object *box;
   Evas_Object *grid;
   Evas_Object *widgets_list;
   Evas_Object *config_list;
   Evas_Object *selector;
   Evas_Object *pager;
   Eina_List *buttons;
   Eina_List *grid_items;
};

struct _Elfe_Grid_Item
{
   const char *icon_path;
   Efreet_Menu *menu;
   Elfe_Allapps *allapps;
};

static Elm_Gengrid_Item_Class app_itc;


static char *
_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
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
   Evas_Object *ic = NULL;
   Elfe_Grid_Item *gitem = data;

   if (!gitem)
     return NULL;

   /* Fixme use elfe_util_fdo_icon_add */
   if (!strcmp(part, "elm.swallow.icon"))
     {
	ic = elm_icon_add(obj);
        if (gitem->icon_path && gitem->icon_path[0] == '/')
            elm_icon_file_set(ic, gitem->icon_path, NULL);
        else
            elm_icon_file_set(ic, elfe_home_cfg->theme, gitem->icon_path);
        evas_object_size_hint_min_set(ic, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
        evas_object_size_hint_max_set(ic, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
     }

   return ic;
}



static void
_gl_longpress(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Grid_Item *gitem;
   Elfe_Allapps *allapps = data;
   Elm_Gengrid_Item *it = event_info;

   gitem = elm_gengrid_item_data_get(event_info);

   if (gitem->menu->type != EFREET_MENU_ENTRY_DESKTOP)
     return;

   evas_object_smart_callback_call(allapps->box,
				   "entry,longpressed", gitem->menu);
   elm_gengrid_item_selected_set(it, EINA_FALSE);
}

static void
_widget_longpress(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   const char *name = event_info;
   Elfe_Allapps *allapps = data;
   Elm_Genlist_Item *it = event_info;

   printf("Send widget longpressed\n");
   evas_object_smart_callback_call(allapps->box,
				   "gadget,longpressed", (void*)name);
   elm_genlist_item_selected_set(it, EINA_FALSE);
}

static void
_item_selected(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Grid_Item *gitem = data;
   Elm_Gengrid_Item *it = event_info;

   evas_object_smart_callback_call(gitem->allapps->box, "item,selected", gitem->menu);
   elm_gengrid_item_selected_set(it, EINA_FALSE);
}


static void
_add_items(Elfe_Allapps *allapps, Efreet_Menu *entry)
{
   Efreet_Menu *menu;
   Efreet_Menu *it;
   Eina_List *l;
   Elfe_Grid_Item *gitem;

   if (!entry)
     menu = efreet_menu_get();
   else
     menu = entry;

   if (!menu) return ;

   if (!menu) return;

   EINA_LIST_FOREACH(menu->entries, l, it)
     {
	switch (it->type)
	  {
	   case EFREET_MENU_ENTRY_MENU :
	      _add_items(allapps, it);
	      break;
	   case EFREET_MENU_ENTRY_DESKTOP :
	      gitem = calloc(1, sizeof(Elfe_Grid_Item));
	      gitem->allapps = allapps;
	      gitem->icon_path = elfe_utils_fdo_icon_path_get(it, elfe_home_cfg->icon_size);
              if (!gitem->icon_path)
                  gitem->icon_path = eina_stringshare_add("icon/application-default");
	      gitem->menu = it;
	      allapps->grid_items = eina_list_append(allapps->grid_items, gitem);
	      elm_gengrid_item_append(allapps->grid, &app_itc, gitem, _item_selected, gitem);
	   default:
	      break;
	  }
     }
}

static void
_obj_resize_cb(void *data , Evas *e __UNUSED__ , Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Coord w, h;
   Elfe_Allapps *allapps = data;
   Evas_Coord ow;

   evas_object_geometry_get(allapps->box, NULL, NULL, &w, &h);

   ow = w / (elfe_home_cfg->icon_size + w / 10);

   printf("Nb elements : ow %d\n", ow);

   elm_gengrid_item_size_set(allapps->grid, w / ow, (w /ow) + 20);

   elm_gengrid_align_set(allapps->grid, 0.5, 0);

}

static void
_obj_del_cb(void *data , Evas *e __UNUSED__ , Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elfe_Allapps *allapps = data;
   Elfe_Grid_Item *it;

   EINA_LIST_FREE(allapps->grid_items, it)
     {
	if (!it) continue;
	if (it->icon_path)
	  eina_stringshare_del(it->icon_path);
	free(it);
     }
   allapps->grid_items = NULL;
}

static void
_apps_clicked_cb(void *data , Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elfe_Allapps *allapps = data;
   Evas_Object *bt;

   bt = eina_list_nth(allapps->buttons, 0);
   elm_object_disabled_set(bt, EINA_TRUE);

   bt = eina_list_nth(allapps->buttons, 1);
   elm_object_disabled_set(bt, EINA_FALSE);

   bt = eina_list_nth(allapps->buttons, 2);
   elm_object_disabled_set(bt, EINA_FALSE);

   elm_pager_content_promote(allapps->pager, allapps->grid);

}


static void
_widgets_clicked_cb(void *data , Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elfe_Allapps *allapps = data;
   Evas_Object *bt;

   bt = eina_list_nth(allapps->buttons, 0);
   elm_object_disabled_set(bt, EINA_FALSE);

   bt = eina_list_nth(allapps->buttons, 1);
   elm_object_disabled_set(bt, EINA_TRUE);

   bt = eina_list_nth(allapps->buttons, 2);
   elm_object_disabled_set(bt, EINA_FALSE);

   elm_pager_content_promote(allapps->pager, allapps->widgets_list);
}


static void
_config_clicked_cb(void *data , Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elfe_Allapps *allapps = data;
   Evas_Object *bt;

   bt = eina_list_nth(allapps->buttons, 0);
   elm_object_disabled_set(bt, EINA_FALSE);

   bt = eina_list_nth(allapps->buttons, 1);
   elm_object_disabled_set(bt, EINA_FALSE);

   bt = eina_list_nth(allapps->buttons, 2);
   elm_object_disabled_set(bt, EINA_TRUE);

   elm_pager_content_promote(allapps->pager, allapps->config_list);

}

Evas_Object *
elfe_allapps_add(Evas_Object *parent)
{
   Elfe_Allapps *allapps;
   Evas_Object *ic;
   Evas_Object *bt;
   Evas_Object *sep;

   /* Fixme create an edje layout instead of a box */

   allapps = calloc(1, sizeof(Elfe_Allapps));

   /* Create BOX */
   allapps->box = elm_box_add(parent);

   /* Create buttons selector */
   allapps->selector = elm_box_add(allapps->box);
   elm_box_horizontal_set(allapps->selector, EINA_TRUE);

   bt = elm_button_add(allapps->selector);
   elm_object_text_set(bt, "Apps");
   ic = elm_icon_add(bt);
   elm_icon_file_set(ic, elfe_home_cfg->theme, "icon/applications");
   evas_object_show(ic);
   elm_button_icon_set(bt, ic);
   evas_object_show(bt);
   elm_box_pack_end(allapps->selector, bt);
   evas_object_smart_callback_add(bt, "clicked", _apps_clicked_cb, allapps);
   allapps->buttons = eina_list_append(allapps->buttons, bt);

   sep = elm_separator_add(allapps->selector);
   evas_object_show(sep);
   elm_box_pack_end(allapps->selector, sep);

   bt = elm_button_add(allapps->selector);
   elm_object_text_set(bt, "Widgets");
   ic = elm_icon_add(bt);
   elm_icon_file_set(ic, elfe_home_cfg->theme, "icon/widgets");
   evas_object_show(ic);
   elm_button_icon_set(bt, ic);
   evas_object_show(bt);
   elm_box_pack_end(allapps->selector, bt);
   evas_object_smart_callback_add(bt, "clicked", _widgets_clicked_cb, allapps);
   allapps->buttons = eina_list_append(allapps->buttons, bt);

   sep = elm_separator_add(allapps->selector);
   evas_object_show(sep);
   elm_box_pack_end(allapps->selector, sep);

   bt = elm_button_add(allapps->selector);
   elm_object_text_set(bt, "Config");
   ic = elm_icon_add(bt);
   elm_icon_file_set(ic, elfe_home_cfg->theme, "icon/configuration");
   evas_object_show(ic);
   elm_button_icon_set(bt, ic);
   evas_object_show(bt);
   elm_box_pack_end(allapps->selector, bt);
   evas_object_smart_callback_add(bt, "clicked", _config_clicked_cb, allapps);
   allapps->buttons = eina_list_append(allapps->buttons, bt);

   evas_object_show(allapps->selector);
   elm_box_pack_end(allapps->box, allapps->selector);

   allapps->pager = elm_pager_add(allapps->box);
   evas_object_show(allapps->pager);
   elm_box_pack_end(allapps->box, allapps->pager);
   elm_object_style_set(allapps->pager, "slide_invisible");

   allapps->grid = elm_gengrid_add(parent);
   elm_gengrid_item_size_set(allapps->grid, 0, 0);

   evas_object_event_callback_add(allapps->box, EVAS_CALLBACK_RESIZE,
				  _obj_resize_cb, allapps);
   evas_object_event_callback_add(allapps->box, EVAS_CALLBACK_DEL,
				  _obj_del_cb, allapps);

   elm_gengrid_multi_select_set(allapps->grid, EINA_FALSE);
   elm_gengrid_bounce_set(allapps->grid, EINA_FALSE, EINA_TRUE);

   evas_object_size_hint_weight_set(allapps->grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(allapps->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(allapps->grid, "longpressed", _gl_longpress, allapps);
   evas_object_show(allapps->grid);

   evas_object_data_set(allapps->box, "elfe_allapps", allapps);

   app_itc.item_style     = "default";
   app_itc.func.label_get = _label_get;
   app_itc.func.icon_get  = _icon_get;
   app_itc.func.state_get = NULL;
   app_itc.func.del       = NULL;

   _add_items(allapps, NULL);
   evas_object_show(allapps->grid);


   allapps->widgets_list = elfe_gadget_list_add(allapps->pager);
   evas_object_smart_callback_add(allapps->widgets_list, "list,longpressed", _widget_longpress, allapps);
   evas_object_show(allapps->widgets_list);

   evas_object_show(allapps->box);

   evas_object_size_hint_weight_set(allapps->pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(allapps->pager, EVAS_HINT_FILL, EVAS_HINT_FILL);

   elm_pager_content_push(allapps->pager, allapps->widgets_list);
   elm_pager_content_push(allapps->pager, allapps->grid);

   return allapps->box;
}

