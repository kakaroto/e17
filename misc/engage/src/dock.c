#include "engage.h"
#include <math.h>
#ifdef DMALLOC
#include "dmalloc.h"
#endif

OD_Dock         dock;
Ecore_Timer    *od_dock_zoom_timer;

static void     od_dock_reposition();
static void     zoom_function(double d, double *zoom, double *disp);
static int      od_dock_icon_appear(void *data);
static int      od_dock_icon_disappear(void *data);
static int      od_dock_zoom_in_slave(void *data);
static int      od_dock_zoom_out_slave(void *data);

void
od_dock_init()
{
  dock.icons = dock.applnks = dock.dicons = dock.minwins = NULL;
  dock.state = unzoomed;
  dock.zoom = 1.0;
  dock.x = 400.0;

  int             height = (int) options.size + 2 * (int) options.arrow_size;
  int            *pic1 = (int *) malloc(sizeof(int) * height);
  int            *pic2 = (int *) malloc(sizeof(int) * height);
  int             y;

  for (y = 0; y < height; y++) {
    pic1[y] = options.bg_fore;
    pic2[y] = options.bg_back;
  }
  pic2[0] = options.bg_fore;

  int             i;

  for (i = 0; i < 4; i++) {
    dock.background[i] = evas_object_image_add(evas);
    if (options.mode == OM_ONTOP)
      evas_object_image_alpha_set(dock.background[i], 0);
    else
      evas_object_image_alpha_set(dock.background[i], 1);
    evas_object_image_size_set(dock.background[i], 1, height);
    evas_object_image_smooth_scale_set(dock.background[i], 0);
    if (i != OD_BG_FILL)
      evas_object_image_data_copy_set(dock.background[i], pic1);
    else
      evas_object_image_data_copy_set(dock.background[i], pic2);
    evas_object_image_data_update_add(dock.background[i], 0, 0, 1, height);
    evas_object_image_fill_set(dock.background[i], 0.0, 0.0, 1.0,
                               (double) height);
    evas_object_resize(dock.background[i], 1.0, (double) height);
    if (i != OD_BG_FILL)
      evas_object_layer_set(dock.background[i], 1);
    else
      evas_object_layer_set(dock.background[i], 0);
    if (i != OD_BG_MIDDLE)
      evas_object_show(dock.background[i]);
    else
      evas_object_hide(dock.background[i]);
  }

  free(pic1);
  free(pic2);

  od_dock_reposition();
}

void
od_dock_reposition()
{
  // find the width;
  double          width = 0;

  {
    Evas_List      *item = dock.icons;

    while (item) {
      OD_Icon        *icon = (OD_Icon *) item->data;

      width += options.size * icon->scale + options.spacing;
      item = item->next;
    }
    if (dock.dicons || dock.minwins)
      width += options.spacing + 1.0;   // separator
  }

#define POSITION(__icons) \
		{ \
			Evas_List * __item = __icons; \
			while(__item) { \
				OD_Icon * __icon = (OD_Icon *)__item->data; \
				x += 0.5 * (__icon->scale * options.size + options.spacing); \
				__icon->x = x; \
				__icon->y = y; \
				x += 0.5 * (__icon->scale * options.size + options.spacing); \
				__item = __item->next; \
			} \
		}
  double          x = 0.5 * (options.width - width);
  double          y = options.height - options.arrow_size - 0.5 * options.size;

  POSITION(dock.applnks);
  x += 0.5 * options.spacing;
  dock.middle_pos = x;
  x += 1.0 + 0.5 * options.spacing;
  POSITION(dock.dicons);
  POSITION(dock.minwins);

  dock.left_pos = 0.5 * (options.width - width) - 1.0;
  dock.right_pos = 0.5 * (options.width - width) + width + 1.0;

  need_redraw = true;
}

void
od_dock_redraw(Ecore_Evas * ee)
{
  Evas_List      *item = dock.icons;

  while (item) {
    OD_Icon        *icon = (OD_Icon *) item->data;

    double          distance = (icon->x - dock.x) /
      (options.size + options.spacing);
    double          new_zoom, relative_x, size;

    zoom_function(distance, &new_zoom, &relative_x);
    size = icon->scale * new_zoom * options.size;
    evas_object_image_fill_set(icon->icon, 0.0, 0.0, size, size);
    evas_object_resize(icon->icon, size, size);
    evas_object_move(icon->icon,
                     dock.x + relative_x - 0.5 * size,
                     options.height - options.arrow_size - size);
    evas_object_move(icon->arrow,
                     dock.x + relative_x - options.arrow_size,
                     options.height - options.arrow_size);

    if (fabs(relative_x) < 0.5 * size && dock.state != unzoomed)
      od_icon_tt_show(icon);
    else
      od_icon_tt_hide(icon);

    {
      double          w, h;

      evas_object_geometry_get(icon->tt_txt, NULL, NULL, &w, &h);
      evas_object_move(icon->tt_txt, dock.x + relative_x - 0.5 * w,
                       options.height - 2.0 * options.arrow_size - size - h);
      evas_object_move(icon->tt_shd, dock.x + relative_x - 0.5 * w + 1.0,
                       options.height - 2.0 * options.arrow_size - size - h +
                       1.0);
    }

    item = item->next;
  }

  // positions the background pieces
  {
    double          left_end_disp, right_end_disp, middle_disp;
    double          dummy;

    zoom_function((dock.left_pos - dock.x) / (options.size + options.spacing),
                  &dummy, &left_end_disp);
    zoom_function((dock.right_pos - dock.x) / (options.size + options.spacing),
                  &dummy, &right_end_disp);
    zoom_function((dock.middle_pos - dock.x) / (options.size + options.spacing),
                  &dummy, &middle_disp);
    dock.left_end = left_end_disp + dock.x;
    dock.right_end = right_end_disp + dock.x;
    double          middle = middle_disp + dock.x;

    evas_object_move(dock.background[OD_BG_LEFT], dock.left_end,
                     options.height - options.size - 2.0 * options.arrow_size);
    evas_object_move(dock.background[OD_BG_RIGHT], dock.right_end,
                     options.height - options.size - 2.0 * options.arrow_size);
    if (dock.dicons || dock.minwins) {
      evas_object_move(dock.background[OD_BG_MIDDLE], middle,
                       options.height - options.size -
                       2.0 * options.arrow_size);
      evas_object_show(dock.background[OD_BG_MIDDLE]);
    } else
      evas_object_hide(dock.background[OD_BG_MIDDLE]);
    evas_object_image_fill_set(dock.background[OD_BG_FILL], 0.0, 0.0,
                               dock.right_end - dock.left_end - 1.0,
                               options.size + 2.0 * options.arrow_size);
    evas_object_resize(dock.background[OD_BG_FILL],
                       dock.right_end - dock.left_end - 1.0,
                       options.size + 2.0 * options.arrow_size);
    evas_object_move(dock.background[OD_BG_FILL], dock.left_end + 1.0,
                     options.height - options.size - 2.0 * options.arrow_size);
  }

}

void
zoom_function(double d, double *zoom, double *disp)
{
  double          range = 2.5;
  double          f = 1.5;
  double          x = d / range;

  if (d > -range && d < range) {
    *zoom = (dock.zoom - 1.0) * (options.zoomfactor - 1.0) *
      ((sqrt(f * f - 1.0) - sqrt(f * f - x * x)) / (sqrt(f * f - 1.0) - f)
      )
      + 1.0;
    *disp = (options.size + options.spacing) *
      ((dock.zoom - 1.0) * (options.zoomfactor - 1.0) *
       (range *
        (x * (2 * sqrt(f * f - 1.0) - sqrt(f * f - x * x)) -
         f * f * atan(x / sqrt(f * f - x * x))) / (2.0 * (sqrt(f * f - 1.0) -
                                                          f))
       )
       + d);
  } else {
    *zoom = 1.0;
    *disp = (options.size + options.spacing) *
      ((dock.zoom - 1.0) * (options.zoomfactor - 1.0) *
       (range * (sqrt(f * f - 1.0) - f * f * atan(1.0 / sqrt(f * f - 1.0))) /
        (2.0 * (sqrt(f * f - 1.0) - f))
       )
       + range + fabs(d) - range);
    if (d < 0.0)
      *disp = -(*disp);
  }
}

void
od_dock_add_applnk(OD_Icon * applnk)
{
  dock.icons = evas_list_append(dock.icons, applnk);
  dock.applnks = evas_list_append(dock.applnks, applnk);
  applnk->state |= OD_ICON_STATE_USEABLE;
  applnk->appear_timer = ecore_timer_add(0.05, od_dock_icon_appear, applnk);
}

void
od_dock_add_dicon(OD_Icon * dicon)
{
  dock.icons = evas_list_append(dock.icons, dicon);
  dock.dicons = evas_list_append(dock.dicons, dicon);
  dicon->state |= OD_ICON_STATE_USEABLE;
  dicon->appear_timer = ecore_timer_add(0.05, od_dock_icon_appear, dicon);
}

void
od_dock_add_minwin(OD_Icon * minwin)
{
  dock.icons = evas_list_append(dock.icons, minwin);
  dock.minwins = evas_list_append(dock.minwins, minwin);
  minwin->state |= OD_ICON_STATE_USEABLE;
  minwin->appear_timer = ecore_timer_add(0.05, od_dock_icon_appear, minwin);
}

void
od_dock_del_icon(OD_Icon * icon)
{
  if (icon->appear_timer)
    ecore_timer_del(icon->appear_timer);
  ecore_timer_add(0.05, od_dock_icon_disappear, icon);
}

static int
od_dock_icon_appear(void *data)
{
  OD_Icon        *icon = (OD_Icon *) data;

  if (!(icon->state & OD_ICON_STATE_APPEARING)) {
    icon->start_time = ecore_time_get();
    icon->state |= OD_ICON_STATE_APPEARING;
    icon->scale = 0.0;
    od_dock_reposition();
  }
  need_redraw = true;
  double          delta =
    (ecore_time_get() - icon->start_time) / options.icon_appear_duration -
    icon->scale;
  icon->scale += delta;
  if (icon->scale < 1.0) {
    double          s = 0.5 * delta * options.size;
    Evas_List      *item = dock.icons;

    while (item) {
      OD_Icon        *i = (OD_Icon *) item->data;

      if (i->x < icon->x)
        i->x -= s;
      else if (i->x > icon->x)
        i->x += s;
      item = item->next;
    }
    dock.left_pos -= s;
    dock.right_pos += s;
    if (dock.middle_pos < icon->x)
      dock.middle_pos -= s;
    else
      dock.middle_pos += s;
    return 1;
  } else {
    icon->scale = 1.0;
    od_dock_reposition();
    icon->appear_timer = NULL;
    icon->state &= ~OD_ICON_STATE_APPEARING;
    return 0;
  }
}

static int
od_dock_icon_disappear(void *data)
{
  OD_Icon        *icon = (OD_Icon *) data;

  if (!(icon->state & OD_ICON_STATE_DISAPPEARING)) {
    icon->state &= ~OD_ICON_STATE_USEABLE;
    icon->state |= OD_ICON_STATE_DISAPPEARING;
    icon->scale = 1.0;
    icon->start_time = ecore_time_get();
  }
  need_redraw = true;
  double          delta =
    icon->scale - (1.0 -
                   (ecore_time_get() -
                    icon->start_time) / options.icon_appear_duration);
  icon->scale -= delta;
  if (icon->scale > 0.0) {
    double          s = 0.5 * delta * options.size;
    Evas_List      *item = dock.icons;

    while (item) {
      OD_Icon        *i = (OD_Icon *) item->data;

      if (i->x < icon->x)
        i->x += s;
      else if (i->x > icon->x)
        i->x -= s;
      item = item->next;
    }
    dock.left_pos += s;
    dock.right_pos -= s;
    if (dock.middle_pos < icon->x)
      dock.middle_pos += s;
    else
      dock.middle_pos -= s;
    return 1;
  } else {
    dock.icons = evas_list_remove(dock.icons, icon);
    switch (icon->type) {
    case application_link:
      dock.applnks = evas_list_remove(dock.applnks, icon);
      break;
    case docked_icon:
      dock.dicons = evas_list_remove(dock.dicons, icon);
      break;
    case minimised_window:
      dock.minwins = evas_list_remove(dock.minwins, icon);
      break;
    }
    od_dock_reposition();
    // TODO: fade out
    od_icon_del(icon);
    return 0;
  }
}

void
od_dock_zoom_in()
{
  if (od_dock_zoom_timer)
    ecore_timer_del(od_dock_zoom_timer);
  od_dock_zoom_in_slave(NULL);
  od_dock_zoom_timer = ecore_timer_add(0.05, od_dock_zoom_in_slave, NULL);
  need_redraw = true;
}

void
od_dock_zoom_out()
{
  if (od_dock_zoom_timer)
    ecore_timer_del(od_dock_zoom_timer);
  od_dock_zoom_out_slave(NULL);
  od_dock_zoom_timer = ecore_timer_add(0.05, od_dock_zoom_out_slave, NULL);
  need_redraw = true;
}

static int
od_dock_zoom_in_slave(void *data)
{
  static double   start_time;

  if (dock.state == unzoomed)
    start_time = ecore_time_get();
  else if (dock.state == unzooming)
    start_time = ecore_time_get() -
      (dock.zoom - 1.0) / (options.zoomfactor -
                           1.0) * options.dock_zoom_duration;
  dock.zoom =
    (options.zoomfactor -
     1.0) * ((ecore_time_get() - start_time) / options.dock_zoom_duration) +
    1.0;
  need_redraw = true;
  if (dock.zoom < options.zoomfactor) {
    dock.state = zooming;
    return 1;
  } else {
    dock.zoom = options.zoomfactor;
    dock.state = zoomed;
    od_dock_zoom_timer = NULL;
    return 0;
  }
}

static int
od_dock_zoom_out_slave(void *data)
{
  static double   start_time;

  if (dock.state == zoomed)
    start_time = ecore_time_get();
  else if (dock.state == zooming)
    start_time = ecore_time_get() -
      (options.zoomfactor - dock.zoom) / (options.zoomfactor -
                                          1.0) * options.dock_zoom_duration;
  dock.zoom =
    (options.zoomfactor - 1.0) * (1.0 -
                                  (ecore_time_get() -
                                   start_time) / options.dock_zoom_duration) +
    1.0;
  need_redraw = true;
  if (dock.zoom > 1.0) {
    dock.state = unzooming;
    return 1;
  } else {
    dock.zoom = 1.0;
    dock.state = unzoomed;
    od_dock_zoom_timer = NULL;
    return 0;
  }
}
