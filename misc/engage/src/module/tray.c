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

static int _engage_tray_msg_cb(void *data, int type, void *event);
static void    _engage_bar_cb_tray_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _engage_bar_cb_tray_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);


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
   eb->tray->icons = 1;
   eb->tray->w = 0;
   eb->tray->h = 0;
   eb->tray->wins = NULL;

   eb->tray->tray = evas_object_rectangle_add(eb->evas);
   evas_object_resize(eb->tray->tray, eb->tray->w, eb->tray->h);
   evas_object_color_set(eb->tray->tray, 180, 180, 180, 255);
   evas_object_show(eb->tray->tray);
   evas_object_intercept_move_callback_add(eb->tray->tray, _engage_bar_cb_tray_move, eb);
   evas_object_intercept_resize_callback_add(eb->tray->tray, _engage_bar_cb_tray_resize, eb);

  display = ecore_x_display_get();
  root = RootWindow (display, DefaultScreen(display));

  snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", DefaultScreen(display));
  selection_atom = ecore_x_atom_get(buf);
  XSetSelectionOwner (display, selection_atom, None, CurrentTime);

  if (XGetSelectionOwner (display, selection_atom) == eb->con->win) {
    printf("am a system tray :) :)\n");

    ecore_x_client_message32_send(root, ecore_x_atom_get("MANAGER"),
                                ECORE_X_EVENT_MASK_WINDOW_CONFIGURE, CurrentTime
,
                                selection_atom, eb->con->win, 0, 0);
  }

  evas_object_geometry_get(eb->tray->tray, &x, &y, &w, &h);
  eb->tray->tray_container = ecore_x_window_new(eb->con->win, x, y, w, h);
  ecore_x_window_container_manage(eb->tray->tray_container);
  ecore_x_window_background_color_set(eb->tray->tray_container, 0xcccc, 0xcccc, 0xcccc);
  ecore_x_window_show(eb->tray->tray_container);
  
  ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, _engage_tray_msg_cb, eb);
  ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, _engage_tray_msg_cb, eb);

}

void
_engage_tray_shutdown(Engage_Bar *eb)
{
   evas_list_free(eb->tray->wins);
   evas_object_del(eb->tray->tray);

}

static void
_engage_tray_add(Engage_Bar *eb, Ecore_X_Window win) {

  Display *display;

  display = ecore_x_display_get();
    
  /* we want to insert at the end, so as not to move all icons on each add */
  eb->tray->wins = evas_list_append(eb->tray->wins, &win);
  eb->tray->icons++;
    
  XReparentWindow (display, win, eb->tray->tray_container, 0, 0);
  ecore_x_window_resize(win, 24, 24);
//  od_tray_layout();
//  od_dock_reposition();
  ecore_x_window_show(win);
}

static void
_engage_tray_remove(Engage_Bar *eb, Ecore_X_Window win) {

  if (!evas_list_remove(eb->tray->wins, &win)) /* if was not found */
    return;

  eb->tray->icons--;
//  od_tray_layout();
//  od_dock_reposition();
}

static int
_engage_tray_msg_cb(void *data, int type, void *event)
{
  Ecore_X_Event_Client_Message *ev;
  Ecore_X_Event_Window_Destroy *dst;
  Engage_Bar *eb;
  
  eb = data;
  if (type == ECORE_X_EVENT_CLIENT_MESSAGE) {
    ev = event;
    printf("got message\n");
    if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_OPCODE")) {
printf("add\n");    
      _engage_tray_add(eb, (Ecore_X_Window) ev->data.l[2]);
      
      /* Should proto be set according to clients _XEMBED_INFO? */
      ecore_x_client_message32_send(ev->data.l[2], ecore_x_atom_get("_XEMBED"),
                                  ECORE_X_EVENT_MASK_NONE, CurrentTime,
                                  XEMBED_EMBEDDED_NOTIFY, 0, eb->con->win, /*proto*/1);                              

    } else if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_MESSAGE_DATA")) {
      printf("got message\n");
    } 
  } else if (type == ECORE_X_EVENT_WINDOW_DESTROY) {
    dst = ev;
printf("del\n");
    _engage_tray_remove(eb, (Ecore_X_Window) dst->win);
  } 
  
  return 1;
  
}

static void
_engage_bar_cb_tray_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   Engage_Bar *eb;

   eb = data;
   evas_object_move(o, x, y);
   ecore_x_window_move(eb->tray->tray_container, x, y);
}

static void
_engage_bar_cb_tray_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Engage_Bar *eb;

   eb = data;
   evas_object_resize(o, w, h);
   ecore_x_window_resize(eb->tray->tray_container, w, h);
}

