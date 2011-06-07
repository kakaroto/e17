
#include <e.h>
#include <Elementary.h>

#include "desktop_page.h"
#include "elfe_config.h"
#include "desktop_item.h"

typedef struct _Elfe_Desktop_Page Elfe_Desktop_Page;

struct _Elfe_Desktop_Page
{
   int desktop;
   Evas_Object *layout;
   E_Gadcon_Location *location;
   E_Gadcon *gc;
   Eina_Matrixsparse *items;
};

static void
_pos_to_geom(Elfe_Desktop_Page *page,
	     int row, int col,
	     Evas_Coord *x, Evas_Coord *y,
	     Evas_Coord *w, Evas_Coord *h)
{
    Evas_Coord ox, oy, ow, oh;

    if(!page)
        return;

    evas_object_geometry_get(page->layout, &ox, &oy, &ow, &oh);

    if (elfe_home_cfg->cols && w)
        *w = ow / elfe_home_cfg->cols;
    if (elfe_home_cfg->rows && h)
        *h = oh / elfe_home_cfg->rows;

    if (x && w)
        *x = col * *w;
    if (y && h)
        *y = row * *h;
}

static void
_xy_to_pos(Elfe_Desktop_Page *page, Evas_Coord x, Evas_Coord y,
	   int *row, int *col)
{
   Evas_Coord ow, oh;
   Evas_Coord w = 0, h = 0;

   if(!page)
     return;

   evas_object_geometry_get(page->layout, NULL, NULL, &ow, &oh);

   if (elfe_home_cfg->cols && col)
     {
	w = ow / elfe_home_cfg->cols;
	if (w)
	  *col = (x/w) % elfe_home_cfg->cols;
     }
   if (elfe_home_cfg->rows && row)
     {
	h = oh / elfe_home_cfg->rows;
	if (h)
	  *row = (y/h) % elfe_home_cfg->rows;
     }
}

static void
_gadcon_populate_class(void *data __UNUSED__, E_Gadcon *gc __UNUSED__, const E_Gadcon_Client_Class *cc __UNUSED__)
{
}


static int
_gadcon_client_add(void *data __UNUSED__, const E_Gadcon_Client_Class *cc __UNUSED__)
{
   return 0;
}

static void
_gadcon_client_remove(void *data __UNUSED__, E_Gadcon_Client *gcc __UNUSED__)
{
}

static E_Gadcon *
_gadcon_add(Evas *e,
            const char *loc_name,
            E_Gadcon_Location *location,
            E_Zone *zone)
{
   E_Gadcon *gc;

   gc = E_OBJECT_ALLOC(E_Gadcon, E_GADCON_TYPE, NULL);
   if (!gc)
     return NULL;

   gc->name = eina_stringshare_add(loc_name);
   gc->orient = E_GADCON_ORIENT_FLOAT;
   gc->location = location;
   gc->layout_policy = E_GADCON_LAYOUT_POLICY_PANEL;
   gc->evas = e;
   e_gadcon_populate_callback_set(gc, _gadcon_populate_class, gc);

   gc->id = 0;
   gc->edje.o_parent = NULL;
   gc->edje.swallow_name = NULL;
   gc->shelf = NULL;
   gc->toolbar = NULL;
   gc->editing = 0;
   gc->o_container = NULL;
   gc->frame_request.func = NULL;
   gc->resize_request.func = NULL;
   gc->min_size_request.func = NULL;
   e_gadcon_zone_set(gc, zone);
   e_gadcon_custom_new(gc);
   e_gadcon_zone_set(gc, zone);
   e_gadcon_custom_new(gc);
   return gc;
}

static void
_item_delete_cb(void *data , Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Object *item = event_info;
   Elfe_Desktop_Page *page = data;
   int row, col;

   elfe_desktop_item_pos_get(item, &row, &col);
   eina_matrixsparse_cell_idx_clear(page->items, row, col);
   evas_object_del(item);
   elfe_home_config_desktop_item_del(page->desktop,
				     row, col);

}

static void
_populate_page(Elfe_Desktop_Page *page)
{
   Elfe_Desktop_Config *dc;
   Elfe_Desktop_Item_Config *dic;
   Evas_Object *item;
   Eina_List *l;

   dc = eina_list_nth(elfe_home_cfg->desktops, page->desktop);
   EINA_LIST_FOREACH(dc->items, l, dic)
     {
	Evas_Coord x = 0, y = 0, w = 0, h = 0;

	/* This position is already in use, this is a  conf issue! */
	/* FIXME: delete item from config ? */
	if (eina_matrixsparse_data_idx_get(page->items, dic->row, dic->col))
	     continue;

	item = elfe_desktop_item_add(page->layout, dic->row, dic->col,
				     dic->name, dic->type, page->gc);
	if (!item)
	  continue;

	evas_object_smart_callback_add(item, "item,delete", _item_delete_cb, page);

	e_layout_pack(page->layout, item);
	_pos_to_geom(page, dic->row, dic->col, &x, &y, &w, &h);
        e_layout_child_resize(item, w, h);
        e_layout_child_move(item, x, y);
        evas_object_show(item);
	eina_matrixsparse_data_idx_set(page->items, dic->row, dic->col, item);
     }
}

static void
_page_resize_cb(void *data , Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elfe_Desktop_Page *page = data;
   Evas_Coord x, y, w, h;
   Evas_Coord ow, oh;
   Evas_Object *item;
   int col = 0, row = 0;
   Eina_Iterator *iter;
   Eina_Matrixsparse_Cell *cell;

   evas_object_geometry_get(page->layout, NULL, NULL, &ow, &oh);
   e_layout_virtual_size_set(page->layout, ow, oh);

   iter = eina_matrixsparse_iterator_new(page->items);
   EINA_ITERATOR_FOREACH(iter, cell)
     {
	item  = eina_matrixsparse_cell_data_get(cell);
	elfe_desktop_item_pos_get(item, &row, &col);
	_pos_to_geom(page, row, col, &x, &y, &w, &h);
	e_layout_child_resize(item, w, h);
        e_layout_child_move(item, x, y);
	evas_object_show(item);
     }
   eina_iterator_free(iter);
}

Eina_Bool
elfe_desktop_page_pos_is_free(Evas_Object *obj, int row, int col)
{
   Elfe_Desktop_Page *page = evas_object_data_get(obj, "desktop_page");
   if (eina_matrixsparse_data_idx_get(page->items, row, col))
     return EINA_TRUE;
   else
     return EINA_FALSE;
}

void
elfe_desktop_page_item_gadget_add(Evas_Object *obj, const char *name,
				Evas_Coord x, Evas_Coord y)
{
    Elfe_Desktop_Page *page = evas_object_data_get(obj, "desktop_page");
    Evas_Object *item;
    Evas_Coord ox = 0, oy = 0, ow = 0, oh = 0;
    int row = 0, col = 0;

    _xy_to_pos(page, x, y, &row, &col);

    /* This position is already used by another item! */
    if (eina_matrixsparse_data_idx_get(page->items, row, col)) return;

    item = elfe_desktop_item_add(page->layout, row, col,
				 name,
                                 ELFE_DESKTOP_ITEM_GADGET, page->gc);
    e_layout_pack(page->layout, item);
    _pos_to_geom(page, row, col, &ox, &oy, &ow, &oh);
    e_layout_child_resize(item, ow, oh);
    e_layout_child_move(item, ox, oy);
    evas_object_show(item);
    evas_object_raise(item);
    eina_matrixsparse_data_idx_set(page->items, row, col, item);

    elfe_home_config_desktop_item_add(page->desktop,
				      ELFE_DESKTOP_ITEM_GADGET,
				      row, col,
				      0, 0, 0, 0,
				      name);
    evas_object_smart_callback_add(item, "item,delete", _item_delete_cb, page);
}

void
elfe_desktop_page_item_app_add(Evas_Object *obj, Efreet_Menu *menu,
				Evas_Coord x, Evas_Coord y)
{
    Elfe_Desktop_Page *page = evas_object_data_get(obj, "desktop_page");
    Evas_Object *item;
    Evas_Coord ox = 0, oy = 0, ow = 0, oh = 0;
    int row = 0, col = 0;

    _xy_to_pos(page, x, y, &row, &col);

    /* This position is already used by another item! */
    if (eina_matrixsparse_data_idx_get(page->items, row, col)) return;

    item = elfe_desktop_item_add(page->layout, row, col,
				 menu->desktop->orig_path,
                                 ELFE_DESKTOP_ITEM_APP, NULL);
    e_layout_pack(page->layout, item);
    _pos_to_geom(page, row, col, &ox, &oy, &ow, &oh);
    e_layout_child_resize(item, ow, oh);
    e_layout_child_move(item, ox, oy);
    evas_object_show(item);
    evas_object_raise(item);

    eina_matrixsparse_data_idx_set(page->items, row, col, item);

    elfe_home_config_desktop_item_add(page->desktop,
				      ELFE_DESKTOP_ITEM_APP,
				      row, col,
				      0, 0, 0, 0,
				      menu->desktop->orig_path);
    evas_object_smart_callback_add(item, "item,delete", _item_delete_cb, page);
}

void
elfe_desktop_page_edit_mode_set(Evas_Object *obj, Eina_Bool mode)
{
   Elfe_Desktop_Page *page = evas_object_data_get(obj, "desktop_page");;
   Evas_Object *item;
   Eina_Iterator *iter;
   Eina_Matrixsparse_Cell *cell;

   iter = eina_matrixsparse_iterator_new(page->items);
   EINA_ITERATOR_FOREACH(iter, cell)
     {
	item  = eina_matrixsparse_cell_data_get(cell);
	elfe_desktop_item_edit_mode_set(item, mode);
     }
   eina_iterator_free(iter);
}

Evas_Object *
elfe_desktop_page_add(Evas_Object *parent, E_Zone *zone,
                      int desktop, const char *desktop_name)
{
   Elfe_Desktop_Page *page;
   const char *loc_name;

   page = calloc(1, sizeof(Elfe_Desktop_Page));
   if (!page)
     return NULL;

   page->items = eina_matrixsparse_new(elfe_home_cfg->rows, elfe_home_cfg->cols,
				       NULL, NULL);

   page->layout = e_layout_add(evas_object_evas_get(parent));
   evas_object_data_set(page->layout, "desktop_page", page);
   evas_object_event_callback_add(page->layout, EVAS_CALLBACK_RESIZE,
				  _page_resize_cb, page);

   loc_name = eina_stringshare_printf("Elfe %s\n", desktop_name);

   page->location = e_gadcon_location_new (loc_name,
					   E_GADCON_SITE_DESKTOP,
					   _gadcon_client_add, page,
					   _gadcon_client_remove, page);

   e_gadcon_location_set_icon_name(page->location, "preferences-desktop");
   e_gadcon_location_register(page->location);

   page->gc = _gadcon_add(evas_object_evas_get(parent),
			  loc_name,
			  page->location,
			  zone);

   eina_stringshare_del(loc_name);
   page->desktop = desktop;

   /* Populate page with items from config */
   _populate_page(page);

   return page->layout;
}
