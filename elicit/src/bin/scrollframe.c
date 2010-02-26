/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <Edje.h>
#include <string.h>
#include "pan.h"
#include "scrollframe.h"

#define SMART_NAME "scrollframe"
#define API_ENTRY Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;
typedef struct _E_Smart_Data Smart_Data;

struct _E_Smart_Data
{ 
   Evas_Coord   x, y, w, h;
   
   Evas_Object *smart_obj;
   Evas_Object *child_obj;
   Evas_Object *pan_obj;
   Evas_Object *edje_obj;
   Evas_Object *event_obj;

   Scrollframe_Policy hbar_flags, vbar_flags;
   
   struct {
      Evas_Coord w, h;
   } child;
   struct {
      Evas_Coord x, y;
   } step, page;

   struct {
      void (*set) (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
      void (*get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
      void (*max_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
      void (*child_size_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
      void (*fill_policy_set) (Evas_Object *obj, int fill_x, int fill_y);
   } pan_func;
   
   unsigned char hbar_visible : 1;
   unsigned char vbar_visible : 1;
   unsigned char extern_pan : 1;
}; 

/* local subsystem functions */
static void _smart_child_del_hook(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _smart_pan_changed_hook(void *data, Evas_Object *obj, void *event_info);
static void _smart_pan_pan_changed_hook(void *data, Evas_Object *obj, void *event_info);
static void _smart_event_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _smart_event_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _smart_edje_drag_v(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _smart_edje_drag_h(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _smart_scrollbar_read(Smart_Data *sd);
static void _smart_scrollbar_reset(Smart_Data *sd);
static int  _smart_scrollbar_bar_h_visibility_adjust(Smart_Data *sd);
static int  _smart_scrollbar_bar_v_visibility_adjust(Smart_Data *sd);
static void _smart_scrollbar_bar_visibility_adjust(Smart_Data *sd);
static void _smart_scrollbar_size_adjust(Smart_Data *sd);
static void _smart_reconfigure(Smart_Data *sd);
static void _smart_add(Evas_Object *obj);
static void _smart_del(Evas_Object *obj);
static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *obj);
static void _smart_hide(Evas_Object *obj);
static void _smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object *obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object *obj);
static void _smart_init(void);

/* local subsystem globals */
static Evas_Smart *_smart = NULL;

/* externally accessible functions */
Evas_Object *
scrollframe_add(Evas *evas)
{
   _smart_init();
   return evas_object_smart_add(evas, _smart);
}

void
scrollframe_theme_set(Evas_Object *obj, const char *file, const char *group)
{
   API_ENTRY return;

   edje_object_file_set(sd->edje_obj, file, group);
}

void
scrollframe_child_set(Evas_Object *obj, Evas_Object *child)
{
   Evas_Coord w, h;
   Evas_Object *o;
   
   API_ENTRY return;
   if (sd->child_obj)
     {
	pan_child_set(sd->pan_obj, NULL);
	evas_object_event_callback_del(sd->child_obj, EVAS_CALLBACK_FREE, _smart_child_del_hook);
     }
   
   sd->child_obj = child;
   if (!child) return;

   if (!sd->pan_obj)
     {
	o = pan_add(evas_object_evas_get(obj));
	sd->pan_obj = o;
	evas_object_smart_callback_add(o, "changed", _smart_pan_changed_hook, sd);
	evas_object_smart_callback_add(o, "pan_changed", _smart_pan_pan_changed_hook, sd);
	evas_object_show(o);
	edje_object_part_swallow(sd->edje_obj, "elicit.scrollframe.content", o);
     }
   
   sd->pan_func.set = pan_set;
   sd->pan_func.get = pan_get;
   sd->pan_func.max_get = pan_max_get;
   sd->pan_func.child_size_get = pan_child_size_get;
   sd->pan_func.fill_policy_set = pan_fill_policy_set;
   
   evas_object_event_callback_add(child, EVAS_CALLBACK_FREE, _smart_child_del_hook, sd);
   pan_child_set(sd->pan_obj, sd->child_obj);
   sd->pan_func.child_size_get(sd->pan_obj, &w, &h);
   sd->child.w = w;
   sd->child.h = h;
   _smart_scrollbar_size_adjust(sd);
   _smart_scrollbar_reset(sd);
}

void
scrollframe_extern_pan_set(Evas_Object *obj, Evas_Object *pan,
			     void (*pan_set) (Evas_Object *obj, Evas_Coord x, Evas_Coord y),
			     void (*pan_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y),
			     void (*pan_max_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y),
			     void (*pan_child_size_get) (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y))
{
   API_ENTRY return;
   
   scrollframe_child_set(obj, NULL);
   if (sd->extern_pan)
     {
	if (sd->pan_obj)
	  {
	     edje_object_part_unswallow(sd->edje_obj, sd->pan_obj);
	     sd->pan_obj = NULL;
	  }
     }
   else
     {
	if (sd->pan_obj)
	  {
	     evas_object_del(sd->pan_obj);
	     sd->pan_obj = NULL;
	  }
     }
   if (!pan)
     {
	sd->extern_pan = 0;
	return;
     }

   sd->pan_obj = pan;
   sd->pan_func.set = pan_set;
   sd->pan_func.get = pan_get;
   sd->pan_func.max_get = pan_max_get;
   sd->pan_func.child_size_get = pan_child_size_get;
   sd->extern_pan = 1;
   evas_object_smart_callback_add(sd->pan_obj, "changed", _smart_pan_changed_hook, sd);
   evas_object_smart_callback_add(sd->pan_obj, "pan_changed", _smart_pan_pan_changed_hook, sd);
   edje_object_part_swallow(sd->edje_obj, "elicit.scrollframe.content", sd->pan_obj);
   evas_object_show(sd->pan_obj);
}
#if 0
void
scrollframe_custom_theme_set(Evas_Object *obj, char *custom_category, char *custom_group)
{
   API_ENTRY return;
   
   e_theme_edje_object_set(sd->edje_obj, custom_category, custom_group);
   if (sd->pan_obj)
     edje_object_part_swallow(sd->edje_obj, "elicit.scrollframe.content", sd->pan_obj);
   sd->vbar_visible = !sd->vbar_visible;
   sd->hbar_visible = !sd->hbar_visible;
   _smart_scrollbar_bar_visibility_adjust(sd);
}

void
scrollframe_custom_edje_file_set(Evas_Object *obj, char *file, char *group)
{
   API_ENTRY return;

   edje_object_file_set(sd->edje_obj, file, group);
   if (sd->pan_obj)
     edje_object_part_swallow(sd->edje_obj, "elicit.scrollframe.content", sd->pan_obj);
   sd->vbar_visible = !sd->vbar_visible;
   sd->hbar_visible = !sd->hbar_visible;
   _smart_scrollbar_bar_visibility_adjust(sd);
}
#endif

void
scrollframe_child_pos_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Evas_Coord mx = 0, my = 0;
   double vx, vy;
   
   API_ENTRY return;
   sd->pan_func.max_get(sd->pan_obj, &mx, &my);
   if (mx > 0) vx = (double)x / (double)mx;
   else vx = 0.0;
   if (vx < 0.0) vx = 0.0;
   else if (vx > 1.0) vx = 1.0;
   if (my > 0) vy = (double)y / (double)my;
   else vy = 0.0;
   if (vy < 0.0) vy = 0.0;
   else if (vy > 1.0) vy = 1.0;
   edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.vbar", 0.0, vy);
   edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.hbar", vx, 0.0);
   sd->pan_func.set(sd->pan_obj, x, y);
}

void
scrollframe_child_pos_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y)
{
   API_ENTRY return;
   sd->pan_func.get(sd->pan_obj, x, y);
}

void
scrollframe_child_region_show(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
   Evas_Coord mx = 0, my = 0, cw = 0, ch = 0, px = 0, py = 0, nx, ny;
   
   API_ENTRY return;
   sd->pan_func.max_get(sd->pan_obj, &mx, &my);
   sd->pan_func.child_size_get(sd->pan_obj, &cw, &ch);
   sd->pan_func.get(sd->pan_obj, &px, &py);
   
   nx = px;
   if (x < px) nx = x;
   else if ((x + w) > (px + (cw - mx)))
     {
	nx = x + w - (cw - mx);
	if (nx > x) nx = x;
     }
   ny = py;
   if (y < py) ny = y;
   else if ((y + h) > (py + (ch - my))) 
     {
	ny = y + h - (ch - my);
	if (ny > y) ny = y;
     }
   if ((nx == px) && (ny == py)) return;
   scrollframe_child_pos_set(obj, nx, ny);
}

void
scrollframe_child_viewport_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
{
   API_ENTRY return;
   edje_object_calc_force(sd->edje_obj);
   evas_object_geometry_get(sd->pan_obj, NULL, NULL, w, h);
}

void
scrollframe_step_size_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   API_ENTRY return;
   if (x < 1) x = 1;
   if (y < 1) y = 1;
   sd->step.x = x;
   sd->step.y = y;
   _smart_scrollbar_size_adjust(sd);
}

void
scrollframe_step_size_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y)
{
   API_ENTRY return;
   if (x) *x = sd->step.x;
   if (y) *y = sd->step.y;
}

void
scrollframe_page_size_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   API_ENTRY return;
   sd->page.x = x;
   sd->page.y = y;
   _smart_scrollbar_size_adjust(sd);
}

void
scrollframe_page_size_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y)
{
   API_ENTRY return;
   if (x) *x = sd->page.x;
   if (y) *y = sd->page.y;
}

void
scrollframe_policy_set(Evas_Object *obj, Scrollframe_Policy hbar, Scrollframe_Policy vbar)
{
   API_ENTRY return;
   if ((sd->hbar_flags == hbar) && (sd->vbar_flags == vbar)) return;
   sd->hbar_flags = hbar;
   sd->vbar_flags = vbar;
   _smart_scrollbar_size_adjust(sd);
}

void
scrollframe_policy_get(Evas_Object *obj, Scrollframe_Policy *hbar, Scrollframe_Policy *vbar)
{
   API_ENTRY return;
   if (hbar) *hbar = sd->hbar_flags;
   if (vbar) *vbar = sd->vbar_flags;
}

Evas_Object *
scrollframe_edje_object_get(Evas_Object *obj)
{
   API_ENTRY return NULL;
   return sd->edje_obj;
}

void
scrollframe_fill_policy_set(Evas_Object *obj, int fill_x, int fill_y)
{
   API_ENTRY return;
   if (sd->pan_func.fill_policy_set)
     sd->pan_func.fill_policy_set(sd->pan_obj, fill_x, fill_y);
}

void
scrollframe_scroll_pos_set(Evas_Object *obj, float vx, float vy)
{
   Evas_Coord mx = 0, my = 0;
   API_ENTRY return;

   sd->pan_func.max_get(sd->pan_obj, &mx, &my);
   if (vx < 0.0) vx = 0.0;
   else if (vx > 1.0) vx = 1.0;
   if (vy < 0.0) vy = 0.0;
   else if (vy > 1.0) vy = 1.0;
   edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.vbar", 0.0, vy);
   edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.hbar", vx, 0.0);
   sd->pan_func.set(sd->pan_obj, vx * mx, vy * my);

}

/* local subsystem functions */
static void
_smart_edje_drag_v(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Smart_Data *sd;
   
   sd = data;
   _smart_scrollbar_read(sd);
}

static void
_smart_edje_drag_h(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Smart_Data *sd;
   
   sd = data;
   _smart_scrollbar_read(sd);
}

static void
_smart_child_del_hook(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Smart_Data *sd;
   
   sd = data;
   sd->child_obj = NULL;
   _smart_scrollbar_size_adjust(sd);
   _smart_scrollbar_reset(sd);
}

static void
_smart_pan_changed_hook(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Coord w, h;
   Smart_Data *sd;
   
   sd = data;
   sd->pan_func.child_size_get(sd->pan_obj, &w, &h);
   if ((w != sd->child.w) || (h != sd->child.h))
     {
	sd->child.w = w;
	sd->child.h = h;
	_smart_scrollbar_size_adjust(sd);
     }
}

static void
_smart_pan_pan_changed_hook(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Coord x, y;
   Smart_Data *sd;
   
   sd = data;
   sd->pan_func.get(sd->pan_obj, &x, &y);
   scrollframe_child_pos_set(sd->smart_obj, x, y);
}

static void
_smart_event_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Wheel *ev;
   Smart_Data *sd;
   Evas_Coord x = 0, y = 0;

   sd = data;
   ev = event_info;
   scrollframe_child_pos_get(sd->smart_obj, &x, &y);
   y += ev->z * sd->step.y;
   scrollframe_child_pos_set(sd->smart_obj, x, y);
}

static void
_smart_event_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev;
   Smart_Data *sd;
   Evas_Coord x = 0, y = 0, vw = 0, vh = 0, mx = 0, my = 0;
   
   sd = data;
   ev = event_info;
   scrollframe_child_pos_get(sd->smart_obj, &x, &y);
   sd->pan_func.max_get(sd->pan_obj, &mx, &my);
   evas_object_geometry_get(sd->pan_obj, NULL, NULL, &vw, &vh);
   if (!strcmp(ev->keyname, "Left"))
     x -= sd->step.x;
   else if (!strcmp(ev->keyname, "Right"))
     x += sd->step.x;
   else if (!strcmp(ev->keyname, "Up"))
     y -= sd->step.y;
   else if (!strcmp(ev->keyname, "Home"))
     y = 0;
   else if (!strcmp(ev->keyname, "End"))
     y = my;
   else if (!strcmp(ev->keyname, "Down"))
     y += sd->step.y;
   else if (!strcmp(ev->keyname, "Prior"))
     {
	if (sd->page.y < 0)
	  y -= -(sd->page.y * vh) / 100;
	else
	  y -= sd->page.y;
     }
   else if (!strcmp(ev->keyname, "Next"))
     {
	if (sd->page.y < 0)
	  y += -(sd->page.y * vh) / 100;
	else
	  y += sd->page.y;
     }
   scrollframe_child_pos_set(sd->smart_obj, x, y);
}

static void
_smart_scrollbar_read(Smart_Data *sd)
{
   Evas_Coord x, y, mx = 0, my = 0;
   double vx, vy;
   
   edje_object_part_drag_value_get(sd->edje_obj, "elicit.scrollframe.vbar", NULL, &vy);
   edje_object_part_drag_value_get(sd->edje_obj, "elicit.scrollframe.hbar", &vx, NULL);
   sd->pan_func.max_get(sd->pan_obj, &mx, &my);
   x = vx * (double)mx;
   y = vy * (double)my;
   sd->pan_func.set(sd->pan_obj, x, y);
}

static void
_smart_scrollbar_reset(Smart_Data *sd)
{
   edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.vbar", 0.0, 0.0);
   edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.hbar", 0.0, 0.0);
   if ((!sd->child_obj) && (!sd->extern_pan))
     {
	edje_object_part_drag_size_set(sd->edje_obj, "elicit.scrollframe.vbar", 1.0, 1.0);
	edje_object_part_drag_size_set(sd->edje_obj, "elicit.scrollframe.hbar", 1.0, 1.0);
     }
   sd->pan_func.set(sd->pan_obj, 0, 0);
}

static int
_smart_scrollbar_bar_v_visibility_adjust(Smart_Data *sd)
{
   int scroll_v_vis_change = 0;
   Evas_Coord w, h, vw, vh;
   
   w = sd->child.w;
   h = sd->child.h;
   evas_object_geometry_get(sd->pan_obj, NULL, NULL, &vw, &vh);
   if (sd->vbar_visible)
     {
	if (sd->vbar_flags == SCROLLFRAME_POLICY_AUTO)
	  {
	     if ((sd->child_obj) || (sd->extern_pan))
	       {
		  if (h <= vh)
		    {
		       scroll_v_vis_change = 1;
		       sd->vbar_visible = 0;
		    }
	       }
	     else
	       {
		  scroll_v_vis_change = 1;
		  sd->vbar_visible = 0;
	       }
	  }
	else if (sd->vbar_flags == SCROLLFRAME_POLICY_OFF)
	  {
	     scroll_v_vis_change = 1;
	     sd->vbar_visible = 0;
	  }
     }
   else
     {
	if (sd->vbar_flags == SCROLLFRAME_POLICY_AUTO)
	  {
	     if ((sd->child_obj) || (sd->extern_pan))
	       {
		  if (h > vh)
		    {
		       scroll_v_vis_change = 1;
		       sd->vbar_visible = 1;
		    }
	       }
	  }
	else if (sd->vbar_flags == SCROLLFRAME_POLICY_ON)
	  {
	     scroll_v_vis_change = 1;
	     sd->vbar_visible = 1;
	  }
     }
   if (scroll_v_vis_change)
     {
	if (sd->vbar_visible)
	  edje_object_signal_emit(sd->edje_obj, "elicit,scrollframe,show,vbar", "e");
	else
	  edje_object_signal_emit(sd->edje_obj, "elicit,scrollframe,hide,vbar", "e");
	edje_object_message_signal_process(sd->edje_obj);
	_smart_scrollbar_size_adjust(sd);
     }
   return scroll_v_vis_change;
}

static int
_smart_scrollbar_bar_h_visibility_adjust(Smart_Data *sd)
{
   int scroll_h_vis_change = 0;
   Evas_Coord w, h, vw, vh;
   
   w = sd->child.w;
   h = sd->child.h;
   evas_object_geometry_get(sd->pan_obj, NULL, NULL, &vw, &vh);
   if (sd->hbar_visible)
     {
	if (sd->hbar_flags == SCROLLFRAME_POLICY_AUTO)
	  {
	     if ((sd->child_obj) || (sd->extern_pan))
	       {
		  if (w <= vw)
		    {
		       scroll_h_vis_change = 1;
		       sd->hbar_visible = 0;
		    }
	       }
	     else
	       {
		  scroll_h_vis_change = 1;
		  sd->hbar_visible = 0;
	       }
	  }
	else if (sd->hbar_flags == SCROLLFRAME_POLICY_OFF)
	  {
	     scroll_h_vis_change = 1;
	     sd->hbar_visible = 0;
	  }
     }
   else
     {
	if (sd->hbar_flags == SCROLLFRAME_POLICY_AUTO)
	  {
	     if ((sd->child_obj) || (sd->extern_pan))
	       {
		  if (w > vw)
		    {
		       scroll_h_vis_change = 1;
		       sd->hbar_visible = 1;
		    }
	       }
	  }
	else if (sd->hbar_flags == SCROLLFRAME_POLICY_ON)
	  {
	     scroll_h_vis_change = 1;
	     sd->hbar_visible = 1;
	  }
     }
   if (scroll_h_vis_change)
     {
	if (sd->hbar_visible)
	  edje_object_signal_emit(sd->edje_obj, "elicit,scrollframe,show,hbar", "e");
	else
	  edje_object_signal_emit(sd->edje_obj, "elicit,scrollframe,hide,hbar", "e");
	edje_object_message_signal_process(sd->edje_obj);
	_smart_scrollbar_size_adjust(sd);
     }
   return scroll_h_vis_change;
}

static void
_smart_scrollbar_bar_visibility_adjust(Smart_Data *sd)
{
   int changed = 0;
   
   changed |= _smart_scrollbar_bar_h_visibility_adjust(sd);
   changed |= _smart_scrollbar_bar_v_visibility_adjust(sd);
   if (changed)
     {
	_smart_scrollbar_bar_h_visibility_adjust(sd);
	_smart_scrollbar_bar_v_visibility_adjust(sd);
     }
}

static void
_smart_scrollbar_size_adjust(Smart_Data *sd)
{
   if ((sd->child_obj) || (sd->extern_pan))
     {
	Evas_Coord x, y, w, h, mx = 0, my = 0, vw = 0, vh = 0;
	double vx, vy, size;
	
	evas_object_geometry_get(sd->pan_obj, NULL, NULL, &vw, &vh);
	w = sd->child.w;
	if (w < 1) w = 1;
	size = (double)vw / (double)w;
	if (size > 1.0)
	  {
	     size = 1.0;
	     edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.hbar", 0.0, 0.0);
	  }
	edje_object_part_drag_size_set(sd->edje_obj, "elicit.scrollframe.hbar", size, 1.0);
	
	h = sd->child.h;
	if (h < 1) h = 1;
	size = (double)vh / (double)h;
	if (size > 1.0)
	  {
	     size = 1.0;
	     edje_object_part_drag_value_set(sd->edje_obj, "elicit.scrollframe.vbar", 0.0, 0.0);
	  }
	edje_object_part_drag_size_set(sd->edje_obj, "elicit.scrollframe.vbar", 1.0, size);

	edje_object_part_drag_value_get(sd->edje_obj, "elicit.scrollframe.hbar", &vx, NULL);
	edje_object_part_drag_value_get(sd->edje_obj, "elicit.scrollframe.vbar", NULL, &vy);
	sd->pan_func.max_get(sd->pan_obj, &mx, &my);
	x = vx * mx;
	y = vy * my;
	
	edje_object_part_drag_step_set(sd->edje_obj, "elicit.scrollframe.hbar", (double)sd->step.x / (double)w, 0.0);
	edje_object_part_drag_step_set(sd->edje_obj, "elicit.scrollframe.vbar", 0.0, (double)sd->step.y / (double)h);
	if (sd->page.x > 0)
	  edje_object_part_drag_page_set(sd->edje_obj, "elicit.scrollframe.hbar", (double)sd->page.x / (double)w, 0.0);
	else
	  edje_object_part_drag_page_set(sd->edje_obj, "elicit.scrollframe.hbar", -((double)sd->page.x * ((double)vw / (double)w)) / 100.0, 0.0);
	if (sd->page.y > 0)
	  edje_object_part_drag_page_set(sd->edje_obj, "elicit.scrollframe.vbar", 0.0, (double)sd->page.y / (double)h);
	else
	  edje_object_part_drag_page_set(sd->edje_obj, "elicit.scrollframe.vbar", 0.0, -((double)sd->page.y * ((double)vh / (double)h)) / 100.0);

	sd->pan_func.set(sd->pan_obj, x, y);
     }
   else
     {
	edje_object_part_drag_size_set(sd->edje_obj, "elicit.scrollframe.vbar", 1.0, 1.0);
	edje_object_part_drag_size_set(sd->edje_obj, "elicit.scrollframe.hbar", 1.0, 1.0);
	sd->pan_func.set(sd->pan_obj, 0, 0);
     }
   _smart_scrollbar_bar_visibility_adjust(sd);
}
                    
static void
_smart_reconfigure(Smart_Data *sd)
{
   evas_object_move(sd->edje_obj, sd->x, sd->y);
   evas_object_resize(sd->edje_obj, sd->w, sd->h);
   evas_object_move(sd->event_obj, sd->x, sd->y);
   evas_object_resize(sd->event_obj, sd->w, sd->h);
   _smart_scrollbar_size_adjust(sd);
}

static void
_smart_add(Evas_Object *obj)
{
   Smart_Data *sd;
   Evas_Object *o;
   
   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   evas_object_smart_data_set(obj, sd);
   
   sd->smart_obj = obj;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   sd->step.x = 32;
   sd->step.y = 32;
   sd->page.x = -50;
   sd->page.y = -50;
   sd->hbar_flags = SCROLLFRAME_POLICY_AUTO;
   sd->vbar_flags = SCROLLFRAME_POLICY_AUTO;
   sd->hbar_visible = 1;
   sd->vbar_visible = 1;
   
   evas_object_event_callback_add(obj, EVAS_CALLBACK_KEY_DOWN, _smart_event_key_down, sd);
   evas_object_propagate_events_set(obj, 0);
   
   o = edje_object_add(evas_object_evas_get(obj));
   sd->edje_obj = o;

   edje_object_signal_callback_add(o, "drag*", "elicit.scrollframe.vbar", _smart_edje_drag_v, sd);
   edje_object_signal_callback_add(o, "drag*", "elicit.scrollframe.hbar", _smart_edje_drag_h, sd);
   evas_object_smart_member_add(o, obj);
   
   o = evas_object_rectangle_add(evas_object_evas_get(obj));
   sd->event_obj = o;
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_WHEEL, _smart_event_wheel, sd);
   evas_object_smart_member_add(o, obj);
   evas_object_repeat_events_set(o, 1);

   sd->pan_func.set = pan_set;
   sd->pan_func.get = pan_get;
   sd->pan_func.max_get = pan_max_get;
   sd->pan_func.child_size_get = pan_child_size_get;
   
   _smart_scrollbar_reset(sd);
}

static void
_smart_del(Evas_Object *obj)
{
   INTERNAL_ENTRY;
   scrollframe_child_set(obj, NULL);
   if (!sd->extern_pan) evas_object_del(sd->pan_obj);
   evas_object_del(sd->edje_obj);
   evas_object_del(sd->event_obj);
   free(sd);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   INTERNAL_ENTRY;
   sd->x = x;
   sd->y = y;
   _smart_reconfigure(sd);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   INTERNAL_ENTRY;
   sd->w = w;
   sd->h = h;
   _smart_reconfigure(sd);
}

static void
_smart_show(Evas_Object *obj)
{
   INTERNAL_ENTRY;
   evas_object_show(sd->edje_obj);
   evas_object_show(sd->event_obj);
}

static void
_smart_hide(Evas_Object *obj)
{
   INTERNAL_ENTRY;
   evas_object_hide(sd->edje_obj);
   evas_object_hide(sd->event_obj);
}

static void
_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   INTERNAL_ENTRY;
   evas_object_color_set(sd->edje_obj, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object *obj, Evas_Object * clip)
{
   INTERNAL_ENTRY;
   evas_object_clip_set(sd->edje_obj, clip);
   evas_object_clip_set(sd->event_obj, clip);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
   INTERNAL_ENTRY;
   evas_object_clip_unset(sd->edje_obj);
   evas_object_clip_unset(sd->event_obj);
}  

/* never need to touch this */

static void
_smart_init(void)
{
   if (_smart) return;
     {
	static const Evas_Smart_Class sc =
	  {
	     SMART_NAME,
	       EVAS_SMART_CLASS_VERSION,
	       _smart_add,
	       _smart_del, 
	       _smart_move,
	       _smart_resize,
	       _smart_show,
	       _smart_hide,
	       _smart_color_set,
	       _smart_clip_set,
	       _smart_clip_unset,
	       NULL,
	       NULL,
	       NULL,
	       NULL
	  };
	_smart = evas_smart_class_new(&sc);
     }
}

