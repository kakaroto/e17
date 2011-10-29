#include <e.h>
#include <Elementary.h>
#include "winlist.h"
#include "main.h"
#include "elfe_config.h"

#define GO_EDGE 0
#define GO_ACTION 1
#define GO_KEY 2
#define GO_MOUSE 3
#define SWIPE_MOVES 12

static E_Action *act = NULL;

typedef struct _E_Comp_Win E_Comp_Win;
typedef struct _Elfe_Winlist_Item Elfe_Winlist_Item;
typedef struct _Elfe_Winlist Elfe_Winlist;
static Eina_Bool show = EINA_FALSE;

struct _E_Comp_Win
{
  EINA_INLIST;

  void                  *c;
  Ecore_X_Window         win;
  E_Border              *bd;
  E_Popup               *pop;
  E_Menu                *menu;
  int                    x, y, w, h;
  struct {
    int                  x, y, w, h;
  } hidden;
  int                    pw, ph;
  int                    border;
  Ecore_X_Pixmap         pixmap;
  Ecore_X_Damage         damage;
  Ecore_X_Visual         vis;
  int                    depth;
  Evas_Object           *obj;
  Evas_Object           *shobj;
  Eina_List             *obj_mirror;
  Ecore_X_Image         *xim;
  void                  *up;
  E_Object_Delfn        *dfn;
  Ecore_X_Sync_Counter   counter;
  Ecore_Timer           *update_timeout;
  Ecore_Timer           *ready_timeout;
  int                    dmg_updates;
  Ecore_X_Rectangle     *rects;
  int                    rects_num;

  Ecore_X_Pixmap        cache_pixmap;
  int                   cache_w, cache_h;
  int                   update_count;
  double                last_visible_time;
  double                last_draw_time;

  int                   pending_count;

  unsigned int         opacity;

  char                 *title, *name, *clas, *role;
  Ecore_X_Window_Type   primary_type;

  Eina_Bool             delete_pending : 1;
  Eina_Bool             hidden_override : 1;

  Eina_Bool             animating : 1;
  Eina_Bool             force : 1;
  Eina_Bool             defer_hide : 1;
  Eina_Bool             delete_me : 1;

  Eina_Bool             visible : 1;
  Eina_Bool             input_only : 1;
  Eina_Bool             override : 1;
  Eina_Bool             argb : 1;
  Eina_Bool             shaped : 1;
  Eina_Bool             update : 1;
  Eina_Bool             redirected : 1;
  Eina_Bool             shape_changed : 1;
  Eina_Bool             native : 1;
  Eina_Bool             drawme : 1;
  Eina_Bool             invalid : 1;
  Eina_Bool             nocomp : 1;
  Eina_Bool             needpix : 1;
  Eina_Bool             needxim : 1;
  Eina_Bool             real_hid : 1;
  Eina_Bool             inhash : 1;
  Eina_Bool             show_ready : 1;
};

struct _Elfe_Winlist_Item
{
   Elfe_Winlist *winlist;
   Eina_List *pads;
   Evas_Object *layout;
   Evas_Object *tb;
   Evas_Object *placeholder;
   Evas_Object *win;
   E_Border *bd;
   int movements;
     struct {
        Evas_Coord x, y;
   } history[SWIPE_MOVES];
   Ecore_Timer *swipe_timer;
   E_Manager_Comp_Source *src;
   E_Manager *man;
};

struct _Elfe_Winlist
{
   Evas_Object *rect;
   Evas_Object *layout;
   Ecore_X_Window input_win;
   Eina_List *windows;
   Eina_List *handlers;
   E_Zone *zone;
};

static void _winlist_del(Elfe_Winlist *winlist);

static Eina_Bool
_scale_cb_key_down(void *data, int type, void *event)
{

   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_scale_cb_key_up(void *data, int type, void *event)
{
   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_scale_cb_mouse_down(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;
   Evas_Button_Flags flags = EVAS_BUTTON_NONE;
   Eina_List *l;

   evas_event_feed_mouse_down((Evas *)data, ev->buttons, flags, ev->timestamp, NULL);

   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_scale_cb_mouse_up(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;
   Evas_Button_Flags flags = EVAS_BUTTON_NONE;

   evas_event_feed_mouse_up((Evas *)data, ev->buttons, flags, ev->timestamp, NULL);

   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_scale_cb_mouse_move(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Move *ev = event;

   evas_event_feed_mouse_move((Evas *) data, ev->x, ev->y, ev->timestamp, NULL);

   return ECORE_CALLBACK_DONE;
}


static Eina_Bool
_scale_cb_window_focus_in(void *data, int type __UNUSED__, void *event)
{
   Ecore_X_Event_Window_Focus_In *ev;
   Evas *e = data;

   ev = event;
   evas_focus_in(e);
   evas_event_feed_mouse_in(e, ev->time, NULL);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_swipe_cancel(void *data)
{
   Elfe_Winlist_Item *it = data;

   it->movements = 0;
   it->swipe_timer = NULL;
   return ECORE_CALLBACK_RENEW;
}

static void
_window_mouse_up_cb(void *data,Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Winlist_Item *it = data;

   if (it->swipe_timer)
     {
        ecore_timer_del(it->swipe_timer);
        it->swipe_timer = NULL;
     }
}


static void
_window_mouse_down_cb(void *data, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Winlist_Item *it = data;
   Evas_Event_Mouse_Move *ev = event_info;

   if( it->swipe_timer)
     {
        ecore_timer_del(it->swipe_timer);
     }

   it->swipe_timer = ecore_timer_add(0.4, _swipe_cancel, it);
}


static void
_transit_swipe_del_cb(void *data, Elm_Transit *transit)
{
   Elfe_Winlist_Item *it = data;

   if (!it->bd->lock_close)
     {
        Elfe_Winlist *winlist = it->winlist;
        e_border_act_close_begin(it->bd);
        it->winlist->windows = eina_list_remove(it->winlist->windows,
                                                it);
        evas_object_del(it->layout);
        evas_object_del(it->tb);
        e_object_unref(E_OBJECT(it->bd));
        it->bd = NULL;
        if (it->swipe_timer)
          ecore_timer_del(it->swipe_timer);
        free(it);
        if (!eina_list_count(winlist->windows))
          _winlist_del(winlist);

     }
}

static void
_window_mouse_move_cb(void *data,Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Winlist_Item *it = data;
   Evas_Event_Mouse_Move *ev = event_info;

   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     {
        if (it->movements == SWIPE_MOVES)
          {
             it->movements = 0;
             if( it->swipe_timer)
               {
                  ecore_timer_del(it->swipe_timer);
                  it->swipe_timer = NULL;
               }
             return;
          }
        it->history[it->movements].x = ev->cur.canvas.x;
        it->history[it->movements].y = ev->cur.canvas.y;
        if ((it->history[0].y - it->history[it->movements].y) <= 80)
             it->movements++;
        else
          {
             Elm_Transit *transit;

             it->movements = 0;
             if( it->swipe_timer)
               {
                  ecore_timer_del(it->swipe_timer);
                  it->swipe_timer = NULL;
               }

             transit = elm_transit_add();
             elm_transit_object_add(transit, it->layout);
             elm_transit_effect_translation_add(transit, 0, 0, 0, -1000);
             elm_transit_effect_zoom_add(transit, 1.0, 0.5);
             elm_transit_objects_final_state_keep_set(transit, EINA_TRUE);
             elm_transit_duration_set(transit, 0.3);
             elm_transit_del_cb_set(transit, _transit_swipe_del_cb, it);
             elm_transit_go(transit);

             transit = elm_transit_add();
             elm_transit_object_add(transit, it->tb);

             elm_transit_objects_final_state_keep_set(transit, EINA_TRUE);
             elm_transit_duration_set(transit, 0.29);
             elm_transit_go(transit);

          }
     }
}

static void
_transit_win_anim_del_cb(void *data, Elm_Transit *transit)
{
   Elfe_Winlist_Item *it = data;

   elm_object_content_part_set(it->layout, "elfe.swallow.content", it->win);
   if (it->bd->client.netwm.name)
     elm_object_text_part_set(it->layout, "elfe.text.label",
                              it->bd->client.netwm.name);
   else if (it->bd->client.icccm.title)
     elm_object_text_part_set(it->layout, "elfe.text.label",
                              it->bd->client.icccm.title);
}

static void
_item_layout_resize_cb(void *data,Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elfe_Winlist_Item *it = data;
   Elm_Transit *transit;
   Evas_Coord dx, dy, dw, dh;
   Evas_Coord x, y, w, h;

   evas_object_show(it->win);

   evas_object_geometry_get(it->placeholder, &x, &y, &w, &h);

   transit = elm_transit_add();
   elm_transit_object_add(transit, it->win);

   if (it->bd->focused)
     {
        evas_object_resize(it->win, it->bd->w, it->bd->h);
        evas_object_move(it->win, it->bd->x, it->bd->y);
        elm_transit_effect_resizing_add(transit, it->bd->w, it->bd->h, w, h);
        elm_transit_effect_translation_add(transit, it->bd->x, it->bd->y, x, y);
     }
   else
     {
        evas_object_resize(it->win, w, h);
        evas_object_move(it->win, x, y);
        elm_transit_effect_zoom_add(transit, 0.1, 1.0);
     }

   elm_transit_del_cb_set(transit, _transit_win_anim_del_cb, it);
   elm_transit_objects_final_state_keep_set(transit, EINA_TRUE);
   elm_transit_duration_set(transit, 0.5);
   elm_transit_go(transit);
}

static void
_winlist_del(Elfe_Winlist *winlist)
{
   Elfe_Winlist_Item *it;
   Ecore_Event_Handler *h;


   EINA_LIST_FREE(winlist->windows, it)
     {
        e_manager_comp_src_hidden_set(it->man, it->src, EINA_FALSE);
        e_object_unref(E_OBJECT(it->bd));
        if (it->swipe_timer)
          ecore_timer_del(it->swipe_timer);
        free(it);
     }

   EINA_LIST_FREE(winlist->handlers, h)
     {
        ecore_event_handler_del(h);
     }

   e_grabinput_release(winlist->input_win, winlist->input_win);
   ecore_x_window_free(winlist->input_win);
   winlist->input_win = 0;
   evas_object_del(winlist->layout);
   evas_object_del(winlist->rect);
   elfe_home_winlist_show(EINA_TRUE);
   free(winlist);
}

static void
_winlist_layout_mouse_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   _winlist_del(data);
}

static void
_window_mouse_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Elfe_Winlist_Item *it = data;

   ecore_x_netwm_client_active_request(0, it->bd->client.win,
                                       1, 0);

   e_manager_comp_src_hidden_set(it->man, it->src, EINA_FALSE);
   _winlist_del(it->winlist);
}

static void
_elfe_action(const char *params, int modifiers, int method)
{
   E_Manager_Comp_Source *src;
   Eina_List *l;
   Eina_List *handlers;

   if (!params) return;

   if (strcmp(params, "go_elfe_winlist"))
     return;

   elfe_home_winlist_show(show);
   show = ~show;

   if (show)
     {
	E_Manager *man;
	Evas *e;
	Evas_Object *sc, *bx, *win;
	int i;
	Ecore_Event_Handler *h;
        char buf[4096];
        Elfe_Winlist *winlist;

        winlist = calloc(1, sizeof(Elfe_Winlist));

        snprintf(buf, sizeof(buf), "%s/default.edj",
            elfe_home_cfg->mod_dir);

	man = eina_list_data_get(e_manager_list());

	if (!man) return;

	winlist->zone = e_util_zone_current_get(e_manager_current_get());
	winlist->input_win = ecore_x_window_input_new(winlist->zone->container->win,
                                                      0, 0, winlist->zone->w, winlist->zone->h);
	ecore_x_window_show(winlist->input_win);
	if (!e_grabinput_get(winlist->input_win, 0, winlist->input_win))
	  {
	     ecore_x_window_free(winlist->input_win);
	     winlist->input_win = 0;
	     return;
	  }

	e = e_manager_comp_evas_get(man);
	evas_focus_in(e);
	h = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
				    _scale_cb_mouse_down, e);
	handlers = eina_list_append(handlers, h);

	h = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,
				    _scale_cb_mouse_up, e);
	handlers = eina_list_append(handlers, h);

	h = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,
				    _scale_cb_mouse_move, e);
	handlers = eina_list_append(handlers, h);

	h = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
				    _scale_cb_key_down, e);
	handlers = eina_list_append(handlers, h);

	h = ecore_event_handler_add(ECORE_EVENT_KEY_UP,
				    _scale_cb_key_up, e);
	handlers = eina_list_append(handlers, h);

	h = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_IN, _scale_cb_window_focus_in, e);
	handlers = eina_list_append(handlers, h);

        winlist->handlers = handlers;

	e_grabinput_focus(winlist->zone->container->win, E_FOCUS_METHOD_GLOBALLY_ACTIVE);

	winlist->rect = evas_object_rectangle_add(e);
	evas_object_color_set(winlist->rect, 0, 0, 0, 20);
	evas_object_resize(winlist->rect, winlist->zone->w, winlist->zone->h);
	evas_object_move(winlist->rect, 0, 0);
	evas_object_show(winlist->rect);

        winlist->layout = elm_layout_add(winlist->rect);
        elm_object_theme_set(winlist->layout, elfe_theme);
        elm_layout_file_set(winlist->layout, buf, "elfe/winlist/layout");
        evas_object_show(winlist->layout);
        evas_object_move(winlist->layout, winlist->zone->x, winlist->zone->y);
        evas_object_resize(winlist->layout, winlist->zone->w, winlist->zone->h);
        edje_object_signal_callback_add(elm_layout_edje_get(winlist->layout),
                                        "elfe,action,click", "*",
                                        _winlist_layout_mouse_clicked_cb, winlist);


        edje_object_signal_emit(elm_layout_edje_get(winlist->layout),
                                "elfe,winlist,show", "elfe");

        sc = elm_scroller_add(winlist->layout);
	elm_scroller_bounce_set(sc, EINA_TRUE, EINA_FALSE);
	elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_relative_set(sc, 1.0, 1.0);
	evas_object_show(sc);
        elm_object_content_part_set(winlist->layout, "elfe.swallow.windows", sc);

	bx = elm_box_add(sc);
	evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_homogeneous_set(bx, EINA_TRUE);
	elm_box_horizontal_set(bx, EINA_TRUE);
	evas_object_show(bx);

	EINA_LIST_FOREACH((Eina_List *)e_manager_comp_src_list(man), l, src)
	  {
	     E_Comp_Win *cw = (void*)src;
	     Evas_Object *o, *tb, *pad;
             Evas_Object *layout;
	     Evas_Object *sel;
             Elfe_Winlist_Item *item;

	     if (!cw->bd)
               {
                  if (cw->win == winlist->zone->container->bg_win)
                    {
                       Evas_Object *bg;

                       bg = e_manager_comp_src_shadow_get(man, src);
                       evas_object_color_set(bg, 128, 128, 128, 255);
                       e_manager_comp_evas_update(e_manager_current_get());
                    }
                  continue;
               }

             if (cw->pop)
               continue;


             if (e_util_glob_match(cw->bd->client.icccm.class, "Illume*") ||
                 e_util_glob_match(cw->bd->client.icccm.class, "Virtual-Keyboard") )
               continue;

             item = calloc(1, sizeof(Elfe_Winlist_Item));

             winlist->windows = eina_list_append(winlist->windows,
                                                 item);

             item->winlist = winlist;


	     e_manager_comp_src_hidden_set(man, src, EINA_TRUE);

             item->man = man;
             item->src = src;

	     tb = elm_table_add(bx);
	     evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	     evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
	     evas_object_show(tb);

	     pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	     evas_object_size_hint_min_set(pad, winlist->zone->w/2 - 2 * ELFE_DESKTOP_PADDING_W , ELFE_DESKTOP_PADDING_H);
	     elm_table_pack(tb, pad, 1, 0, 1, 1);
	     //evas_object_show(pad);
             item->pads = eina_list_append(item->pads, pad);

	     pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	     evas_object_size_hint_min_set(pad, winlist->zone->w/2 - 2 * ELFE_DESKTOP_PADDING_W, ELFE_DESKTOP_PADDING_H);
	     elm_table_pack(tb, pad, 1, 2, 1, 1);
	     //evas_object_show(pad);
             item->pads = eina_list_append(item->pads, pad);

	     pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	     evas_object_size_hint_min_set(pad, ELFE_DESKTOP_PADDING_W, winlist->zone->h - 2 * ELFE_DESKTOP_PADDING_H);
	     elm_table_pack(tb, pad, 0, 1, 1, 1);
	     //evas_object_show(pad);
             item->pads = eina_list_append(item->pads, pad);

	     pad = evas_object_rectangle_add(evas_object_evas_get(bx));
	     evas_object_size_hint_min_set(pad, ELFE_DESKTOP_PADDING_W,  winlist->zone->h - 2 * ELFE_DESKTOP_PADDING_H);
	     elm_table_pack(tb, pad, 2, 1, 1, 1);
	     //evas_object_show(pad);
             item->pads = eina_list_append(item->pads, pad);

	     evas_object_show(tb);
             item->tb = tb;

	     elm_box_pack_end(bx, tb);


             layout = elm_layout_add(tb);
             elm_layout_file_set(layout, buf, "elfe/winlist/window");
             evas_object_event_callback_add(layout, EVAS_CALLBACK_MOUSE_UP, _window_mouse_up_cb, item);
             evas_object_event_callback_add(layout, EVAS_CALLBACK_MOUSE_DOWN, _window_mouse_down_cb, item);
             evas_object_event_callback_add(layout, EVAS_CALLBACK_MOUSE_MOVE, _window_mouse_move_cb, item);
             edje_object_signal_callback_add(elm_layout_edje_get(layout),
                                             "elfe,action,click", "*",
                                             _window_mouse_clicked_cb, item);

             evas_object_size_hint_align_set(layout, -1, -1);
             evas_object_show(layout);

             win = evas_object_image_filled_add(e);
	     o = e_manager_comp_src_image_get(man, src);
	     evas_object_image_source_set(win, o);
	     evas_object_image_smooth_scale_set(win, evas_object_image_smooth_scale_get(o));
             evas_object_size_hint_max_set(win,
                                           cw->bd->w/2.5,
                                           cw->bd->h/2.5);
             evas_object_size_hint_aspect_set(win,
                                              EVAS_ASPECT_CONTROL_BOTH,
                                              cw->bd->w/2, cw->bd->h/2);

             item->layout = layout;
             item->bd = cw->bd;
	     e_object_ref(E_OBJECT(item->bd));
             item->win = win;

             item->placeholder = evas_object_rectangle_add(e);
             evas_object_color_set(item->placeholder, 0, 0, 0, 255);
             evas_object_size_hint_max_set(item->placeholder,
                                           cw->bd->w/2.5,
                                           cw->bd->h/2.5);
             evas_object_size_hint_aspect_set(item->placeholder,
                                              EVAS_ASPECT_CONTROL_BOTH,
                                              cw->bd->w/2, cw->bd->h/2);
             elm_object_content_part_set(layout, "elfe.swallow.content", item->placeholder);
             elm_table_pack(tb, item->layout, 1, 1, 1, 1);
             evas_object_show(item->placeholder);
             evas_object_event_callback_add(item->placeholder, EVAS_CALLBACK_RESIZE, _item_layout_resize_cb, item);
	     evas_object_show(tb);
	  }

	evas_object_show(bx);
	elm_scroller_content_set(sc, bx);
     }
}

static void
_elfe_winlist_edge_cb(E_Object *obj, const char *params, E_Event_Zone_Edge *ev)
{
   _elfe_action(params, 0, GO_EDGE);
}

static void
_elfe_winlist_action_cb(E_Object *obj, const char *params)
{
   _elfe_action(params, 0, GO_ACTION);
}

static void
_elfe_winlist_key_cb(E_Object *obj, const char *params, Ecore_Event_Key *ev)
{
   _elfe_action(params, ev->modifiers, GO_KEY);
}

static void
_elfe_winlist_mouse_cb(E_Object *obj, const char *params, Ecore_Event_Mouse_Button *ev)
{
   _elfe_action(params, 0, GO_MOUSE);
}

/* called from the module core */
EAPI int
elfe_winlist_init(void)
{
   act = e_action_add("elfe-winlist");
   if (act)
     {
	act->func.go = _elfe_winlist_action_cb;
	act->func.go_key = _elfe_winlist_key_cb;
	act->func.go_mouse = _elfe_winlist_mouse_cb;
	act->func.go_edge = _elfe_winlist_edge_cb;

	e_action_predef_name_set("Elfe windows list", "Elfe windows list","elfe-winlist", "go_elfe_winlist", NULL, 0);
     }

   return 1;
}

EAPI int
elfe_winlist_shutdown(void)
{
   if (act)
     {
	e_action_predef_name_del("Elfe windows list", "Elfe windows list");
	e_action_del("elfe-winlist");
     }
}

EAPI Evas_Object *
elfe_winlist_add(Evas_Object *parent)
{
    return NULL;
}

