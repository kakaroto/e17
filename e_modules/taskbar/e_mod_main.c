/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "e_mod_main.h"

/***************************************************************************/
 /**/
/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);

/* and actually define the gadcon class that this module provides (just 1) */
static E_Gadcon_Client_Class _gadcon_class = {
   GADCON_CLIENT_CLASS_VERSION,
   "taskbar",
   {
    _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon}
};
 /**/
/***************************************************************************/
/***************************************************************************/
    /**/
/* actual module specifics */
   typedef struct _Instance Instance;

typedef struct _Taskbar Taskbar;
typedef struct _Taskbar_Icon Taskbar_Icon;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_taskbar;
   Taskbar *taskbar;
};

struct _Taskbar
{
   Instance *inst;
   Evas_Object *o_box;
   Evas_Object *o_empty;
   Evas_List *icons;
   Evas_List *borders;
   E_Zone *zone;
   int xpos;
   int ypos;
   int xmax;
   int ymax;
   int bwidth;
   int bheight;
   int bwmin;
   int bhmin;
};

struct _Taskbar_Icon
{
   Taskbar *taskbar;
   Evas_Object *o_holder;
   Evas_Object *o_icon;
   Evas_Object *o_holder2;
   Evas_Object *o_icon2;
   E_Border *border;
};

static Taskbar *_taskbar_new(Evas *evas, E_Zone *zone);
static void _taskbar_free(Taskbar *b);
static void _taskbar_cb_empty_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_empty_handle(Taskbar *b);
static void _taskbar_fill(Taskbar *b);
static void _taskbar_repack(Taskbar *b);
static void _taskbar_empty(Taskbar *b);
static void _taskbar_orient_set(Taskbar *b, int horizontal);
static void _taskbar_resize_handle(Taskbar *b);
static Taskbar_Icon *_taskbar_icon_find(Taskbar *b, E_Border *bd);
static Taskbar_Icon *_taskbar_icon_at_coord(Taskbar *b, Evas_Coord x, Evas_Coord y);
static Taskbar_Icon *_taskbar_icon_new(Taskbar *b, E_Border *bd);
static int  _taskbar_icon_check_add(Taskbar *b, E_Border *bd);
static void _taskbar_icon_add(Taskbar *b, E_Border *bd);
static void _taskbar_icon_remove(Taskbar *b, E_Border *bd);
static void _taskbar_icon_free(Taskbar_Icon *ic);
static void _taskbar_icon_fill(Taskbar_Icon *ic);
static void _taskbar_icon_empty(Taskbar_Icon *ic);
static void _taskbar_icon_signal_emit(Taskbar_Icon *ic, char *sig, char *src);
static void _taskbar_cb_obj_moveresize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _taskbar_cb_menu_post(void *data, E_Menu *m);
static void _taskbar_cb_icon_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_resize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int _taskbar_cb_event_border_add(void *data, int type, void *event);
static int _taskbar_cb_event_border_remove(void *data, int type, void *event);
static int _taskbar_cb_event_border_iconify(void *data, int type, void *event);
static int _taskbar_cb_event_border_uniconify(void *data, int type, void *event);
static int _taskbar_cb_event_border_icon_change(void *data, int type, void *event);
static int _taskbar_cb_event_border_zone_set(void *data, int type, void *event);
static int _taskbar_cb_event_border_desk_set(void *data, int type, void *event);
static int _taskbar_cb_window_focus_in(void *data, int type, void *event);
static int _taskbar_cb_window_focus_out(void *data, int type, void *event);
static int _taskbar_cb_window_property(void *data, int type, void *event);
static int _taskbar_cb_event_desk_show(void *data, int type, void *event);
static Config_Item *_taskbar_config_item_get(const char *id);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *taskbar_config = NULL;

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Taskbar *b;
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   Evas_Coord x, y, w, h;
   int cx, cy, cw, ch;
   Config_Item *ci;

   inst = E_NEW(Instance, 1);
   ci = _taskbar_config_item_get(id);
   if (!ci->id) ci->id = evas_stringshare_add(id);

   b = _taskbar_new(gc->evas, gc->zone);

   b->inst = inst;
   inst->taskbar = b;
   o = b->o_box;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_taskbar = o;

   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
   evas_object_geometry_get(o, &x, &y, &w, &h);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOVE, _taskbar_cb_obj_moveresize, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, _taskbar_cb_obj_moveresize, inst);
   taskbar_config->instances = evas_list_append(taskbar_config->instances, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = (Instance *)gcc->data;
   taskbar_config->instances = evas_list_remove(taskbar_config->instances, inst);
   _taskbar_free(inst->taskbar);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;
   int w;

   inst = (Instance *)gcc->data;
   
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
        _taskbar_orient_set(inst->taskbar, 1);
        //e_gadcon_client_aspect_set(gcc, evas_list_count(inst->taskbar->borders) * 16 * 3, 16);
	w =evas_list_count(inst->taskbar->borders);

	e_gadcon_client_aspect_set(gcc, w * inst->taskbar->bwmin, inst->taskbar->bheight);
        break;
     case E_GADCON_ORIENT_VERT:
     case E_GADCON_ORIENT_LEFT:
     case E_GADCON_ORIENT_RIGHT:
     case E_GADCON_ORIENT_CORNER_LT:
     case E_GADCON_ORIENT_CORNER_RT:
     case E_GADCON_ORIENT_CORNER_LB:
     case E_GADCON_ORIENT_CORNER_RB:
        _taskbar_orient_set(inst->taskbar, 0);
        e_gadcon_client_aspect_set(gcc, 16, evas_list_count(inst->taskbar->borders) * 16 * 3);
        break;
     default:
        break;
     }
   if (evas_list_count(inst->taskbar->borders) < 1)
      e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void)
{
   return D_("Taskbar");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(taskbar_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}
 /**/
/***************************************************************************/
/***************************************************************************/
 /**/ 

static Taskbar *
_taskbar_new(Evas *evas, E_Zone *zone)
{
   Taskbar *b;

   b = E_NEW(Taskbar, 1);
   b->o_box = e_table_add(evas);
   b->xpos = 0;
   b->ypos = 0;
   b->xmax = 1;
   b->ymax = 1;

   Evas_Coord w, h;

   evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);

   e_table_homogenous_set(b->o_box, 0);
   e_table_align_set(b->o_box, 0.5, 0.5);
   b->zone = zone;
   return b;
}

static void
_taskbar_free(Taskbar *b)
{
   _taskbar_empty(b);
   evas_object_del(b->o_box);
   if (b->o_empty)
      evas_object_del(b->o_empty);
   free(b);
}

static void
_taskbar_cb_empty_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Taskbar *b;

   ev = event_info;
   b = data;

   if (!taskbar_config->menu)
     {
        E_Menu *mn;
	E_Menu_Item *mi;
        int cx, cy, cw, ch;

        mn = e_menu_new();
        e_menu_post_deactivate_callback_set(mn, _taskbar_cb_menu_post, NULL);
        taskbar_config->menu = mn;

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _taskbar_cb_menu_configure, b);

        e_gadcon_client_util_menu_items_append(b->inst->gcc, mn, 0);

        e_gadcon_canvas_zone_geometry_get(b->inst->gcc->gadcon, &cx, &cy, &cw, &ch);
        e_menu_activate_mouse(mn,
                              e_util_zone_current_get(e_manager_current_get()),
                              cx + ev->output.x, cy + ev->output.y, 1, 1, 
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        evas_event_feed_mouse_up(b->inst->gcc->gadcon->evas, ev->button, 
	                         EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_taskbar_empty_handle(Taskbar *b)
{
   if (!b->borders)
     {
        if (!b->o_empty)
          {
             Evas_Coord w, h;

             b->o_empty = evas_object_rectangle_add(evas_object_evas_get(b->o_box));
             evas_object_event_callback_add(b->o_empty, EVAS_CALLBACK_MOUSE_DOWN, _taskbar_cb_empty_mouse_down, b);
             evas_object_color_set(b->o_empty, 0, 0, 0, 0);
             evas_object_show(b->o_empty);
             e_table_pack(b->o_box, b->o_empty, 0, 0, 1, 1);
             evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);

             e_table_pack_options_set(b->o_empty, 1, 1,   /* fill */
                                    1, 1,                 /* expand */
                                    0.5, 0.5,             /* align */
                                    0, 0,                 /* min */
                                    -1, -1                /* max */
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
_taskbar_fill(Taskbar *b)
{
   Taskbar_Icon *ic;
   E_Border_List *bl;
   E_Border *bd;
   Config_Item *ci;

   ci = _taskbar_config_item_get(b->inst->gcc->id);
   bl = e_container_border_list_first(b->zone->container);
   while ((bd = e_container_border_list_next(bl)))
     {
        if (bd->user_skip_winlist)
           continue;
        if (bd->client.netwm.state.skip_taskbar)
           continue;
        if (((bd->desk == e_desk_current_get(b->zone)) && (bd->zone == b->zone)) || (bd->sticky) || (ci->show_all))
          {
	     b->borders = evas_list_append(b->borders, bd);
             ic = _taskbar_icon_new(b, bd);
             b->icons = evas_list_append(b->icons, ic);
             e_table_pack(b->o_box, ic->o_holder, b->xpos, b->ypos, 1, 1);
     	     e_table_pack_options_set(ic->o_holder, 0, 0,                     /* fill */
						    0, 0,                     /* expand */
		     				    0.5, 0.5,                 /* align */
		     				    b->bwidth, b->bheight,    /* min */
		     				    b->bwidth, b->bheight     /* max */
		   );
             if (bd->iconic)
                _taskbar_icon_signal_emit(ic, "instant_iconify", "");
             if (bd->focused)
                _taskbar_icon_signal_emit(ic, "instant_focused", "");
	     b->xpos++;
	     if (b->xpos >= b->xmax)
	       {
		  b->xpos = 0;
		  b->ypos ++;
		  //if (b->ypos >= b->ymax)
	       }
          }
     }

   _taskbar_empty_handle(b);
   _taskbar_resize_handle(b);
}

static void
_taskbar_repack(Taskbar *b)
{
   Evas_List *borders;
   E_Border *bd;
   Taskbar_Icon *ic;

   e_table_freeze(b->o_box);
   
   while (b->icons)
     {
	_taskbar_icon_free(b->icons->data);
	b->icons = evas_list_remove_list(b->icons, b->icons);
     }
   b->xpos = 0;
   b->ypos = 0;

   for(borders = b->borders; borders; borders = evas_list_next(borders))
     {
	bd = borders->data;
	ic = _taskbar_icon_new(b, bd);
	b->icons = evas_list_append(b->icons, ic);
	e_table_pack(b->o_box, ic->o_holder, b->xpos, b->ypos, 1, 1);
	e_table_pack_options_set(ic->o_holder, 0, 0,                     /* fill */
					       0, 0,                     /* expand */
					       0.5, 0.5,                 /* align */
					       b->bwidth, b->bheight,    /* min */
					       b->bwidth, b->bheight     /* max */
	      );

	if (bd->iconic)
	  _taskbar_icon_signal_emit(ic, "instant_iconify", "");
	if (bd->focused)
	  _taskbar_icon_signal_emit(ic, "instant_focused", "");
	b->xpos++;
	if (b->xpos >= b->xmax)
	  {
	     b->xpos = 0;
	     b->ypos ++;
	     //if (b->ypos >= b->ymax)
	  }
     }

   e_table_thaw(b->o_box);
}

static void
_taskbar_empty(Taskbar *b)
{
   while (b->icons)
     {
        _taskbar_icon_free(b->icons->data);
        b->icons = evas_list_remove_list(b->icons, b->icons);
     }
   while (b->borders)
     {
	b->borders = evas_list_remove_list(b->borders, b->borders);
     }
   b->xpos = 0;
   b->ypos = 0;
   _taskbar_empty_handle(b);
}

static void
_taskbar_orient_set(Taskbar *b, int horizontal)
{
   e_table_align_set(b->o_box, 0.5, 0.5);
}

static void
_taskbar_resize_handle(Taskbar *b)
{
   Evas_List *l;
   Taskbar_Icon *ic;
   Evas_Coord w, h, wmin, hmin;
   int wnum, wnum2, hnum;

   evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);
   if (!b->icons)
     return;
   ic = b->icons->data;
   wnum = evas_list_count(b->borders);
   edje_object_size_min_calc(ic->o_holder, &wmin, &hmin);
   wnum2 = w / wmin;
   if (wnum < wnum2)
     wnum2 = wnum;
   if (wnum2 < 1)
     wnum2 = 1;
   hnum = h / hmin;
   if (hnum < 1)
     hnum = 1;
   b->xmax = wnum2;
   b->bwidth = w / wnum2;
   b->bheight = h / hnum;
   b->bwmin = wmin;
   b->bhmin = hmin;

   _taskbar_repack(b);
}

static Taskbar_Icon *
_taskbar_icon_find(Taskbar *b, E_Border *bd)
{
   Evas_List *l;
   Taskbar_Icon *ic;

   for (l = b->icons; l; l = l->next)
     {
        ic = l->data;

        if (ic->border == bd)
           return ic;
     }
   return NULL;
}

static Taskbar_Icon *
_taskbar_icon_at_coord(Taskbar *b, Evas_Coord x, Evas_Coord y)
{
   Evas_List *l;
   Taskbar_Icon *ic;

   for (l = b->icons; l; l = l->next)
     {
        Evas_Coord dx, dy, dw, dh;

        ic = l->data;

        evas_object_geometry_get(ic->o_holder, &dx, &dy, &dw, &dh);
        if (E_INSIDE(x, y, dx, dy, dw, dh))
           return ic;
     }
   return NULL;
}

static Taskbar_Icon *
_taskbar_icon_new(Taskbar *b, E_Border *bd)
{
   Taskbar_Icon *ic;
   char buf[4096];

   ic = E_NEW(Taskbar_Icon, 1);
   e_object_ref(E_OBJECT(bd));
   ic->taskbar = b;
   ic->border = bd;
   ic->o_holder = edje_object_add(evas_object_evas_get(b->o_box));
   snprintf(buf, sizeof(buf), "%s/taskbar.edj", e_module_dir_get(taskbar_config->module));
   if (!e_theme_edje_object_set(ic->o_holder, "base/theme/modules/taskbar", "modules/taskbar/icon"))
      edje_object_file_set(ic->o_holder, buf, "modules/taskbar/icon");
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_IN, _taskbar_cb_icon_mouse_in, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_OUT, _taskbar_cb_icon_mouse_out, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_DOWN, _taskbar_cb_icon_mouse_down, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_UP, _taskbar_cb_icon_mouse_up, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOVE, _taskbar_cb_icon_move, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_RESIZE, _taskbar_cb_icon_resize, ic);
   evas_object_show(ic->o_holder);

   ic->o_holder2 = edje_object_add(evas_object_evas_get(b->o_box));
   if (!e_theme_edje_object_set(ic->o_holder2, "base/theme/modules/taskbar", "modules/taskbar/icon_overlay"))
      edje_object_file_set(ic->o_holder2, buf, "modules/taskbar/icon_overlay");
   evas_object_layer_set(ic->o_holder2, 9999);
   evas_object_pass_events_set(ic->o_holder2, 1);
   evas_object_show(ic->o_holder2);

   _taskbar_icon_fill(ic);
   return ic;
}

static int
_taskbar_icon_check_add(Taskbar *b, E_Border *bd)
{
   Config_Item *ci;
   
   ci = _taskbar_config_item_get(b->inst->gcc->id);
   
   if (bd->user_skip_winlist)
     return 1;
   if (bd->client.netwm.state.skip_taskbar)
     return 1;
   if (_taskbar_icon_find(b, bd))
     return 1;
   if (!(bd->sticky || ci->show_all))
     {
	if (bd->zone != b->zone)
	  return 1;
	if (bd->desk != e_desk_current_get(bd->zone))
	  return 1;
     }

   _taskbar_icon_add(b, bd);
   return 0;
}

static void
_taskbar_icon_add(Taskbar *b, E_Border *bd)
{

   b->borders = evas_list_append(b->borders, bd);
   _taskbar_repack(b);
   _taskbar_empty_handle(b);
   _taskbar_resize_handle(b);

   _gc_orient(b->inst->gcc);
}

static void
_taskbar_icon_remove(Taskbar *b, E_Border *bd)
{
   b->borders = evas_list_remove(b->borders, bd);
   _taskbar_repack(b);
   _taskbar_empty_handle(b);
   _taskbar_resize_handle(b);
   _gc_orient(b->inst->gcc);
}

static void
_taskbar_icon_free(Taskbar_Icon *ic)
{

   if (taskbar_config->menu)
     {
        e_menu_post_deactivate_callback_set(taskbar_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(taskbar_config->menu));
        taskbar_config->menu = NULL;
     }
   _taskbar_icon_empty(ic);
   evas_object_del(ic->o_holder);
   evas_object_del(ic->o_holder2);
   e_object_unref(E_OBJECT(ic->border));
   free(ic);
}

static void
_taskbar_icon_fill(Taskbar_Icon *ic)
{
   char *label;

   ic->o_icon = e_border_icon_add(ic->border, evas_object_evas_get(ic->taskbar->o_box));
   edje_object_part_swallow(ic->o_holder, "item", ic->o_icon);
   evas_object_pass_events_set(ic->o_icon, 1);
   evas_object_show(ic->o_icon);
   ic->o_icon2 = e_border_icon_add(ic->border, evas_object_evas_get(ic->taskbar->o_box));
   edje_object_part_swallow(ic->o_holder2, "item", ic->o_icon2);
   evas_object_pass_events_set(ic->o_icon2, 1);
   evas_object_show(ic->o_icon2);

   label = ic->border->client.netwm.name;
   if (!label)
      label = ic->border->client.icccm.title;
   if (!label)
      label = "?";
   edje_object_part_text_set(ic->o_holder, "label", label);
   edje_object_part_text_set(ic->o_holder2, "label", label);
}

static void
_taskbar_icon_empty(Taskbar_Icon *ic)
{
   if (ic->o_icon)
      evas_object_del(ic->o_icon);
   if (ic->o_icon2)
      evas_object_del(ic->o_icon2);
   ic->o_icon = NULL;
   ic->o_icon2 = NULL;
}

static void
_taskbar_icon_signal_emit(Taskbar_Icon *ic, char *sig, char *src)
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

static void
_taskbar_cb_obj_moveresize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;

   inst = (Instance *)data;
   _taskbar_resize_handle(inst->taskbar);
   
}

static void
_taskbar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Taskbar *b;
   Config_Item *ci;

   b = (Taskbar *)data;
   ci = _taskbar_config_item_get(b->inst->gcc->id);
   _config_taskbar_module(ci);
}

void
_taskbar_config_updated(const char *id)
{
   Evas_List *l;
   Config_Item *ci;
   if (!taskbar_config) return;
   ci = _taskbar_config_item_get(id);
   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;
	inst = l->data;
	if (!inst->gcc->id) continue;
	if (!strcmp(inst->gcc->id, ci->id))
	  {
	     for (l = inst->taskbar->icons; l; l = l->next)
	       {
		  if (!ci->show_label)
		    edje_object_signal_emit(l->data, "label_hidden", "");
		  else
		    edje_object_signal_emit(l->data, "label_visible", "");

	       }

	     _taskbar_empty(inst->taskbar);
	     _taskbar_fill(inst->taskbar);
	     _taskbar_resize_handle(inst->taskbar);
	     _gc_orient(inst->gcc);
	     break;
	  }
     }

}

static void
_taskbar_cb_menu_post(void *data, E_Menu *m)
{
   if (!taskbar_config->menu)
      return;
   e_object_del(E_OBJECT(taskbar_config->menu));
   taskbar_config->menu = NULL;
}

static void
_taskbar_cb_icon_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Taskbar_Icon *ic;
   Config_Item *ci;

   ic = (Taskbar_Icon *)data;
   ci = _taskbar_config_item_get(ic->taskbar->inst->gcc->id);
   _taskbar_icon_signal_emit(ic, "active", "");
   if (ci->show_label)
      _taskbar_icon_signal_emit(ic, "label_active", "");
}

static void
_taskbar_cb_icon_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Taskbar_Icon *ic;
   Config_Item *ci;

   ic = (Taskbar_Icon *)data;
   ci = _taskbar_config_item_get(ic->taskbar->inst->gcc->id);
   _taskbar_icon_signal_emit(ic, "passive", "");
   if (ci->show_label)
      _taskbar_icon_signal_emit(ic, "label_passive", "");
}

static void
_taskbar_cb_icon_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{

   Evas_Event_Mouse_Down *ev;
   Taskbar_Icon *ic;

   ic = (Taskbar_Icon *)data;
   ev = event_info;

   if ((ev->button == 3) && (!taskbar_config->menu))
     {

        int cx, cy, cw, ch;

        e_gadcon_canvas_zone_geometry_get(ic->taskbar->inst->gcc->gadcon, &cx, &cy, &cw, &ch);
        if (evas_key_modifier_is_set(ev->modifiers, "Alt"))
          {

             E_Menu *mn;
	     E_Menu_Item *mi;

             mn = e_menu_new();
             e_menu_post_deactivate_callback_set(mn, _taskbar_cb_menu_post, NULL);
             taskbar_config->menu = mn;

	     mi = e_menu_item_new(mn);
	     e_menu_item_label_set(mi, D_("Configuration"));
	     e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	     e_menu_item_callback_set(mi, _taskbar_cb_menu_configure, ic->taskbar);

             /* FIXME: other icon options go here too */

             e_gadcon_client_util_menu_items_append(ic->taskbar->inst->gcc, mn, 0);

             e_menu_activate_mouse(mn,
                                   e_util_zone_current_get(e_manager_current_get()),
                                   cx + ev->output.x, cy + ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
          }
        else
          {
             e_int_border_menu_show(ic->border, cx + ev->output.x, cy + ev->output.y, 0, ev->timestamp);

          }

        evas_event_feed_mouse_up(ic->taskbar->inst->gcc->gadcon->evas, ev->button, EVAS_BUTTON_NONE, ev->timestamp, NULL);

     }

}

static void
_taskbar_cb_icon_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   Taskbar_Icon *ic;
   Config_Item *ci;

   ev = event_info;
   ic = data;
   ci = _taskbar_config_item_get(ic->taskbar->inst->gcc->id);
   if (ev->button == 1)
     {
	if (!ic->border->sticky && ci->show_all)
	  e_desk_show(ic->border->desk);
        if (evas_key_modifier_is_set(ev->modifiers, "Alt"))
          {
             if (ic->border->iconic)
                e_border_uniconify(ic->border);
             else
                e_border_iconify(ic->border);
          }
        else if (evas_key_modifier_is_set(ev->modifiers, "Control"))
          {
             if (ic->border->maximized)
                e_border_unmaximize(ic->border, e_config->maximize_policy);
             else
                e_border_maximize(ic->border, e_config->maximize_policy);
          }
        else if (evas_key_modifier_is_set(ev->modifiers, "Shift"))
          {
             if (ic->border->shaded)
                e_border_unshade(ic->border, ic->border->shade.dir);
             else
                e_border_shade(ic->border, ic->border->shade.dir);
          }
        else if (evas_key_modifier_is_set(ev->modifiers, "Super"))
          {
             e_border_act_close_begin(ic->border);
          }
        else
          {
             if (ic->border->iconic)
               {
                  e_border_uniconify(ic->border);
                  e_border_focus_set(ic->border, 1, 1);
               }
             else
               {
                  if (ic->border->focused)
                    {
                       e_border_iconify(ic->border);
                    }
                  else
                    {
                       e_border_raise(ic->border);
                       e_border_focus_set(ic->border, 1, 1);
                    }
               }
          }
     }
   else if (ev->button == 2)
     {
	if (!ic->border->sticky && ci->show_all)
	  e_desk_show(ic->border->desk);
        e_border_raise(ic->border);
        e_border_focus_set(ic->border, 1, 1);
        if (ic->border->maximized)
           e_border_unmaximize(ic->border, e_config->maximize_policy);
        else
           e_border_maximize(ic->border, e_config->maximize_policy);
     }
}

static void
_taskbar_cb_icon_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Taskbar_Icon *ic;
   Evas_Coord x, y;

   ic = data;
   evas_object_geometry_get(ic->o_holder, &x, &y, NULL, NULL);
   evas_object_move(ic->o_holder2, x, y);
   evas_object_raise(ic->o_holder2);
}

static void
_taskbar_cb_icon_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Taskbar_Icon *ic;
   Evas_Coord w, h;

   ic = data;
   evas_object_geometry_get(ic->o_holder, NULL, NULL, &w, &h);
   evas_object_resize(ic->o_holder2, w, h);
   evas_object_raise(ic->o_holder2);
}

static int
_taskbar_cb_event_border_add(void *data, int type, void *event)
{
   E_Event_Border_Add *ev;
   Taskbar *b;
   Evas_List *l;

   ev = event;
   if (ev->border->client.netwm.state.skip_taskbar)
     return 1;
   if (ev->border->user_skip_winlist)
     return 1;
   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	_taskbar_icon_check_add(inst->taskbar, ev->border);
     }
   return 1;
}

static int
_taskbar_cb_event_border_remove(void *data, int type, void *event)
{
   E_Event_Border_Remove *ev;
   Taskbar *b;
   Evas_List *l;

   ev = event;
   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	_taskbar_icon_remove(inst->taskbar, ev->border);
     }
   return 1;
}

static int
_taskbar_cb_event_border_iconify(void *data, int type, void *event)
{
   E_Event_Border_Iconify *ev;
   Taskbar_Icon *ic;
   Evas_List *l;

   ev = event;
   /* do some sort of anim when iconifying */
   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
     	ic = _taskbar_icon_find(inst->taskbar, ev->border);
     	if (!ic)
    	  continue;
     	_taskbar_icon_signal_emit(ic, "iconify", "");
     }
   return 1;
}

static int
_taskbar_cb_event_border_uniconify(void *data, int type, void *event)
{
   E_Event_Border_Uniconify *ev;
   Taskbar_Icon *ic;
   Evas_List *l;

   ev = event;
   /* do some sort of anim when uniconifying */
   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
     	ic = _taskbar_icon_find(inst->taskbar, ev->border);
     	if (!ic)
    	  continue;
     	_taskbar_icon_signal_emit(ic, "uniconify", "");
     }
   return 1;
}

static int
_taskbar_cb_event_border_icon_change(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   Taskbar_Icon *ic;
   Evas_List *l;

   ev = event;
   /* update icon */
   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	ic = _taskbar_icon_find(inst->taskbar, ev->border);
     	if (!ic)
	  continue;
     	_taskbar_icon_empty(ic);
     	_taskbar_icon_fill(ic);
     }
   return 1;
}

static int
_taskbar_cb_event_border_zone_set(void *data, int type, void *event)
{
   E_Event_Border_Zone_Set *ev;
   Taskbar_Icon *ic;
   Config_Item *ci;
   Evas_List *l;

   ev = event;

   for (l = taskbar_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
	ci = _taskbar_config_item_get(inst->gcc->id);
        if ((inst->taskbar->zone == ev->border->zone)||ci->show_all)
          {
	     _taskbar_icon_check_add(inst->taskbar, ev->border);
          }
        else
          {
	     _taskbar_icon_remove(inst->taskbar, ev->border);
          }
     }
   return 1;
}

static int
_taskbar_cb_event_border_desk_set(void *data, int type, void *event)
{
   E_Event_Border_Desk_Set *ev;
   Evas_List *l;
   Config_Item *ci;

   ev = event;

   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	ci = _taskbar_config_item_get(inst->gcc->id);
     	if ((ev->border->desk == e_desk_current_get(ev->border->zone)) || (ev->border->sticky) || (ci->show_all))
     	  {
     	     _taskbar_icon_check_add(inst->taskbar, ev->border);
     	  }
     	else
     	  {
     	     _taskbar_icon_remove(inst->taskbar, ev->border);
     	  }
     }
   return 0;
}

static int
_taskbar_cb_window_focus_in(void *data, int type, void *event)
{
   Ecore_X_Event_Window_Focus_In *ev;
   E_Border *bd;
   Taskbar_Icon *ic;
   Evas_List *l;

   ev = event;
   bd = e_border_find_by_client_window(ev->win);
   if (!bd)
      return 1;

   for (l = taskbar_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        ic = _taskbar_icon_find(inst->taskbar, bd);
        if (ic)
           _taskbar_icon_signal_emit(ic, "focused", "");
     }
   return 1;
}
static int
_taskbar_cb_window_focus_out(void *data, int type, void *event)
{
   Ecore_X_Event_Window_Focus_Out *ev;
   E_Border *bd;
   Taskbar_Icon *ic;
   Evas_List *l;

   ev = event;
   bd = e_border_find_by_client_window(ev->win);
   if (!bd)
      return 1;

   for (l = taskbar_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        ic = _taskbar_icon_find(inst->taskbar, bd);
        if (ic)
           _taskbar_icon_signal_emit(ic, "unfocused", "");
     }
   return 1;
}

static int
_taskbar_cb_window_property(void *data, int type, void *event)
{
   Ecore_X_Event_Window_Property *ev;
   E_Border *bd;
   Taskbar_Icon *ic;
   Evas_List *l;
   char *label;
   
   ev = event;
   bd = e_border_find_by_client_window(ev->win);
   if (!bd)
     return 1;

   for (l = taskbar_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        ic = _taskbar_icon_find(inst->taskbar, bd);
        if (ic) {
	     label = ic->border->client.netwm.name;
	     if (!label)
	       label = ic->border->client.icccm.title;
	     if (!label)
	       label = "?";
	     edje_object_part_text_set(ic->o_holder, "label", label);
	     edje_object_part_text_set(ic->o_holder2, "label", label);
	     _gc_orient(inst->gcc);

	}
     }
}

static int
_taskbar_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   Evas_List *l;
   Config_Item *ci;

   ev = event;
   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	ci = _taskbar_config_item_get(inst->gcc->id);
	if ((inst->taskbar->zone == ev->desk->zone) && !(ci->show_all))
	  {
     	     _taskbar_empty(inst->taskbar);
     	     _taskbar_fill(inst->taskbar);
     	     _taskbar_empty_handle(inst->taskbar);
     	     _taskbar_resize_handle(inst->taskbar);
     	     _gc_orient(inst->gcc);
	  }
     }
   return 1;
}

static Config_Item *
_taskbar_config_item_get(const char *id)
{
   Evas_List *l;
   Config_Item *ci;

   for (l = taskbar_config->items; l; l = l->next)
     {
        ci = l->data;
        if (!ci->id) continue;
        if (!strcmp(ci->id, id)) return ci;
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->show_label = 1;
   ci->show_all   = 0;

   taskbar_config->items = evas_list_append(taskbar_config->items, ci);
   return ci;
}


/***************************************************************************/
 /**/
/* module setup */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Taskbar"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Taskbar_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
  E_CONFIG_VAL(D, T, id, STR);
  E_CONFIG_VAL(D, T, show_label, INT);
  E_CONFIG_VAL(D, T, show_all, INT);

  conf_edd = E_CONFIG_DD_NEW("TClock_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
  E_CONFIG_LIST(D, T, items, conf_item_edd);
  
   taskbar_config = e_config_domain_load("module.taskbar", conf_edd);
   if (!taskbar_config) {
	Config_Item *ci;
	taskbar_config = E_NEW(Config, 1);
	ci = E_NEW(Config_Item, 1);
	ci->id = evas_stringshare_add("0");
	ci->show_label = 1;
	ci->show_all   = 0;

	taskbar_config->items = evas_list_append(taskbar_config->items, ci);
   }
   
   taskbar_config->module = m;

   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_ADD, _taskbar_cb_event_border_add, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_REMOVE, _taskbar_cb_event_border_remove, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_ICONIFY, _taskbar_cb_event_border_iconify, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_UNICONIFY, _taskbar_cb_event_border_uniconify, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_ICON_CHANGE, _taskbar_cb_event_border_icon_change, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_DESK_SET, _taskbar_cb_event_border_desk_set, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_ZONE_SET, _taskbar_cb_event_border_zone_set, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (ECORE_X_EVENT_WINDOW_FOCUS_IN, _taskbar_cb_window_focus_in, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (ECORE_X_EVENT_WINDOW_FOCUS_OUT, _taskbar_cb_window_focus_out, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (ECORE_X_EVENT_WINDOW_PROPERTY, _taskbar_cb_window_property, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_DESK_SHOW, _taskbar_cb_event_desk_show, NULL));

   e_gadcon_provider_register(&_gadcon_class);
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_gadcon_provider_unregister(&_gadcon_class);

   while (taskbar_config->config_dialog)
     e_object_del(E_OBJECT(taskbar_config->config_dialog));

   while (taskbar_config->handlers)
     {
        ecore_event_handler_del(taskbar_config->handlers->data);
        taskbar_config->handlers = evas_list_remove_list(taskbar_config->handlers, taskbar_config->handlers);
     }
   if (taskbar_config->menu)
     {
        e_menu_post_deactivate_callback_set(taskbar_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(taskbar_config->menu));
        taskbar_config->menu = NULL;
     }
   free(taskbar_config);
   taskbar_config = NULL;
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Evas_List *l;

   for (l = taskbar_config->instances; l; l = l->next)
     {
	Instance *inst;
	Config_Item *ci;

	inst = l->data;
	ci = _taskbar_config_item_get(inst->gcc->id);
	if (ci->id) evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->id);
     }
   e_config_domain_save("module.taskbar", conf_edd, taskbar_config);
   return 1;
}


EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(m, D_("Enlightenment Taskbar Module"),
                        D_("This is the Taskbar Iconified Application module for Enlightenment.<br>"
                          "It will hold minimized applications"));
   return 1;
}

 /**/
/***************************************************************************/
