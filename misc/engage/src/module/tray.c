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

static void    _engage_tray_cb_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _engage_tray_cb_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);

static void    _engage_tray_layout(Engage_Bar *eb);
extern void    _engage_bar_frame_resize(Engage_Bar *eb);

void
_engage_tray_init(Engage_Bar *eb)
{
   Display *display;
   Window root;
   Evas_Coord x, y, w, h;
   char buf[32];
   Atom selection_atom;

   /* FIXME - temp */
   eb->tray = malloc(sizeof(Engage_Tray));
   eb->tray->icons = 0;
   eb->tray->w = 1;
   eb->tray->h = 1;
   eb->tray->wins = NULL;

   eb->tray->tray = evas_object_rectangle_add(eb->evas);
   evas_object_resize(eb->tray->tray, eb->tray->w, eb->tray->h);
   evas_object_color_set(eb->tray->tray, 180, 0, 0, 255);
   evas_object_show(eb->tray->tray);
   evas_object_intercept_move_callback_add(eb->tray->tray, _engage_tray_cb_move, eb);
   evas_object_intercept_resize_callback_add(eb->tray->tray, _engage_tray_cb_resize, eb);

  display = ecore_x_display_get();
  root = RootWindow (display, DefaultScreen(display));

  snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", DefaultScreen(display));
  selection_atom = ecore_x_atom_get(buf);
  XSetSelectionOwner (display, selection_atom, eb->con->bg_win, CurrentTime);

  if (XGetSelectionOwner (display, selection_atom) == eb->con->bg_win) {
    printf("am a system tray :) :)\n");

    ecore_x_client_message32_send(root, ecore_x_atom_get("MANAGER"),
                                ECORE_X_EVENT_MASK_WINDOW_CONFIGURE, CurrentTime
,
                                selection_atom, eb->con->bg_win, 0, 0);
  }

  evas_object_geometry_get(eb->tray->tray, &x, &y, &w, &h);
  eb->tray->win = ecore_x_window_new(eb->con->bg_win, x, y, w, h);
  ecore_x_window_container_manage(eb->tray->win);
  ecore_x_window_background_color_set(eb->tray->win, 0xcccc, 0xcccc, 0xcccc);
  ecore_x_window_show(eb->tray->win);
  
  eb->tray->msg_handler = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, _engage_tray_cb_msg, eb);
  eb->tray->dst_handler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, _engage_tray_cb_msg, eb);

}

void
_engage_tray_shutdown(Engage_Bar *eb)
{
   evas_list_free(eb->tray->wins);
   evas_object_del(eb->tray->tray);

   ecore_event_handler_del(eb->tray->msg_handler);
   ecore_event_handler_del(eb->tray->dst_handler);
}

static void
_engage_tray_add(Engage_Bar *eb, Ecore_X_Window win) {

  /* we want to insert at the end, so as not to move all icons on each add */
  eb->tray->wins = evas_list_append(eb->tray->wins, (void *)win);
  eb->tray->icons++;
  ecore_x_window_resize(win, 24, 24);

  ecore_x_window_reparent(win, eb->tray->win, 0, 0);
  _engage_tray_layout(eb);
  _engage_bar_frame_resize(eb);

  ecore_x_window_show(win);

}

static void
_engage_tray_remove(Engage_Bar *eb, Ecore_X_Window win) {

  if (!win)
    return;
  if (!evas_list_find(eb->tray->wins, (void *)win)) /* if was not found */
    return;

  eb->tray->wins = evas_list_remove(eb->tray->wins, (void *)win);
  eb->tray->icons--;
  _engage_tray_layout(eb);
  _engage_bar_frame_resize(eb);
}

static int
_engage_tray_cb_msg(void *data, int type, void *event)
{
  Ecore_X_Event_Client_Message *ev;
  Ecore_X_Event_Window_Destroy *dst;
  Engage_Bar *eb;
  
  eb = data;
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
   ecore_x_window_move(eb->tray->win, (int) x, (int) y);
}

static void
_engage_tray_cb_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Engage_Bar *eb;

   eb = data;
   evas_object_resize(o, w, h);

   e_box_pack_options_set(o,
			  1, 1, /* fill */
			  0, 0, /* expand */
			  0.5, 0.5, /* align */
			  w, h, /* min */
			  w, h /* max */
			  );
   ecore_x_window_resize(eb->tray->win, (int) w, (int) h);
}

static void
_engage_tray_layout(Engage_Bar *eb)
{
   Evas_Coord w, h;
   int x, y;
   Evas_List *wins;
   
   /* FIXME - this is a simple placeholder - need to check the height of the
    * bar and the orientation... */
   h = 24;
   w = eb->tray->icons * 24;
   evas_object_resize(eb->tray->tray, w, h);
   
   x = 0;
   y = 0;
   for (wins = eb->tray->wins; wins; wins = wins->next)
     {
	ecore_x_window_move((Ecore_X_Window) wins->data, x, y);

	x += 24;
     }
}
