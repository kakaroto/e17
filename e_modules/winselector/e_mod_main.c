/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"

/* gadcon requirements */
static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc);
static char *_gc_label (void);
static Evas_Object *_gc_icon (Evas * evas);

/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "winselector",
  {
   _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon}
};

/* actual module specifics */
typedef struct _Instance Instance;
struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object     *o_button;
  Evas_Object     *bd_icon;
  E_Menu          *win_menu;
};

static void _button_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
      void *event_info);
static void _menu_cb_post(void *data, E_Menu *m);
static E_Menu * _win_menu_new(Instance *inst);
static void _win_menu_pre_cb(void *data, E_Menu *m);
static void _win_menu_item_cb(void *data, E_Menu *m, E_Menu_Item *mi);
static void _win_menu_free_hook(void *obj);
static void _win_menu_item_create(E_Border *bd, E_Menu *m, Instance *inst);
static int _window_cb_focus_in(void *data, int type, void *event);
static int _window_cb_focus_out(void *data, int type, void *event);
static int _window_cb_icon_change(void *data, int type, void *event);
static void _focus_in(E_Border *bd, Instance *inst);
static void _focus_out(Instance *inst);

static E_Module *winsel_module = NULL;
Evas_List *handlers;

static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{
  Evas_Object *o;
  E_Gadcon_Client *gcc;
  Instance *inst;
  char buf[4096];

  inst = E_NEW (Instance, 1);

  snprintf (buf, sizeof (buf), "%s/winselector.edj", e_module_dir_get
	(winsel_module));
  o = edje_object_add (gc->evas);
  if (!e_theme_edje_object_set (o, "base/theme/modules/winselector",
	   "modules/winselector/main"))
    edje_object_file_set (o, buf, "modules/winselector/main");
  evas_object_show (o);
  edje_object_signal_emit (o, "passive", "");

  gcc = e_gadcon_client_new (gc, name, id, style, o);
  gcc->data = inst;

  inst->gcc = gcc;
  inst->o_button = o;

  e_gadcon_client_util_menu_attach (gcc);

  handlers = evas_list_append (handlers, ecore_event_handler_add
	(ECORE_X_EVENT_WINDOW_FOCUS_IN, _window_cb_focus_in, inst));
  handlers = evas_list_append (handlers, ecore_event_handler_add
	(ECORE_X_EVENT_WINDOW_FOCUS_OUT, _window_cb_focus_out, inst));
  handlers = evas_list_append (handlers, ecore_event_handler_add
	(E_EVENT_BORDER_ICON_CHANGE, _window_cb_icon_change, inst));
  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
	_button_cb_mouse_down, inst);
  return gcc;
}

   static void
_gc_shutdown (E_Gadcon_Client * gcc)
{
   Instance *inst;

  inst = gcc->data;
   if (inst->win_menu)
     {
	e_menu_post_deactivate_callback_set(inst->win_menu, NULL, NULL);
	e_object_del(E_OBJECT(inst->win_menu));
	inst->win_menu = NULL;
     }
  evas_object_del (inst->o_button);
  free (inst);
}

static void
_gc_orient (E_Gadcon_Client * gcc)
{
  e_gadcon_client_aspect_set (gcc, 16, 16);
  e_gadcon_client_min_size_set (gcc, 16, 16);
}

static char *
_gc_label (void)
{
  return _("Winselector");
}

static Evas_Object *
_gc_icon (Evas * evas)
{
  Evas_Object *o;
  char buf[4096];

  o = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/module.eap",
	    e_module_dir_get (winsel_module));
  edje_object_file_set (o, buf, "icon");
  return o;
}

static void
_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   
   inst = data;
   ev = event_info;
   if (ev->button == 1)
     {
	Evas_Coord x, y, w, h;
	int cx, cy, cw, ch;
	
	evas_object_geometry_get(inst->o_button, &x, &y, &w, &h); 
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
	      &cx, &cy, &cw, &ch);
	x += cx;
	y += cy;
	if (!inst->win_menu)
	  inst->win_menu = _win_menu_new(inst);
	if (inst->win_menu)
	  {
	     int dir;
	     
	     e_menu_post_deactivate_callback_set(inst->win_menu, _menu_cb_post,
		   inst);
	     dir = E_MENU_POP_DIRECTION_AUTO;
	     switch (inst->gcc->gadcon->orient)
	       {
		case E_GADCON_ORIENT_TOP:
		  dir = E_MENU_POP_DIRECTION_DOWN;
		  break;
		case E_GADCON_ORIENT_BOTTOM:
		  dir = E_MENU_POP_DIRECTION_UP;
		  break;
		case E_GADCON_ORIENT_LEFT:
		  dir = E_MENU_POP_DIRECTION_RIGHT;
		  break;
		case E_GADCON_ORIENT_RIGHT:
		  dir = E_MENU_POP_DIRECTION_LEFT;
		  break;
		case E_GADCON_ORIENT_CORNER_TL:
		  dir = E_MENU_POP_DIRECTION_DOWN;
		  break;
		case E_GADCON_ORIENT_CORNER_TR:
		  dir = E_MENU_POP_DIRECTION_DOWN;
		  break;
		case E_GADCON_ORIENT_CORNER_BL:
		  dir = E_MENU_POP_DIRECTION_UP;
		  break;
		case E_GADCON_ORIENT_CORNER_BR:
		  dir = E_MENU_POP_DIRECTION_UP;
		  break;
		case E_GADCON_ORIENT_CORNER_LT:
		  dir = E_MENU_POP_DIRECTION_RIGHT;
		  break;
		case E_GADCON_ORIENT_CORNER_RT:
		  dir = E_MENU_POP_DIRECTION_LEFT;
		  break;
		case E_GADCON_ORIENT_CORNER_LB:
		  dir = E_MENU_POP_DIRECTION_RIGHT;
		  break;
		case E_GADCON_ORIENT_CORNER_RB:
		  dir = E_MENU_POP_DIRECTION_LEFT;
		  break;
		case E_GADCON_ORIENT_FLOAT:
		case E_GADCON_ORIENT_HORIZ:
		case E_GADCON_ORIENT_VERT:
		default:
		  dir = E_MENU_POP_DIRECTION_AUTO;
		  break;
	       }
	     e_menu_activate_mouse(inst->win_menu,
		   e_util_zone_current_get(e_manager_current_get()),
		   x, y, w, h, dir, ev->timestamp);
	     edje_object_signal_emit(inst->o_button, "active", "");
	     evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
		   EVAS_BUTTON_NONE, ev->timestamp, NULL);
	  }
     }
}

static E_Menu *
_win_menu_new(Instance *inst)
{
   E_Menu *m;

   m = e_menu_new();
   e_menu_pre_activate_callback_set(m, _win_menu_pre_cb, inst);
   return m;
}

static void
_win_menu_pre_cb(void *data, E_Menu *m)
{
   E_Menu_Item *mi;
   Evas_List *l, *borders = NULL, *alt = NULL;
   E_Zone *zone = NULL;
   E_Desk *desk = NULL;
   Instance *inst;

   inst = data;
   e_menu_pre_activate_callback_set(m, NULL, NULL);
   /* get the current clients */
   zone = e_util_zone_current_get (e_manager_current_get ());
   desk = e_desk_current_get(zone);
   for (l = e_border_client_list(); l; l = l->next)
     {
	E_Border *border;

	border = l->data;
	if (border->client.netwm.state.skip_taskbar) continue;
	if (border->user_skip_winlist) continue;
	if ((border->zone == zone) || (border->iconic))
	  borders = evas_list_append(borders, border);
     }

   if (!borders)
     { 
	/* FIXME here we want nothing, but that crashes!!! */
	mi = e_menu_item_new(m);
	e_menu_item_label_set(mi, _("(No Windows)"));
	return;
     }
   for (l = borders; l; l = l->next)
     {
	E_Border *bd = l->data;

	if (inst->gcc->gadcon->orient == E_GADCON_ORIENT_BOTTOM 
	      || inst->gcc->gadcon->orient == E_GADCON_ORIENT_CORNER_BL
	      || inst->gcc->gadcon->orient == E_GADCON_ORIENT_CORNER_BR
	      || inst->gcc->gadcon->orient == E_GADCON_ORIENT_CORNER_RB
	      || inst->gcc->gadcon->orient == E_GADCON_ORIENT_CORNER_LB)
	  {
	     if (bd->desk == desk)
	       {
		  alt = evas_list_append(alt, bd);
		  continue;
	       }
	  }
	else 
	  {
	     if (bd->desk != desk)
	       {
		  alt = evas_list_append(alt, bd);
		  continue;
	       }
	  }
	_win_menu_item_create(bd, m, inst);
     }
   mi = e_menu_item_new(m);
   e_menu_item_separator_set(mi, 1);

   for (l = alt; l; l = l->next)
     {
	E_Border *bd = l->data;

	_win_menu_item_create(bd, m, inst);
     }
   e_object_free_attach_func_set(E_OBJECT(m), _win_menu_free_hook);
   e_object_data_set(E_OBJECT(m), borders);
}

static void
_win_menu_item_create(E_Border *bd, E_Menu *m, Instance *inst)
{
   Evas_Object *icon;
   E_Menu_Item *mi;
   const char *title;

   title = e_border_name_get(bd);
   mi = e_menu_item_new(m);
   e_menu_item_check_set(mi, 1);
   if ((title) && (title[0]))
     e_menu_item_label_set(mi, title);
   else
     e_menu_item_label_set(mi, _("No name!!"));
   /* ref the border as we implicitly unref it in the callback */
   e_object_ref(E_OBJECT(bd));
/*   e_object_breadcrumb_add(E_OBJECT(bd), "clients_menu");*/
   e_menu_item_callback_set(mi, _win_menu_item_cb, bd);
   if (!bd->iconic) e_menu_item_toggle_set(mi, 1);
   icon = e_border_icon_add(bd, evas_object_evas_get(inst->o_button));
   e_menu_item_icon_object_set(mi, icon);
}

static void 
_win_menu_item_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;
   
   bd = data;
   E_OBJECT_CHECK(bd);
   if (bd->iconic)
     {
	if (!bd->lock_user_iconify)
	  e_border_uniconify(bd);
     }
   e_desk_show(bd->desk);
   if (!bd->lock_user_stacking)
     e_border_raise(bd);
   if (!bd->lock_focus_out)
     {
	if (e_config->focus_policy != E_FOCUS_CLICK)
	  ecore_x_pointer_warp(bd->zone->container->win,
			       bd->x + (bd->w / 2),
			       bd->y + (bd->h / 2));
	e_border_focus_set(bd, 1, 1);
     }
}

static void
_win_menu_free_hook(void *obj)
{
   E_Menu *m;
   Evas_List *borders;
   
   m = obj;
   borders = e_object_data_get(E_OBJECT(m));
   while (borders)
     {
	E_Border *bd;
	
	bd = borders->data;
	borders = evas_list_remove_list(borders, borders);
//	e_object_breadcrumb_del(E_OBJECT(bd), "clients_menu");
	e_object_unref(E_OBJECT(bd));
     }
}

static void
_menu_cb_post(void *data, E_Menu *m)
{
   Instance *inst;
   
   inst = data;
   if (!inst->win_menu) return;
   edje_object_signal_emit(inst->o_button, "passive", "");
   e_object_del(E_OBJECT(inst->win_menu));
   inst->win_menu = NULL;
}

static int _window_cb_focus_in(void *data, int type, void *event)
{
   Ecore_X_Event_Window_Focus_In *ev;
   E_Border *bd;
   Instance *inst;

   ev = event;
   inst = data;
   if (!(bd = e_border_find_by_client_window(ev->win)))
     return 1;

   _focus_in(bd, inst);
   return 1;
}

static int _window_cb_focus_out(void *data, int type, void *event)
{ 
   Ecore_X_Event_Window_Focus_In *ev;
   E_Border *bd;
   Instance *inst;

   ev = event;
   inst = data;
   if (!(bd = e_border_find_by_client_window(ev->win)))
     return 1;

   _focus_out(inst);

   return 1;
}

static int
_window_cb_icon_change(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   E_Border *bd;
   Instance *inst;

   ev = event;
   inst = data;
   if (!(bd = ev->border)) return 1;

   _focus_in(bd, inst);
   return 1;
}

static void _focus_in(E_Border *bd, Instance *inst)
{
   _focus_out(inst);
   inst->bd_icon = e_border_icon_add(bd, evas_object_evas_get(inst->o_button));
   edje_object_signal_emit(inst->o_button, "focus_in", "");
   edje_object_part_swallow(inst->o_button, "icon", inst->bd_icon);
   evas_object_show(inst->bd_icon);
}

static void _focus_out(Instance *inst)
{
   edje_object_signal_emit(inst->o_button, "focus_out", "");
   if (inst->bd_icon) evas_object_del(inst->bd_icon);
   inst->bd_icon = NULL;

}

/* module setup */
EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Winselector"
};

EAPI void *
e_modapi_init (E_Module * m)
{
   winsel_module = m;
   e_gadcon_provider_register (&_gadcon_class);
   return winsel_module;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
  winsel_module = NULL;
  while (handlers) 
    {
       ecore_event_handler_del(handlers->data);
       handlers = evas_list_remove_list(handlers, handlers);
    }
  e_gadcon_provider_unregister (&_gadcon_class);
  return 1;
}

EAPI int
e_modapi_save (E_Module * m)
{
  return 1;
}

EAPI int
e_modapi_about (E_Module * m)
{
  e_module_dialog_show (m, _("Enlightenment Winselector Module"), 
	_ ("This module provides a menu-based access to all the windows."));
  return 1;
}
