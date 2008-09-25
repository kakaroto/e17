/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"
#include "e_int_config_iiirk.h"

/***************************************************************************/
/**/
/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);
static void _gc_id_del(const char *id);
/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
     "iiirk",
     {
        _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, _gc_id_del
     },
   E_GADCON_CLIENT_STYLE_INSET
};
/**/
/***************************************************************************/

/***************************************************************************/
/**/
/* actual module specifics */

typedef struct _Instance  Instance;

typedef struct _IIirk      IIirk;
typedef struct _IIirk_Icon IIirk_Icon;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object     *o_iiirk;
   IIirk            *iiirk;
   E_Drop_Handler  *drop_handler;
   Config_Item     *ci;
};

struct _IIirk
{
   Instance       *inst;
   Evas_Object    *o_box;
   Evas_Object    *o_drop;
   Evas_Object    *o_drop_over;
   Evas_Object    *o_empty;
   IIirk_Icon     *ic_drop_before;
   int             drop_before;
   Evas_List      *icons;
   E_Zone         *zone;
   Evas_Coord      dnd_x, dnd_y;
   E_Order        *apps;
};

struct _IIirk_Icon
{
   IIirk        *iiirk;
   Evas_Object *o_holder;
   Evas_Object *o_icon;
   Evas_Object *o_holder2;
   Evas_Object *o_icon2;
   E_Border    *border;
   int		skip_winlist;
   int		skip_pager;
   int		skip_taskbar;
   int		mouse_down;
   struct {
      unsigned char  start : 1;
      unsigned char  dnd : 1;
      int            x, y;
      int            dx, dy;
   } drag;
};

static IIirk *_iiirk_new(Evas *evas, E_Zone *zone, Instance *inst);
static void _iiirk_free(IIirk *b);
static void _iiirk_cb_empty_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_empty_handle(IIirk *b);
static void _iiirk_fill(IIirk *b);
static void _iiirk_empty(IIirk *b);
static void _iiirk_orient_set(IIirk *b, int horizontal);
static void _iiirk_resize_handle(IIirk *b);
static void _iiirk_instance_drop_zone_recalc(Instance *inst);
static IIirk_Icon *_iiirk_icon_find(IIirk *b, E_Border *bd);
static IIirk_Icon *_iiirk_icon_at_coord(IIirk *b, Evas_Coord x, Evas_Coord y);
static IIirk_Icon *_iiirk_icon_new(IIirk *b, E_Border *bd);
static void _iiirk_icon_free(IIirk_Icon *ic);
static void _iiirk_icon_fill(IIirk_Icon *ic);
static void _iiirk_icon_fill_label(IIirk_Icon *ic);
static void _iiirk_icon_empty(IIirk_Icon *ic);
static void _iiirk_icon_signal_emit(IIirk_Icon *ic, char *sig, char *src);
static Evas_List *_iiirk_zone_find(E_Zone *zone);
static void _iiirk_cb_app_change(void *data, E_Order *eo);
static void _iiirk_cb_obj_moveresize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_menu_post(void *data, E_Menu *m);
static void _iiirk_cb_menu_configuration(void *data, E_Menu *m, E_Menu_Item *mi);
static void _iiirk_cb_icon_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_icon_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_icon_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_icon_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_icon_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_icon_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_icon_resize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _iiirk_cb_drag_finished(E_Drag *drag, int dropped);
static void _iiirk_inst_cb_enter(void *data, const char *type, void *event_info);
static void _iiirk_inst_cb_move(void *data, const char *type, void *event_info);
static void _iiirk_inst_cb_leave(void *data, const char *type, void *event_info);
static void _iiirk_inst_cb_drop(void *data, const char *type, void *event_info);
static void _iiirk_drop_position_update(Instance *inst, Evas_Coord x, Evas_Coord y);
static void _iiirk_inst_cb_scroll(void *data);
static int _iiirk_cb_event_border_property(void *data, int type, void *event);
static int _iiirk_cb_event_border_add(void *data, int type, void *event);
static int _iiirk_cb_event_border_remove(void *data, int type, void *event);
static int _iiirk_cb_event_border_iconify(void *data, int type, void *event);
static int _iiirk_cb_event_border_uniconify(void *data, int type, void *event);
static int _iiirk_cb_event_border_icon_change(void *data, int type, void *event);
static int _iiirk_cb_event_border_urgent_change(void *data, int type, void *event);
static int _iiirk_cb_event_desk_show(void *data, int type, void *event);
static Config_Item *_iiirk_config_item_get(const char *id);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

static int uuid = 0;

Config *iiirk_config = NULL;

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   IIirk *b;
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   Evas_Coord x, y, w, h;
   const char *drop[] = { "enlightenment/border" };
   Config_Item *ci;

   inst = E_NEW(Instance, 1);

   ci = _iiirk_config_item_get(id);
   inst->ci = ci;
   if (!ci->dir) ci->dir = evas_stringshare_add("default");

   b = _iiirk_new(gc->evas, gc->zone, inst);
   b->inst = inst;
   inst->iiirk = b;
   _iiirk_fill(b);
   o = b->o_box;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_iiirk = o;

   evas_object_geometry_get(o, &x, &y, &w, &h);
   inst->drop_handler =
     e_drop_handler_add(E_OBJECT(inst->gcc), inst,
			_iiirk_inst_cb_enter, _iiirk_inst_cb_move,
			_iiirk_inst_cb_leave, _iiirk_inst_cb_drop,
			drop, 1, x, y, w, h);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOVE,
				  _iiirk_cb_obj_moveresize, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE,
				  _iiirk_cb_obj_moveresize, inst);
   iiirk_config->instances = evas_list_append(iiirk_config->instances, inst);
   _gc_orient(gcc);

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   iiirk_config->instances = evas_list_remove(iiirk_config->instances, inst);
   e_drop_handler_del(inst->drop_handler);
   _iiirk_free(inst->iiirk);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   switch (gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_FLOAT:
      case E_GADCON_ORIENT_HORIZ:
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	_iiirk_orient_set(inst->iiirk, 1);
	e_gadcon_client_aspect_set(gcc, MAX(evas_list_count(inst->iiirk->icons), 1) * 16, 16);
	break;
      case E_GADCON_ORIENT_VERT:
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_RIGHT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_LB:
      case E_GADCON_ORIENT_CORNER_RB:
	_iiirk_orient_set(inst->iiirk, 0);
	e_gadcon_client_aspect_set(gcc, 16, MAX(evas_list_count(inst->iiirk->icons), 1) * 16);
	break;
      default:
	break;
     }
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void)
{
   return D_("IIirk");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-iiirk.edj",
	    e_module_dir_get(iiirk_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void)
{
   Config_Item *ci;

   ci = _iiirk_config_item_get(NULL);
   return ci->id;
}

static void
_gc_id_del(const char *id)
{
   Config_Item *ci;

   ci = _iiirk_config_item_get(id);
   if (ci)
     {
	if (ci->id) evas_stringshare_del(ci->id);
	iiirk_config->items = evas_list_remove(iiirk_config->items, ci);
     }
}

/**/
/***************************************************************************/

void
iiirk_create_default_profile(const char *text)
{
   char buf[4096];
   char tmp[4096];
   FILE *f;
   
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/iiirk/%s", 
	    e_user_homedir_get(), text);

   if (!ecore_file_exists(buf)) 
     {
	ecore_file_mkpath(buf);
	snprintf(buf, sizeof(buf), "%s/.e/e/applications/iiirk/%s/.order", 
		 e_user_homedir_get(), text);
	f = fopen(buf, "w");
	if (f) 
	  {
	     /* Populate this .order file with some defaults */
	     snprintf(tmp, sizeof(tmp), "xterm.desktop\n" "sylpheed.desktop\n" 
		      "firefox.desktop\n" "openoffice.desktop\n" "xchat.desktop\n"
		      "gimp.desktop\n" "xmms.desktop\n");
	     fwrite(tmp, sizeof(char), strlen(tmp), f);
	     fclose(f);
	  }
     }
}

/***************************************************************************/
/**/

static IIirk *
_iiirk_new(Evas *evas, E_Zone *zone, Instance *inst)
{
   IIirk *b;
   char buf[4096];

   b = E_NEW(IIirk, 1);
   b->o_box = e_box_add(evas);
   e_box_homogenous_set(b->o_box, 1);
   e_box_orientation_set(b->o_box, 1);
   e_box_align_set(b->o_box, 0.5, 0.5);
   b->zone = zone;
   if (inst->ci->dir[0] != '/')
     {
	const char *homedir;

	homedir = e_user_homedir_get();
	snprintf(buf, sizeof(buf), "%s/.e/e/applications/iiirk/%s/.order", homedir, inst->ci->dir);
	iiirk_create_default_profile(inst->ci->dir);
     }
   else
     snprintf(buf, sizeof(buf), inst->ci->dir);
   b->apps = e_order_new(buf);
   e_order_update_callback_set(b->apps, _iiirk_cb_app_change, b);
   _iiirk_fill(b);
   return b;
}

static void
_iiirk_free(IIirk *b)
{
   _iiirk_empty(b);
   evas_object_del(b->o_box);
   if (b->o_drop) evas_object_del(b->o_drop);
   if (b->o_drop_over) evas_object_del(b->o_drop_over);
   if (b->o_empty) evas_object_del(b->o_empty);
   e_order_update_callback_set(b->apps, NULL, NULL);
   if (b->apps) e_object_del(E_OBJECT(b->apps));
   free(b);
}

static void
_iiirk_cb_empty_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   IIirk *b;

   ev = event_info;
   b = data;
   if (!iiirk_config->menu)
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int cx, cy, cw, ch;
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _iiirk_cb_menu_post, NULL);
	iiirk_config->menu = mn;

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _iiirk_cb_menu_configuration, b);

	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);
	
	e_gadcon_client_util_menu_items_append(b->inst->gcc, mn, 0);
	
	e_gadcon_canvas_zone_geometry_get(b->inst->gcc->gadcon,
					  &cx, &cy, &cw, &ch);
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      cx + ev->output.x, cy + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
     }
}

static void
_iiirk_empty_handle(IIirk *b)
{
   if (!b->icons)
     {
	if (!b->o_empty)
	  {
	     Evas_Coord w, h;
	
	     b->o_empty = evas_object_rectangle_add(evas_object_evas_get(b->o_box));
	     evas_object_event_callback_add(b->o_empty, EVAS_CALLBACK_MOUSE_DOWN, _iiirk_cb_empty_mouse_down, b);
	     evas_object_color_set(b->o_empty, 0, 0, 0, 0);
	     evas_object_show(b->o_empty);
	     e_box_pack_end(b->o_box, b->o_empty);
	     evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);
	     if (e_box_orientation_get(b->o_box))
	       w = h;
	     else
	       h = w;
	     e_box_pack_options_set(b->o_empty,
				    1, 1, /* fill */
				    1, 1, /* expand */
				    0.5, 0.5, /* align */
				    w, h, /* min */
				    9999, 9999 /* max */
				    );
	  }
     }
   else if (b->o_empty)
     {
	evas_object_del(b->o_empty);
	b->o_empty = NULL;
     }
}

static void
_iiirk_fill(IIirk *b)
{
   if (b && b->apps && b->inst && b->inst->ci)
     {
	Efreet_Desktop *desktop;
	Evas_List *l;

	for (l = b->apps->desktops; l; l = l->next)
	  {
	     E_Border_List *bl;
	     E_Border *bd;
	     int ok;
	     
	     if (!(desktop = l->data)) continue;
	     bl = e_container_border_list_first(b->zone->container);
	     while ((bd = e_container_border_list_next(bl)))
	       {
		  if (!bd->desktop) continue;

		  ok = 0;
		  if ((b->inst->ci->show_zone == 0) && (bd->desktop == desktop))
		    {
		       ok = 1;
		    }
		  else if((b->inst->ci->show_zone == 1) && (bd->desktop == desktop))
		    {
		       if ((b->inst->ci->show_desk == 0) && (bd->zone == b->zone))
			 ok = 1;
		       else if((b->inst->ci->show_desk == 1) && (bd->zone == b->zone) &&
			     (bd->desk == e_desk_current_get(b->zone)))
			 ok = 1;
		    }

		  if (ok)
		    {
		       IIirk_Icon *ic;

		       ic = _iiirk_icon_new(b, bd);
		       b->icons = evas_list_append(b->icons, ic);
		       e_box_pack_end(b->o_box, ic->o_holder);
		    }
	       }
	     e_container_border_list_free(bl);
	  }
     }
   _iiirk_empty_handle(b);
   _iiirk_resize_handle(b);
}

static void
_iiirk_empty(IIirk *b)
{
   while (b->icons)
     {
	_iiirk_icon_free(b->icons->data);
	b->icons = evas_list_remove_list(b->icons, b->icons);
     }
   _iiirk_empty_handle(b);
}

static void
_iiirk_orient_set(IIirk *b, int horizontal)
{
   e_box_orientation_set(b->o_box, horizontal);
   e_box_align_set(b->o_box, 0.5, 0.5);
}

static void
_iiirk_resize_handle(IIirk *b)
{
   Evas_List *l;
   IIirk_Icon *ic;
   Evas_Coord w, h;

   evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);
   if (e_box_orientation_get(b->o_box))
     w = h;
   else
     h = w;
   e_box_freeze(b->o_box);
   for (l = b->icons; l; l = l->next)
     {
	ic = l->data;
	e_box_pack_options_set(ic->o_holder,
			       1, 1, /* fill */
			       0, 0, /* expand */
			       0.5, 0.5, /* align */
			       w, h, /* min */
			       w, h /* max */
			       );
     }
   e_box_thaw(b->o_box);
}

static void
_iiirk_instance_drop_zone_recalc(Instance *inst)
{
   Evas_Coord x, y, w, h;

   evas_object_geometry_get(inst->o_iiirk, &x, &y, &w, &h);
   e_drop_handler_geometry_set(inst->drop_handler, x, y, w, h);
}

static IIirk_Icon *
_iiirk_icon_find(IIirk *b, E_Border *bd)
{
   Evas_List *l;
   IIirk_Icon *ic;

   for (l = b->icons; l; l = l->next)
     {
	ic = l->data;
	
	if (ic->border == bd) return ic;
     }
   return NULL;
}

static IIirk_Icon *
_iiirk_icon_at_coord(IIirk *b, Evas_Coord x, Evas_Coord y)
{
   Evas_List *l;
   IIirk_Icon *ic;

   for (l = b->icons; l; l = l->next)
     {
        Evas_Coord dx, dy, dw, dh;
	ic = l->data;
	
        evas_object_geometry_get(ic->o_holder, &dx, &dy, &dw, &dh);
	if (E_INSIDE(x, y, dx, dy, dw, dh)) return ic;
     }
   return NULL;
}

static IIirk_Icon *
_iiirk_icon_new(IIirk *b, E_Border *bd)
{
   IIirk_Icon *ic;

   ic = E_NEW(IIirk_Icon, 1);
   e_object_ref(E_OBJECT(bd));
   ic->iiirk = b;
   ic->border = bd;
   ic->o_holder = edje_object_add(evas_object_evas_get(b->o_box));
   e_theme_edje_object_set(ic->o_holder, "base/theme/modules/ibox",
			   "e/modules/ibox/icon");
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_IN,  _iiirk_cb_icon_mouse_in,  ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_OUT, _iiirk_cb_icon_mouse_out, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_DOWN, _iiirk_cb_icon_mouse_down, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_UP, _iiirk_cb_icon_mouse_up, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_MOVE, _iiirk_cb_icon_mouse_move, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOVE, _iiirk_cb_icon_move, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_RESIZE, _iiirk_cb_icon_resize, ic);
   evas_object_show(ic->o_holder);

   ic->o_holder2 = edje_object_add(evas_object_evas_get(b->o_box));
   e_theme_edje_object_set(ic->o_holder2, "base/theme/modules/ibox",
			   "e/modules/ibox/icon_overlay");
   evas_object_layer_set(ic->o_holder2, 9999);
   evas_object_pass_events_set(ic->o_holder2, 1);
   evas_object_show(ic->o_holder2);

   _iiirk_icon_fill(ic);
   return ic;
}

static void
_iiirk_icon_free(IIirk_Icon *ic)
{
   if (iiirk_config->menu)
     {
	e_menu_post_deactivate_callback_set(iiirk_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(iiirk_config->menu));
	iiirk_config->menu = NULL;
     }
   if (ic->iiirk->ic_drop_before == ic)
     ic->iiirk->ic_drop_before = NULL;
   _iiirk_icon_empty(ic);
   evas_object_del(ic->o_holder);
   evas_object_del(ic->o_holder2);
   e_object_unref(E_OBJECT(ic->border));
   free(ic);
}

static void
_iiirk_icon_fill(IIirk_Icon *ic)
{
   ic->o_icon = e_border_icon_add(ic->border, evas_object_evas_get(ic->iiirk->o_box));
   edje_object_part_swallow(ic->o_holder, "e.swallow.content", ic->o_icon);
   evas_object_pass_events_set(ic->o_icon, 1);
   evas_object_show(ic->o_icon);
   ic->o_icon2 = e_border_icon_add(ic->border, evas_object_evas_get(ic->iiirk->o_box));
   edje_object_part_swallow(ic->o_holder2, "e.swallow.content", ic->o_icon2);
   evas_object_pass_events_set(ic->o_icon2, 1);
   evas_object_show(ic->o_icon2);

   _iiirk_icon_fill_label(ic);
}

static void
_iiirk_icon_fill_label(IIirk_Icon *ic)
{
   const char *label = NULL;

   switch (ic->iiirk->inst->ci->icon_label)
     {
      case 0:
	label = ic->border->client.netwm.name;
	if (!label)
	  label = ic->border->client.icccm.name;
	break;
      case 1:
	label = ic->border->client.icccm.title;
	break;
      case 2:
	label = ic->border->client.icccm.class;
	break;
      case 3:
	label = ic->border->client.netwm.icon_name;
	if (!label)
	  label = ic->border->client.icccm.icon_name;
	break;
      case 4:
	label = e_border_name_get(ic->border);
	break;
     }

   if (!label) label = "?";
   edje_object_part_text_set(ic->o_holder, "e.text.label", label);
   edje_object_part_text_set(ic->o_holder2, "e.text.label", label);
}

static void
_iiirk_icon_empty(IIirk_Icon *ic)
{
   if (ic->o_icon) evas_object_del(ic->o_icon);
   if (ic->o_icon2) evas_object_del(ic->o_icon2);
   ic->o_icon = NULL;
   ic->o_icon2 = NULL;
}

static void
_iiirk_icon_signal_emit(IIirk_Icon *ic, char *sig, char *src)
{
   if (ic->o_holder)
     edje_object_signal_emit(ic->o_holder, sig, src);
   if (ic->o_icon)
     edje_object_signal_emit(ic->o_icon, sig, src);
   if (ic->o_holder2)
     edje_object_signal_emit(ic->o_holder2, sig, src);
   if (ic->o_icon2)
     edje_object_signal_emit(ic->o_icon2, sig, src);
}

static Evas_List *
_iiirk_zone_find(E_Zone *zone)
{
   Evas_List *iiirk = NULL;
   Evas_List *l;

   for (l = iiirk_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	if (inst->ci->show_zone == 0)
	  iiirk = evas_list_append(iiirk, inst->iiirk);
	else if ((inst->ci->show_zone == 1) && (inst->iiirk->zone == zone))
	  iiirk = evas_list_append(iiirk, inst->iiirk);
     }
   return iiirk;
}

static void
_iiirk_cb_app_change(void *data, E_Order *eo)
{
   IIirk *b;

   b = data;
   if (!b->apps) return;
   _iiirk_empty(b);
   _iiirk_fill(b);
   _iiirk_resize_handle(b);
   if (b->inst)
     _gc_orient(b->inst->gcc);
}

static void
_iiirk_cb_obj_moveresize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;

   inst = data;
   _iiirk_resize_handle(inst->iiirk);
   _iiirk_instance_drop_zone_recalc(inst);
}

static void
_iiirk_cb_menu_post(void *data, E_Menu *m)
{
   if (!iiirk_config->menu) return;
   e_object_del(E_OBJECT(iiirk_config->menu));
   iiirk_config->menu = NULL;
}

static void
_iiirk_cb_icon_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   IIirk_Icon *ic;

   ev = event_info;
   ic = data;
   _iiirk_icon_signal_emit(ic, "e,state,focused", "e");
   if (ic->iiirk->inst->ci->show_label)
     {
	_iiirk_icon_fill_label(ic);
	_iiirk_icon_signal_emit(ic, "e,action,show,label", "e");
     }
}

static void
_iiirk_cb_icon_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   IIirk_Icon *ic;

   ev = event_info;
   ic = data;
   _iiirk_icon_signal_emit(ic, "e,state,unfocused", "e");
   if (ic->iiirk->inst->ci->show_label)
     _iiirk_icon_signal_emit(ic, "e,action,hide,label", "e");
}

static void
_iiirk_cb_icon_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   IIirk_Icon *ic;

   ev = event_info;
   ic = data;
   if (ev->button == 1)
     {
	ic->drag.x = ev->output.x;
	ic->drag.y = ev->output.y;
	ic->drag.start = 1;
	ic->drag.dnd = 0;
	ic->mouse_down = 1;
     }
   else if ((ev->button == 3) && (!iiirk_config->menu))
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int cx, cy, cw, ch;
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _iiirk_cb_menu_post, NULL);
	iiirk_config->menu = mn;

	/* FIXME: other icon options go here too */
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _iiirk_cb_menu_configuration, ic->iiirk);

	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);
	
	e_gadcon_client_util_menu_items_append(ic->iiirk->inst->gcc, mn, 0);
	
	e_gadcon_canvas_zone_geometry_get(ic->iiirk->inst->gcc->gadcon,
					  &cx, &cy, &cw, &ch);
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      cx + ev->output.x, cy + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
     }
}

static void
_iiirk_cb_icon_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   IIirk_Icon *ic;

   ev = event_info;
   ic = data;
   if ((ev->button == 1) && (!ic->drag.dnd) && (ic->mouse_down == 1))
     {
	if (ic->border->iconic)
	  {
	     e_border_uniconify(ic->border);
	     if (ic->iiirk->inst->ci->hide_window)
	       {
		  ic->border->user_skip_winlist = ic->skip_winlist;
		  ic->border->client.netwm.state.skip_pager = ic->skip_pager;
		  ic->border->client.netwm.state.skip_taskbar = ic->skip_taskbar;
	       }
	  }
	else
	  {
	     e_border_iconify(ic->border);
	     if (ic->iiirk->inst->ci->hide_window)
	       {
		  ic->border->user_skip_winlist = 1;
		  ic->border->client.netwm.state.skip_pager = 1;
		  ic->border->client.netwm.state.skip_taskbar = 1;
	       }
	  }
	e_border_focus_set(ic->border, 1, 1);

	ic->drag.start = 0;
	ic->drag.dnd = 0;
	ic->mouse_down = 0;
     }
}

static void
_iiirk_cb_icon_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   IIirk_Icon *ic;

   ev = event_info;
   ic = data;
   if (ic->drag.start)
     {
	int dx, dy;

	dx = ev->cur.output.x - ic->drag.x;
	dy = ev->cur.output.y - ic->drag.y;
	if (((dx * dx) + (dy * dy)) >
	    (e_config->drag_resist * e_config->drag_resist))
	  {
	     E_Drag *d;
	     Evas_Object *o;
	     Evas_Coord x, y, w, h;
	     const char *drag_types[] = { "enlightenment/border" };

	     ic->drag.dnd = 1;
	     ic->drag.start = 0;

	     evas_object_geometry_get(ic->o_icon, &x, &y, &w, &h);
	     d = e_drag_new(ic->iiirk->inst->gcc->gadcon->zone->container,
			    x, y, drag_types, 1,
			    ic->border, -1, NULL, _iiirk_cb_drag_finished);
	     o = e_border_icon_add(ic->border, e_drag_evas_get(d));
	     e_drag_object_set(d, o);

	     e_drag_resize(d, w, h);
	     e_drag_start(d, ic->drag.x, ic->drag.y);
	     e_object_ref(E_OBJECT(ic->border));
	     ic->iiirk->icons = evas_list_remove(ic->iiirk->icons, ic);
	     if (ic->border->desktop) e_order_remove(ic->iiirk->apps, ic->border->desktop);
	     _iiirk_resize_handle(ic->iiirk);
	     _gc_orient(ic->iiirk->inst->gcc);
	     _iiirk_icon_free(ic);
	  }
     }
}

static void
_iiirk_cb_icon_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   IIirk_Icon *ic;
   Evas_Coord x, y;

   ic = data;
   evas_object_geometry_get(ic->o_holder, &x, &y, NULL, NULL);
   evas_object_move(ic->o_holder2, x, y);
   evas_object_raise(ic->o_holder2);
}

static void
_iiirk_cb_icon_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   IIirk_Icon *ic;
   Evas_Coord w, h;

   ic = data;
   evas_object_geometry_get(ic->o_holder, NULL, NULL, &w, &h);
   evas_object_resize(ic->o_holder2, w, h);
   evas_object_raise(ic->o_holder2);
}

static void
_iiirk_cb_drag_finished(E_Drag *drag, int dropped)
{
   E_Border *bd;

   bd = drag->data;
   if (!dropped) e_border_uniconify(bd);
   e_object_unref(E_OBJECT(bd));
}

static void
_iiirk_cb_drop_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   IIirk *b;
   Evas_Coord x, y;

   b = data;
   evas_object_geometry_get(b->o_drop, &x, &y, NULL, NULL);
   evas_object_move(b->o_drop_over, x, y);
}

static void
_iiirk_cb_drop_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   IIirk *b;
   Evas_Coord w, h;

   b = data;
   evas_object_geometry_get(b->o_drop, NULL, NULL, &w, &h);
   evas_object_resize(b->o_drop_over, w, h);
}

static void
_iiirk_inst_cb_scroll(void *data)
{
   Instance *inst;

   /* Update the position of the dnd to handle for autoscrolling
    * gadgets. */
   inst = data;
   _iiirk_drop_position_update(inst, inst->iiirk->dnd_x, inst->iiirk->dnd_y);
}

static void
_iiirk_drop_position_update(Instance *inst, Evas_Coord x, Evas_Coord y)
{
   Evas_Coord xx, yy;
   int ox, oy;
   IIirk_Icon *ic;

   inst->iiirk->dnd_x = x;
   inst->iiirk->dnd_y = y;

   if (inst->iiirk->o_drop)
      e_box_unpack(inst->iiirk->o_drop);
   evas_object_geometry_get(inst->iiirk->o_box, &xx, &yy, NULL, NULL);
   e_box_align_pixel_offset_get(inst->gcc->o_box, &ox, &oy);
   ic = _iiirk_icon_at_coord(inst->iiirk, x + xx + ox, y + yy + oy);
   inst->iiirk->ic_drop_before = ic;
   if (ic)
     {
	Evas_Coord ix, iy, iw, ih;
	int before = 0;
	
	evas_object_geometry_get(ic->o_holder, &ix, &iy, &iw, &ih);
	if (e_box_orientation_get(inst->iiirk->o_box))
	  {
	     if ((x + xx) < (ix + (iw / 2))) before = 1;
	  }
	else
	  {
	     if ((y + yy) < (iy + (ih / 2))) before = 1;
	  }
	if (before)
	  e_box_pack_before(inst->iiirk->o_box, inst->iiirk->o_drop, ic->o_holder);
	else
	  e_box_pack_after(inst->iiirk->o_box, inst->iiirk->o_drop, ic->o_holder);
	inst->iiirk->drop_before = before;
     }
   else e_box_pack_end(inst->iiirk->o_box, inst->iiirk->o_drop);
   e_box_pack_options_set(inst->iiirk->o_drop,
			  1, 1, /* fill */
			  1, 1, /* expand */
			  0.5, 0.5, /* align */
			  1, 1, /* min */
			  -1, -1 /* max */
			  );
   _iiirk_resize_handle(inst->iiirk);
   _gc_orient(inst->gcc);
}

static void
_iiirk_inst_cb_enter(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Enter *ev;
   Instance *inst;
   Evas_Object *o, *o2;

   ev = event_info;
   inst = data;
   o = edje_object_add(evas_object_evas_get(inst->iiirk->o_box));
   inst->iiirk->o_drop = o;
   o2 = edje_object_add(evas_object_evas_get(inst->iiirk->o_box));
   inst->iiirk->o_drop_over = o2;
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOVE, _iiirk_cb_drop_move, inst->iiirk);
   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, _iiirk_cb_drop_resize, inst->iiirk);
   e_theme_edje_object_set(o, "base/theme/modules/iiirk",
			   "e/modules/iiirk/drop");
   e_theme_edje_object_set(o2, "base/theme/modules/iiirk",
			   "e/modules/iiirk/drop_overlay");
   evas_object_layer_set(o2, 19999);
   evas_object_show(o);
   evas_object_show(o2);
   _iiirk_drop_position_update(inst, ev->x, ev->y);
   e_gadcon_client_autoscroll_cb_set(inst->gcc, _iiirk_inst_cb_scroll, inst);
   e_gadcon_client_autoscroll_update(inst->gcc, ev->x, ev->y);
}

static void
_iiirk_inst_cb_move(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Move *ev;
   Instance *inst;

   ev = event_info;
   inst = data;
   _iiirk_drop_position_update(inst, ev->x, ev->y);
   e_gadcon_client_autoscroll_update(inst->gcc, ev->x, ev->y);
}

static void
_iiirk_inst_cb_leave(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Leave *ev;
   Instance *inst;

   ev = event_info;
   inst = data;
   inst->iiirk->ic_drop_before = NULL;
   evas_object_del(inst->iiirk->o_drop);
   inst->iiirk->o_drop = NULL;
   evas_object_del(inst->iiirk->o_drop_over);
   inst->iiirk->o_drop_over = NULL;
   e_gadcon_client_autoscroll_cb_set(inst->gcc, NULL, NULL);
   _iiirk_resize_handle(inst->iiirk);
   _gc_orient(inst->gcc);
}

static void
_iiirk_inst_cb_drop(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Drop *ev;
   Instance *inst;
   E_Border *bd = NULL;
   IIirk *b;
   IIirk_Icon *ic, *ic2;
   Evas_List *l;

   ev = event_info;
   inst = data;
   if (!strcmp(type, "enlightenment/border"))
     {
	bd = ev->data;
	if (!bd) return;
     }

   ic2 = inst->iiirk->ic_drop_before;
   if (ic2)
     {
	/* Add new eapp before this icon */
	if (!inst->iiirk->drop_before)
	  {
	     for (l = inst->iiirk->icons; l; l = l->next)
	       {
		  if (l->data == ic2)
		    {
		       if (l->next)
			 ic2 = l->next->data;
		       else
			 ic2 = NULL;
		       break;
		    }
	       }
	  }
	if (!ic2) goto atend;
	b = inst->iiirk;
	if (_iiirk_icon_find(b, bd)) return;
	ic = _iiirk_icon_new(b, bd);
	if (!ic) return;
	b->icons = evas_list_prepend_relative(b->icons, ic, ic2);
	e_box_pack_before(b->o_box, ic->o_holder, ic2->o_holder);
     }
   else
     {
	atend:
	b = inst->iiirk;
	if (_iiirk_icon_find(b, bd)) return;
	ic = _iiirk_icon_new(b, bd);
	if (!ic) return;
	b->icons = evas_list_append(b->icons, ic);
	e_box_pack_end(b->o_box, ic->o_holder);
     }

   if (inst->ci->hide_window)
     {
	ic->skip_winlist = bd->user_skip_winlist;
	ic->skip_pager   = bd->client.netwm.state.skip_pager;
	ic->skip_taskbar = bd->client.netwm.state.skip_taskbar;
     }

   if (bd->desktop && b->apps) 
     {
	/* Search for the .desktop in the current list.
	 * If it was not found, append it to the list
	 */
	for (l = b->apps->desktops; l && l->data != bd->desktop; l = l->next);
	if (!l) e_order_append(b->apps, bd->desktop);
     }

   evas_object_del(inst->iiirk->o_drop);
   inst->iiirk->o_drop = NULL;
   evas_object_del(inst->iiirk->o_drop_over);
   inst->iiirk->o_drop_over = NULL;
   _iiirk_empty_handle(b);
   e_gadcon_client_autoscroll_cb_set(inst->gcc, NULL, NULL);
   _iiirk_resize_handle(inst->iiirk);
   _gc_orient(inst->gcc);
}

static int
_iiirk_cb_event_border_property(void *data, int type, void *event)
{
   E_Event_Border_Property *ev;
   IIirk *b;
   IIirk_Icon *ic;
   E_Desk *desk;
   Evas_List *l, *iiirk;

   ev = event;
   desk = e_desk_current_get(ev->border->zone);
   iiirk = _iiirk_zone_find(ev->border->zone);
   for (l = iiirk; l; l = l->next)
     {
	b = l->data;
	if (!(ic = _iiirk_icon_find(b, ev->border))) continue;
	if (!ic->iiirk->inst->ci->hide_window) continue;

	ic->skip_winlist = ev->border->user_skip_winlist;
	ic->skip_pager   = ev->border->client.netwm.state.skip_pager;
	ic->skip_taskbar = ev->border->client.netwm.state.skip_taskbar;
     }

   while (iiirk)
     iiirk = evas_list_remove_list(iiirk, iiirk);
   return 1;
}

static int
_iiirk_cb_event_border_add(void *data, int type, void *event)
{
   E_Event_Border_Add *ev;
   IIirk *b;
   IIirk_Icon *ic;
   E_Desk *desk;
   Efreet_Desktop *desktop;
   Evas_List *l, *iiirk;

   ev = event;
   if (!ev || !ev->border || !ev->border->desktop) return 1;
   /* add if the border's desktop icon match one of our list */

   desk = e_desk_current_get(ev->border->zone);
   iiirk = _iiirk_zone_find(ev->border->zone);
   for (l = iiirk; l; l = l->next)
     {
	Evas_List *ll;

	b = l->data;
	if (_iiirk_icon_find(b, ev->border)) continue;
	if ((b->inst->ci->show_desk) && (ev->border->desk != desk)) continue;
	if (!b->apps) continue;

	for (ll = b->apps->desktops; ll; ll = ll->next)
	  {
	     if (!(desktop = ll->data)) continue;
	     if (desktop == ev->border->desktop)
	       {
		  ic = _iiirk_icon_new(b, ev->border);
		  if (!ic) continue;
		  b->icons = evas_list_append(b->icons, ic);
		  e_box_pack_end(b->o_box, ic->o_holder);
		  _iiirk_empty_handle(b);
		  _iiirk_resize_handle(b);
		  _gc_orient(b->inst->gcc);
	       }
	  }
     }

   while (iiirk)
     iiirk = evas_list_remove_list(iiirk, iiirk);
   return 1;
}

static int
_iiirk_cb_event_border_remove(void *data, int type, void *event)
{
   E_Event_Border_Remove *ev;
   IIirk *b;
   IIirk_Icon *ic;
   Evas_List *l, *iiirk;

   ev = event;
   /* find icon and remove if there */
   iiirk = _iiirk_zone_find(ev->border->zone);
   for (l = iiirk; l; l = l->next)
     {
	b = l->data;
	ic = _iiirk_icon_find(b, ev->border);
	if (!ic) continue;
	_iiirk_icon_free(ic);
	b->icons = evas_list_remove(b->icons, ic);
	_iiirk_empty_handle(b);
	_iiirk_resize_handle(b);
	_gc_orient(b->inst->gcc);
     }
   while (iiirk)
     iiirk = evas_list_remove_list(iiirk, iiirk);

   return 1;
}

static int
_iiirk_cb_event_border_iconify(void *data, int type, void *event)
{
   E_Event_Border_Iconify *ev;
   IIirk *b;
   IIirk_Icon *ic;
   Evas_List *l, *iiirk;
   E_Desk *desk;

   ev = event;
   desk = e_desk_current_get(ev->border->zone);
   iiirk = _iiirk_zone_find(ev->border->zone);
   for (l = iiirk; l; l = l->next)
     {
	b = l->data;
	if (!(ic = _iiirk_icon_find(b, ev->border))) continue;
	if (ic->iiirk->inst->ci->hide_window)
	  {
	     ic->border->user_skip_winlist = 1;
	     ic->border->client.netwm.state.skip_pager = 1;
	     ic->border->client.netwm.state.skip_taskbar = 1;
	  }
     }

   while (iiirk)
     iiirk = evas_list_remove_list(iiirk, iiirk);
   return 1;
}

static int
_iiirk_cb_event_border_uniconify(void *data, int type, void *event)
{
   E_Event_Border_Uniconify *ev;
   IIirk *b;
   IIirk_Icon *ic;
   Evas_List *l, *iiirk;

   ev = event;
   iiirk = _iiirk_zone_find(ev->border->zone);
   for (l = iiirk; l; l = l->next)
     {
	b = l->data;
	if (!(ic = _iiirk_icon_find(b, ev->border))) continue;
	if (ic->iiirk->inst->ci->hide_window)
	  {
	     ic->border->user_skip_winlist = ic->skip_winlist;
	     ic->border->client.netwm.state.skip_pager = ic->skip_pager;
	     ic->border->client.netwm.state.skip_taskbar = ic->skip_taskbar;
	  }
     }

   while (iiirk)
     iiirk = evas_list_remove_list(iiirk, iiirk);

   return 1;
}

static int
_iiirk_cb_event_border_icon_change(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   IIirk *b;
   IIirk_Icon *ic;
   Evas_List *l, *iiirk;

   ev = event;
   /* update icon */
   iiirk = _iiirk_zone_find(ev->border->zone);
   for (l = iiirk; l; l = l->next)
     {
	b = l->data;
	ic = _iiirk_icon_find(b, ev->border);
	if (!ic) continue;
	_iiirk_icon_empty(ic);
	_iiirk_icon_fill(ic);
     }

   while (iiirk)
     iiirk = evas_list_remove_list(iiirk, iiirk);

   return 1;
}

static int
_iiirk_cb_event_border_urgent_change(void *data, int type, void *event)
{
   E_Event_Border_Urgent_Change *ev;
   IIirk *b;
   IIirk_Icon *ic;
   Evas_List *l, *iiirk;

   ev = event;
   /* update icon */
   iiirk = _iiirk_zone_find(ev->border->zone);
   for (l = iiirk; l; l = l->next)
     {
	b = l->data;
	ic = _iiirk_icon_find(b, ev->border);
	if (!ic) continue;
	if (ev->border->client.icccm.urgent)
	  {
	     edje_object_signal_emit(ic->o_holder, "e,state,urgent", "e");
	     edje_object_signal_emit(ic->o_holder2, "e,state,urgent", "e");
	  }
	else
	  {
	     edje_object_signal_emit(ic->o_holder, "e,state,not_urgent", "e");
	     edje_object_signal_emit(ic->o_holder2, "e,state,not_urgent", "e");
	  }
     }

   return 1;
}

static int
_iiirk_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   IIirk *b;
   Evas_List *l, *iiirk;

   ev = event;
   /* delete all wins from iiirk and add only for current desk */
   iiirk = _iiirk_zone_find(ev->desk->zone);
   for (l = iiirk; l; l = l->next)
     {
	b = l->data;
	if (b->inst->ci->show_desk)
	  {
	     _iiirk_empty(b);
	     _iiirk_fill(b);
	     _iiirk_resize_handle(b);
	     _gc_orient(b->inst->gcc);
	  }
     }

   while (iiirk)
     iiirk = evas_list_remove_list(iiirk, iiirk);

   return 1;
}

static Config_Item *
_iiirk_config_item_get(const char *id)
{
   Evas_List *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
     {
	snprintf(buf, sizeof(buf), "%s.%d", _gadcon_class.name, ++uuid);
	id = buf;
     }
   else
     {
	/* Find old config */
	for (l = iiirk_config->items; l; l = l->next)
	  {
	     ci = l->data;
	     if ((ci->id) && (!strcmp(ci->id, id)))
	       return ci;
	  }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->hide_window = 1;
   ci->show_label = 0;
   ci->show_zone = 1;
   ci->show_desk = 0;
   ci->icon_label = 0;
   iiirk_config->items = evas_list_append(iiirk_config->items, ci);
   return ci;
}

void
_iiirk_config_update(Config_Item *ci)
{
   Evas_List *l;
   for (l = iiirk_config->instances; l; l = l->next)
     {
	Instance *inst;
	char buf[4096];

	inst = l->data;
	if (inst->ci != ci) continue;

	if (inst->ci->dir[0] != '/')
	  {
	     const char *homedir;

	     homedir = e_user_homedir_get();
	     snprintf(buf, sizeof(buf), "%s/.e/e/applications/iiirk/%s/.order", homedir, inst->ci->dir);
	  }
	else
	  snprintf(buf, sizeof(buf), inst->ci->dir);
	if (inst->iiirk->apps) e_object_del(E_OBJECT(inst->iiirk->apps));
	inst->iiirk->apps = e_order_new(buf);
	_iiirk_empty(inst->iiirk);
	_iiirk_fill(inst->iiirk);
	_iiirk_resize_handle(inst->iiirk);
	_gc_orient(inst->gcc);
     }
}

static void
_iiirk_cb_menu_configuration(void *data, E_Menu *m, E_Menu_Item *mi)
{
   IIirk *b;
   int ok = 1;
   Evas_List *l;

   b = data;
   for (l = iiirk_config->config_dialog; l; l = l->next)
     {
	E_Config_Dialog *cfd;

	cfd = l->data;
	if (cfd->data == b->inst->ci)
	  {
	     ok = 0;
	     break;
	  }
     }
   if (ok) _config_iiirk_module(b->inst->ci);
}

/***************************************************************************/
/**/
/* module setup */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
     "IIirk"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   conf_item_edd = E_CONFIG_DD_NEW("IIirk_Config_Item", Config_Item);
   #undef T
   #undef D
   #define T Config_Item
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, hide_window, INT);
   E_CONFIG_VAL(D, T, show_label, INT);
   E_CONFIG_VAL(D, T, show_zone, INT);
   E_CONFIG_VAL(D, T, show_desk, INT);
   E_CONFIG_VAL(D, T, icon_label, INT);

   conf_edd = E_CONFIG_DD_NEW("IIirk_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   iiirk_config = e_config_domain_load("module.iiirk", conf_edd);
   if (!iiirk_config)
     {
	Config_Item *ci;

	iiirk_config = E_NEW(Config, 1);
	
	ci = E_NEW(Config_Item, 1);
	ci->id = evas_stringshare_add("0");
	ci->hide_window = 1;
	ci->show_label = 0;
	ci->show_zone = 1;
	ci->show_desk = 0;
	ci->icon_label = 0;
	iiirk_config->items = evas_list_append(iiirk_config->items, ci);
     }
   else
     {
	Config_Item *ci;
	const char *p;

	/* Init uuid */
	ci = evas_list_last(iiirk_config->items)->data;
	p = strrchr(ci->id, '.');
	if (p) uuid = atoi(p + 1);
     }

   iiirk_config->module = m;

   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_PROPERTY, _iiirk_cb_event_border_property, NULL));
   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_ADD, _iiirk_cb_event_border_add, NULL));
   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_REMOVE, _iiirk_cb_event_border_remove, NULL));
   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_ICONIFY, _iiirk_cb_event_border_iconify, NULL));
   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_UNICONIFY, _iiirk_cb_event_border_uniconify, NULL));
   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_ICON_CHANGE, _iiirk_cb_event_border_icon_change, NULL));
   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_URGENT_CHANGE,
       _iiirk_cb_event_border_urgent_change, NULL));
   iiirk_config->handlers = evas_list_append
     (iiirk_config->handlers, ecore_event_handler_add
      (E_EVENT_DESK_SHOW, _iiirk_cb_event_desk_show, NULL));

   e_configure_registry_category_add("applications", 20, D_("Applications"), 
				     NULL, "enlightenment/applications");
   e_configure_registry_item_add("applications/iiirk_applications", 20, D_("IIirk Applications"), 
				 NULL, "enlightenment/iiirk_applications", e_int_config_apps_iiirk);
   e_configure_registry_category_add("internal", -1, D_("Internal"), NULL, "enlightenment/internal");
   e_configure_registry_item_add("internal/iiirk_other", -1, D_("IIirk Other"), 
				 NULL, "enlightenment/windows", e_int_config_apps_iiirk_other);
   
   e_gadcon_provider_register(&_gadcon_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_gadcon_provider_unregister(&_gadcon_class);

   while (iiirk_config->handlers)
     {
	ecore_event_handler_del(iiirk_config->handlers->data);
	iiirk_config->handlers = evas_list_remove_list(iiirk_config->handlers, iiirk_config->handlers);
     }

   while (iiirk_config->config_dialog)
     /* there is no need to eves_list_remove_list. It is done implicitly in
      * dialog _free_data function
      */
     e_object_del(E_OBJECT(iiirk_config->config_dialog->data));

   if (iiirk_config->menu)
     {
	e_menu_post_deactivate_callback_set(iiirk_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(iiirk_config->menu));
	iiirk_config->menu = NULL;
     }
   while (iiirk_config->items)
     {
	Config_Item *ci;
	
	ci = iiirk_config->items->data;
	iiirk_config->items = evas_list_remove_list(iiirk_config->items, iiirk_config->items);
	if (ci->id)
	  evas_stringshare_del(ci->id);
	free(ci);
     }

   free(iiirk_config);
   iiirk_config = NULL;
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.iiirk", conf_edd, iiirk_config);
   return 1;
}

/**/
/***************************************************************************/
