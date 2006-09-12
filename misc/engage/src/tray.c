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

typedef struct _Window_List Window_List;
struct _Window_List {
  Ecore_X_Window win;
  char          *title;
  Window_List   *next;
};

int tray_count = 0;
double tray_x = 0.0;
/* negative so as to not use up space if tray not enabled */
int tray_width = -4;
Window_List *tray_list = NULL;

static Display *display;
static Window root, tray_container;
static int tray_init;

void
od_tray_layout() {
  Window_List *tmp;
  int xpos;
  int oddflag;

  tray_width = ((tray_count + 1) / 2) * 24;
  ecore_x_window_resize(tray_container, tray_width, options.height -
      options.size - ARROW_SPACE_DOUBLE - 1.0);

  tmp = tray_list;
  xpos = 0;
  oddflag = 0;
  while(tmp) {
    ecore_x_window_move(tmp->win, xpos, 24 - oddflag);

    tmp = tmp->next;
    if (oddflag) {
      oddflag = 0;
      xpos += 24;
    } else {
      oddflag = 24;
    }
  }
}

void
od_tray_add(Ecore_X_Window win) {
  Window_List *new, *insert_after;
  
  new = malloc(sizeof(Window_List));
  new->win = win;
  ecore_x_netwm_name_get(win, &new->title);
  if (!new->title)
    new->title = ecore_x_icccm_title_get(win);
  new->next = NULL;

  /* we want to insert at the end, so as not to move all icons on each add */
  insert_after = tray_list;
  if (insert_after)
    while(insert_after->next)
      insert_after = insert_after->next;
  if (!insert_after)
    tray_list = new;
  else
    insert_after->next = new;
  tray_count++;
  
  XReparentWindow (display, win, tray_container, 0, 0);  
  ecore_x_window_resize(win, 24, 24);
  od_tray_layout();
  od_dock_reposition();
  ecore_x_window_show(win);
}

void
od_tray_remove(Ecore_X_Window win) {
  Window_List *tmp, *ptr;

  ptr = NULL;
  tmp = tray_list;
  while (tmp) {
    if (tmp->win == win)
      break;
    ptr = tmp;
    tmp = tmp->next;
  }
  if (!tmp)
    return;
  
  tray_count--;
  if (ptr)
    ptr->next = tmp->next;
  else
    tray_list = tmp->next;
  if (tmp->title)
    free(tmp->title);
  free(tmp);
  od_tray_layout();
  od_dock_reposition();
}

int
od_tray_msg_cb(void *data, int type, void *event)
{
  Ecore_X_Event_Client_Message *ev = event;
  Ecore_X_Event_Window_Destroy *dst = event;

  if (type == ECORE_X_EVENT_CLIENT_MESSAGE) {
    if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_OPCODE")) {

      od_tray_add((Ecore_X_Window) ev->data.l[2]);
      
      /* Should proto be set according to clients _XEMBED_INFO? */
      ecore_x_client_message32_send(ev->data.l[2], ecore_x_atom_get("_XEMBED"),
                                  ECORE_X_EVENT_MASK_NONE, CurrentTime,
                                  XEMBED_EMBEDDED_NOTIFY, 0, od_window, /*proto*/1);

    } else if (ev->message_type == ecore_x_atom_get("_NET_SYSTEM_TRAY_MESSAGE_DATA")) {
      printf("got message\n");
    }
  } else if (type == ECORE_X_EVENT_WINDOW_DESTROY) {
    od_tray_remove((Ecore_X_Window) dst->win);
  }

  return 1;

}

void
od_tray_move(double xx)
{
  /* small check, as we are not really integrating yet */
  if (!tray_init)
    return;

  if (xx == tray_x)
    return;

  ecore_x_window_move(tray_container, xx, options.height - options.size
      - ARROW_SPACE_DOUBLE + 1.0);
  tray_x = xx;
}

void
od_tray_init()
{
  char buf[32];
  Atom selection_atom;

  tray_init = 1;
  display = ecore_x_display_get();
  root = RootWindow (display, DefaultScreen(display));

  snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", DefaultScreen(display));
  selection_atom = ecore_x_atom_get(buf);
  XSetSelectionOwner (display, selection_atom, od_window, CurrentTime);

  if (XGetSelectionOwner (display, selection_atom) == od_window) {
    printf("am a system tray :) :)\n");

    ecore_x_client_message32_send(root, ecore_x_atom_get("MANAGER"),
                                ECORE_X_EVENT_MASK_WINDOW_CONFIGURE, CurrentTime,
                                selection_atom, od_window, 0, 0);
  }

  if(options.use_composite)
     tray_container = ecore_x_window_argb_new(od_window, 0, options.height - 48, 96, 
                                      48);
  else
     tray_container = ecore_x_window_new(od_window, 0, options.height - 48, 96, 
                                      48);
  ecore_x_window_container_manage(tray_container);
#if 1 /* Nono */
  ecore_x_window_background_color_set(tray_container, 0xcccc, 0xcccc, 0xcccc);
#elif 0 /* ParentRelative */
  XSetWindowBackgroundPixmap(display, tray_container, ParentRelative);
#else /* XAlloc...Color */
  {
    XColor xc;
#if 0 /* XAllocColor */
    xc.red = 0xffff;
    xc.green = 0;
    xc.blue = 0xffff;
    XAllocColor(display, DefaultColormap(display, DefaultScreen(display)), &xc);
#else /* XAllocNamedColor */
    XAllocNamedColor(display, DefaultColormap(display, DefaultScreen(display)), "Yellow", &xc, &xc);
#endif
    ecore_x_window_background_color_set(tray_container, xc.pixel);
  }
#endif
  ecore_x_window_show(tray_container);

  ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, od_tray_msg_cb, NULL);
  ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, od_tray_msg_cb, NULL);

  od_tray_layout();
}

