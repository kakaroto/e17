#include "engage.h"
#include "config.h"
#include "Ecore_X.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

Display *display;
Window Root;
int tray_init;

int
od_tray_msg_cb(void *data, int type, void *event)
{
  Ecore_X_Event_Client_Message *ev = event;

  if (type == ECORE_X_EVENT_CLIENT_MESSAGE) {
    if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_OPCODE")) {
      XEvent xevent;
      printf("need to add tray icon for win %s\n", ecore_x_window_prop_title_get(ev->data.l[2]));

      XReparentWindow (display, ev->data.l[2], od_window, 0, 0);
      ecore_x_window_resize(ev->data.l[2], options.size, options.size);
      ecore_x_window_prop_layer_set(ev->data.l[2], ECORE_X_WINDOW_LAYER_BELOW);

      OD_Icon *new = od_icon_new_sysicon("","",ev->data.l[2]);
      od_icon_grab(new, ev->data.l[2]);
      od_dock_add_sysicon(new);

      xevent.xclient.window = ev->data.l[2];
      xevent.xclient.type = ClientMessage;
      xevent.xclient.message_type = ecore_x_atom_get("_XEMBED");
      xevent.xclient.format = 32;
      xevent.xclient.data.l[0] = CurrentTime;
      xevent.xclient.data.l[1] = SYSTEM_TRAY_REQUEST_DOCK;
      xevent.xclient.data.l[2] = 0;
      xevent.xclient.data.l[3] = od_window;
      xevent.xclient.data.l[4] = 0;
      XSendEvent (display, Root, False, NoEventMask, &xevent);
      
    }
    if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_CLIENT_MESSAGE"))
      printf("got message\n");
  } else if (type == ECORE_X_EVENT_WINDOW_DESTROY ||
             type == ECORE_X_EVENT_WINDOW_HIDE) {
    /* FIXME - WHY are we never called???? */
    printf("need to remove");
  }
  return 1;

}

void
od_tray_move(OD_Icon *icon)
{
  int x, y, w, h;
  Screen         *scr;
  int             def;
  int             res_x, res_y;

  /* small check, as we are not really integrating yet */
  if (!tray_init)
    return;
  // ecore_x_window_geometry_get(od_window, &x, &y, &w, &h); // no work :(
  def = DefaultScreen(display);
  scr = ScreenOfDisplay(display, def);
  res_x = scr->width;
  res_y = scr->height;
  x = (res_x - options.width) / 2;
  y = res_y - options.height;

  if (icon->data.minwin.window) {
    ecore_x_window_prop_xy_set(icon->data.minwin.window, x + (int) icon->x - options.size + 4, y + (int) icon->y - (options.size / 2));
    /* hack to update icon background */
    ecore_x_window_resize(icon->data.minwin.window, 0, 0);
    ecore_x_window_resize(icon->data.minwin.window, options.size, options.size);
  }

}

void
od_tray_init()
{
  Atom selection_atom;

  tray_init = 1;
  display = (Display *)ecore_x_display_get();

  selection_atom = ecore_x_atom_get("_NET_SYSTEM_TRAY_S0");
  XSetSelectionOwner (display, selection_atom, od_window, CurrentTime);

  if (XGetSelectionOwner (display, selection_atom) == od_window) {
    printf("am a system tray :) :)\n");

    XClientMessageEvent xev;
    int scr = DefaultScreen (display);
    Root = RootWindow (display, scr);
    xev.type = ClientMessage;
    xev.window = Root;
    xev.message_type = XInternAtom (display, "MANAGER", False);

    xev.format = 32;
    xev.data.l[0] = CurrentTime;
    xev.data.l[1] = selection_atom;
    xev.data.l[2] = od_window;
    xev.data.l[3] = 0;  /* manager specific data */
    xev.data.l[4] = 0;  /* manager specific data */
    XSendEvent (display, Root, False, StructureNotifyMask, (XEvent *) & xev);

  }


  ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, od_tray_msg_cb, NULL);
  ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, od_tray_msg_cb, NULL);
  ecore_event_handler_add(ECORE_X_EVENT_WINDOW_HIDE, od_tray_msg_cb, NULL);


}


