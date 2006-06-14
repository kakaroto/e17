/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "e_mod_main.h"

/***************************************************************************/
 /**/
/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, char *name, char *id, char *style);
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
   E_Drop_Handler *drop_handler;
   Ecore_Timer *drop_recalc_timer;
};

struct _Taskbar
{
   Instance *inst;
   Evas_Object *o_box;
   Evas_Object *o_drop;
   Evas_Object *o_drop_over;
   Evas_Object *o_empty;
   Taskbar_Icon *ic_drop_before;
   int drop_before;
   Evas_List *icons;
   int show_label;
   E_Zone *zone;
};

struct _Taskbar_Icon
{
   Taskbar *taskbar;
   Evas_Object *o_holder;
   Evas_Object *o_icon;
   Evas_Object *o_holder2;
   Evas_Object *o_icon2;
   E_Border *border;
   struct
   {
      unsigned char start:1;
      unsigned char dnd:1;
      int x, y;
      int dx, dy;
   } drag;
};

static Taskbar *_taskbar_new(Evas *evas, E_Zone * zone);
static void _taskbar_free(Taskbar * b);
static void _taskbar_cb_empty_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_empty_handle(Taskbar * b);
static void _taskbar_fill(Taskbar * b);
static void _taskbar_empty(Taskbar * b);
static void _taskbar_orient_set(Taskbar * b, int horizontal);
static void _taskbar_resize_handle(Taskbar * b);
static void _taskbar_instance_drop_zone_recalc(Instance *inst);
static Taskbar_Icon *_taskbar_icon_find(Taskbar * b, E_Border * bd);
static Taskbar_Icon *_taskbar_icon_at_coord(Taskbar * b, Evas_Coord x, Evas_Coord y);
static Taskbar_Icon *_taskbar_icon_new(Taskbar * b, E_Border * bd);
static void _taskbar_icon_free(Taskbar_Icon * ic);
static void _taskbar_icon_fill(Taskbar_Icon * ic);
static void _taskbar_icon_empty(Taskbar_Icon * ic);
static void _taskbar_icon_signal_emit(Taskbar_Icon * ic, char *sig, char *src);
static Taskbar *_taskbar_zone_find(E_Zone * zone);
static int _taskbar_cb_timer_drop_recalc(void *data);
static void _taskbar_cb_obj_moveresize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_menu_post(void *data, E_Menu *m);
static void _taskbar_cb_icon_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_icon_resize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_drag_finished(E_Drag *drag, int dropped);
static void _taskbar_inst_cb_enter(void *data, const char *type, void *event_info);
static void _taskbar_inst_cb_move(void *data, const char *type, void *event_info);
static void _taskbar_inst_cb_leave(void *data, const char *type, void *event_info);
static void _taskbar_inst_cb_drop(void *data, const char *type, void *event_info);
static int _taskbar_cb_event_border_add(void *data, int type, void *event);
static int _taskbar_cb_event_border_remove(void *data, int type, void *event);
static int _taskbar_cb_event_border_iconify(void *data, int type, void *event);
static int _taskbar_cb_event_border_uniconify(void *data, int type, void *event);
static int _taskbar_cb_event_border_icon_change(void *data, int type, void *event);
static int _taskbar_cb_event_border_zone_set(void *data, int type, void *event);
static int _taskbar_cb_event_border_desk_set(void *data, int type, void *event);
static int _taskbar_cb_window_focus_in(void *data, int type, void *event);
static int _taskbar_cb_window_focus_out(void *data, int type, void *event);
static int _taskbar_cb_event_desk_show(void *data, int type, void *event);

Config *taskbar_config = NULL;

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, char *name, char *id, char *style)
{
   Taskbar *b;
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   Evas_Coord x, y, w, h;
   int cx, cy, cw, ch;
   const char *drop[] = { "enlightenment/border" };

   inst = E_NEW(Instance, 1);

   b = _taskbar_new(gc->evas, gc->zone);
   b->show_label = 1;
   b->inst = inst;
   inst->taskbar = b;
   o = b->o_box;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_taskbar = o;

   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
   evas_object_geometry_get(o, &x, &y, &w, &h);
   inst->drop_handler =
      e_drop_handler_add(inst,
                         _taskbar_inst_cb_enter, _taskbar_inst_cb_move,
                         _taskbar_inst_cb_leave, _taskbar_inst_cb_drop, drop, 1, cx + x, cy + y, w, h);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOVE, _taskbar_cb_obj_moveresize, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, _taskbar_cb_obj_moveresize, inst);
   taskbar_config->instances = evas_list_append(taskbar_config->instances, inst);
   /* FIXME: HACK!!!! */
   inst->drop_recalc_timer = ecore_timer_add(1.0, _taskbar_cb_timer_drop_recalc, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   ecore_timer_del(inst->drop_recalc_timer);
   taskbar_config->instances = evas_list_remove(taskbar_config->instances, inst);
   e_drop_handler_del(inst->drop_handler);
   _taskbar_free(inst->taskbar);
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
        _taskbar_orient_set(inst->taskbar, 1);
        e_gadcon_client_aspect_set(gcc, evas_list_count(inst->taskbar->icons) * 16, 16);
        break;
     case E_GADCON_ORIENT_VERT:
     case E_GADCON_ORIENT_LEFT:
     case E_GADCON_ORIENT_RIGHT:
     case E_GADCON_ORIENT_CORNER_LT:
     case E_GADCON_ORIENT_CORNER_RT:
     case E_GADCON_ORIENT_CORNER_LB:
     case E_GADCON_ORIENT_CORNER_RB:
        _taskbar_orient_set(inst->taskbar, 0);
        e_gadcon_client_aspect_set(gcc, 16, evas_list_count(inst->taskbar->icons) * 16);
        break;
     default:
        break;
     }
   if (evas_list_count(inst->taskbar->icons) < 1)
      e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void)
{
   return _("Taskbar");
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
 /**/ static Taskbar *
_taskbar_new(Evas *evas, E_Zone * zone)
{
   Taskbar *b;

   b = E_NEW(Taskbar, 1);
   b->o_box = e_box_add(evas);

   Evas_Coord w, h;

   evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);

   e_box_homogenous_set(b->o_box, 0);
   e_box_orientation_set(b->o_box, 1);
   e_box_align_set(b->o_box, 0.5, 0.5);
   b->zone = zone;
   _taskbar_fill(b);
   return b;
}

static void
_taskbar_free(Taskbar * b)
{
   _taskbar_empty(b);
   evas_object_del(b->o_box);
   if (b->o_drop)
      evas_object_del(b->o_drop);
   if (b->o_drop_over)
      evas_object_del(b->o_drop_over);
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
        int cx, cy, cw, ch;

        mn = e_menu_new();
        e_menu_post_deactivate_callback_set(mn, _taskbar_cb_menu_post, NULL);
        taskbar_config->menu = mn;

        e_gadcon_client_util_menu_items_append(b->inst->gcc, mn, 0);

        e_gadcon_canvas_zone_geometry_get(b->inst->gcc->gadcon, &cx, &cy, &cw, &ch);
        e_menu_activate_mouse(mn,
                              e_util_zone_current_get(e_manager_current_get()),
                              cx + ev->output.x, cy + ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        evas_event_feed_mouse_up(b->inst->gcc->gadcon->evas, ev->button, EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_taskbar_empty_handle(Taskbar * b)
{
   if (!b->icons)
     {
        if (!b->o_empty)
          {
             Evas_Coord w, h;

             b->o_empty = evas_object_rectangle_add(evas_object_evas_get(b->o_box));
             evas_object_event_callback_add(b->o_empty, EVAS_CALLBACK_MOUSE_DOWN, _taskbar_cb_empty_mouse_down, b);
             evas_object_color_set(b->o_empty, 0, 0, 0, 0);
             evas_object_show(b->o_empty);
             e_box_pack_end(b->o_box, b->o_empty);
             evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);
             if (e_box_orientation_get(b->o_box))
                w = h;
             else
                h = w;
             e_box_pack_options_set(b->o_empty, 1, 1,   /* fill */
                                    1, 1,       /* expand */
                                    0.5, 0.5,   /* align */
                                    w, h,       /* min */
                                    w, h        /* max */
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
_taskbar_fill(Taskbar * b)
{
   Taskbar_Icon *ic;
   E_Border_List *bl;
   E_Border *bd;

   bl = e_container_border_list_first(b->zone->container);
   while ((bd = e_container_border_list_next(bl)))
     {
        if (bd->user_skip_winlist)
           continue;
        if (bd->client.netwm.state.skip_taskbar)
           continue;
        if (((bd->desk == e_desk_current_get(b->zone)) && (bd->zone == b->zone)) || (bd->sticky))
          {
             ic = _taskbar_icon_new(b, bd);
             b->icons = evas_list_append(b->icons, ic);
             e_box_pack_end(b->o_box, ic->o_holder);
             if (bd->iconic)
                _taskbar_icon_signal_emit(ic, "iconify", "");
             if (bd->focused)
                _taskbar_icon_signal_emit(ic, "focused", "");
          }
     }

   _taskbar_empty_handle(b);
   _taskbar_resize_handle(b);
}

static void
_taskbar_empty(Taskbar * b)
{
   while (b->icons)
     {
        _taskbar_icon_free(b->icons->data);
        b->icons = evas_list_remove_list(b->icons, b->icons);
     }
   _taskbar_empty_handle(b);
}

static void
_taskbar_orient_set(Taskbar * b, int horizontal)
{
   e_box_orientation_set(b->o_box, horizontal);
   e_box_align_set(b->o_box, 0.5, 0.5);
}

static void
_taskbar_resize_handle(Taskbar * b)
{
   Evas_List *l;
   Taskbar_Icon *ic;
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
        e_box_pack_options_set(ic->o_holder, 1, 1,      /* fill */
                               0, 0,    /* expand */
                               0.5, 0.5,        /* align */
                               w, h,    /* min */
                               w, h     /* max */
           );
     }
   e_box_thaw(b->o_box);
}

static void
_taskbar_instance_drop_zone_recalc(Instance *inst)
{
   Evas_Coord x, y, w, h;
   int cx, cy, cw, ch;

   evas_object_geometry_get(inst->o_taskbar, &x, &y, &w, &h);
   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
   e_drop_handler_geometry_set(inst->drop_handler, cx + x, cy + y, w, h);
}

static Taskbar_Icon *
_taskbar_icon_find(Taskbar * b, E_Border * bd)
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
_taskbar_icon_at_coord(Taskbar * b, Evas_Coord x, Evas_Coord y)
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
_taskbar_icon_new(Taskbar * b, E_Border * bd)
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
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_MOVE, _taskbar_cb_icon_mouse_move, ic);
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

static void
_taskbar_icon_free(Taskbar_Icon * ic)
{

   if (taskbar_config->menu)
     {
        e_menu_post_deactivate_callback_set(taskbar_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(taskbar_config->menu));
        taskbar_config->menu = NULL;
     }
   if (ic->taskbar->ic_drop_before == ic)
      ic->taskbar->ic_drop_before = NULL;
   _taskbar_icon_empty(ic);
   evas_object_del(ic->o_holder);
   evas_object_del(ic->o_holder2);
   e_object_unref(E_OBJECT(ic->border));
   free(ic);
}

static void
_taskbar_icon_fill(Taskbar_Icon * ic)
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

   /* FIXME: preferences for icon name */
   label = ic->border->client.netwm.name;
   if (!label)
      label = ic->border->client.icccm.title;
   if (!label)
      label = "?";
   edje_object_part_text_set(ic->o_holder, "label", label);
   edje_object_part_text_set(ic->o_holder2, "label", label);
}

static void
_taskbar_icon_empty(Taskbar_Icon * ic)
{
   if (ic->o_icon)
      evas_object_del(ic->o_icon);
   if (ic->o_icon2)
      evas_object_del(ic->o_icon2);
   ic->o_icon = NULL;
   ic->o_icon2 = NULL;
}

static void
_taskbar_icon_signal_emit(Taskbar_Icon * ic, char *sig, char *src)
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

static Taskbar *
_taskbar_zone_find(E_Zone * zone)
{
   Evas_List *l;

   for (l = taskbar_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        if (inst->taskbar->zone == zone)
           return inst->taskbar;
     }
   return NULL;
}

static int
_taskbar_cb_timer_drop_recalc(void *data)
{
   _taskbar_instance_drop_zone_recalc((Instance *)data);
   return 1;
}

static void
_taskbar_cb_obj_moveresize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;

   inst = (Instance *)data;
   _taskbar_resize_handle(inst->taskbar);
   _taskbar_instance_drop_zone_recalc(inst);
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
   Evas_Event_Mouse_In *ev;
   Taskbar_Icon *ic;

   ev = event_info;
   ic = data;
   _taskbar_icon_signal_emit(ic, "active", "");
   if (ic->taskbar->show_label)
      _taskbar_icon_signal_emit(ic, "label_active", "");
}

static void
_taskbar_cb_icon_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   Taskbar_Icon *ic;

   ev = event_info;
   ic = data;
   _taskbar_icon_signal_emit(ic, "passive", "");
   if (ic->taskbar->show_label)
      _taskbar_icon_signal_emit(ic, "label_passive", "");
}

static void
_taskbar_cb_icon_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{

   Evas_Event_Mouse_Down *ev;
   Taskbar_Icon *ic;

   ic = data;
   ev = event_info;

   if ((ev->button == 3) && (!taskbar_config->menu))
     {

        int cx, cy, cw, ch;

        e_gadcon_canvas_zone_geometry_get(ic->taskbar->inst->gcc->gadcon, &cx, &cy, &cw, &ch);
        if (evas_key_modifier_is_set(ev->modifiers, "Alt"))
          {

             E_Menu *mn;

             mn = e_menu_new();
             e_menu_post_deactivate_callback_set(mn, _taskbar_cb_menu_post, NULL);
             taskbar_config->menu = mn;

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

   ev = event_info;
   ic = data;
   if (ev->button == 1)
     {
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
        e_border_raise(ic->border);
        e_border_focus_set(ic->border, 1, 1);
        if (ic->border->maximized)
           e_border_unmaximize(ic->border, e_config->maximize_policy);
        else
           e_border_maximize(ic->border, e_config->maximize_policy);
     }
}

static void
_taskbar_cb_icon_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   Taskbar_Icon *ic;

   ev = event_info;
   ic = data;
   if (ic->drag.start)
     {
        int dx, dy;

        dx = ev->cur.output.x - ic->drag.x;
        dy = ev->cur.output.y - ic->drag.y;
        if (((dx * dx) + (dy * dy)) > (e_config->drag_resist * e_config->drag_resist))
          {
             E_Drag *d;
             Evas_Object *o;
             Evas_Coord x, y, w, h;
             const char *drag_types[] = { "enlightenment/border" };

             ic->drag.dnd = 1;
             ic->drag.start = 0;

             evas_object_geometry_get(ic->o_icon, &x, &y, &w, &h);
             d = e_drag_new(ic->taskbar->inst->gcc->gadcon->zone->container,
                            x, y, drag_types, 1, ic->border, -1, _taskbar_cb_drag_finished);
             o = e_border_icon_add(ic->border, e_drag_evas_get(d));
             e_drag_object_set(d, o);

             e_drag_resize(d, w, h);
             e_drag_start(d, ic->drag.x, ic->drag.y);
             evas_event_feed_mouse_up(ic->taskbar->inst->gcc->gadcon->evas, 1, EVAS_BUTTON_NONE, ecore_x_current_time_get(), NULL);
             e_object_ref(E_OBJECT(ic->border));
             ic->taskbar->icons = evas_list_remove(ic->taskbar->icons, ic);
             _taskbar_resize_handle(ic->taskbar);
             _gc_orient(ic->taskbar->inst->gcc);
             _taskbar_icon_free(ic);
          }
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

static void
_taskbar_cb_drag_finished(E_Drag *drag, int dropped)
{
   E_Border *bd;

   bd = drag->data;
   if (!dropped)
      e_border_uniconify(bd);
   e_object_unref(E_OBJECT(bd));
}

static void
_taskbar_cb_drop_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Taskbar *b;
   Evas_Coord x, y;

   b = data;
   evas_object_geometry_get(b->o_drop, &x, &y, NULL, NULL);
   evas_object_move(b->o_drop_over, x, y);
}

static void
_taskbar_cb_drop_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Taskbar *b;
   Evas_Coord w, h;

   b = data;
   evas_object_geometry_get(b->o_drop, NULL, NULL, &w, &h);
   evas_object_resize(b->o_drop_over, w, h);
}

static void
_taskbar_inst_cb_enter(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Enter *ev;
   Instance *inst;
   Evas_Object *o, *o2;
   Taskbar_Icon *ic;
   int cx, cy, cw, ch;

   ev = event_info;
   inst = data;
   o = edje_object_add(evas_object_evas_get(inst->taskbar->o_box));
   inst->taskbar->o_drop = o;
   o2 = edje_object_add(evas_object_evas_get(inst->taskbar->o_box));
   inst->taskbar->o_drop_over = o2;
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOVE, _taskbar_cb_drop_move, inst->taskbar);
   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, _taskbar_cb_drop_resize, inst->taskbar);
   e_theme_edje_object_set(o, "base/theme/modules/taskbar", "modules/taskbar/drop");
   e_theme_edje_object_set(o2, "base/theme/modules/taskbar", "modules/taskbar/drop_overlay");
   evas_object_layer_set(o2, 19999);
   evas_object_show(o);
   evas_object_show(o2);
   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
   ic = _taskbar_icon_at_coord(inst->taskbar, ev->x - cx, ev->y - cy);
   inst->taskbar->ic_drop_before = ic;
   if (ic)
     {
        Evas_Coord ix, iy, iw, ih;
        int before = 0;

        evas_object_geometry_get(ic->o_holder, &ix, &iy, &iw, &ih);
        if (e_box_orientation_get(inst->taskbar->o_box))
          {
             if ((ev->x - cx) < (ix + (iw / 2)))
                before = 1;
          }
        else
          {
             if ((ev->y - cy) < (iy + (ih / 2)))
                before = 1;
          }
        if (before)
           e_box_pack_before(inst->taskbar->o_box, inst->taskbar->o_drop, ic->o_holder);
        else
           e_box_pack_after(inst->taskbar->o_box, inst->taskbar->o_drop, ic->o_holder);
        inst->taskbar->drop_before = before;
     }
   else
      e_box_pack_end(inst->taskbar->o_box, o);
   e_box_pack_options_set(o, 1, 1,      /* fill */
                          0, 0, /* expand */
                          0.5, 0.5,     /* align */
                          1, 1, /* min */
                          -1, -1        /* max */
      );
   _taskbar_resize_handle(inst->taskbar);
   _gc_orient(inst->gcc);
}

static void
_taskbar_inst_cb_move(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Move *ev;
   Instance *inst;
   Taskbar_Icon *ic;
   int cx, cy, cw, ch;

   ev = event_info;
   inst = data;
   e_box_unpack(inst->taskbar->o_drop);
   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
   ic = _taskbar_icon_at_coord(inst->taskbar, ev->x - cx, ev->y - cy);
   inst->taskbar->ic_drop_before = ic;
   if (ic)
     {
        Evas_Coord ix, iy, iw, ih;
        int before = 0;

        evas_object_geometry_get(ic->o_holder, &ix, &iy, &iw, &ih);
        if (e_box_orientation_get(inst->taskbar->o_box))
          {
             if ((ev->x - cx) < (ix + (iw / 2)))
                before = 1;
          }
        else
          {
             if ((ev->y - cy) < (iy + (ih / 2)))
                before = 1;
          }
        if (before)
           e_box_pack_before(inst->taskbar->o_box, inst->taskbar->o_drop, ic->o_holder);
        else
           e_box_pack_after(inst->taskbar->o_box, inst->taskbar->o_drop, ic->o_holder);
        inst->taskbar->drop_before = before;
     }
   else
      e_box_pack_end(inst->taskbar->o_box, inst->taskbar->o_drop);
   e_box_pack_options_set(inst->taskbar->o_drop, 1, 1,  /* fill */
                          0, 0, /* expand */
                          0.5, 0.5,     /* align */
                          1, 1, /* min */
                          -1, -1        /* max */
      );
   _taskbar_resize_handle(inst->taskbar);
   _gc_orient(inst->gcc);
}

static void
_taskbar_inst_cb_leave(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Leave *ev;
   Instance *inst;

   ev = event_info;
   inst = data;
   inst->taskbar->ic_drop_before = NULL;
   evas_object_del(inst->taskbar->o_drop);
   inst->taskbar->o_drop = NULL;
   evas_object_del(inst->taskbar->o_drop_over);
   inst->taskbar->o_drop_over = NULL;
   _taskbar_resize_handle(inst->taskbar);
   _gc_orient(inst->gcc);
}

static void
_taskbar_inst_cb_drop(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Drop *ev;
   Instance *inst;
   E_Border *bd = NULL;
   Taskbar *b;
   Taskbar_Icon *ic, *ic2;
   Evas_List *l;

   ev = event_info;
   inst = data;
   if (!strcmp(type, "enlightenment/border"))
     {
        bd = ev->data;
        if (!bd)
           return;
     }

   if (!bd->iconic)
      e_border_iconify(bd);

   ic2 = inst->taskbar->ic_drop_before;
   if (ic2)
     {
        /* Add new eapp before this icon */
        if (!inst->taskbar->drop_before)
          {
             for (l = inst->taskbar->icons; l; l = l->next)
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
        if (!ic2)
           goto atend;
        b = inst->taskbar;
        if (_taskbar_icon_find(b, bd))
           return;
        ic = _taskbar_icon_new(b, bd);
        if (!ic)
           return;
        b->icons = evas_list_prepend_relative(b->icons, ic, ic2);
        e_box_pack_before(b->o_box, ic->o_holder, ic2->o_holder);
     }
   else
     {
      atend:
        b = inst->taskbar;
        if (_taskbar_icon_find(b, bd))
           return;
        ic = _taskbar_icon_new(b, bd);
        if (!ic)
           return;
        b->icons = evas_list_append(b->icons, ic);
        e_box_pack_end(b->o_box, ic->o_holder);
     }

   evas_object_del(inst->taskbar->o_drop);
   inst->taskbar->o_drop = NULL;
   evas_object_del(inst->taskbar->o_drop_over);
   inst->taskbar->o_drop_over = NULL;
   _taskbar_empty_handle(b);
   _taskbar_resize_handle(inst->taskbar);
   _gc_orient(inst->gcc);
}

static int
_taskbar_cb_event_border_add(void *data, int type, void *event)
{
   E_Event_Border_Add *ev;
   Taskbar *b;
   Taskbar_Icon *ic;

   ev = event;
   b = _taskbar_zone_find(ev->border->zone);
   if (!b)
      return 1;
   if (_taskbar_icon_find(b, ev->border))
      return 1;
   ic = _taskbar_icon_new(b, ev->border);
   if (!ic)
      return 1;
   if (ev->border->client.netwm.state.skip_taskbar)
      return 1;
   if (ev->border->user_skip_winlist)
      return 1;
   if ((ev->border->desk == e_desk_current_get(ev->border->zone)) || (ev->border->sticky))
     {
        b->icons = evas_list_append(b->icons, ic);
        e_box_pack_end(b->o_box, ic->o_holder);
        _taskbar_empty_handle(b);
        _taskbar_resize_handle(b);
        _gc_orient(b->inst->gcc);
     }
   return 1;
}

static int
_taskbar_cb_event_border_remove(void *data, int type, void *event)
{
   E_Event_Border_Remove *ev;
   Taskbar *b;
   Taskbar_Icon *ic;

   ev = event;
   /* find icon and remove if there */
   b = _taskbar_zone_find(ev->border->zone);
   if (!b)
      return 1;
   ic = _taskbar_icon_find(b, ev->border);
   if (!ic)
      return 1;
   _taskbar_icon_free(ic);
   b->icons = evas_list_remove(b->icons, ic);
   _taskbar_empty_handle(b);
   _taskbar_resize_handle(b);
   _gc_orient(b->inst->gcc);
   return 1;
}

static int
_taskbar_cb_event_border_iconify(void *data, int type, void *event)
{
   E_Event_Border_Iconify *ev;
   Taskbar *b;
   Taskbar_Icon *ic;

   ev = event;
   /* do some sort of anim when iconifying */
   b = _taskbar_zone_find(ev->border->zone);
   if (!b)
      return 1;
   ic = _taskbar_icon_find(b, ev->border);
   if (!ic)
      return 1;
   _taskbar_icon_signal_emit(ic, "iconify", "");
   return 1;
}

static int
_taskbar_cb_event_border_uniconify(void *data, int type, void *event)
{
   E_Event_Border_Uniconify *ev;
   Taskbar *b;
   Taskbar_Icon *ic;

   ev = event;
   /* do some sort of anim when uniconifying */
   b = _taskbar_zone_find(ev->border->zone);
   if (!b)
      return 1;
   ic = _taskbar_icon_find(b, ev->border);
   if (!ic)
      return 1;
   _taskbar_icon_signal_emit(ic, "uniconify", "");
   return 1;
}

static int
_taskbar_cb_event_border_icon_change(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   Taskbar *b;
   Taskbar_Icon *ic;

   ev = event;
   /* update icon */
   b = _taskbar_zone_find(ev->border->zone);
   if (!b)
      return 1;
   ic = _taskbar_icon_find(b, ev->border);
   if (!ic)
      return 1;
   _taskbar_icon_empty(ic);
   _taskbar_icon_fill(ic);

   return 1;
}

static int
_taskbar_cb_event_border_zone_set(void *data, int type, void *event)
{
   E_Event_Border_Zone_Set *ev;
   Taskbar_Icon *ic;

   ev = event;

   Evas_List *l;

   for (l = taskbar_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        if (inst->taskbar->zone == ev->border->zone)
          {
             if ((ev->border->desk == e_desk_current_get(ev->border->zone)) || (ev->border->sticky))
               {
                  ic = _taskbar_icon_find(inst->taskbar, ev->border);
                  if (!ic)
                    {
                       if (ev->border->user_skip_winlist)
                          continue;
                       if (ev->border->client.netwm.state.skip_taskbar)
                          continue;
                       ic = _taskbar_icon_new(inst->taskbar, ev->border);
                       inst->taskbar->icons = evas_list_append(inst->taskbar->icons, ic);
                       e_box_pack_end(inst->taskbar->o_box, ic->o_holder);
                       _taskbar_empty_handle(inst->taskbar);
                       _taskbar_resize_handle(inst->taskbar);
                       _gc_orient(inst->taskbar->inst->gcc);
                    }
               }
          }
        else
          {
             ic = _taskbar_icon_find(inst->taskbar, ev->border);
             if ((ic) && (!ev->border->sticky))
               {
                  _taskbar_icon_free(ic);
                  inst->taskbar->icons = evas_list_remove(inst->taskbar->icons, ic);
                  _taskbar_empty_handle(inst->taskbar);
                  _taskbar_resize_handle(inst->taskbar);
                  _gc_orient(inst->taskbar->inst->gcc);
               }
          }
     }
   return 1;
}

static int
_taskbar_cb_event_border_desk_set(void *data, int type, void *event)
{
   E_Event_Border_Desk_Set *ev;
   Taskbar *b;
   Taskbar_Icon *ic;
   E_Border *bd;

   ev = event;
   bd = ev->border;
   if (!bd)
      return 1;
   /* do some sort of anim when uniconifying */
   b = _taskbar_zone_find(bd->zone);
   if (!b)
      return 1;

   if ((ev->border->desk == e_desk_current_get(ev->border->zone)) || (bd->sticky))
     {
        ic = _taskbar_icon_find(b, bd);
        if (ic)
           return 1;
        ic = _taskbar_icon_new(b, ev->border);
        if (b->zone == ev->border->zone)
          {
             if (bd->user_skip_winlist)
                return 1;
             if (bd->client.netwm.state.skip_taskbar)
                return 1;
             b->icons = evas_list_append(b->icons, ic);
             e_box_pack_end(b->o_box, ic->o_holder);
             _taskbar_empty_handle(b);
             _taskbar_resize_handle(b);
             _gc_orient(b->inst->gcc);
          }
     }
   else
     {
        ic = _taskbar_icon_find(b, bd);
        if (!ic)
           return 1;
        _taskbar_icon_free(ic);
        b->icons = evas_list_remove(b->icons, ic);
        _taskbar_empty_handle(b);
        _taskbar_resize_handle(b);
        _gc_orient(b->inst->gcc);
     }
   return 0;
}

static int
_taskbar_cb_window_focus_in(void *data, int type, void *event)
{
   Ecore_X_Event_Window_Focus_In *ev;
   E_Border *bd;
   Taskbar *b;
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
   Taskbar *b;
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
_taskbar_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *e;
   Taskbar *b;

   e = event;
   b = _taskbar_zone_find(e->desk->zone);
   if (!b)
      return 1;
   _taskbar_empty(b);
   _taskbar_fill(b);
   _taskbar_empty_handle(b);
   _taskbar_resize_handle(b);
   _gc_orient(b->inst->gcc);
   return 1;
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
   taskbar_config = E_NEW(Config, 1);

   taskbar_config->module = m;

   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ADD, _taskbar_cb_event_border_add, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_REMOVE, _taskbar_cb_event_border_remove, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ICONIFY, _taskbar_cb_event_border_iconify, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_UNICONIFY, _taskbar_cb_event_border_uniconify, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ICON_CHANGE, _taskbar_cb_event_border_icon_change, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_DESK_SET, _taskbar_cb_event_border_desk_set, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ZONE_SET, _taskbar_cb_event_border_zone_set, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_IN, _taskbar_cb_window_focus_in, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_OUT, _taskbar_cb_window_focus_out, NULL));
   taskbar_config->handlers = evas_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_DESK_SHOW, _taskbar_cb_event_desk_show, NULL));

/* FIXME: add these later for things taskbar-like functionality   
   taskbar_config->handlers = evas_list_append
     (taskbar_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_DESK_SET, _taskbar_cb_event_border_zone_set, NULL));
   taskbar_config->handlers = evas_list_append
     (taskbar_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_SHOW, _taskbar_cb_event_border_zone_set, NULL));
   taskbar_config->handlers = evas_list_append
     (taskbar_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_HIDE, _taskbar_cb_event_border_zone_set, NULL));
   taskbar_config->handlers = evas_list_append
     (taskbar_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_STACK, _taskbar_cb_event_border_zone_set, NULL));
   taskbar_config->handlers = evas_list_append
     (taskbar_config->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_STICK, _taskbar_cb_event_border_zone_set, NULL));
 */
   e_gadcon_provider_register(&_gadcon_class);
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_gadcon_provider_unregister(&_gadcon_class);

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
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/*
EAPI int
e_modapi_info(E_Module *m)
{
   char buf[4096];
   
   snprintf(buf, sizeof(buf), "%s/module_icon.png", e_module_dir_get(m));
   m->icon_file = strdup(buf);
   return 1;
}*/

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(_("Enlightenment Taskbar Module"),
                        _("This is the Taskbar Iconified Application module for Enlightenment.<br>"
                          "It will hold minimized applications"));
   return 1;
}

 /**/
/***************************************************************************/
