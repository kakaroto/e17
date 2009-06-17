#include "engage.h"
#include "config.h"
#include "Ecore_X.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include <assert.h>

/**
 * A list of all the X Window clients, data is type OD_Window *
 */
Eina_List      *clients = NULL;

/**
 * A hash of all the X Window clients, window id -> OD_Window* mapping
 */
static Evas_Hash *clients_hash = NULL;

/**
 * A hash of the "current" window for the window class, 
 * window class -> OD_Window* mapping
 */
static Evas_Hash *clients_current = NULL;

static int      od_sync_clients(void *data);
static int      od_window_id_comp(const void *a, const void *b);
static Ecore_X_Window *od_wm_get_clients(int *size);
static bool     od_wm_ignored(Ecore_X_Window win);

OD_Window      *
od_wm_window_current_by_window_class_get(const char *name)
{
  Eina_List      *l = NULL;
  OD_Window      *win = NULL;
  OD_Window      *result = NULL;
  OD_Window      *current = NULL;

#if 0
  printf("trying to find %s\n", name);
#endif
  if ((current = evas_hash_find(clients_current, name)) == NULL) {
    for (l = clients; l; l = l->next) {
      if ((win = l->data)) {
        if (od_wm_iconified(win->id))
          continue;
        if (win->applnk && win->applnk->data.applnk.winclass) {
          if (!strcmp(name, win->applnk->data.applnk.winclass)) {
            result = win;
#if 0
            fprintf(stderr, "%s(%8x)\n", name, win->id);
#endif
            break;
          }
        }
      }
    }
  }
  return (result);
}

OD_Window      *
od_wm_window_next_by_window_class_get(const char *name)
{
  Eina_List      *l = NULL;
  Eina_List      *tmp = NULL;
  OD_Window      *win = NULL;
  OD_Window      *result = NULL;
  OD_Window      *current = NULL;

#if 0
  printf("trying to find %s\n", name);
#endif
  if ((current = evas_hash_find(clients_current, name))) {
    clients_current = evas_hash_del(clients_current, name, current);
    for (l = clients; l; l = l->next) {
      if ((win = l->data)) {
        if (od_wm_iconified(win->id))
          continue;
        if (win->applnk && win->applnk->data.applnk.winclass) {
          if (!strcmp(name, win->applnk->data.applnk.winclass)) {
            tmp = eina_list_append(tmp, win);
          }
        }
      }
    }
    for (l = tmp; l; l = l->next) {
#if 0
      fprintf(stderr, "%8x %8x\n", ((OD_Window *) l->data)->id, current->id);
#endif
      if ((l->data == current) && (l->next)) {
        result = l->next->data;
      }
    }
    if (!result && tmp)
      result = tmp->data;
    eina_list_free(tmp);
  } else {
    for (l = clients; l; l = l->next) {
      if ((win = l->data)) {
        if (od_wm_iconified(win->id))
          continue;
        if (win->applnk && win->applnk->data.applnk.winclass) {
          if (!strcmp(name, win->applnk->data.applnk.winclass)) {
            result = win;
#if 0
            fprintf(stderr, "%s(%8x)\n", name, win->id);
#endif
            break;
          }
        }
      }
    }
  }
  if (result) {
    clients_current = evas_hash_add(clients_current, name, result);
  }
  return (result);
}

OD_Window      *
od_wm_window_prev_by_window_class_get(const char *name)
{
  Eina_List      *l = NULL;
  Eina_List      *tmp = NULL;
  Eina_List      *last = NULL;
  OD_Window      *win = NULL;
  OD_Window      *result = NULL;
  OD_Window      *current = NULL;

  if ((current = evas_hash_find(clients_current, name))) {
    clients_current = evas_hash_del(clients_current, name, current);
    for (l = clients; l; l = l->next) {
      if ((win = l->data)) {
        if (od_wm_iconified(win->id))
          continue;
        if (win->applnk && win->applnk->data.applnk.winclass) {
          if (!strcmp(name, win->applnk->data.applnk.winclass)) {
            tmp = eina_list_append(tmp, win);
          }
        }
      }
    }
    for (l = tmp; l; l = l->next) {
#if 0
      fprintf(stderr, "%8x %8x\n", ((OD_Window *) l->data)->id, current->id);
#endif
      if ((l->data == current) && (l->prev)) {
        result = l->prev->data;
      }
    }

    if (tmp) {
      last = eina_list_last(tmp);
      if (!result && last)
        result = last->data;
    }
    eina_list_free(tmp);
  } else {
    for (l = clients; l; l = l->next) {
      if ((win = l->data)) {
        if (win->applnk && win->applnk->data.applnk.winclass) {
          if (od_wm_iconified(win->id))
            continue;
          if (!strcmp(name, win->applnk->data.applnk.winclass)) {
            result = win;
#if 0
            fprintf(stderr, "%s(%8x)\n", name, win->id);
#endif
            break;
          }
        }
      }
    }
  }
  if (result) {
    clients_current = evas_hash_add(clients_current, name, result);
  }
  return (result);
}

char           *
od_wm_get_winclass(Ecore_X_Window win)
{
  char           *ret = NULL;

  ecore_x_icccm_name_class_get(win, NULL, &ret);
  if (ret)
    return ret;
  else
    return strdup("");
}

char           *
od_wm_get_title(Ecore_X_Window win)
{
  char *ret;
  
  ecore_x_netwm_name_get(win, &ret);
  if (!ret)
    ret = ecore_x_icccm_title_get(win);

  if (ret)
    return ret;
  else
    return strdup("");
}

bool
od_wm_iconified(Ecore_X_Window window)
{
  int             size = 0;
  Atom           *atom = NULL;

  if (ecore_x_window_prop_property_get(window, ecore_x_atom_get("WM_STATE"),
                                       ecore_x_atom_get("WM_STATE"), 32,
                                       (unsigned char **) &atom, &size)) {
    bool            ret = (*atom == IconicState);

    free(atom);
    return ret;
  }

  if (ecore_x_window_prop_property_get
      (window, ecore_x_atom_get("_NET_WM_STATE"), XA_ATOM, 32,
       (unsigned char **) &atom, &size)) {
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

int
window_prop_change_cb(void *data, int type, void *event)
{
  Ecore_X_Event_Window_Property *ev = event;

  if ((ev->atom == ecore_x_atom_get("_NET_CLIENT_LIST")) ||
      (ev->atom == ecore_x_atom_get("_NET_WM_STATE")))
    od_sync_clients(NULL);
  return 1;                     // carry on
}

void
od_dock_icons_update_begin()
{
  od_sync_clients(NULL);
  ecore_x_event_mask_set(DefaultRootWindow(ecore_x_display_get()),
                         ECORE_X_EVENT_MASK_WINDOW_PROPERTY | 
                         ECORE_X_EVENT_MASK_WINDOW_MANAGE);
  ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, window_prop_change_cb,
                          NULL);
}

Evas_Bool
od_wm_current_window_by_class_fix(Evas_Hash * hash, const char *key,
                                  void *data, void *fdata)
{
  if (data == fdata) {
    clients_current = evas_hash_del(clients_current, key, data);
    return (0);
  }
  return (1);
}

int
od_sync_clients(void *data)
{
  int             num_clients;
  char           *title = NULL;
  char           *winclass = NULL;
  char            buf[32];
  OD_Window      *owd = NULL;
  Eina_List      *dirty = NULL;
  Eina_List      *fresh = NULL;
  Eina_List      *item = NULL;
  Eina_List      *to_free = NULL;
  Ecore_X_Window *win = NULL;
  Ecore_X_Window *windows = NULL;

  dirty = clients;
  clients = NULL;
  windows = od_wm_get_clients(&num_clients);
  qsort(windows, num_clients, sizeof(Ecore_X_Window), od_window_id_comp);
  win = windows;

  while (num_clients--) {
    owd = NULL;
    if (!od_wm_ignored(*win)) {
      snprintf(buf, 32, "%8x", *win);
      if ((owd = evas_hash_find(clients_hash, buf))) {
        dirty = eina_list_remove(dirty, owd);
        if (!owd->minwin && od_wm_iconified(owd->id) && !options.ignore_min) {
          owd->minwin = od_icon_new_minwin(owd->id, od_wm_get_title(owd->id),
                                           od_wm_get_winclass(owd->id));
          od_dock_add_minwin(owd->minwin);
        } else if (owd->minwin && !od_wm_iconified(owd->id)) {
          od_dock_del_icon(owd->minwin);
          owd->minwin = NULL;
        } else if (owd->minwin) {
          title = od_wm_get_title(owd->id);
          if (strcmp(title, owd->minwin->name) != 0)
            od_icon_name_change(owd->minwin, title);
          free(title);
        }
      } else {
#if 0
        fprintf(stderr, "New %8x window id\n", *win);
#endif
        owd = (OD_Window *) malloc(sizeof(OD_Window));
        memset(owd, 0, sizeof(OD_Window));
        owd->id = *win;
        clients_hash = evas_hash_add(clients_hash, buf, owd);
        fresh = eina_list_append(fresh, owd);
      }
      clients = eina_list_append(clients, owd);
    }
    win++;
  }
  free(windows);

  /* pending windows that aren't present anymore */
  to_free = dirty;
  while (dirty) {
    if (dirty->data) {
      owd = dirty->data;
      snprintf(buf, 32, "%8x", owd->id);
#if 0
      fprintf(stderr, "%s no longer exists\n", buf);
#endif
      evas_hash_foreach(clients_current, od_wm_current_window_by_class_fix,
                        owd);
      clients_hash = evas_hash_del(clients_hash, buf, owd);
      if (owd->minwin)
        od_dock_del_icon(owd->minwin);
      if ((owd->applnk) && (owd->applnk->data.applnk.count > 0)) {
        owd->applnk->data.applnk.count--;
        if (owd->applnk->data.applnk.count == 0) {
          od_icon_arrow_hide(owd->applnk);
          if (owd->applnk->launcher == 0) {
            od_dock_del_icon(owd->applnk);
            free(owd);
          }
        }
      }
    }
    dirty = dirty->next;
  }
  to_free = eina_list_free(to_free);

  to_free = fresh;
  while (fresh) {
    if (fresh->data) {
      owd = fresh->data;
      title = od_wm_get_title(owd->id);
      ecore_x_event_mask_set(owd->id, ECORE_X_EVENT_MASK_WINDOW_PROPERTY);
      winclass = od_wm_get_winclass(owd->id);
      item = dock.applnks;

      while (item) {
        OD_Icon        *applnk = (OD_Icon *) item->data;
        if (applnk->data.applnk.winclass &&
            strcmp(applnk->data.applnk.winclass, winclass) == 0) {
          owd->applnk = applnk;
          break;
        }
        item = item->next;
      }
      if (!owd->applnk) {
        owd->applnk = od_icon_new_applnk(od_icon_mapping_get(winclass), title, winclass);
	if (!options.ignore_run)
          od_dock_add_applnk(owd->applnk);
      }
      owd->applnk->data.applnk.count++;
      if (owd->applnk->data.applnk.count == 1)
        od_icon_arrow_show(owd->applnk);
      if (od_wm_iconified(owd->id) && !options.ignore_min) {
        owd->minwin = od_icon_new_minwin(owd->id, title, winclass);
        od_dock_add_minwin(owd->minwin);
      }
#ifdef HAVE_IMLIB
      if (options.grab_app_icons)
        od_icon_grab(owd->applnk, owd->id);
#endif
      free(title);
      free(winclass);
    }
    fresh->data = NULL;
    fresh = fresh->next;
  }
  to_free = eina_list_free(to_free);
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
  Ecore_X_Window *win_list = NULL;

  if (!ecore_x_window_prop_property_get(0, ecore_x_atom_get("_NET_CLIENT_LIST"),
                                        XA_WINDOW, 32,
                                        (unsigned char **) &win_list, size)) {
    if (!ecore_x_window_prop_property_get
        (0, ecore_x_atom_get("_WIN_CLIENT_LIST"), XA_CARDINAL, 32,
         (unsigned char **) &win_list, size)) {
      *size = 0;
      return NULL;
    }
  }
  return win_list;
}

bool
od_wm_ignored(Ecore_X_Window win)
{
  bool            result = false;
  static char    *ignore[] = { "engage", "kicker", "trayer", "", NULL };
  char          **cur = ignore;
  char           *winclass = od_wm_get_winclass(win);
  Ecore_X_Window_State *state, *tmp;
  unsigned int num, counter;

  while (*cur) {
    if (strcmp(*cur, winclass) == 0)
      result = true;
    cur++;
  }

  ecore_x_netwm_window_state_get(win, &state, &num);
  counter = 0;
  for (tmp = state; counter < num; tmp++, counter++)
    if (*tmp == ECORE_X_WINDOW_STATE_SKIP_TASKBAR)
      result = true;

  if (winclass)
    free(winclass);
  return result;
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
  ecore_x_icccm_iconic_request_send(win, 0);
}
