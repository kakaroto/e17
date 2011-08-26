#include <e.h>
#include <Elementary.h>

#include "dock.h"
#include "elfe_config.h"
#include "desktop_item.h"
#include "utils.h"

typedef struct _Elfe_Dock Elfe_Dock;

struct _Elfe_Dock
{
   Evas_Object *edje;
   Evas_Object *table;
   Eina_Matrixsparse *items;
   Evas_Object *allapps_icon;
   Eina_Bool edit_mode:  1;
   Eina_Bool place_mode: 1;
   Eina_List *overs;
};

static void _allapps_icon_add(Elfe_Dock *dock, const char *name);

static void
_xy_to_pos(Elfe_Dock *dock, Evas_Coord x, Evas_Coord y __UNUSED__,
	   int *col)
{
   Evas_Coord ow, oh;
   Evas_Coord w = 0;

   if(!dock)
     return;

   evas_object_geometry_get(dock->table, NULL, NULL, &ow, &oh);

   if (elfe_home_cfg->cols && col)
     {
	w = ow / elfe_home_cfg->cols;
	if (w)
	  *col = (x/w) % elfe_home_cfg->cols;
     }

}


static void
_item_delete_cb(void *data , Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Object *item = event_info;
   Elfe_Dock *dock = data;
   int row, col;

   elfe_desktop_item_pos_get(item, &row, &col);
   eina_matrixsparse_cell_idx_clear(dock->items, row, col);
   evas_object_del(item);
   elfe_home_config_dock_item_del(col);

}

static Eina_Bool
_pos_is_free(Elfe_Dock *dock, int row, int col)
{
   if (eina_matrixsparse_data_idx_get(dock->items, row, col))
     return EINA_TRUE;
   else
     return EINA_FALSE;
}

static void
_app_icon_clicked_cb(void *data , Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elfe_Dock *dock = data;

   evas_object_smart_callback_call(dock->edje, "allapps,clicked", NULL);
}

static void
_allapps_icon_add(Elfe_Dock *dock, const char *name)
{
   Evas_Object *ic;

   if (dock->allapps_icon)
     {
	evas_object_del(dock->allapps_icon);
     }

   ic = elfe_desktop_item_add(dock->table, 0, elfe_home_cfg->cols - 1,
                              name, ELFE_DESKTOP_ITEM_ICON, NULL);
   evas_object_smart_callback_add(ic, "clicked", _app_icon_clicked_cb, dock);
   evas_object_show(ic);
   evas_object_size_hint_min_set(ic, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
   evas_object_size_hint_max_set(ic, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);

   elm_table_pack(dock->table, ic, elfe_home_cfg->cols - 1, 0, 1, 1);
   evas_object_show(ic);
   eina_matrixsparse_data_idx_set(dock->items, 0, elfe_home_cfg->cols - 1, ic);

   dock->allapps_icon = ic;
}

static void
_populate_dock(Elfe_Dock *dock)
{
   Elfe_Desktop_Item_Config *dic;
   Evas_Object *item;
   Eina_List *l;

   EINA_LIST_FOREACH(elfe_home_cfg->dock_items, l, dic)
     {

	/* This position is already in use, this is a  conf issue! */
	/* FIXME: delete item from config ? */
	if (eina_matrixsparse_data_idx_get(dock->items, 0, dic->col))
	     continue;

	item = elfe_desktop_item_add(dock->table, 0, dic->col,
				     dic->name, dic->type, NULL);
	if (!item)
	  continue;

	evas_object_smart_callback_add(item, "item,delete", _item_delete_cb, dock);
        evas_object_size_hint_min_set(item, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
        evas_object_size_hint_max_set(item, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
        evas_object_size_hint_align_set(item, 0.5, 0.5);

        elm_table_pack(dock->table, item, dic->col, 0, 1, 1);
        evas_object_show(item);
	eina_matrixsparse_data_idx_set(dock->items, 0, dic->col, item);
     }

   _allapps_icon_add(dock,  "icon/widgets");
}

void
elfe_dock_place_mode_set(Evas_Object *obj, Eina_Bool mode)
{
   Elfe_Dock *dock = evas_object_data_get(obj, "dock");
   int m;
   int i,j;
   Evas_Object *over;
   char buf[PATH_MAX];

   if (dock->place_mode == mode)
       return;

   dock->place_mode = mode;

   if (mode)
     {
	Evas_Coord x, y, w, h;

	evas_object_geometry_get(dock->table, &x, &y, &w, &h);
	m = w / elfe_home_cfg->cols;

        for (i = 0; i < elfe_home_cfg->cols; i++)
          {
             Evas_Object *o_edje;
             over = elm_layout_add(dock->table);
             elm_layout_file_set(over, buf, "elfe/gadget/places/over");
             o_edje = elm_layout_edje_get(over);
             if (_pos_is_free(dock, 0, i))
               edje_object_signal_emit(o_edje, "place,busy", "elfe");
             else
               edje_object_signal_emit(o_edje, "place,free", "elfe");

             evas_object_resize(o_edje, m, h);
             evas_object_move(o_edje, x + i*m,  y);
             evas_object_show(over);
             dock->overs = eina_list_append(dock->overs, over);
          }
     }
   else
     {
	EINA_LIST_FREE(dock->overs, over)
	  {
	     evas_object_del(over);
	  }

	dock->overs = NULL;

     }

}

void
elfe_dock_edit_mode_set(Evas_Object *obj, Eina_Bool mode)
{
   Elfe_Dock *dock = evas_object_data_get(obj, "dock");
   Evas_Object *item;
   Eina_Iterator *iter;
   Eina_Matrixsparse_Cell *cell;

   if (dock->edit_mode == mode)
       return;

   dock->edit_mode = mode;

   iter = eina_matrixsparse_iterator_new(dock->items);
   EINA_ITERATOR_FOREACH(iter, cell)
     {
	item  = eina_matrixsparse_cell_data_get(cell);
	elfe_desktop_item_edit_mode_set(item, mode);
     }
   eina_iterator_free(iter);

   if (mode)
     _allapps_icon_add(dock, "icon/delete");
   else
     _allapps_icon_add(dock, "icon/widgets");
}

void
elfe_dock_item_app_add(Evas_Object *obj, Efreet_Menu *menu,
                       Evas_Coord x, Evas_Coord y)
{
    Elfe_Dock *dock = evas_object_data_get(obj, "dock");
    Evas_Object *item;
    int col = 0;

    _xy_to_pos(dock, x, y, &col);

    /* This position is already used by another item! */
    if (eina_matrixsparse_data_idx_get(dock->items, 0, col)) return;

    item = elfe_desktop_item_add(dock->table, 0, col,
                                 menu->desktop->orig_path,
                                 ELFE_DESKTOP_ITEM_ICON, NULL);
    evas_object_show(item);
    evas_object_size_hint_min_set(item, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
    evas_object_size_hint_max_set(item, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
    evas_object_size_hint_align_set(item, 0.5, 0.5);

    elm_table_pack(dock->table, item, col, 0, 1, 1);

    eina_matrixsparse_data_idx_set(dock->items, 0, col, item);
    elfe_home_config_dock_item_add(col,
                                      menu->desktop->orig_path);
    evas_object_smart_callback_add(item, "item,delete", _item_delete_cb, dock);
}

Evas_Object *
elfe_dock_add(Evas_Object *parent)
{
   Elfe_Dock *dock;

   dock = calloc(1, sizeof(Elfe_Dock));
   if (!dock)
     return NULL;

   dock->items = eina_matrixsparse_new(1, elfe_home_cfg->cols,
				       NULL, NULL);

   dock->table = elm_table_add(parent);
   elm_table_homogeneous_set(dock->table, EINA_TRUE);

   _populate_dock(dock);

   dock->edje = elm_layout_add(parent);
   elm_layout_file_set(dock->edje, elfe_home_cfg->theme, "elfe/dock/layout");
   evas_object_show(dock->edje);
   evas_object_data_set(dock->edje, "dock", dock);

   elm_layout_content_set(dock->edje, "elfe.swallow.content", dock->table);

   return dock->edje;
}


