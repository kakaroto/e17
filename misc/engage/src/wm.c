#include "engage.h"
#include "config.h"
#include "Ecore_X.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#ifdef DMALLOC
#include "dmalloc.h"
#endif

Evas_List      *clients = NULL;

static int      od_sync_clients(void *data);
static int      od_window_id_comp(const void *a, const void *b);
static Ecore_X_Window *od_wm_get_clients(int *size);
static bool     od_wm_ignored(Ecore_X_Window win);

char           *
od_wm_get_winclass(Ecore_X_Window win)
{
  char           *ret, *dummy;

  ecore_x_window_prop_name_class_get(win, &dummy, &ret);
  free(dummy);
  if (ret)
    return ret;
  else
    return strdup("");
}

char           *
od_wm_get_title(Ecore_X_Window win)
{
  char           *ret = ecore_x_window_prop_title_get(win);

  if (ret)
    return ret;
  else
    return strdup("");
}

bool
od_wm_iconified(Ecore_X_Window window)
{
  int             size;
  Atom           *atom;

  if (ecore_x_window_prop_property_get(window, ecore_x_atom_get("WM_STATE"),
                                       ecore_x_atom_get("WM_STATE"), 32, &atom,
                                       &size)) {
    bool            ret = (*atom == IconicState);

    free(atom);
    return ret;
  }

  if (ecore_x_window_prop_property_get
      (window, ecore_x_atom_get("_NET_WM_STATE"), XA_ATOM, 32, &atom, &size)) {
    int             i;
    bool            hidden = false, shaded = false;

    for (i = 0; i < size / sizeof(Atom); i++) {
      if (atom[i] == ecore_x_atom_get("_NET_WM_STATE_SHADED"))
        shaded = true;
      if (atom[i] == ecore_x_atom_get("_NET_WM_STATE_HIDDEN"))
        hidden = true;
    }
    free(atom);
    return hidden && !shaded;
  }

  return false;                 // anything we've missed ???

}

void
od_dock_icons_update_begin()
{
  ecore_timer_add(1.0, od_sync_clients, NULL);

}

int
od_sync_clients(void *data)
{
  Evas_List      *current = clients;
  Evas_List      *fresh = NULL;

  {
    int             num_clients;
    Ecore_X_Window *windows = od_wm_get_clients(&num_clients);

    qsort(windows, num_clients, sizeof(Ecore_X_Window), od_window_id_comp);
    Ecore_X_Window *win = windows;

    while (num_clients--) {
      OD_Window      *owd = (OD_Window *) malloc(sizeof(OD_Window));

      owd->id = *win;
      owd->applnk = NULL;
      owd->minwin = NULL;
      fresh = evas_list_append(fresh, owd);
      win++;
    }
    free(windows);
  }

  Evas_List      *pcurrent = current;
  Evas_List      *pfresh = fresh;

  while (pcurrent || pfresh) {
    OD_Window      *wcurrent = (OD_Window *) (pcurrent ? pcurrent->data : NULL);
    OD_Window      *wfresh = (OD_Window *) (pfresh ? pfresh->data : NULL);

    if (wcurrent && (!wfresh || wcurrent->id < wfresh->id)) {   // wcurrent disappeared
#if 0
      fprintf(stderr, "window disappeared: id=0x%.8x\n", wcurrent->id);
#endif
      if (wcurrent->minwin)
        od_dock_del_icon(wcurrent->minwin);
      if (wcurrent->applnk) {
        if (wcurrent->applnk->data.applnk.count == 0) {
          fprintf(stderr, "eek! applnk %s already has no windows\n",
                  wcurrent->applnk->name);
          exit(EXIT_FAILURE);
        }
        wcurrent->applnk->data.applnk.count--;
        if (wcurrent->applnk->data.applnk.count == 0) {
          od_icon_arrow_hide(wcurrent->applnk);
          if (wcurrent->applnk->data.applnk.command[0] == '\0')
            od_dock_del_icon(wcurrent->applnk);
        }
      }

      pcurrent = pcurrent->next;
    } else if (wcurrent && wfresh && wcurrent->id == wfresh->id) {      // update info
      wfresh->applnk = wcurrent->applnk;
      wfresh->minwin = wcurrent->minwin;
      char           *title = NULL;

      if (!wfresh->minwin && od_wm_iconified(wfresh->id)) {
        wfresh->minwin = od_icon_new_minwin(wfresh->id);
        od_dock_add_minwin(wfresh->minwin);
      } else if (wfresh->minwin && !od_wm_iconified(wfresh->id)) {
        od_dock_del_icon(wfresh->minwin);
        wfresh->minwin = NULL;
      } else if (wfresh->minwin &&
                 strcmp((title =
                         od_wm_get_title(wcurrent->id)),
                        wfresh->minwin->name) != 0) {
        od_icon_name_change(wfresh->minwin, title);
        if (title)
          free(title);
      }

      pcurrent = pcurrent->next;
      pfresh = pfresh->next;
    } else if (wfresh && (!wcurrent || wcurrent->id > wfresh->id)) {    // new window: wfresh
      char           *title = od_wm_get_title(wfresh->id);
      char           *winclass = od_wm_get_winclass(wfresh->id);

#if 0
      fprintf(stderr,
              "window appeared: id=0x%.8x, name=\"%s\", winclass=\"%s\"\n",
              wfresh->id, title, winclass);
#endif

      if (!od_wm_ignored(wfresh->id)) {
        {
          wfresh->applnk = NULL;
          Evas_List      *item = dock.applnks;

          while (item) {
            OD_Icon        *applnk = (OD_Icon *) item->data;

            if (strcmp(applnk->data.applnk.winclass, winclass) == 0) {
              wfresh->applnk = applnk;
              break;
            }
            item = item->next;
          }
        }
        if (!wfresh->applnk) {
          wfresh->applnk = od_icon_new_applnk("", winclass);
          od_dock_add_applnk(wfresh->applnk);
        }
        wfresh->applnk->data.applnk.count++;
        od_icon_arrow_show(wfresh->applnk);

        if (od_wm_iconified(wfresh->id)) {
          wfresh->minwin = od_icon_new_minwin(wfresh->id);
          od_dock_add_minwin(wfresh->minwin);
        } else
          wfresh->minwin = NULL;
#ifdef HAVE_IMLIB
        if (options.grab_app_icons)
          od_icon_grab(wfresh->applnk, wfresh->id);
#endif
      }

      free(title);
      free(winclass);
      pfresh = pfresh->next;
    } else {
      fprintf(stderr, "eeek!!!\n");
      exit(EXIT_FAILURE);
    }
  }

  // swap the two lists
  {
    clients = fresh;
    Evas_List      *item = current;

    while (item) {
      free(item->data);
      item = evas_list_next(item);
    }
    evas_list_free(current);
  }
  return 1;                     // keep going
}

int
od_window_id_comp(const void *a, const void *b)
{
  return *((Ecore_X_Window *) a) - *((Ecore_X_Window *) b);
}

Ecore_X_Window *
od_wm_get_clients(int *size)
{
  Ecore_X_Window *win_list;

  if (!ecore_x_window_prop_property_get(0, ecore_x_atom_get("_NET_CLIENT_LIST"),
                                        XA_WINDOW, 32, &win_list, size)) {
    if (!ecore_x_window_prop_property_get
        (0, ecore_x_atom_get("_WIN_CLIENT_LIST"), XA_CARDINAL, 32, &win_list,
         size)) {
      *size = 0;
      return NULL;
    }
  }
  return win_list;
}

bool
od_wm_ignored(Ecore_X_Window win)
{
  static char    *ignore[] = { "engage", "kicker", "", NULL };
  char          **cur = ignore;
  char           *winclass = od_wm_get_winclass(win);

  while (*cur) {
    if (strcmp(*cur, winclass) == 0)
      return true;
    cur++;
  }

  if (ecore_x_window_prop_state_isset(win, ECORE_X_WINDOW_STATE_SKIP_TASKBAR))
    return true;
  return false;
}

void
od_wm_activate_window(Ecore_X_Window win)
{
  ecore_x_window_show(win);
  ecore_x_window_raise(win);
}

void
od_wm_deactivate_window(Ecore_X_Window win)
{
  ecore_x_window_lower(win);
  ecore_x_window_prop_state_request(win, ECORE_X_WINDOW_STATE_ICONIFIED, 1);
}
