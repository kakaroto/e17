/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "list.h"

#define PADDING 20

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Entry Entry;

typedef enum
{
   LIST_TOP,
   LIST_RIGHT,
   LIST_BOTTOM,
   LIST_LEFT,
   LIST_FLOAT
} List_Orient;

struct _Instance 
{
   Drawer_View *view;

   Evas *evas;

   Eina_List *entries;

   Evas_Object *o_box, *o_con;

   Ecore_Timer *scroll_timer;
   Ecore_Animator *scroll_animator;
   double scroll_wanted;
   double scroll_pos;

   char theme_file[4096];

   List_Orient orient;
};

struct _Entry
{
   Instance *inst;
   Evas_Object *o_holder, *o_icon;

   Drawer_Source_Item *si;
};

static void _list_containers_create(Instance *inst);
static Entry * _list_horizontal_entry_create(Instance *inst, Drawer_Source_Item *si);
static Entry * _list_vertical_entry_create(Instance *inst, Drawer_Source_Item *si);
static void _list_item_pack_options(Instance *inst, Entry *e);
static void _list_autoscroll_update(Instance *inst, Evas_Coord x, Evas_Coord y);

static int _list_scroll_timer(void *data);
static int _list_scroll_animator(void *data);
static void _list_cb_list_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _list_entry_select_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _list_entry_activate_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _list_event_activate_free(void *data __UNUSED__, void *event);

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "List"};

EAPI void *
drawer_plugin_init(Drawer_Plugin *p, const char *id)
{
   Instance *inst = NULL;

   inst = E_NEW(Instance, 1);

   inst->view = DRAWER_VIEW(p);

   snprintf(inst->theme_file, sizeof(inst->theme_file),
	 "%s/e-module-drawer.edj", drawer_conf->module->dir);

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;

   inst = p->data;

   if (inst->o_box) evas_object_del(inst->o_box);
   if (inst->o_con) evas_object_del(inst->o_con);

   E_FREE(inst);

   return 1;
}

EAPI Evas_Object *
drawer_view_render(Drawer_View *v, Evas *evas, Eina_List *items)
{
   Instance *inst = NULL;
   Eina_List *l, *ll;

   inst = DRAWER_PLUGIN(v)->data;

   inst->evas = evas;

   if (inst->o_box) evas_object_del(inst->o_box);
   if (inst->o_con) evas_object_del(inst->o_con);
   _list_containers_create(inst);

   e_box_freeze(inst->o_box);
   while (inst->entries)
     {
	Entry *e;

	e = inst->entries->data;
	evas_object_del(e->o_icon);
	evas_object_del(e->o_holder);
	E_FREE(e);

	inst->entries = eina_list_remove_list(inst->entries, inst->entries);
     }
   switch (inst->orient)
     {
      case LIST_BOTTOM:
      case LIST_RIGHT:
	 ll = eina_list_reverse(items);
	 break;
      default:
	 ll = items;
	 break;
     }

   for (l = ll; l; l = l->next)
     {
	Drawer_Source_Item *si;
	Entry *e;

	si = l->data;

	switch(inst->orient)
	  {
	   case LIST_TOP:
	   case LIST_BOTTOM:
	      e = _list_vertical_entry_create(inst, si);
	      break;
	   default:
	      e = _list_horizontal_entry_create(inst, si);
	      break;
	  }
	inst->entries = eina_list_append(inst->entries, e);
	e_box_pack_end(inst->o_box, e->o_holder);
	_list_item_pack_options(inst, e);
     }
   e_box_thaw(inst->o_box);
   return inst->o_con;
}

EAPI void
drawer_view_content_size_get(Drawer_View *v, E_Gadcon_Client *gcc, int *w, int *h)
{
   Instance *inst = NULL;
   Evas_Coord bw, bh, gx, gy, gw, gh, zw, zh, zx, zy;

   inst = DRAWER_PLUGIN(v)->data;
   e_box_min_size_get(inst->o_box, &bw, &bh);
   edje_extern_object_min_size_set(inst->o_box, bw, bh);
   edje_object_size_min_calc(inst->o_con, w, h);
   edje_extern_object_min_size_set(inst->o_box, 0, 0);

   /* XXX: Include real the size of the popup, instead of PADDING? */
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
	 if (gx - *w < zx)
	   *w = gx - zx - PADDING;
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	if (gx + gw + *w > zx + zw)
	  *w = zx + zw - gx - gw - PADDING;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	if (gy + gh + *h > zy + zh)
	  *h = zy + zh - gy - gh - PADDING;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	if (gy - *h < zy)
	  *h = gy - zy - PADDING;
	break;
      case E_GADCON_ORIENT_FLOAT:
	if (*w > zw)
	  *w = zw - 2 * PADDING;
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
	e_box_orientation_set(inst->o_box, 1);
	inst->orient = LIST_RIGHT;
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	e_box_orientation_set(inst->o_box, 1);
	inst->orient = LIST_LEFT;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	e_box_orientation_set(inst->o_box, 0);
	inst->orient = LIST_TOP;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	e_box_orientation_set(inst->o_box, 0);
	inst->orient = LIST_BOTTOM;
	break;
      case E_GADCON_ORIENT_FLOAT:
	e_box_orientation_set(inst->o_box, 1);
	inst->orient = LIST_FLOAT;
	break;
     }
}

static void
_list_containers_create(Instance *inst)
{
   Evas *evas;
   const char *group;

   evas = inst->evas;
   inst->o_con = edje_object_add(evas);

   inst->o_box = e_box_add(evas);
   e_box_homogenous_set(inst->o_box, 1);
   switch(inst->orient)
     {
      case LIST_TOP:
	 e_box_orientation_set(inst->o_box, 0);
	 e_box_align_set(inst->o_box, 0.5, 1);
	 group = eina_stringshare_add("modules/drawer/list/vertical");
	 break;
      case LIST_BOTTOM:
	 e_box_orientation_set(inst->o_box, 0);
	 e_box_align_set(inst->o_box, 0.5, 0);
	 group = eina_stringshare_add("modules/drawer/list/vertical");
	 break;
      case LIST_LEFT:
      case LIST_FLOAT:
	 e_box_orientation_set(inst->o_box, 1);
	 e_box_align_set(inst->o_box, 1, 0.5);
	 group = eina_stringshare_add("modules/drawer/list/horizontal");
	 break;
      case LIST_RIGHT:
	 e_box_orientation_set(inst->o_box, 1);
	 e_box_align_set(inst->o_box, 0, 0.5);
	 group = eina_stringshare_add("modules/drawer/list/horizontal");
	 break;
     }

   if (!e_theme_edje_object_set(inst->o_con, "base/theme/modules/drawer", group))
     {
	char buf[4096];

	snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_conf->module->dir);
	edje_object_file_set(inst->o_con, buf, group);
     }

   if (group) eina_stringshare_del(group);
   edje_object_part_swallow(inst->o_con, "e.swallow.content", inst->o_box);
   evas_object_show(inst->o_con);

   evas_object_event_callback_add(inst->o_con, EVAS_CALLBACK_MOUSE_MOVE,
	 _list_cb_list_mouse_move, inst);
}

static Entry *
_list_horizontal_entry_create(Instance *inst, Drawer_Source_Item *si)
{
   Entry *e;
   Evas_Coord w, h;

   e = E_NEW(Entry, 1);

   e->o_holder = edje_object_add(inst->evas);
   if (!e_theme_edje_object_set(e->o_holder, "base/theme/modules/drawer",
	 "modules/drawer/list/horizontal_entry"))
     edje_object_file_set(e->o_holder, inst->theme_file,
	   "modules/drawer/list/horizontal_entry");

   edje_object_part_geometry_get(e->o_holder, "e.swallow.content", NULL, NULL, &w, &h);
   e->o_icon = drawer_util_icon_create(si, inst->evas, w, h);

   edje_object_part_swallow(e->o_holder, "e.swallow.content", e->o_icon);
   evas_object_pass_events_set(e->o_icon, 1);
   evas_object_show(e->o_icon);

   e->inst = inst;
   e->si = si;

   edje_object_signal_callback_add(e->o_holder, "e,action,select", "drawer", 
	 _list_entry_select_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,activate", "drawer", 
	 _list_entry_activate_cb, e);

   return e;
}

static Entry *
_list_vertical_entry_create(Instance *inst, Drawer_Source_Item *si)
{
   Entry *e;
   Evas_Coord w, h;

   e = E_NEW(Entry, 1);

   e->o_holder = edje_object_add(inst->evas);
   if (!e_theme_edje_object_set(e->o_holder, "base/theme/modules/drawer",
	 "modules/drawer/list/vertical_entry"))
     edje_object_file_set(e->o_holder, inst->theme_file, "modules/drawer/list/vertical_entry");

   edje_object_part_geometry_get(e->o_holder, "e.swallow.content", NULL, NULL, &w, &h);
   e->o_icon = drawer_util_icon_create(si, inst->evas, w, h);

   edje_object_part_swallow(e->o_holder, "e.swallow.content", e->o_icon);
   evas_object_pass_events_set(e->o_icon, 1);
   evas_object_show(e->o_icon);

   edje_object_part_text_set(e->o_holder, "e.text.label", si->label);
   edje_object_part_text_set(e->o_holder, "e.text.description", si->description);

   e->inst = inst;
   e->si = si;

   edje_object_signal_callback_add(e->o_holder, "e,action,select", "drawer", 
	 _list_entry_select_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,activate", "drawer", 
	 _list_entry_activate_cb, e);

   return e;
}

static void
_list_item_pack_options(Instance *inst, Entry *e)
{
   Evas_Coord w, h, mw, mh;

   w = h = 0;
   edje_object_size_min_calc(e->o_holder, &w, &h);
   edje_object_size_max_get(e->o_holder, &mw, &mh);
   if (e_box_orientation_get(inst->o_box) == 1)
     e_box_pack_options_set(e->o_holder,
			    1, 1, /* fill */
			    1, 1, /* expand */
			    0.5, 0.5, /* align */
			    w, h, /* min */
			    mw, mh /* max */
			    );
   else
     e_box_pack_options_set(e->o_holder,
			    1, 1, /* fill */
			    1, 1, /* expand */
			    0.5, 0.5, /* align */
			    w, h, /* min */
			    mw, mh /* max */
			    );
   evas_object_show(e->o_holder);
}

static void
_list_autoscroll_update(Instance *inst, Evas_Coord x, Evas_Coord y)
{
   Evas_Coord w, h;
   double d;
   
   evas_object_geometry_get(inst->o_box, NULL, NULL, &w, &h);
   if (e_box_orientation_get(inst->o_box))
     {
	if (w > 1) d = (double)x / (double)(w - 1);
	else d = 0;
     }
   else
     {
	if (h > 1) d = (double)y / (double)(h - 1);
	else d = 0;
     }
   if (d < 0.0) d = 0.0;
   else if (d > 1.0) d = 1.0;
   if (!inst->scroll_timer)
     inst->scroll_timer = 
     ecore_timer_add(0.01, _list_scroll_timer, inst);
   if (!inst->scroll_animator)
     inst->scroll_animator = 
     ecore_animator_add(_list_scroll_animator, inst);
   inst->scroll_wanted = d;
}

static int
_list_scroll_timer(void *data)
{
   Instance *inst = NULL;
   double d, v;
   
   inst = data;
   d = inst->scroll_wanted - inst->scroll_pos;
   if (d < 0) d = -d;
   if (d < 0.001)
     {
	inst->scroll_pos =  inst->scroll_wanted;
	inst->scroll_timer = NULL;
	return 0;
     }
   v = 0.05;
   inst->scroll_pos = (inst->scroll_pos * (1.0 - v)) + (inst->scroll_wanted * v);
   return 1;
}

static int
_list_scroll_animator(void *data)
{
   Instance *inst = NULL;
   
   inst = data;
   if (e_box_orientation_get(inst->o_box))
     e_box_align_set(inst->o_box, 1.0 - inst->scroll_pos, 0.5);
   else
     e_box_align_set(inst->o_box, 0.5, 1.0 - inst->scroll_pos);
   if (!inst->scroll_timer)
     {
	inst->scroll_animator = NULL;
	return 0;
     }

   /* Have scroll_cb func if d&d is ever implemented. See e_gadcon.c */

   return 1;
}

static void
_list_cb_list_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst = NULL;
   Evas_Event_Mouse_Move *ev;
   Evas_Coord x, y;
   
   ev = event_info;
   inst = data;
   evas_object_geometry_get(inst->o_box, &x, &y, NULL, NULL);
   _list_autoscroll_update(inst, ev->cur.output.x - x, 
	 ev->cur.output.y - y);
}

static void
_list_entry_select_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Entry *e = NULL;
   Instance *inst = NULL;
   Drawer_Source_Item *si = NULL;

   e = data;
   inst = e->inst;
   si = e->si;
   edje_object_part_text_set(inst->o_con, "e.text.label", si->label);
   edje_object_part_text_set(inst->o_con, "e.text.description", si->description);
}

static void
_list_entry_activate_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Entry *e = NULL;
   Drawer_Event_View_Activate *ev;

   e = data;
   ev = E_NEW(Drawer_Event_View_Activate, 1);
   ev->data = e->si;
   ev->view = e->inst->view;
   ecore_event_add(DRAWER_EVENT_VIEW_ITEM_ACTIVATE, ev, _list_event_activate_free, NULL);

   /* XXX: this doesn't seem to work */
   edje_object_signal_emit(e->inst->o_con, "e,action,activate", "drawer");
}

static void
_list_event_activate_free(void *data __UNUSED__, void *event)
{
   free(event);
}
