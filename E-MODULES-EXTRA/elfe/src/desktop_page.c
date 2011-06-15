
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
   Evas_Object *icon_moved;
   Eina_Bool edit_mode;
   Evas_Object *parent;
   Elm_Transit *placement;
   struct {
      Ecore_Timer *timer;
      Evas_Coord dx;
      Evas_Coord dy;
      Evas_Coord x;
      Evas_Coord y;
      Elm_Transit *transit;
   }swap;
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


/* Get closest position(column, row) of a given position (x,y) */
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

Evas_Object *
elfe_desktop_page_obj_at_get(Evas_Object *obj, int row, int col)
{
   Elfe_Desktop_Page *page = evas_object_data_get(obj, "desktop_page");
   Evas_Object *item;

   item = eina_matrixsparse_data_idx_get(page->items, row, col);
   return item;
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


static void
_icon_mouse_down_cb(void *data, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info)
{
    Elfe_Desktop_Page *page = data;

    if (page->edit_mode)
        {
	   Evas_Coord x, y;
	   page->icon_moved = obj;
	   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
//	   page->icon_moved_x = x;
//	   page->icon_moved_y = y;
        }
}

static void
_transit_swap_del_cb(void *data, Elm_Transit *transit)
{
   Elfe_Desktop_Page *page = data;

   page->swap.transit = NULL;
}

_icon_placement(Elfe_Desktop_Page *page, Evas_Coord evx, Evas_Coord evy)
{
   int row, col;
   Evas_Coord x, y, w, h, fx, fy, tx, ty;

   _xy_to_pos(page, evx, evy,
              &row, &col);

   _pos_to_geom(page, row, col,
                &tx, &ty, &w, &h);

   page->placement = elm_transit_add();
   elm_transit_object_add(page->placement, page->icon_moved);
   evas_object_geometry_get(page->layout, &x, &y, NULL, NULL);
   evas_object_geometry_get(page->icon_moved, &fx, &fy, NULL, NULL);

   elm_transit_effect_translation_add(page->placement, 0, 0, x + tx - fx, y + ty - fy);
   elm_transit_objects_final_state_keep_set(page->placement, EINA_TRUE);
   elm_transit_duration_set(page->placement, 0.3);
   elm_transit_go(page->placement);

   page->icon_moved = NULL;
   if (page->swap.timer)
     {
        ecore_timer_del(page->swap.timer);
        page->swap.timer = NULL;
     }
   page->swap.dx = 0;
   page->swap.dy = 0;

}

static Eina_Bool
_swap_timer_cb(void *data)
{
   int row, col;
   int trow, tcol;
   Evas_Object *item;
   Evas_Coord tx, ty, tw, th;
   Evas_Coord fx, fy, fw, fh;

   Elfe_Desktop_Page *page = data;

   _xy_to_pos(page, page->swap.x, page->swap.y,
              &row, &col);

   /* Get item under the mouse */
   item = elfe_desktop_page_obj_at_get(page->layout,
                                       row, col);
   /* If a transit is always active or we object under mouse
      is the dragged item : do nothing */
   if (!item || item == page->icon_moved || page->swap.transit)
     {
        page->swap.timer = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   /* Create Transit */
   page->swap.transit = elm_transit_add();

   /* Get the old position of draggef item */
   elfe_desktop_item_pos_get(page->icon_moved, &trow, &tcol);

   /* Transform this position into x,y position */
   _pos_to_geom(page, trow, tcol, &tx, &ty, &tw, &th);

   /* Transform the current position into x,y position */
   _pos_to_geom(page, row, col, &fx, &fy, &fw, &fh);

   /* Add item under the mouse to the transit */
   elm_transit_object_add(page->swap.transit, item);


   elm_transit_effect_translation_add(page->swap.transit, 0, 0, tx - fx, ty - fy);
   elm_transit_objects_final_state_keep_set(page->swap.transit, EINA_TRUE);
   elm_transit_duration_set(page->swap.transit, 0.3);
   elm_transit_go(page->swap.transit);
   elm_transit_del_cb_set(page->swap.transit, _transit_swap_del_cb, page);
   elfe_desktop_item_pos_set(item, trow, tcol);
   elfe_desktop_item_pos_set(page->icon_moved, row, col);

   elfe_desktop_item_pos_set(item, trow, tcol);
   elfe_desktop_item_pos_set(page->icon_moved, row, col);

   _icon_placement(page, page->swap.x, page->swap.y);

#if 0
	  /*
	     This position is not free, move the item under the mouse
	     at the previous position of the dragged icon
	  */
	  Evas_Coord x, y, fx, fy, tx, ty, tw, th;
	  Evas_Object *item;
	  int trow, tcol;

	  /* Get item under the mouse */

	  /* If a transit is always active or we object under mouse
	     is the dragged item do nothing */
	  if (!item || item == page->icon_moved || page->old)
	       return;

	  /* Create Transit */
	  page->old = elm_transit_add();

	  /* Get the position of dragged item */
	  elfe_desktop_item_pos_get(page->icon_moved, &trow, &tcol);

	  /* Transform this position into x,y position */
	  _pos_to_geom(page, trow, tcol, &tx, &ty, &tw, &th);

	  /* Add item under the mouse to the transit */
	  elm_transit_object_add(page->old, item);

	  /* Get the current position of the item under the mouse */
	  evas_object_geometry_get(item, &fx, &fy, NULL, NULL);
	  elm_transit_effect_translation_add(page->old, 0, 0, tx - fx, ty - fy);
	  elm_transit_objects_final_state_keep_set(page->old, EINA_TRUE);
	  elm_transit_duration_set(page->old, 1.0);
	  /* And move it */
	  elm_transit_go(page->old);
	  elm_transit_del_cb_set(page->old, _transit_old_del_cb, page);
	  elfe_desktop_item_pos_set(item, trow, tcol);
	  elfe_desktop_item_pos_set(page->icon_moved, row, col);
   #endif

   page->swap.timer = NULL;
   return ECORE_CALLBACK_CANCEL;
}


static void
_icon_mouse_move_cb(void *data,Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Desktop_Page *page = data;
   Evas_Event_Mouse_Move *ev = event_info;
   Evas_Coord w, h;
   int col, row;

   if (!page->edit_mode || !page->icon_moved)
     return;

   /* Move dragged icon on the canvas */
   evas_object_geometry_get(page->icon_moved, NULL, NULL, &w, &h);
   evas_object_move(page->icon_moved, ev->cur.output.x - w /2  , ev->cur.output.y - h / 2);

   if (page->swap.timer)
     {
        page->swap.dx += ev->cur.output.x - ev->prev.output.x;
        page->swap.dy += ev->cur.output.y - ev->prev.output.y;
        if (abs(page->swap.dx) > LONGPRESS_THRESHOLD ||
            abs(page->swap.dy) > LONGPRESS_THRESHOLD)
          {
             ecore_timer_del(page->swap.timer);
             page->swap.timer = NULL;
             page->swap.dx = 0;
             page->swap.dy = 0;
          }
     }
   else
     {
        /* Whats the position ? */
        _xy_to_pos(page, ev->cur.output.x, ev->cur.output.y,
                   &row, &col);

        if (elfe_desktop_page_pos_is_free(page->layout,
                                          row,col))
          {
             page->swap.x = ev->cur.output.x;
             page->swap.y = ev->cur.output.y;
             page->swap.timer = ecore_timer_add(0.6, _swap_timer_cb, page);
          }
     }
}


static void
_icon_mouse_up_cb(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Desktop_Page *page = data;
   Evas_Event_Mouse_Up *ev = event_info;

   if (page->edit_mode && page->icon_moved)
     {
        _icon_placement(page, ev->output.x, ev->output.y);
     }
}

void
elfe_desktop_page_edit_mode_set(Evas_Object *obj, Eina_Bool mode)
{
   Elfe_Desktop_Page *page = evas_object_data_get(obj, "desktop_page");;
   Evas_Object *item;
   Eina_Iterator *iter;
   Eina_Matrixsparse_Cell *cell;

   if (page->edit_mode == mode)
     return;

   page->edit_mode = mode;

   if (mode)
     elm_object_scroll_freeze_push(page->parent);
   else
     elm_object_scroll_freeze_push(page->parent);

   iter = eina_matrixsparse_iterator_new(page->items);
   EINA_ITERATOR_FOREACH(iter, cell)
     {
	item  = eina_matrixsparse_cell_data_get(cell);
	elfe_desktop_item_edit_mode_set(item, mode);
        evas_object_event_callback_add(item, EVAS_CALLBACK_MOUSE_DOWN, _icon_mouse_down_cb, page);
        evas_object_event_callback_add(page->layout, EVAS_CALLBACK_MOUSE_MOVE, _icon_mouse_move_cb, page);
        evas_object_event_callback_add(item, EVAS_CALLBACK_MOUSE_UP, _icon_mouse_up_cb, page);
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

   /*
      This is a warkarround :
      I need a layout (free position of children) and i'm still using
      e_layout for that.
      And as e_layout Object doesn't handle elementary objects
      hierarchy for scroll_freeze, i keep a pointer on the parent,
      which is in this case an elm_scroller.
      TODO: use elm_grid instead of e_layout !
    */
   page->parent = parent;

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
