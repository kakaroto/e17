#include <e.h>
#include "e_mod_main.h"

#include "Ecore_X.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY      0

static int     _engage_tray_cb_msg(void *data, int type, void *event);
void           _engage_tray_active_set(Engage_Bar *eb, int active);

static void    _engage_tray_cb_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _engage_tray_cb_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);

void           _engage_tray_layout(Engage_Bar *eb);
void           _engage_tray_freeze(Engage_Bar *eb);
void           _engage_tray_thaw(Engage_Bar *eb);
extern void    _engage_bar_frame_resize(Engage_Bar *eb);

void
_engage_tray_init(Engage_Bar *eb)
{
   /* FIXME - temp */
   eb->tray = malloc(sizeof(Engage_Tray));
   eb->tray->icons = 0;
   eb->tray->w = 1;
   eb->tray->h = 1;
   eb->tray->wins = NULL;

   eb->tray->tray = evas_object_rectangle_add(eb->evas);

   eb->tray->msg_handler = NULL;
   eb->tray->dst_handler = NULL;
   eb->tray->win = 0;

   evas_object_resize(eb->tray->tray, eb->tray->w, eb->tray->h);
   evas_object_color_set(eb->tray->tray, 180, 0, 0, 255);
   evas_object_show(eb->tray->tray);
   _engage_tray_thaw(eb);

   edje_object_part_swallow(eb->bar_object, "tray", eb->tray->tray);
   _engage_tray_active_set(eb, eb->conf->tray);

}

void
_engage_tray_shutdown(Engage_Bar *eb)
{
   _engage_tray_freeze(eb);
   edje_object_part_unswallow(eb->bar_object, eb->tray->tray);
   _engage_tray_active_set(eb, 0);

   eina_list_free(eb->tray->wins);
   evas_object_del(eb->tray->tray);
   free(eb->tray);
   eb->tray = NULL;
}

void
_engage_tray_active_set(Engage_Bar *eb, int active)
{
   Ecore_X_Window win;
   Display *display;
   Window root;
   char buf[32];
   Atom selection_atom;
   Evas_Coord x, y, w, h;
   Ecore_X_Time time;

   win = 0;
   if (active)
     {
	win = eb->con->bg_win;
	time = ecore_x_current_time_get();
	eb->tray->select_time = time;
     }
   else
     {
	time = eb->tray->select_time;
     }

   display = ecore_x_display_get();
   root = RootWindow (display, DefaultScreen(display));

   snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", DefaultScreen(display));
   selection_atom = ecore_x_atom_get(buf);
   XSetSelectionOwner (display, selection_atom, win, time);

   if (active &&
       XGetSelectionOwner (display, selection_atom) == eb->con->bg_win)
     {
	printf("am a system tray :) :)\n");

	ecore_x_client_message32_send(root, ecore_x_atom_get("MANAGER"),
				      ECORE_X_EVENT_MASK_WINDOW_CONFIGURE,
				      CurrentTime, selection_atom, win, 0, 0);

	evas_object_geometry_get(eb->tray->tray, &x, &y, &w, &h);
	if (w < 1) w = 1;
	if (h < 1) h = 1;
	eb->tray->win = ecore_x_window_new(eb->con->bg_win, x, y, w, h);
	ecore_x_window_container_manage(eb->tray->win);
	ecore_x_window_background_color_set(eb->tray->win, 0xcccc, 0xcccc, 0xcccc);

	eb->tray->msg_handler = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, _engage_tray_cb_msg, eb);
	eb->tray->dst_handler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, _engage_tray_cb_msg, eb);
     }
   else
     {
	if (eb->tray->msg_handler)
	  ecore_event_handler_del(eb->tray->msg_handler);
	if (eb->tray->dst_handler)
	  ecore_event_handler_del(eb->tray->dst_handler);
	if (eb->tray->win)
	  ecore_x_window_del(eb->tray->win);
     }
}

void
_engage_tray_freeze(Engage_Bar *eb)
{
   if (!eb->tray)
     return;
   evas_object_intercept_move_callback_del(eb->tray->tray, _engage_tray_cb_move);
   evas_object_intercept_resize_callback_del(eb->tray->tray, _engage_tray_cb_resize);
}

void
_engage_tray_thaw(Engage_Bar *eb)
{
   if (!eb->tray)
     return;
   evas_object_intercept_move_callback_add(eb->tray->tray, _engage_tray_cb_move, eb);
   evas_object_intercept_resize_callback_add(eb->tray->tray, _engage_tray_cb_resize, eb);
}

static void
_engage_tray_add(Engage_Bar *eb, Ecore_X_Window win)
{

  if (!eb->conf->tray)
    return;

  if (eina_list_data_find(eb->tray->wins, (void *)win))
    return;
  ecore_x_window_show(eb->tray->win);

  /* we want to insert at the end, so as not to move all icons on each add */
  eb->tray->wins = eina_list_append(eb->tray->wins, (void *)win);
  eb->tray->icons++;

  ecore_x_window_resize(win, 24, 24);

  ecore_x_window_reparent(win, eb->tray->win, 0, 0);
  _engage_tray_layout(eb);
  _engage_bar_frame_resize(eb);

  ecore_x_window_show(win);

}

static void
_engage_tray_remove(Engage_Bar *eb, Ecore_X_Window win) {

  if (!eb->conf->tray)
    return;

  if (!win)
    return;
  if (!eina_list_data_find(eb->tray->wins, (void *)win)) /* if was not found */
    return;

  eb->tray->wins = eina_list_remove(eb->tray->wins, (void *)win);
  eb->tray->icons--;
  _engage_tray_layout(eb);
  _engage_bar_frame_resize(eb);
  if (eb->tray->icons == 0)
    ecore_x_window_hide(eb->tray->win);
}

static int
_engage_tray_cb_msg(void *data, int type, void *event)
{
  Ecore_X_Event_Client_Message *ev;
  Ecore_X_Event_Window_Destroy *dst;
  Engage_Bar *eb;

  eb = data;
  if (!eb->conf->tray)
    return 1;
  
  if (type == ECORE_X_EVENT_CLIENT_MESSAGE) {
    ev = event;
    if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_OPCODE")) {
      _engage_tray_add(eb, (Ecore_X_Window) ev->data.l[2]);
      
      /* Should proto be set according to clients _XEMBED_INFO? */
      ecore_x_client_message32_send(ev->data.l[2], ecore_x_atom_get("_XEMBED"),
                                  ECORE_X_EVENT_MASK_NONE, CurrentTime,
                                  XEMBED_EMBEDDED_NOTIFY, 0, eb->con->bg_win, /*proto*/1);

    } else if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_MESSAGE_DATA")) {
      printf("got message\n");
    } 
  } else if (type == ECORE_X_EVENT_WINDOW_DESTROY) {
    dst = event;
    _engage_tray_remove(eb, (Ecore_X_Window) dst->win);
  } 
  
  return 1;
  
}

static void
_engage_tray_cb_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   Engage_Bar *eb;

   eb = data;
   evas_object_move(o, x, y);
   if (eb->conf->tray)
     ecore_x_window_move(eb->tray->win, (int) x, (int) y);
}

static void
_engage_tray_cb_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Engage_Bar *eb;

   eb = data;
   evas_object_resize(o, w, h);

   _engage_tray_layout(eb);
}

void
_engage_tray_layout(Engage_Bar *eb)
{
   Evas_Coord w, h, c, d;
   int x, y;
   Eina_List *wins;
   E_Gadman_Edge edge;

   if (!eb->gmc || !eb->conf || !eb->tray)
     return;

   edge = e_gadman_client_edge_get(eb->gmc);
   if (!eb->conf->tray)
     {
	int w, h;

	if (edge == E_GADMAN_EDGE_BOTTOM || edge == E_GADMAN_EDGE_TOP)
	  {
	     w = 0;
	     h = eb->conf->iconsize;
	  }
	else
	  {
	     w = eb->conf->iconsize;
	     h = 0;
	  }
	edje_object_part_unswallow(eb->bar_object, eb->tray->tray);
	evas_object_resize(eb->tray->tray, w, h);

	edje_extern_object_min_size_set(eb->tray->tray, w, h);
	edje_extern_object_max_size_set(eb->tray->tray, w, h);
	edje_object_part_swallow(eb->bar_object, "tray", eb->tray->tray);
	return;
     }

   h = eb->conf->iconsize;
   if (h < 24)
     h = 24;
   c = (h - (h % 24)) / 24;
   w = (eb->tray->icons / c);
   if ((eb->tray->icons % c) != 0)
     w++;
   w *= 24;

   _engage_tray_freeze(eb);
   edje_object_part_unswallow(eb->bar_object, eb->tray->tray);
   if (edge == E_GADMAN_EDGE_BOTTOM || edge == E_GADMAN_EDGE_TOP) {
     evas_object_resize(eb->tray->tray, w, h);
     ecore_x_window_resize(eb->tray->win, (int) w, (int) h);

     edje_extern_object_min_size_set(eb->tray->tray, w, h);
     edje_extern_object_max_size_set(eb->tray->tray, w, h);
   } else {
     evas_object_resize(eb->tray->tray, h, w);
     ecore_x_window_resize(eb->tray->win, (int) h, (int) w);

     edje_extern_object_min_size_set(eb->tray->tray, h, w);
     edje_extern_object_max_size_set(eb->tray->tray, h, w);
   }
   _engage_tray_thaw(eb);
   edje_object_part_swallow(eb->bar_object, "tray", eb->tray->tray);
   
   x = 0;
   if (edge == E_GADMAN_EDGE_BOTTOM || edge == E_GADMAN_EDGE_RIGHT)
     y = h - 24;
   else
     y = 0;
   d = 0;
   for (wins = eb->tray->wins; wins; wins = wins->next) {
     if (edge == E_GADMAN_EDGE_BOTTOM || edge == E_GADMAN_EDGE_TOP)
       ecore_x_window_move((Ecore_X_Window) wins->data, x, y);
     else
       ecore_x_window_move((Ecore_X_Window) wins->data, y, x);

     d++;
     if (d % c == 0) {
       x += 24;
       if (edge == E_GADMAN_EDGE_BOTTOM || edge == E_GADMAN_EDGE_RIGHT)
         y = h - 24;
       else
         y = 0;
     } else
       if (edge == E_GADMAN_EDGE_BOTTOM || edge == E_GADMAN_EDGE_RIGHT)
         y -= 24;
       else
         y += 24;
   }
}
