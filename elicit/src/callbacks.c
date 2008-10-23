#include "Elicit.h"
#include "callbacks.h"

#include <string.h>

void _elicit_color_signal_process(Elicit *el);
static int elicit_timer_color(void *data);

void
elicit_cb_exit(Ecore_Evas *ee)
{
  ecore_main_loop_quit();
}    

void
elicit_cb_enter(Ecore_Evas *ee)
{
  Evas_Object *gui;
  gui = evas_object_name_find(ecore_evas_get(ee), "gui");
  edje_object_signal_emit(gui, "elicit,window,enter", "Elicit");
}

void
elicit_cb_leave(Ecore_Evas *ee)
{
  Evas_Object *gui;
  gui = evas_object_name_find(ecore_evas_get(ee), "gui");
  edje_object_signal_emit(gui, "elicit,window,leave", "Elicit");
}

void 
elicit_cb_resize(Ecore_Evas *ee)
{
  int w, h;
  Evas_Coord sw, sh;
  Evas_Object *gui, *draggie;

  /* keep the gui the same size as the window */
  ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

  gui = evas_object_name_find(ecore_evas_get(ee), "gui");
  draggie = evas_object_name_find(ecore_evas_get(ee), "draggie");

  evas_object_resize(gui, w, h);
  evas_object_resize(draggie, w, h);
}

void elicit_cb_pick(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  if (!strcmp(sig, "elicit,pick,start"))
  {
    el->flags.picking = 1;
  }
  else if (!strcmp(sig, "elicit,pick,stop"))
  {
    el->flags.picking = 0;
  }
}

void elicit_cb_shoot(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  if (!strcmp(sig, "elicit,shoot,start"))
  {
    el->flags.shot_taken = 1;
    el->flags.shooting = 1;
  }
  else if (!strcmp(sig, "elicit,shoot,stop"))
  {
    el->flags.shooting = 0;
  }
}

void elicit_cb_move(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  if (el->flags.picking == 1)
  {
    elicit_util_color_at_pointer_get(&(el->color.r), &(el->color.g), &(el->color.b));
    evas_object_color_set(el->swatch, el->color.r, el->color.g, el->color.b, 255);
    elicit_util_colors_set_from_rgb(el);
    elicit_ui_update(el);
  }

  if (el->flags.shooting == 1)
  {
    elicit_zoom(el->shot);
  }
}


void
elicit_cb_colors(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  if (elicit_glob_match(sig, "*,start"))
  {
    el->flags.changing = 1;
    el->change_sig = strdup(sig);
    el->change_timer = ecore_timer_add(0.01, elicit_timer_color, data);
  }
  else if (elicit_glob_match(sig, "*,stop"))
  {
    el->flags.changing = 0;
    if (el->change_sig)
    {
      free(el->change_sig);
      el->change_sig = NULL;
    }
  }
  else if (elicit_glob_match(sig, "*,up") || elicit_glob_match(sig, "*,down"))
  {
    el->flags.changing = 1;
    el->change_sig = (char *)sig;
    _elicit_color_signal_process(data);
    el->change_sig = NULL;
  }
}

void
elicit_cb_switch(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  /* FIXME: i may need to free something here... */
  char *theme = elicit_config_theme_get();
  char *file = elicit_theme_find(theme);
  char group[PATH_MAX];
  Eina_List *groups = edje_file_collection_list(file);
  Eina_List *l;

  sscanf(sig, "elicit,switch,%s", group);

  for (l = groups; l; l = l->next)
  {
    char *gp = l->data;
    if (!strcmp(gp, group))
    {
      elicit_ui_theme_set(el, theme, group); 
      return;
    }
  }
  free(theme);
  edje_file_collection_list_free(groups);
  printf("Error: group %s does not exist in file %s\n", group, file);
}

void
elicit_cb_resize_sig(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  int ow = 0, oh = 0;
  int w = 0, h = 0;
  char arg[PATH_MAX];
  char *wstr, *hstr, *sub;
  int woff = 0, hoff = 0;

  ecore_evas_geometry_get(el->ee, NULL, NULL, &ow, &oh);

  /* if we have an arg */
  if (sscanf(sig, "elicit,resize,%s", arg))
  {
    wstr = arg;
    sub = strstr(arg, ",");
    //printf("arg: %s :: sub: %s\n", arg, sub);
    sub[0] = '\0';
    hstr = sub + 1;

    if (wstr[0] == '+') woff = 1;
    else if (wstr[0] == '-') woff = -1;
    if (hstr[0] == '+') hoff = 1;
    else if (hstr[0] == '-') hoff = -1;

    //printf("h: %s, w: %s off:(%d,%d)\n", hstr, wstr, hoff, woff);
    if (woff != 0) wstr = wstr + 1;
    if (hoff != 0) hstr = hstr + 1;
    //printf("h: %s, w: %s off:(%d,%d)\n", hstr, wstr, hoff, woff);
    w = atoi(wstr);
    h = atoi(hstr);

    //printf("resize: (%d, %d) (%d, %d)\n", w, h, woff, hoff);
    ecore_evas_resize(el->ee,
                      woff ? ow + w * woff : w,
                      hoff ? oh + h * hoff : h);
  }
}

void
elicit_cb_size_min(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  int ow = 0, oh = 0;
  int w = 0, h = 0;
  char arg[PATH_MAX];
  char *wstr, *hstr, *sub;
  int woff = 0, hoff = 0;

  ecore_evas_size_min_get(el->ee, &ow, &oh);

  /* if we have an arg */
  if (sscanf(sig, "elicit,size,min,%s", arg))
  {
    wstr = arg;
    sub = strstr(arg, ",");
    //printf("arg: %s :: sub: %s\n", arg, sub);
    sub[0] = '\0';
    hstr = sub + 1;

    if (wstr[0] == '+') woff = 1;
    else if (wstr[0] == '-') woff = -1;
    if (hstr[0] == '+') hoff = 1;
    else if (hstr[0] == '-') hoff = -1;

    //printf("h: %s, w: %s off:(%d,%d)\n", hstr, wstr, hoff, woff);
    if (woff != 0) wstr = wstr + 1;
    if (hoff != 0) hstr = hstr + 1;
    //printf("h: %s, w: %s off:(%d,%d)\n", hstr, wstr, hoff, woff);
    w = atoi(wstr);
    h = atoi(hstr);

    //printf("resize: (%d, %d) (%d, %d)\n", w, h, woff, hoff);
    ecore_evas_size_min_set(el->ee,
                      woff ? ow + w * woff : w,
                      hoff ? oh + h * hoff : h);
  }
}

void
elicit_cb_copy(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  Ecore_X_Window win = ecore_evas_software_x11_window_get(el->ee);

  ecore_x_selection_primary_set(win, el->color.hex, strlen(el->color.hex));
}

void
elicit_cb_slider(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  double vx;

  edje_object_part_drag_value_get(el->gui, src, &vx, NULL);

  if (!strcmp(src, "red-slider"))
  {
    el->color.r = vx * 255;
    elicit_util_colors_set_from_rgb(el);
  }
  else if (!strcmp(src, "green-slider"))
  {
    el->color.g = vx * 255;
    elicit_util_colors_set_from_rgb(el);
  }
  else if (!strcmp(src, "blue-slider"))
  {
    el->color.b = vx * 255;
    elicit_util_colors_set_from_rgb(el);
  }
  else if (!strcmp(src, "hue-slider"))
  {
    el->color.h = vx * 360;
    elicit_util_colors_set_from_hsv(el);
  }
  else if (!strcmp(src, "sat-slider"))
  {
    el->color.s = vx;
    elicit_util_colors_set_from_hsv(el);
  }
  else if (!strcmp(src, "val-slider"))
  {
    el->color.v = vx;
    elicit_util_colors_set_from_hsv(el);
  }
  else if (!strcmp(src, "zoom-slider"))
  {
    el->zoom = 1 + (el->zoom_max - 1) * vx;
  }

  elicit_ui_update(el);
  
}

void
elicit_cb_colorclass(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  edje_object_color_class_set(el->gui, "border", el->color.r, el->color.g, el->color.b, 255, 0, 0, 0, 0, 0, 0, 0, 0);
  elicit_config_colorclass_set(el->color.r, el->color.g, el->color.b);
}

void
elicit_cb_edit(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  char buf[PATH_MAX];
  char tmp[PATH_MAX];
  char plate[18] ="/tmp/elicit-XXXXXX";

  if (el && el->swatch)
  {
    char *ed = elicit_config_editor_get();

    if (!ed) return;

    if (!el->tmpdir)
    {
      if (mkdtemp(plate))
      {
        el->tmpdir = strdup(plate);
      }
    }

    if (el->tmpdir)
    {
      snprintf(tmp, PATH_MAX, "%s/elicit_%f.png", el->tmpdir, ecore_time_get() );
      elicit_util_shot_save(el, tmp);
      snprintf(buf, PATH_MAX, "%s %s", ed, tmp);

      free(ed);

      ecore_exe_run(buf, NULL);
    }
  }
}

void
elicit_cb_freeze(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  edje_object_freeze(el->gui);
}
void
elicit_cb_thaw(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  edje_object_thaw(el->gui);
}

void
elicit_cb_grid(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  if (!strcmp(sig, "elicit,grid,toggle")) {
    elicit_zoom_grid_visible_set(el->shot, !elicit_config_grid_visible_get());
  }
  else if (!strcmp(sig, "elicit,grid,show")) {
    elicit_zoom_grid_visible_set(el->shot, 1);
  }
  else if (!strcmp(sig, "elicit,grid,hide")) {
    elicit_zoom_grid_visible_set(el->shot, 0);
  }
}

static int
elicit_timer_color(void *data)
{
  Elicit *el = data;
  static double start = 0.0;
  double duration = 2.0;
  double r, d;

  /* we're done */
  if (!el->flags.changing)
  {
    if (el->change_timer)
    {
      ecore_timer_del(el->change_timer);
      el->change_timer = NULL;
    }
    return 0;
  }
 
  /* this is the first time */
  if (el->flags.changing == 1)
  { 
    start = ecore_time_get();
    el->flags.changing = 2;
  }

  d = (ecore_time_get() - start) / duration;
  if (d > 1) d = 1.0;
  r = sin(d * .5 * M_PI);

  _elicit_color_signal_process(el);
  
  /* we want the time to wait to depend on how far along it is... */
  el->change_timer = ecore_timer_add(.16 - .15*r, elicit_timer_color, el);

  return 0;
}

void _elicit_color_signal_process(Elicit *el)
{
  int dir;

  if (!el->change_sig) return;

  if (elicit_glob_match(el->change_sig, "*,up*")) dir = 1;
  else if (elicit_glob_match(el->change_sig, "*,down*")) dir = -1;
  else dir = 0; /* this shouldn't happen */

  if (elicit_glob_match(el->change_sig, "*,r,*"))
  {
    el->color.r += dir;
    if (el->color.r > 255) el->color.r = 255;
    if (el->color.r < 0) el->color.r = 0;

    elicit_util_colors_set_from_rgb(el);
  }

  else if (elicit_glob_match(el->change_sig, "*,g,*"))
  {
    el->color.g += dir;
    if (el->color.g > 255) el->color.g = 255;
    if (el->color.g < 0) el->color.g = 0;
    
    elicit_util_colors_set_from_rgb(el);
  }

  else if (elicit_glob_match(el->change_sig, "*,b,*"))
  {
    el->color.b += dir;
    if (el->color.b > 255) el->color.b = 255;
    if (el->color.b < 0) el->color.b = 0;
    
    elicit_util_colors_set_from_rgb(el);
  }

  else if (elicit_glob_match(el->change_sig, "*,h,*"))
  {
    el->color.h += dir;
    if (el->color.h > 360) el->color.h = 360;
    if (el->color.h < 0) el->color.h = 0;

    elicit_util_colors_set_from_hsv(el);
  }

  else if (elicit_glob_match(el->change_sig, "*,s,*"))
  {
    el->color.s += dir * .01;
    if (el->color.s > 1) el->color.s = 1;
    if (el->color.s < 0) el->color.s = 0;

    elicit_util_colors_set_from_hsv(el);
  }

  else if (elicit_glob_match(el->change_sig, "*,v,*"))
  {
    el->color.v += dir * .01;
    if (el->color.v > 1) el->color.v = 1;
    if (el->color.v < 0) el->color.v = 0;
    
    elicit_util_colors_set_from_hsv(el);
  }

  else if (elicit_glob_match(el->change_sig, "*,zoom,*"))
  {
    el->zoom += dir * 1;
    if (el->zoom < 1) el->zoom = 1;
    if (el->zoom > 20) el->zoom = 20;
  }

  elicit_ui_update(el);
  evas_object_color_set(el->swatch, el->color.r, el->color.g, el->color.b, 255);

  el->flags.changed = 1;
}
