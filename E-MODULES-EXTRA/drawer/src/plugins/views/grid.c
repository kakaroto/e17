/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include "grid.h"

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Item Item;

typedef enum
{
   GRID_TOP,
   GRID_RIGHT,
   GRID_BOTTOM,
   GRID_LEFT,
   GRID_FLOAT
} Grid_Orient;

struct _Instance 
{
   Drawer_View *view;

   Evas *evas;

   Eina_List *items;

   Evas_Object *o_box, *o_con, *o_scroll;

   char theme_file[4096];

   const char *parent_id;

   Grid_Orient orient;
};

struct _Item
{
   Instance *inst;
   Evas_Object *o_holder, *o_icon;

   Drawer_Source_Item *si;
};

static void _grid_containers_create(Instance *inst);
static Item *_grid_item_create(Instance *inst, Drawer_Source_Item *si);
static Item *_grid_category_create(Instance *inst, Drawer_Source_Item *si);
static void _grid_items_free(Instance *inst);

static int  _grid_sort_by_category_cb(const void *d1, const void *d2);
static void _grid_entry_select_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _grid_entry_deselect_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _grid_entry_activate_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _grid_event_activate_free(void *data __UNUSED__, void *event);

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "Grid"};

EAPI void *
drawer_plugin_init(Drawer_Plugin *p, const char *id)
{
   Instance *inst = NULL;

   inst = E_NEW(Instance, 1);

   inst->view = DRAWER_VIEW(p);

   inst->parent_id = eina_stringshare_add(id);

   snprintf(inst->theme_file, sizeof(inst->theme_file),
	    "%s/e-module-drawer.edj", drawer_conf->module->dir);

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;

   inst = p->data;

   _grid_items_free(inst);
   eina_stringshare_del(inst->parent_id);
   if (inst->o_box) evas_object_del(inst->o_box);
   if (inst->o_con) evas_object_del(inst->o_con);
   if (inst->o_scroll) evas_object_del(inst->o_scroll);

   E_FREE(inst);

   return 1;
}

EAPI Evas_Object *
drawer_view_render(Drawer_View *v, Evas *evas, Eina_List *items)
{
   Instance *inst = NULL;
   Eina_List *l = NULL, *ll = NULL;
   Drawer_Source_Item *si;
   const char *cat = NULL;
   Eina_Bool change = EINA_FALSE;

   inst = DRAWER_PLUGIN(v)->data;

   inst->evas = evas;

   if (inst->o_box) evas_object_del(inst->o_box);
   if (inst->o_con) evas_object_del(inst->o_con);
   if (inst->o_scroll) evas_object_del(inst->o_scroll);
   _grid_containers_create(inst);
   _grid_items_free(inst);

   EINA_LIST_FOREACH(items, l, si)
      ll = eina_list_append(ll, si);
   ll = eina_list_sort(ll, eina_list_count(ll), _grid_sort_by_category_cb);
   switch (inst->orient)
     {
      case GRID_BOTTOM:
      case GRID_RIGHT:
	 ll = eina_list_reverse(ll);
	 break;
     }

   EINA_LIST_FOREACH(ll, l, si)
     {
	Item *e;

	if (!cat && si->category)
	  {
	     cat = eina_stringshare_add(si->category);
	     change = EINA_TRUE;
	  }
	else if (cat && !si->category)
	  {
	     eina_stringshare_del(cat);
	     cat = NULL;
	     change = EINA_TRUE;
	  }
	else if (cat && si->category && (strcmp(cat, si->category)))
	  {
	     eina_stringshare_del(cat);
	     cat = eina_stringshare_add(si->category);
	     change = EINA_TRUE;
	  }
	else
	  change = EINA_FALSE;

	if (change)
	  {
	     Item *c;

	     c = _grid_category_create(inst, si);
	     inst->items = eina_list_append(inst->items, c);
	     edje_object_part_box_append(inst->o_box, "e.box.content", c->o_holder);
	  }
	e = _grid_item_create(inst, si);
	inst->items = eina_list_append(inst->items, e);
	edje_object_part_box_append(inst->o_box, "e.box.content", e->o_holder);
     }
   eina_stringshare_del(cat);

   return inst->o_con;
}

EAPI void
drawer_view_content_size_get(Drawer_View *v, E_Gadcon_Client *gcc, Drawer_Content_Margin *margin, int *w, int *h)
{
   Instance *inst = NULL;
   Evas_Coord gx, gy, gw, gh, zw, zh, zx, zy;

   inst = DRAWER_PLUGIN(v)->data;
   edje_object_size_min_calc(inst->o_con, w, h);

   e_gadcon_client_geometry_get(gcc, &gx, &gy, &gw, &gh);
   zx = gcc->gadcon->zone->x;
   zy = gcc->gadcon->zone->y;
   zw = gcc->gadcon->zone->w;
   zh = gcc->gadcon->zone->h;
   switch (gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	 if (gx - *w < zx + margin->left)
	   *w = gx - zx - margin->left;
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	if (gx + gw + *w > zx + zw + margin->right)
	  *w = zx + zw - gx - gw + margin->right;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	if (gy + gh + *h > zy + zh + margin->bottom)
	  *h = zy + zh - gy - gh + margin->bottom;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	if (gy - *h < zy + margin->top)
	  *h = gy - zy - margin->top;
	break;
      case E_GADCON_ORIENT_FLOAT:
	if (*w > zw - margin->left - margin->right)
	  *w = zw - margin->left - margin->right;
	break;
     }
}

EAPI void
drawer_view_orient_set(Drawer_View *v, E_Gadcon_Orient orient)
{
   Instance *inst = NULL;
   
   inst = DRAWER_PLUGIN(v)->data;

   switch (orient)
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	inst->orient = GRID_RIGHT;
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	inst->orient = GRID_LEFT;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	inst->orient = GRID_TOP;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	inst->orient = GRID_BOTTOM;
	break;
      case E_GADCON_ORIENT_FLOAT:
	inst->orient = GRID_FLOAT;
	break;
     }
}

static void
_grid_containers_create(Instance *inst)
{
   Evas *evas;

   evas = inst->evas;
   inst->o_con = edje_object_add(evas);
   inst->o_box = edje_object_add(evas);

   if (!e_theme_edje_object_set(inst->o_con, "base/theme/modules/drawer", "modules/drawer/grid"))
     edje_object_file_set(inst->o_con, inst->theme_file, "modules/drawer/grid");
   if (!e_theme_edje_object_set(inst->o_box, "base/theme/modules/drawer", "modules/drawer/grid/box"))
     edje_object_file_set(inst->o_con, inst->theme_file, "modules/drawer/grid/box");


/*   inst->o_scroll = e_widget_scrollframe_simple_add(evas, inst->o_box);*/
   edje_object_part_swallow(inst->o_con, "e.swallow.content", inst->o_box);
   evas_object_show(inst->o_box);
   evas_object_show(inst->o_con);
}

static Item *
_grid_item_create(Instance *inst, Drawer_Source_Item *si)
{
   Item *e;
   Evas_Coord w, h;

   e = E_NEW(Item, 1);

   e->o_holder = edje_object_add(inst->evas);
   if (!e_theme_edje_object_set(e->o_holder, "base/theme/modules/drawer",
				"modules/drawer/grid/item"))
     edje_object_file_set(e->o_holder, inst->theme_file,
			  "modules/drawer/grid/item");

   edje_object_part_geometry_get(e->o_holder, "e.swallow.content", NULL, NULL, &w, &h);
   e->o_icon = drawer_util_icon_create(si, inst->evas, w, h);

   edje_object_part_swallow(e->o_holder, "e.swallow.content", e->o_icon);
   evas_object_pass_events_set(e->o_icon, 1);
   evas_object_show(e->o_icon);

   edje_object_part_text_set(e->o_holder, "e.text.label", si->label);

   evas_object_show(e->o_holder);

   e->inst = inst;
   e->si = si;

   edje_object_signal_callback_add(e->o_holder, "e,action,select", "drawer", 
				   _grid_entry_select_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,deselect", "drawer", 
				   _grid_entry_deselect_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,activate", "drawer", 
				   _grid_entry_activate_cb, e);

   return e;
}

static Item *
_grid_category_create(Instance *inst, Drawer_Source_Item *si)
{
   Item *e;
   Evas_Coord w, h;
   char buf[1024];

   e = E_NEW(Item, 1);

   e->o_holder = edje_object_add(inst->evas);
   if (!e_theme_edje_object_set(e->o_holder, "base/theme/modules/drawer",
				"modules/drawer/grid/category"))
     edje_object_file_set(e->o_holder, inst->theme_file,
			  "modules/drawer/grid/category");

   if (si->category)
     snprintf(buf, sizeof(buf), "%s", si->category);
   else
     snprintf(buf, sizeof(buf), "Uncategorised");

   edje_object_part_text_set(e->o_holder, "e.text.category", buf);

   e->inst = inst;
   e->si = si;

   return e;
}

static void _grid_items_free(Instance *inst)
{
   while (inst->items)
     {
	Item *e;

	e = inst->items->data;
	if (e->o_icon)
	  evas_object_del(e->o_icon);
	if (e->o_holder)
	  evas_object_del(e->o_holder);
	E_FREE(e);

	inst->items = eina_list_remove_list(inst->items, inst->items);
     }
}

static int
_grid_sort_by_category_cb(const void *d1, const void *d2)
{
   const Drawer_Source_Item *si1;
   const Drawer_Source_Item *si2;
   int ret;

   if (!(si1 = d1)) return -1;
   if (!(si2 = d2)) return 1;

   if (!si1->category) return -1;
   if (!si2->category) return 1;

   ret = strcmp(si1->category, si2->category);
   
   return ret > 0 ? 1 : -1;
}

static void
_grid_entry_select_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Item *e = NULL;
   Instance *inst = NULL;
   Drawer_Source_Item *si = NULL;

   e = data;
   inst = e->inst;
   si = e->si;
   edje_object_part_text_set(inst->o_con, "e.text.label", si->label);
   edje_object_part_text_set(inst->o_con, "e.text.description", si->description);
}

static void
_grid_entry_deselect_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Item *e = NULL;
   Instance *inst = NULL;

   e = data;
   inst = e->inst;
   edje_object_part_text_set(inst->o_con, "e.text.label", NULL);
   edje_object_part_text_set(inst->o_con, "e.text.description", NULL);
}

static void
_grid_entry_activate_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Item *e = NULL;
   Drawer_Event_View_Activate *ev;

   e = data;
   ev = E_NEW(Drawer_Event_View_Activate, 1);
   ev->data = e->si;
   ev->view = e->inst->view;
   ev->id = eina_stringshare_add(e->inst->parent_id);
   ecore_event_add(DRAWER_EVENT_VIEW_ITEM_ACTIVATE, ev, _grid_event_activate_free, NULL);

   /* XXX: this doesn't seem to work */
   edje_object_signal_emit(e->inst->o_con, "e,action,activate", "drawer");
}

static void
_grid_event_activate_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_View_Activate *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}
