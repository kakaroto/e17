/**
 * systray implementation following freedesktop.org specification.
 *
 * @see: http://standards.freedesktop.org/systemtray-spec/latest/
 *
 * @todo: fix SelectionClear events, not getting them :-(
 *
 * @todo: implement xembed.
 *        http://standards.freedesktop.org/xembed-spec/latest/
 *
 * @todo: implement messages/popup part of the spec (anyone using this at all?)
 */

#include <e.h>
#include <X11/Xlib.h>
#include "e_mod_main.h"

#define RETRY_TIMEOUT 2.0

#define SYSTEM_TRAY_REQUEST_DOCK 0
#define SYSTEM_TRAY_BEGIN_MESSAGE 1
#define SYSTEM_TRAY_CANCEL_MESSAGE 2
#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0
#define _NET_SYSTEM_TRAY_ORIENTATION_VERT 1

typedef struct _Instance Instance;
typedef struct _Icon Icon;

struct _Icon
{
   Ecore_X_Window win;
   Evas_Object *o;
   Instance *inst;
};

struct _Instance
{
   E_Gadcon_Client *gcc;
   E_Container *con;
   Evas *evas;
   struct
   {
      Ecore_X_Window parent;
      Ecore_X_Window selection;
   } win;
   struct
   {
      Evas_Object *gadget;
   } ui;
   struct
   {
      Ecore_Event_Handler *message;
      Ecore_Event_Handler *destroy;
      Ecore_Event_Handler *sel_clear;
   } handler;
   struct
   {
      Ecore_Timer *retry;
   } timer;
   Eina_List *icons;
   E_Menu *menu;
};

static const char _Name[] = "Systray";
static const char _name[] = "systray";
static const char _group_gadget[] = "e/modules/systray/main";
static const char _part_box[] = "e.box";
static const char _part_size[] = "e.size";
static const char _sig_source[] = "e";
static const char _sig_enable[] = "e,action,enable";
static const char _sig_disable[] = "e,action,disable";

static Ecore_X_Atom _atom_manager = 0;
static Ecore_X_Atom _atom_st_orient = 0;
static Ecore_X_Atom _atom_st_visual = 0;
static Ecore_X_Atom _atom_st_op_code = 0;
static Ecore_X_Atom _atom_st_msg_data = 0;
static Ecore_X_Atom _atom_xembed = 0;
static Ecore_X_Atom _atom_xembed_info = 0;

static E_Module *systray_mod = NULL;
static Instance *instance = NULL; /* only one systray ever possible */
static char tmpbuf[PATH_MAX]; /* general purpose buffer, just use immediately */

static const char *
_systray_theme_path(void)
{
#define TF "/e-module-systray.edj"
   int dirlen;
   const char *moddir = e_module_dir_get(systray_mod);

   dirlen = strlen(moddir);
   if (dirlen >= sizeof(tmpbuf) - sizeof(TF))
     return NULL;

   memcpy(tmpbuf, moddir, dirlen);
   memcpy(tmpbuf + dirlen, TF, sizeof(TF));

   return tmpbuf;
#undef TF
}

static void
_systray_menu_cb_post(void *data, E_Menu *menu)
{
   Instance *inst = data;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

static void
_systray_menu_new(Instance *inst, Evas_Event_Mouse_Down *ev)
{
   E_Zone *zone;
   E_Menu *mn;
   int x, y;

   zone = e_util_zone_current_get(e_manager_current_get());

   mn = e_menu_new();
   e_menu_post_deactivate_callback_set(mn, _systray_menu_cb_post, inst);
   inst->menu = mn;

   e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, NULL, NULL);
   e_menu_activate_mouse(mn, zone, x + ev->output.x, y + ev->output.y,
			 1, 1, E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
   evas_event_feed_mouse_up(inst->evas, ev->button,
			    EVAS_BUTTON_NONE, ev->timestamp, NULL);
}

static void
_systray_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event)
{
   Instance *inst = data;
   Evas_Event_Mouse_Down *ev = event;

   if ((ev->button == 3) && (!inst->menu))
     _systray_menu_new(inst, ev);
}

static void
_systray_size_apply(Instance *inst)
{
   Evas_Coord w, h;
   edje_object_message_signal_process(inst->ui.gadget);
   edje_object_size_min_calc(inst->ui.gadget, &w, &h);
   e_gadcon_client_aspect_set(inst->gcc, w, h);
   e_gadcon_client_min_size_set(inst->gcc, w, h);
}

static void
_systray_icon_cb_move(void *data, Evas *evas, Evas_Object *o, void *event)
{
   Icon *icon = data;
   Evas_Coord x, y;
   evas_object_geometry_get(o, &x, &y, NULL, NULL);
   ecore_x_window_move(icon->win, x, y);
}

static void
_systray_icon_cb_resize(void *data, Evas *evas, Evas_Object *o, void *event)
{
   Icon *icon = data;
   Evas_Coord w, h;
   evas_object_geometry_get(o, NULL, NULL, &w, &h);
   ecore_x_window_resize(icon->win, w, h);
}

static Icon *
_systray_icon_add(Instance *inst, const Ecore_X_Window win)
{
   Evas_Object *o;
   Evas_Coord x, y, w, h;
   Icon *icon;

   edje_object_part_geometry_get(inst->ui.gadget, _part_size,
				 NULL, NULL, &w, &h);
   if (w > h)
     w = h;
   else
     h = w;

   o = evas_object_rectangle_add(inst->evas);
   if (!o)
     return NULL;
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_resize(o, w, h);
   evas_object_show(o);

   icon = malloc(sizeof(*icon));
   if (!icon)
     {
	evas_object_del(o);
	return NULL;
     }
   icon->win = win;
   icon->inst = inst;
   icon->o = o;

   ecore_x_window_resize(win, w, h);
   ecore_x_window_reparent(win, inst->win.parent, 0, 0);
   ecore_x_window_raise(win);
   ecore_x_window_client_manage(win);
   ecore_x_window_save_set_add(win);
   ecore_x_window_shape_events_select(win, 1);
   ecore_x_window_show(win);

   evas_object_event_callback_add
     (o, EVAS_CALLBACK_MOVE, _systray_icon_cb_move, icon);
   evas_object_event_callback_add
     (o, EVAS_CALLBACK_RESIZE, _systray_icon_cb_resize, icon);

   inst->icons = eina_list_append(inst->icons, icon);
   edje_object_part_box_append(inst->ui.gadget, _part_box, o);
   _systray_size_apply(inst);

   return icon;
}

static void
_systray_icon_del_list(Instance *inst, Eina_List *l, Icon *icon)
{
   inst->icons = eina_list_remove_list(inst->icons, l);

   ecore_x_window_save_set_del(icon->win);
   ecore_x_window_reparent(icon->win, inst->con->manager->root, 0, 0);
   evas_object_del(icon->o);
   free(icon);

   _systray_size_apply(inst);
}

static void
_systray_icon_del(Icon *icon)
{
   Eina_List *l;

   if (!icon)
     return;

   l = eina_list_data_find_list(icon->inst->icons, icon);
   if (l)
     _systray_icon_del_list(icon->inst, l, icon);
}

/* XXX TODO: should be in ecore_x */
static Eina_Bool
_systray_selection_owner_set(int screen_num, Ecore_X_Window win, Ecore_X_Window root)
{
   static Ecore_X_Atom atom;
   static int last_num = -1;
   Ecore_X_Display *disp = ecore_x_display_get();
   Ecore_X_Window cur_selection;
   Ecore_X_Time time;
   Eina_Bool ret;

   if ((last_num == -1) || (screen_num != last_num))
     {
	char buf[32];
	snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", screen_num);
	atom = ecore_x_atom_get(buf);
	last_num = screen_num;
     }

   time = ecore_x_current_time_get();
   XSetSelectionOwner(disp, atom, win, time);
   cur_selection = XGetSelectionOwner(disp, atom);

   ret = (cur_selection == win);
   if (ret && (win != None))
     {
	time = ecore_x_current_time_get();
	ecore_x_client_message32_send(root, _atom_manager,
				      ECORE_X_EVENT_MASK_WINDOW_CONFIGURE,
				      time, atom, win, 0, 0);
     }
   else if (!ret)
     fprintf(stderr, "SYSTRAY: tried to set selection to %#x, but got %#x\n",
	     win, cur_selection);

   return ret;
}

static Eina_Bool
_systray_selection_owner_set_current(Instance *inst)
{
   return _systray_selection_owner_set
     (inst->con->manager->num, inst->win.selection, inst->con->manager->root);
}

static void
_systray_deactivate(Instance *inst)
{
   Ecore_X_Window old;
   printf("XXX SYSTRAY: deactivate win.selection: %#x\n", inst->win.selection);
   if (inst->win.selection == None) return;

   edje_object_signal_emit(inst->ui.gadget, _sig_disable, _sig_source);

   while (inst->icons)
     _systray_icon_del_list(inst, inst->icons, inst->icons->data);

   old = inst->win.selection;
   inst->win.selection = None;
   _systray_selection_owner_set_current(inst);
   ecore_x_window_del(old);
   fprintf(stderr, "XXX SYSTRAY: deactivated!\n");
}

static Eina_Bool
_systray_activate(Instance *inst)
{
   Ecore_X_Time time;
   unsigned int visual;

   printf("XXX SYSTRAY: activate win.selection: %#x\n", inst->win.selection);
   if (inst->win.selection != None) return;
   inst->win.selection = ecore_x_window_input_new(inst->win.parent, 0, 0, 1, 1);
   if (inst->win.selection == None) return;

   if (!_systray_selection_owner_set_current(inst))
     {
	ecore_x_window_del(inst->win.selection);
	inst->win.selection = None;
	edje_object_signal_emit(inst->ui.gadget, _sig_disable, _sig_source);
	return 0;
     }

   visual = 32; // XXX TODO: detect based on ecore_evas engine
   ecore_x_window_prop_card32_set
     (inst->win.selection, _atom_st_visual, &visual, 1);

   edje_object_signal_emit(inst->ui.gadget, _sig_enable, _sig_source);

   return 1;
}

static int
_systray_activate_retry(void *data)
{
   Instance *inst = data;
   Eina_Bool ret;

   fputs("SYSTRAY: reactivate...\n", stderr);
   ret = _systray_activate(inst);
   if (ret)
     fputs("SYSTRAY: activate success!\n", stderr);
   else
     fprintf(stderr, "SYSTRAY: activate failure! retrying in %0.1f seconds\n",
	     RETRY_TIMEOUT);

   if (!ret)
     return 1;

   inst->timer.retry = NULL;
   return 0;
}

static void
_systray_retry(Instance *inst)
{
   if (inst->timer.retry) return;
   inst->timer.retry = ecore_timer_add
     (RETRY_TIMEOUT, _systray_activate_retry, inst);
}

static void
_systray_handle_request_dock(Instance *inst, Ecore_X_Event_Client_Message *ev)
{
   Ecore_X_Window win = (Ecore_X_Window)ev->data.l[2];
   const Eina_List *l;
   Icon *icon;

   EINA_LIST_FOREACH(inst->icons, l, icon)
     if (icon->win == win)
       return;

   icon = _systray_icon_add(inst, win);
   if (!icon)
     return;

   // XXX: request xembed
}

static void
_systray_handle_op_code(Instance *inst, Ecore_X_Event_Client_Message *ev)
{
   long message = ev->data.l[1];

   switch (message)
     {
      case SYSTEM_TRAY_REQUEST_DOCK:
	 _systray_handle_request_dock(inst, ev);
	 break;
      case SYSTEM_TRAY_BEGIN_MESSAGE:
      case SYSTEM_TRAY_CANCEL_MESSAGE:
	 fprintf(stderr,
		 "XXX SYSTRAY TODO: handle messages (anyone uses this?)\n");
	 break;
      default:
	 fprintf(stderr, "SYSTRAY: error, unknown message op code: %ld\n",
		 message);
     }
}

static void
_systray_handle_message(Instance *inst, Ecore_X_Event_Client_Message *ev)
{
   fprintf(stderr, "XXX SYSTRAY TODO: op: %ld, data: %ld, %ld, %ld\n",
	   ev->data.l[1], ev->data.l[2], ev->data.l[3], ev->data.l[4]);
}

static void
_systray_handle_xembed(Instance *inst, Ecore_X_Event_Client_Message *ev)
{
   fprintf(stderr, "XXX SYSTRAY TODO: xembed\n");
}

static int
_systray_cb_client_message(void *data, int type, void *event)
{
   Ecore_X_Event_Client_Message *ev = event;
   Instance *inst = data;

   if (ev->message_type == _atom_st_op_code)
     _systray_handle_op_code(inst, ev);
   else if (ev->message_type == _atom_st_msg_data)
     _systray_handle_message(inst, ev);
   else if (ev->message_type == _atom_xembed)
     _systray_handle_xembed(inst, ev);

   return 1;
}

static int
_systray_cb_window_destroy(void *data, int type, void *event)
{
   Ecore_X_Event_Window_Destroy *ev = event;
   Instance *inst = data;
   Icon *icon;
   Eina_List *l;

   EINA_LIST_FOREACH(inst->icons, l, icon)
     if (icon->win == ev->win)
       {
	  _systray_icon_del_list(inst, l, icon);
	  return;
       }
}

static int
_systray_cb_selection_clear(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Clear *ev = event;
   Instance *inst = data;
   fprintf(stderr, "XXX SYSTRAY clear %#x\n", ev->win);
   if (ev->win == inst->win.selection)
     {
	_systray_deactivate(inst);
	_systray_retry(inst);
     }
   return 1;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance *inst;

   if (!systray_mod)
     return NULL;
   if ((!id) || (instance))
     {
	e_alert_show
	  (D_("There can be only one systray gadget and "
	      "another one already exists."));
	return NULL;
     }

   inst = E_NEW(Instance, 1);
   if (!inst)
     return NULL;
   inst->evas = gc->evas;
   inst->con = e_container_current_get(e_manager_current_get());
   if (!inst->con)
     {
	E_FREE(inst);
	return NULL;
     }
   if ((gc->shelf) && (gc->shelf->popup))
     inst->win.parent = gc->shelf->popup->evas_win;
   else
     inst->win.parent = inst->con->bg_win;

   inst->win.selection = None;

   inst->ui.gadget = edje_object_add(inst->evas);
   if (!e_theme_edje_object_set(inst->ui.gadget, "base/theme/modules/systray",
				_group_gadget))
     edje_object_file_set(inst->ui.gadget, _systray_theme_path(),
			  _group_gadget);

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->ui.gadget);
   if (!inst->gcc)
     {
	evas_object_del(inst->ui.gadget);
	E_FREE(inst);
	return NULL;
     }

   inst->gcc->data = inst;

   if (!_systray_activate(inst))
     _systray_retry(inst);

   evas_object_event_callback_add(inst->ui.gadget, EVAS_CALLBACK_MOUSE_DOWN,
				  _systray_cb_mouse_down, inst);

   inst->handler.message = ecore_event_handler_add
     (ECORE_X_EVENT_CLIENT_MESSAGE, _systray_cb_client_message, inst);
   inst->handler.destroy = ecore_event_handler_add
     (ECORE_X_EVENT_WINDOW_DESTROY, _systray_cb_window_destroy, inst);
   inst->handler.sel_clear = ecore_event_handler_add
     (ECORE_X_EVENT_SELECTION_CLEAR, _systray_cb_selection_clear, inst);

   instance = inst;
   return inst->gcc;
}

/* Called when Gadget_Container says stop */
static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst = gcc->data;

   if (!inst)
     return;

   if (inst->menu)
     {
	e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
	e_object_del(E_OBJECT(inst->menu));
     }

   _systray_deactivate(inst);
   evas_object_del(inst->ui.gadget);

   if (inst->handler.message)
     ecore_event_handler_del(inst->handler.message);
   if (inst->handler.destroy)
     ecore_event_handler_del(inst->handler.destroy);
   if (inst->handler.sel_clear)
     ecore_event_handler_del(inst->handler.sel_clear);
   if (inst->timer.retry)
     ecore_timer_del(inst->timer.retry);

   if (instance == inst)
     instance = NULL;

   E_FREE(inst);
   gcc->data = NULL;
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   Instance *inst = gcc->data;
   const char *signal;
   Evas_Coord w, h;
   unsigned int systray_orient;

   if (!inst)
     return;

   switch (orient)
     {
      case E_GADCON_ORIENT_FLOAT:
	 signal = "e,action,orient,float";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_HORIZ:
	 signal = "e,action,orient,horiz";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_VERT:
	 signal = "e,action,orient,vert";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_VERT;
	 break;
      case E_GADCON_ORIENT_LEFT:
	 signal = "e,action,orient,left";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_VERT;
	 break;
      case E_GADCON_ORIENT_RIGHT:
	 signal = "e,action,orient,right";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_VERT;
	 break;
      case E_GADCON_ORIENT_TOP:
	 signal = "e,action,orient,top";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_BOTTOM:
	 signal = "e,action,orient,bottom";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_TL:
	 signal = "e,action,orient,corner_tl";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_TR:
	 signal = "e,action,orient,corner_tr";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_BL:
	 signal = "e,action,orient,corner_bl";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_BR:
	 signal = "e,action,orient,corner_br";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_LT:
	 signal = "e,action,orient,corner_lt";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_RT:
	 signal = "e,action,orient,corner_rt";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_LB:
	 signal = "e,action,orient,corner_lb";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      case E_GADCON_ORIENT_CORNER_RB:
	 signal = "e,action,orient,corner_rb";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
	 break;
      default:
	 signal = "e,action,orient,horiz";
	 systray_orient = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
     }

   ecore_x_window_prop_card32_set
     (inst->win.selection, _atom_st_orient, &systray_orient, 1);

   edje_object_signal_emit(inst->ui.gadget, signal, _sig_source);
   edje_object_message_signal_process(inst->ui.gadget);
   _systray_size_apply(inst);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_(_Name);
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o;

   o = edje_object_add(evas);
   edje_object_file_set(o, _systray_theme_path(), "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   if (!instance)
     return _name;
   else
     return NULL;
}

static const E_Gadcon_Client_Class _gc_class =
{
  GADCON_CLIENT_CLASS_VERSION, _name,
  {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
  E_GADCON_CLIENT_STYLE_INSET
};

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, _Name};

EAPI void *
e_modapi_init(E_Module *m)
{
   systray_mod = m;

   if (snprintf(tmpbuf, sizeof(tmpbuf), "%s/locale", e_module_dir_get(m)) >= sizeof(tmpbuf))
     return NULL;
   bindtextdomain(PACKAGE, tmpbuf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   e_gadcon_provider_register(&_gc_class);

   if (!_atom_manager)
     _atom_manager = ecore_x_atom_get("MANAGER");
   if (!_atom_st_orient)
     _atom_st_orient = ecore_x_atom_get("_NET_SYSTEM_TRAY_ORIENTATION");
   if (!_atom_st_visual)
     _atom_st_visual = ecore_x_atom_get("_NET_SYSTEM_TRAY_VISUAL");
   if (!_atom_st_op_code)
     _atom_st_op_code = ecore_x_atom_get("_NET_SYSTEM_TRAY_OPCODE");
   if (!_atom_st_msg_data)
     _atom_st_msg_data = ecore_x_atom_get("_NET_SYSTEM_TRAY_MESSAGE_DATA");
   if (!_atom_xembed)
     _atom_xembed = ecore_x_atom_get("_XEMBED");
   if (!_atom_xembed_info)
     _atom_xembed_info = ecore_x_atom_get("_XEMBED_INFO");

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_gadcon_provider_unregister(&_gc_class);
   systray_mod = NULL;
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}
