#include "engage.h"
#include "limits.h"
#include "config.h"
#include "battery.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef HAVE_IMLIB
#include <Imlib2.h>
#endif
// for stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include <assert.h>

Evas_List      *icon_mappings = NULL;
Evas_List      *icon_paths = NULL;

static OD_Icon *od_icon_new(E_App *app, char *name_override,
                            char *class_override, int type);
static void     od_icon_edje_app_cb(void *data, Evas_Object * obj,
                                    const char *emission, const char *source);
static void     od_icon_edje_win_minimize_cb(void *data, Evas_Object * obj,
                                             const char *emission,
                                             const char *source);
static void     od_icon_edje_win_raise_cb(void *data, Evas_Object * obj,
                                          const char *emission,
                                          const char *source);

OD_Icon        *
od_icon_new_applnk(E_App *app, char *name_override, char *class_override)
{
  OD_Icon        *ret = NULL;

  ret = od_icon_new(app, name_override, class_override, application_link);

  if (app->exe)
    ret->data.applnk.command = strdup(app->exe);
  if (app->win_class)
    ret->data.applnk.winclass = strdup(class_override? class_override : app->win_class);
  ret->data.applnk.count = 0;
  return ret;
}

OD_Icon        *
od_icon_new_sysicon(const char *name, const char *icon_name)
{
  OD_Icon        *ret = od_icon_new(od_unmatched_app, (char *)name, NULL,
      system_icon);
  ret->icon_file = strdup(icon_name);
  od_icon_reload(ret);
  return ret;
}


OD_Icon        *
od_icon_new_minwin(Ecore_X_Window win, char *name_override, char *class_override)
{
  OD_Icon        *ret = NULL;
  char           *winclass = NULL;
  E_App          *app;

  winclass = od_wm_get_winclass(win);
  app = od_icon_mapping_get(winclass);

  if ((ret = od_icon_new(app, name_override, class_override, minimised_window))) {
#ifdef HAVE_IMLIB
    if (options.grab_min_icons != 0)
      od_icon_grab(ret, win);
#endif
    ret->data.minwin.window = win;
  }

  if (winclass)
    free(winclass);
  return ret;
}

void
od_object_resize_intercept_cb(void *data, Evas_Object * o,
                              Evas_Coord w, Evas_Coord h)
{
  if (o) {
    if (!strcmp("image", evas_object_type_get(o))) {
      evas_object_image_fill_set(o, 0.0, 0.0, w, h);
    } else {
#if 0
      fprintf(stderr, "Intercepting something other than an image(%s)\n",
              evas_object_type_get(o));
#endif
    }
  }
  evas_object_resize(o, w, h);
}

#ifdef HAVE_IMLIB
void
od_icon_grab(OD_Icon * icon, Ecore_X_Window win)
{
  XWMHints       *hints = NULL;
  Imlib_Image     img = NULL;
  Display        *dsp = NULL;
  int             scr = 0, x = 0, y = 0, w = 0, h = 0;
  Pixmap          pmap, mask;
  Evas_Object    *obj = NULL;

  assert(icon);
  dsp = ecore_x_display_get();
  scr = DefaultScreen(dsp);

  hints = XGetWMHints(dsp, win);
  if (hints == NULL)
    goto done;
  pmap = (hints->flags & IconPixmapHint) ? hints->icon_pixmap : None;
  mask = (hints->flags & IconMaskHint) ? hints->icon_mask : None;
  XFree(hints);
  if (pmap == None)
    goto done;

  ecore_x_pixmap_geometry_get(pmap, &x, &y, &w, &h);
  imlib_context_set_display(dsp);
  imlib_context_set_visual(DefaultVisual(dsp, scr));
  imlib_context_set_colormap(None);
  imlib_context_set_dither_mask(0);
  imlib_context_set_drawable(pmap);

/* include the if'd out block if you want to use imlib version < 1.1.1 */
#if 0
  mask = None;
#endif
  img = imlib_create_image_from_drawable(mask, x, y, w, h, 0);
  imlib_context_set_image(img);

  if ((obj = edje_object_part_swallow_get(icon->icon, "EngageIcon"))) {
    edje_object_part_unswallow(icon->icon, icon->pic);
    evas_object_del(icon->pic);
    icon->pic = evas_object_image_add(evas);
    evas_object_image_alpha_set(icon->pic, 1);
    evas_object_image_smooth_scale_set(icon->pic, 1);
    evas_object_pass_events_set(icon->pic, 1);
    evas_object_intercept_resize_callback_add(icon->pic,
                                              od_object_resize_intercept_cb,
                                              NULL);
  }

  evas_object_image_size_set(icon->pic, w, h);
  evas_object_image_data_copy_set(icon->pic,
                                  imlib_image_get_data_for_reading_only());
  edje_object_part_swallow(icon->icon, "EngageIcon", icon->pic);

  imlib_free_image();

done:
  return;                       // just fix compiler warnings - why do we have an empty done?
}
#endif

void
od_icon_reload(OD_Icon * in)
{
  const char     *icon_part = NULL;
  char           *path, *winclass, *name, *icon_file;

  Evas_Object    *icon = NULL;
  Evas_Object    *pic = NULL;

  icon = in->icon;
  winclass = in->winclass;
  name = in->name;
  icon_file = in->icon_file;
  pic = in->pic;

  if (!(path = ecore_config_theme_with_path_from_name_get(options.theme))) {
    path = ecore_config_theme_with_path_from_name_get("gentoo");
  }
  assert(path);
  if (pic) {
    evas_object_del(pic);
    pic = NULL;
  }
  
  edje_object_file_set(icon, path, "Main");
  pic = edje_object_add(evas_object_evas_get(icon));
  if (in->type == system_icon) {
    edje_object_file_set(pic, icon_file, "Main");
  } else if ((icon_part = edje_object_data_get(icon, winclass))) {
    pic = edje_object_add(evas_object_evas_get(icon));
    if (edje_object_file_set(pic, path, icon_part) > 0) {
#if 0
      fprintf(stderr, "Found icon part for %s(%s)\n", name, icon_part);
#endif
    } else if (edje_object_file_set(pic, path, "Unknown") > 0) {
#if 0
      fprintf(stderr, "Didn't Find icon part for %s\n", name);
#endif
    } else {
      evas_object_del(pic);
      pic = NULL;
    }
  } else {
    pic = e_app_icon_add(evas_object_evas_get(icon), in->a);
  }
  if (!pic) {
    pic = evas_object_image_add(evas);
    evas_object_image_file_set(pic, icon_file, NULL);
    evas_object_image_alpha_set(pic, 1);
    evas_object_image_smooth_scale_set(pic, 1);
    evas_object_pass_events_set(pic, 1);
    evas_object_intercept_resize_callback_add(pic,
                                              od_object_resize_intercept_cb,
                                              NULL);
  }
  free(path);

  in->pic = pic;
  evas_object_layer_set(pic, 100);
  evas_object_move(pic, -50, -50);
  evas_object_resize(pic, 32, 32);
  evas_object_show(pic);
  if (edje_object_part_exists(icon, "EngageIcon")) {
    edje_object_part_swallow(icon, "EngageIcon", pic);
    evas_object_pass_events_set(pic, 1);
  } else {
    evas_object_del(pic);
    in->pic = NULL;
  }
  if (edje_object_part_exists(icon, "EngageName")) {
    edje_object_part_text_set(icon, "EngageName", name);
  }
  evas_object_layer_set(icon, 100);
  evas_object_show(icon);

  if (in->type == system_icon) {
    char *tmp;
    // set tmp to be the last thing in the path
    tmp = icon_file;
    while(*tmp)tmp++;
    while(*tmp != '/' && tmp != path)tmp--;
    if(*tmp == '/') tmp++;
    if(!strcmp(tmp,"default_battery.edj")) {
      // hook up battery status if it's the battery icon
      Battery *bat = malloc(sizeof(Battery));
      memset(bat,0,sizeof(Battery));
      bat->object = pic;
      bat->battery_check_mode = CHECK_NONE;
      bat->battery_prev_drain = 1;
      bat->battery_prev_ac = -1;
      bat->battery_prev_battery = -1;
      _battery_cb_check(bat);
      bat->battery_check_timer = ecore_timer_add(5.0, _battery_cb_check, bat);
    }
  }

  if (in->data.applnk.count > 0)
    od_icon_arrow_show(in);
}

OD_Icon        *
od_icon_new(E_App *app, char *name_override, char *class_override, int type)
{
  OD_Icon        *ret = NULL;
//  assert(app->win_class);
  assert(app->name);
  assert(app->path);

  ret = (OD_Icon *) malloc(sizeof(OD_Icon));
  memset(ret, 0, sizeof(OD_Icon));
  ret->a = app;
  if (app->win_class) 
    ret->winclass = strdup(class_override ? class_override : app->win_class);
  ret->name = strdup(name_override ? name_override : app->name);
  ret->icon_file = strdup(app->path);
  ret->icon = edje_object_add(evas);
  ret->type = type;

  if (type != system_icon)
    od_icon_reload(ret);
  if (ret->icon) {
    edje_object_signal_callback_add(ret->icon, "engage,app,*", "*",
                                    od_icon_edje_app_cb, ret);
    edje_object_signal_callback_add(ret->icon, "engage,window,minimize*", "*",
                                    od_icon_edje_win_minimize_cb, ret);
    edje_object_signal_callback_add(ret->icon, "engage,window,raise*", "*",
                                    od_icon_edje_win_raise_cb, ret);
  }

  ret->launcher = 0;
  return ret;
}

void
od_icon_del(OD_Icon * icon)
{
  assert(icon);
  switch (icon->type) {
  case application_link:
    if (icon->data.applnk.command)
      free(icon->data.applnk.command);
    if (icon->data.applnk.winclass)
      free(icon->data.applnk.winclass);
    break;
  case minimised_window:
    break;
  case system_icon:
    break;
  }

  if (icon->icon)
    evas_object_del(icon->icon);
  if (icon->pic)
    evas_object_del(icon->pic);
  if (icon->name)
    free(icon->name);
  free(icon);
}

void
od_icon_arrow_show(OD_Icon * icon)
{
  assert(icon);
  assert(icon->icon);
  edje_object_signal_emit(icon->icon, "engage,app,opened", "");
}

void
od_icon_arrow_hide(OD_Icon * icon)
{
  assert(icon);
  assert(icon->icon);
  edje_object_signal_emit(icon->icon, "engage,app,closed", "");
}

void
od_icon_name_change(OD_Icon * icon, const char *name)
{
  free(icon->name);
  icon->name = strdup(name);
  edje_object_part_text_set(icon->icon, "EngageName", name);
  need_redraw = true;
}

void
od_icon_mapping_add(E_App *app)
{
  icon_mappings = evas_list_append(icon_mappings, app);
}

E_App *
od_icon_mapping_get(const char *winclass)
{
  Evas_List      *item = icon_mappings;
  E_App *app;

#if 0
  printf("getting mapping for %s\n", winclass);
#endif
  while (item) {
    app = item->data;
    if (app->win_class) {
      if (strcmp(winclass, app->win_class) == 0) {
        break;
      }
    }
    item = item->next;
  }
  if (!item)
    return od_unmatched_app;

  return app;
}

void
od_icon_add_path(const char *path)
{
  icon_paths = evas_list_append(icon_paths, strdup(path));
}

void
od_icon_add_kde_set(const char *path)
{
  static char    *sizes[] =
    { "128x128", "96x96", "64x64", "48x48", "32x32", "24x24", "16x16", "" };
  static char    *types[] = { "apps", "devices", "filesystems", "actions", "" };

  char          **size = sizes;

  while ((*size)[0] != '\0') {
    char          **type = types;

    while ((*type)[0] != '\0') {
      char            buffer[PATH_MAX];
      struct stat     dummy;

      strcpy(buffer, path);
      strcat(buffer, "/");
      strcat(buffer, *size);
      strcat(buffer, "/");
      strcat(buffer, *type);
      if (stat(buffer, &dummy) == 0)
        od_icon_add_path(buffer);
      type++;
    }
    size++;
  }
}
static void
od_icon_edje_app_cb(void *data, Evas_Object * obj, const char *emission, const
                    char *source)
{
  const char     *winclass = NULL;
  OD_Icon        *icon = NULL;
  OD_Window      *win = NULL;

  if ((icon = (OD_Icon *) data)) {
    if (!strcmp(emission, "engage,app,open")) {
      switch (icon->type) {
      case application_link:
        if (icon->data.applnk.command) {
          edje_object_signal_emit(obj, "engage,app,open,ok", "");
          ecore_exe_run(icon->data.applnk.command, NULL);
        } else
          printf("ERROR: no command stored in icon\n");
        break;
      case minimised_window:
        break;
      case system_icon:
        break;
      }
    } else if (!strcmp(emission, "engage,app,close")) {
      if (icon->data.applnk.winclass) {
        if ((winclass = icon->data.applnk.winclass)) {
          win = od_wm_window_current_by_window_class_get(winclass);
          ecore_x_icccm_protocol_set(win->id,
                                     ECORE_X_WM_PROTOCOL_DELETE_REQUEST,
                                     1);
        }
      }
    }
  }
}
static void
od_icon_edje_win_minimize_cb(void *data, Evas_Object * obj,
                             const char *emission, const
                             char *source)
{
  Evas_List      *l = NULL;
  OD_Icon        *icon = NULL;

  if ((icon = (OD_Icon *) data)) {
    if (!strcmp(emission, "engage,window,minimize")) {
      switch (icon->type) {
      case application_link:
        for (l = clients; l; l = l->next) {
          OD_Window      *win = (OD_Window *) l->data;

          if (win->applnk == icon && !win->minwin) {
            od_wm_deactivate_window(win->id);
            break;
          }
        }
        break;
      case minimised_window:
        break;
      case system_icon:
        break;
      }
    } else if (!strcmp(emission, "engage,window,minimize,all")) {
      switch (icon->type) {
      case application_link:
      case minimised_window:
        for (l = clients; l; l = l->next) {
          OD_Window      *win = (OD_Window *) l->data;

          if (win->applnk == icon && !win->minwin) {
            od_wm_deactivate_window(win->id);
          }
        }
        break;
      default:
        break;
      }
    }
#if 0
    fprintf(stderr, "Minimize got %s from %s\n", emission, icon->name);
#endif
  }
}
static void
od_icon_edje_win_raise_cb(void *data, Evas_Object * obj, const char *emission, const
                          char *source)
{
  const char     *winclass = NULL;
  Evas_List      *l = NULL;
  OD_Icon        *icon = NULL;
  OD_Window      *win = NULL;

  if ((icon = (OD_Icon *) data)) {
    if (!strcmp(emission, "engage,window,raise")) {
      switch (icon->type) {
      case application_link:
      case minimised_window:
        for (l = clients; l; l = l->next) {
          win = (OD_Window *) l->data;

          if (win->minwin == icon || win->applnk == icon) {
            od_wm_activate_window(win->id);
            break;
          }
        }
        break;
      default:
        break;
      }
    } else if (!strcmp(emission, "engage,window,raise,all")) {
      for (l = clients; l; l = l->next) {
        win = (OD_Window *) l->data;

        if (win->minwin == icon || win->applnk == icon) {
          od_wm_activate_window(win->id);
/*
          ecore_x_window_prop_state_request(win->id,
                                            ECORE_X_WINDOW_STATE_ICONIFIED, 0);
*/
        }
      }
    } else if (!strcmp(emission, "engage,window,raise,next")) {
      switch (icon->type) {
      case application_link:
        if (icon->data.applnk.winclass) {
          winclass = icon->data.applnk.winclass;
          if ((win = od_wm_window_next_by_window_class_get(winclass))) {
            od_wm_activate_window(win->id);
          }
        }
        break;
      case minimised_window:
        break;
      default:
        break;
      }
    } else if (!strcmp(emission, "engage,window,raise,prev")) {
      switch (icon->type) {
      case application_link:
        if (icon->data.applnk.winclass) {
          winclass = icon->data.applnk.winclass;
          if ((win = od_wm_window_prev_by_window_class_get(winclass))) {
            od_wm_activate_window(win->id);
          }
        }
        break;
      case minimised_window:
        break;
      default:
        break;
      }
    }
#if 0
    fprintf(stderr, "Raise got %s from %s\n", emission, icon->name);
#endif
  }
}
