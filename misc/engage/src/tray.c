#include "engage.h"
#include "config.h"
#include "Ecore_X.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY      0

/*
 * This or something similar should probably go into ecore_x
 */
extern Display *_ecore_x_disp;

int
ecore_x_client_message_send(Window win, Atom type, long d0, long d1,
                            long d2, long d3, long d4)
{
    XEvent xev;

    xev.xclient.window = win;
    xev.xclient.type = ClientMessage;
    xev.xclient.message_type = type;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = d0;
    xev.xclient.data.l[1] = d1;
    xev.xclient.data.l[2] = d2;
    xev.xclient.data.l[3] = d3;
    xev.xclient.data.l[4] = d4;

    XSendEvent(_ecore_x_disp, win, False, NoEventMask, &xev);
}

static Display *display;
static Window root;
static int tray_init;

int
od_tray_msg_cb(void *data, int type, void *event)
{
  Ecore_X_Event_Client_Message *ev = event;

  if (type == ECORE_X_EVENT_CLIENT_MESSAGE) {
    if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_OPCODE")) {
      XEvent xevent;
      printf("need to add tray icon for win %s\n",
             ecore_x_window_prop_title_get(ev->data.l[2]));
      printf("ev->data.l: %#lx %#lx %#lx %#lx\n",
             ev->data.l[0], ev->data.l[1], ev->data.l[2], ev->data.l[3]);

#if 0
/*
 * The window should probably be embedded, not sampled
 * A reparenting should be done as part of the embedding
 * but somehow causes trouble atm.
 */
      XReparentWindow (display, ev->data.l[2], od_window, 0, 0);
      ecore_x_window_resize(ev->data.l[2], options.size, options.size);

      OD_Icon *new = od_icon_new_sysicon("","",ev->data.l[2]);
      od_icon_grab(new, ev->data.l[2]);
      od_dock_add_sysicon(new);
#endif
      /* Map the window (will go top-level until reparented) */
      ecore_x_window_show(ev->data.l[2]);

      /* Should proto be set according to clients _XEMBED_INFO? */
      ecore_x_client_message_send(ev->data.l[2], ecore_x_atom_get("_XEMBED"),
                                  CurrentTime, XEMBED_EMBEDDED_NOTIFY,
                                  0, od_window, /*proto*/1);
    } else if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_MESSAGE_DATA")) {
      printf("got message\n");
    }
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
  int scr;

  tray_init = 1;
  display = ecore_x_display_get();
  root = RootWindow (display, DefaultScreen(display));

  selection_atom = ecore_x_atom_get("_NET_SYSTEM_TRAY_S0");
  XSetSelectionOwner (display, selection_atom, od_window, CurrentTime);

  if (XGetSelectionOwner (display, selection_atom) == od_window) {
    printf("am a system tray :) :)\n");

    ecore_x_client_message_send(root, ecore_x_atom_get("MANAGER"),
                                CurrentTime, selection_atom,
                                od_window, 0, 0);
  }

  ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, od_tray_msg_cb, NULL);
  ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, od_tray_msg_cb, NULL);
  ecore_event_handler_add(ECORE_X_EVENT_WINDOW_HIDE, od_tray_msg_cb, NULL);
}
