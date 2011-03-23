#include <e.h>
#include <Elementary.h>

#include "desktop_page.h"
#include "elfe_config.h"
#include "utils.h"
#include "dock.h"

#define ELFE_DESKTOP_NUM 5
#define ELFE_DESKTOP_PADDING_W 10
#define ELFE_DESKTOP_PADDING_H 10

typedef struct _Elfe_Desktop Elfe_Desktop;

struct _Elfe_Desktop
{
   Evas_Object *layout;
   Evas_Object *sc;
   Evas_Object *dock;
   Eina_List *pads;
   Eina_List *gadgets;
   Evas_Object *selector;
   Eina_List *selector_objs;
   int current_desktop;
   Ecore_Timer *longpress_timer;
   Eina_Bool on_hold;
   Eina_Bool edit_mode;
   Eina_List *overs;
};


static Elfe_Desktop *desk;

static void
_scroller_edge_left_cb(void *data , Evas_Object *obj, void *event_info )
{

}

static void
_scroller_edge_right_cb(void *data , Evas_Object *obj, void *event_info )
{

}


static void
_scroller_scroll_cb(void *data , Evas_Object *obj, void *event_info )
{

}


static void
_scroller_scroll_anim_start_cb(void *data , Evas_Object *obj, void *event_info )
{

}

static void
_scroller_scroll_anim_stop_cb(void *data , Evas_Object *obj, void *event_info )
{
   Elfe_Desktop *desk = data;
   Evas_Coord x, y, w, h;
   int num;
   int i;


   elm_scroller_region_get(desk->sc, &x, &y, &w, &h);


   for (i = 0; i < ELFE_DESKTOP_NUM; i++)
     {
	Evas_Object *o_edje = elm_layout_edje_get(eina_list_nth(desk->selector_objs, i));
	if ( (x < ((i+1) * w)) && (x >= ( i * w)))
	  {
              desk->current_desktop = i;
	     edje_object_signal_emit(o_edje, "select", "elfe");
	  }
	else
	  {
	      edje_object_signal_emit(o_edje, "unselect", "elfe");
	  }
     }
}

static void
_scroller_scroll_drag_start_cb(void *data , Evas_Object *obj, void *event_info )
{

}

static void
_scroller_scroll_drag_stop_cb(void *data , Evas_Object *obj, void *event_info )
{

}

static Eina_Bool
_longpress_timer_cb(void *data)
{
   Elfe_Desktop *desk = data;
   Evas_Object *gad;


   printf("Longpress edit mode\n");
   gad = eina_list_nth(desk->gadgets, desk->current_desktop);

   if (!desk->edit_mode)
     {
	desk->edit_mode = EINA_TRUE;
	elfe_desktop_page_edit_mode_set(gad, EINA_TRUE);
	elfe_dock_edit_mode_set(desk->dock, EINA_TRUE);
	desk->longpress_timer = NULL;
	evas_object_smart_callback_call(desk->layout, "editmode,on", NULL);
     }


   return ECORE_CALLBACK_CANCEL;
}

static void
_scroller_mouse_move_cb(void *data,Evas *evas, Evas_Object *obj, void *event_info)
{
   Elfe_Desktop *desk = data;
   Evas_Event_Mouse_Move *ev = event_info;

   if (!desk->on_hold)
     {
	desk->on_hold = EINA_FALSE;
     }
   if (desk->longpress_timer)
     {
	ecore_timer_del(desk->longpress_timer);
	desk->longpress_timer = NULL;
     }

}


static void
_scroller_mouse_up_cb(void *data,Evas *evas, Evas_Object *obj, void *event_info)
{
   Elfe_Desktop *desk = data;

   desk->on_hold = EINA_FALSE;
   if (desk->longpress_timer)
     {
	ecore_timer_del(desk->longpress_timer);
	desk->longpress_timer = NULL;
     }
}


static void
_scroller_mouse_down_cb(void *data,Evas *evas, Evas_Object *obj, void *event_info)
{
   Elfe_Desktop *desk = data;
   Evas_Event_Mouse_Down *ev = event_info;

   desk->on_hold = EINA_TRUE;
   if (desk->longpress_timer)
     ecore_timer_del(desk->longpress_timer);
   desk->longpress_timer = ecore_timer_add(1.0, _longpress_timer_cb, desk);
}





static void
_cb_object_resize(void *data , Evas *e , Evas_Object *obj, void *event_info )
{
   Elfe_Desktop *desk = data;
   Evas_Coord x, y, w, h;
   Eina_Array *pad;
   Eina_List *l;
   Evas_Object *gad;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   EINA_LIST_FOREACH(desk->pads, l, pad)
     {
	evas_object_size_hint_min_set(eina_array_data_get(pad, 0), w - 2 * ELFE_DESKTOP_PADDING_W , ELFE_DESKTOP_PADDING_H);
	evas_object_size_hint_min_set(eina_array_data_get(pad, 1), w - 2 * ELFE_DESKTOP_PADDING_W , ELFE_DESKTOP_PADDING_H);
	evas_object_size_hint_min_set(eina_array_data_get(pad, 2), ELFE_DESKTOP_PADDING_W , h - 2 * ELFE_DESKTOP_PADDING_H);
	evas_object_size_hint_min_set(eina_array_data_get(pad, 3), ELFE_DESKTOP_PADDING_W ,h - 2 * ELFE_DESKTOP_PADDING_H);

     }
   EINA_LIST_FOREACH(desk->gadgets, l, gad)
     {
	evas_object_size_hint_min_set(gad, w - 2 * ELFE_DESKTOP_PADDING_W,  h - 2 * ELFE_DESKTOP_PADDING_H);
     }

}

void
elfe_desktop_app_add(Evas_Object *obj, Efreet_Menu *menu, Evas_Coord x, Evas_Coord y)
{
    Elfe_Desktop *desk = evas_object_data_get(obj, "elfe_desktop");
    Evas_Object *gad;
    Evas_Coord ox, oy, ow, oh;

    gad = eina_list_nth(desk->gadgets, desk->current_desktop);

    evas_object_geometry_get(desk->dock, &ox, &oy, &ow, &oh);
    if (ELM_RECTS_INTERSECT(ox, oy, ow, oh, x, y, 1, 1))
      elfe_dock_item_app_add(desk->dock, menu, x, y);
    else
      elfe_desktop_page_item_app_add(gad, menu, x, y);
}

void
elfe_desktop_gadget_add(Evas_Object *obj, const char *name, Evas_Coord x, Evas_Coord y)
{
    Elfe_Desktop *desk = evas_object_data_get(obj, "elfe_desktop");
    Evas_Object *gad;
    gad = eina_list_nth(desk->gadgets, desk->current_desktop);
    elfe_desktop_page_item_gadget_add(gad, name, x, y);
}


Evas_Object *
elfe_desktop_add(Evas_Object *parent, E_Zone *zone)
{
   Evas_Object *tb, *pad, *bx, *ly;

   int i;
   Evas_Coord x, y, w, h;
   const char *desktop_name;
   Evas_Object *gad;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/default.edj",
            elfe_home_cfg->mod_dir);

   desk = calloc(1, sizeof(Elfe_Desktop));

   desk->layout = elm_layout_add(parent);
   elm_layout_file_set(desk->layout, buf, "elfe/desktop/layout");

   desk->sc = elm_scroller_add(parent);
   elm_scroller_bounce_set(desk->sc, EINA_TRUE, EINA_FALSE);
   elm_scroller_policy_set(desk->sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_page_relative_set(desk->sc, 1.0, 1.0);

   bx = elm_box_add(desk->sc);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_homogenous_set(bx, EINA_TRUE);
   elm_box_horizontal_set(bx, EINA_TRUE);

   desk->selector = elm_box_add(desk->layout);
   elm_box_homogenous_set(desk->selector, EINA_TRUE);
   elm_box_horizontal_set(desk->selector, EINA_TRUE);
   evas_object_show(desk->selector);

   for (i = 0; i < ELFE_DESKTOP_NUM; i++)
     {
	Eina_Array *pad_arr = eina_array_new(4);;
	Evas_Object *sel;

	tb = elm_table_add(bx);
	evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(tb);

	pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	evas_object_size_hint_min_set(pad, 460, ELFE_DESKTOP_PADDING_H);
	elm_table_pack(tb, pad, 1, 0, 1, 1);
	//evas_object_show(pad);
	eina_array_push(pad_arr, pad);


	pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	evas_object_size_hint_min_set(pad, 460, ELFE_DESKTOP_PADDING_H);
	elm_table_pack(tb, pad, 1, 2, 1, 1);
	//evas_object_show(pad);
	eina_array_push(pad_arr, pad);

	pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	evas_object_size_hint_min_set(pad, ELFE_DESKTOP_PADDING_W, 480);
	elm_table_pack(tb, pad, 0, 1, 1, 1);
	//evas_object_show(pad);
	eina_array_push(pad_arr, pad);

	pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	evas_object_size_hint_min_set(pad, ELFE_DESKTOP_PADDING_W, 480);
	elm_table_pack(tb, pad, 2, 1, 1, 1);
	//evas_object_show(pad);
	eina_array_push(pad_arr, pad);

	desk->pads = eina_list_append(desk->pads, pad_arr);

	evas_object_show(tb);

        elm_box_pack_end(bx, tb);

        desktop_name = eina_stringshare_printf("Elfe Desktop %d", i);
        gad = elfe_desktop_page_add(tb, zone, i, desktop_name);
        evas_object_show(gad);

        desk->gadgets = eina_list_append(desk->gadgets, gad);
        eina_stringshare_del(desktop_name);
        elm_table_pack(tb, gad, 1, 1, 1, 1);

	sel = elm_layout_add(bx);
	elm_layout_file_set(sel, buf, "elfe/desktop/selector");
	evas_object_show(sel);
        evas_object_size_hint_min_set(sel, 32, 32);
	if (!i)
	  {
	     Evas_Object *o_edje;
	     o_edje = elm_layout_edje_get(sel);
	     edje_object_signal_emit(o_edje, "select", "elfe");
	  }
	elm_box_pack_end(desk->selector, sel);

	desk->selector_objs = eina_list_append(desk->selector_objs, sel);


     }

   elm_scroller_content_set(desk->sc, bx);
   evas_object_show(bx);

   desk->dock = elfe_dock_add(bx);
   /* evas_object_size_hint_min_set(desk->dock, 0, 80); */
   /* evas_object_size_hint_max_set(desk->dock, 9999, 80); */

   evas_object_smart_callback_add(desk->sc, "edge,left", _scroller_edge_left_cb, desk);
   evas_object_smart_callback_add(desk->sc, "edge,right", _scroller_edge_right_cb, desk);
   evas_object_smart_callback_add(desk->sc, "scroll", _scroller_scroll_cb, desk);
   evas_object_smart_callback_add(desk->sc, "scroll,anim,start", _scroller_scroll_anim_start_cb, desk);
   evas_object_smart_callback_add(desk->sc, "scroll,anim,stop", _scroller_scroll_anim_stop_cb, desk);
   evas_object_smart_callback_add(desk->sc, "scroll,drag,start", _scroller_scroll_drag_start_cb, desk);
   evas_object_smart_callback_add(desk->sc, "scroll,drag,stop", _scroller_scroll_drag_stop_cb, desk);

   evas_object_event_callback_add(desk->sc, EVAS_CALLBACK_MOUSE_MOVE, _scroller_mouse_move_cb, desk);
   evas_object_event_callback_add(desk->sc, EVAS_CALLBACK_MOUSE_UP, _scroller_mouse_up_cb, desk);
   evas_object_event_callback_add(desk->sc, EVAS_CALLBACK_MOUSE_DOWN, _scroller_mouse_down_cb, desk);


   evas_object_event_callback_add(desk->sc, EVAS_CALLBACK_RESIZE,
				  _cb_object_resize, desk);

   evas_object_data_set(desk->layout, "elfe_desktop", desk);

   elm_layout_content_set(desk->layout, "elfe.swallow.content", desk->sc);
   elm_layout_content_set(desk->layout, "elfe.swallow.selector", desk->selector);
   elm_layout_content_set(desk->layout, "elfe.swallow.dock", desk->dock);

   return desk->layout;

}

void
elfe_desktop_edit_mode_set(Evas_Object *obj, Eina_Bool mode)
{
   Elfe_Desktop *desk = evas_object_data_get(obj, "elfe_desktop");
   Evas_Object *gad;
   int m,n;
   int i,j;
   Eina_List *l;
   char buf[PATH_MAX];
   Evas_Object *over;

   if (desk->edit_mode == mode)
       return;
   
   printf("Elfe desktop edit mode\n");

   desk->edit_mode = mode;

   snprintf(buf, sizeof(buf), "%s/default.edj",
            elfe_home_cfg->mod_dir);

   if (mode)
     {
	Evas_Coord x, y, w, h;
	gad = eina_list_nth(desk->gadgets, desk->current_desktop);
	evas_object_geometry_get(gad, &x, &y, &w, &h);

	m = w / elfe_home_cfg->cols;
	n = h / elfe_home_cfg->rows;

	for (i = 0; i < elfe_home_cfg->cols; i++)
	  for (j = 0; j < elfe_home_cfg->rows; j++)
	    {
	       Evas_Object *o_edje;

	       over = elm_layout_add(desk->sc);
	       elm_layout_file_set(over, buf, "elfe/gadget/places/over");
	       o_edje = elm_layout_edje_get(over);
               if (elfe_desktop_page_pos_is_free(gad, j, i))
                   edje_object_signal_emit(o_edje, "place,busy", "elfe");
               else
                   edje_object_signal_emit(o_edje, "place,free", "elfe");

	       evas_object_resize(o_edje, m, n);
	       evas_object_move(o_edje, x + i*m,  y + j*n);
	       evas_object_show(over);
               desk->overs = eina_list_append(desk->overs, over);
	    }
     }
   else
     {
	EINA_LIST_FREE(desk->overs, over)
	  {
	     evas_object_del(over);
	  }

	desk->overs = NULL;

	EINA_LIST_FOREACH(desk->gadgets, l, gad)
	  {
	     elfe_desktop_page_edit_mode_set(gad, EINA_FALSE);
	     evas_object_smart_callback_call(desk->layout, "editmode,off", desk);
	  }
	elfe_dock_edit_mode_set(desk->dock, EINA_FALSE);
     }
}
