#include <X11/Xlib.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <math.h>
#include "elicit.h"
#include "shot.h"
#include "config.h"
#include "grab.h"

static Evas_Smart *_smart;

typedef struct Elicit_Shot Elicit_Shot;
typedef struct Elicit_Shot_Callback Elicit_Shot_Callback;

struct Elicit_Shot_Callback
{
  Elicit_Shot_Callback_Func func;
  void *data;
};

struct Elicit_Shot
{
  Evas_Object *smart_obj;

  Evas_Object *obj;
  Evas_Object *grid;
  Evas_Object *clip;
  Evas_Object *event;

  Evas_Coord ow, oh; // current object size
  int iw, ih; // how large of a shot to take
  
  struct { int x, y, w, h; } cur; //location on screen that current shot came from

  struct {
    Evas_Object *box;

    struct {
      struct {
        int x, y;
      } start, end;
      int w, h;
    } canvas, shot;

    float length;
  } measure;

  Eina_List *select_callbacks;
  Eina_List *zoom_callbacks;

  int zoom;

  unsigned char measuring : 1;
  unsigned char grid_visible : 1;
  unsigned has_data : 1;
};


static void shot_coord_convert(Elicit_Shot *shot, int pointer_x, int pointer_y, int *canvas_x, int *canvas_y, int *shot_x, int *shot_y);
static void shot_select_start(Elicit_Shot *sh, int px, int py);
static void shot_select_update(Elicit_Shot *sh, int px, int py);
static void shot_select_end(Elicit_Shot *sh, int px, int py);
static void shot_selection_clear(Elicit_Shot *sh);

static void _smart_init(void);
static void _smart_add(Evas_Object *o);
static void _smart_del(Evas_Object *o);
static void _smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *o);
static void _smart_hide(Evas_Object *o);
static void _smart_color_set(Evas_Object *o, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object *o, Evas_Object *clip);
static void _smart_clip_unset(Evas_Object *o);


Evas_Object *
elicit_shot_add(Evas *evas) 
{
  _smart_init();
  return evas_object_smart_add(evas, _smart);
}

void
elicit_shot_zoom_set(Evas_Object *o, int zoom)
{
  Elicit_Shot *sh;
  Elicit_Shot_Event_Zoom_Level *event;
  Elicit_Shot_Callback *cb;
  Eina_List *l;
  int iw, ih;

  sh = evas_object_smart_data_get(o);

  if (zoom < 1) zoom = 1;
  if (sh->zoom == zoom) return;

  //XXX if zoom is less than before, retake shot to fill in missing data
  //XXX zoom selection instead of clearing it?
  shot_selection_clear(sh);

  sh->zoom = zoom;
  evas_object_image_size_get(sh->obj, &iw, &ih);
  evas_object_image_fill_set(sh->obj, 0, 0, iw * zoom, ih * zoom);

  evas_object_image_fill_set(sh->grid, 0, 0, zoom, zoom);

  event = calloc(1, sizeof(Elicit_Shot_Event_Zoom_Level));
  event->zoom_level = sh->zoom;
  EINA_LIST_FOREACH(sh->zoom_callbacks, l, cb)
    cb->func(cb->data, event);
  free(event);
}

void
elicit_shot_size_get(Evas_Object *o, int *w, int *h)
{
  Elicit_Shot *sh;
  sh = evas_object_smart_data_get(o);
  //XXX cache this calc on resize or zoom level change?
  if (w) *w = (sh->ow / sh->zoom) + (sh->ow % sh->zoom ? 1 : 0);
  if (h) *h = (sh->oh / sh->zoom) + (sh->oh % sh->zoom ? 1 : 0);
}

void
elicit_shot_grid_visible_set(Evas_Object *o, int visible)
{
  Elicit_Shot *sh;
  Evas_Object *gui;
  sh = evas_object_smart_data_get(o);

  if (sh->grid_visible == visible)
    return;

  sh->grid_visible = visible;
  gui = evas_object_name_find(evas_object_evas_get(o), "gui");

  if (sh->has_data)
  {
    if (visible)
      evas_object_show(sh->grid);
    else
      evas_object_hide(sh->grid);
  }
}

void
elicit_shot_grab(Evas_Object *o, int x, int y, int w, int h, int force)
{
  Elicit_Shot *sh;
  void *data;

  sh = evas_object_smart_data_get(o);

  // don't grab same region twice
  if (!force && (x == sh->cur.x && y == sh->cur.y && w == sh->cur.w && h == sh->cur.h))
    return;

  sh->iw = w;
  sh->ih = h;

  evas_object_image_alpha_set(sh->obj, 0);
  evas_object_image_size_set(sh->obj, sh->iw, sh->ih);
  evas_object_image_smooth_scale_set(sh->obj, 0);

  data = evas_object_image_data_get(sh->obj, 1);

  if (!elicit_grab_region(x, y, sh->iw, sh->ih, 0, data))
  {
    fprintf(stderr, "[Elicit] Error: can't grab region\n");
    return;
  }

  evas_object_image_data_set(sh->obj, data);
  evas_object_image_data_update_add(sh->obj, 0, 0, sh->iw, sh->ih);

  /* set it to fill at the current zoom level */
  evas_object_image_fill_set(sh->obj, 0, 0, sh->iw * sh->zoom, sh->ih * sh->zoom);
  evas_object_resize(sh->obj, sh->iw * sh->zoom, sh->ih * sh->zoom);
  evas_object_show(sh->obj);
  if (sh->grid_visible)
    evas_object_show(sh->grid);

  sh->has_data = 1;
}

void
elicit_shot_data_get(Evas_Object *o, void **data, int *w, int *h) 
{
  Elicit_Shot *sh;
  int iw, ih;

  sh = evas_object_smart_data_get(o);
  if (!sh) return;

  evas_object_image_size_get(sh->obj, &iw, &ih);
  if (w) *w = iw;
  if (h) *h = ih;
  if (data) *data = evas_object_image_data_get(sh->obj, 1);
}

void
elicit_shot_data_set(Evas_Object *o, void *data, int w, int h)
{
  Elicit_Shot *sh;

  sh = evas_object_smart_data_get(o);
  if (!sh) return;

  sh->iw = w;
  sh->ih = h;

  /* get the object ready, copy the data in */
  evas_object_image_alpha_set(sh->obj, 0);
  evas_object_image_size_set(sh->obj, sh->iw, sh->ih);
  evas_object_image_smooth_scale_set(sh->obj, 0);
  evas_object_image_data_copy_set(sh->obj, data);
  
  /* tell evas that we changed part of the image data */
  evas_object_image_data_update_add(sh->obj, 0, 0, sh->iw, sh->ih);

  /* set it to fill at the current zoom level */
  evas_object_image_fill_set(sh->obj, 0, 0, sh->iw * sh->zoom, sh->ih * sh->zoom);
  evas_object_resize(sh->obj, sh->iw * sh->zoom, sh->ih * sh->zoom);

  sh->has_data = 1;
}

static void
shot_coord_convert(Elicit_Shot *shot, int pointer_x, int pointer_y, int *canvas_x, int *canvas_y, int *shot_x, int *shot_y)
{
  Evas_Coord sx, sy;
  int cx, cy, shx, shy;

  evas_object_geometry_get(shot->obj, &sx, &sy, NULL, NULL);
  shx = (pointer_x - sx) / shot->zoom;
  shy = (pointer_y - sy) / shot->zoom;
  cx = shx * shot->zoom + sx - 1;
  cy = shy * shot->zoom + sy - 1;

  if (shot_x) *shot_x = shx;
  if (shot_y) *shot_y = shy;
  if (canvas_x) *canvas_x = cx;
  if (canvas_y) *canvas_y = cy;
}

static void
shot_select_start(Elicit_Shot *sh, int px, int py)
{
  if (!sh->measure.box)
  {
    const char *file;
    file = elicit_data_file_find("images/grid_select_box.png");
    if (file)
    {
      sh->measure.box = evas_object_image_add(evas_object_evas_get(sh->obj));
      evas_object_pass_events_set(sh->measure.box, 1);
      evas_object_image_file_set(sh->measure.box, file, "");
      evas_object_clip_set(sh->measure.box, sh->clip);
      evas_object_image_border_set(sh->measure.box, 2, 2, 3, 3);
      evas_object_smart_member_add(sh->measure.box, sh->smart_obj);
    }
  }
  if (!sh->measure.box) return;

  shot_coord_convert(sh, px, py,
    &(sh->measure.canvas.start.x), &(sh->measure.canvas.start.y),
    &(sh->measure.shot.start.x), &(sh->measure.shot.start.y));

  evas_object_move(sh->measure.box, sh->measure.canvas.start.x, sh->measure.canvas.start.y);
  evas_object_image_fill_set(sh->measure.box, 0, 0, sh->zoom, sh->zoom);
  evas_object_raise(sh->measure.box);
  evas_object_resize(sh->measure.box, sh->zoom + 1, sh->zoom + 1);
  evas_object_hide(sh->measure.box);

  sh->measure.length = 0;
  sh->measuring = 1;
}

static void
shot_select_update(Elicit_Shot *sh, int px, int py)
{
  int cx, cy, cw, ch;
  Eina_List *l;
  Elicit_Shot_Callback *cb;
  Elicit_Shot_Event_Selection *event;

  shot_coord_convert(sh, px, py,
    &(sh->measure.canvas.end.x), &(sh->measure.canvas.end.y),
    &(sh->measure.shot.end.x), &(sh->measure.shot.end.y));

  cx = sh->measure.canvas.start.x;
  if (sh->measure.canvas.end.x < cx)
    cx = sh->measure.canvas.end.x;

  cy = sh->measure.canvas.start.y;
  if (sh->measure.canvas.end.y < cy)
    cy = sh->measure.canvas.end.y;

  cw = abs(sh->measure.canvas.end.x - sh->measure.canvas.start.x) + sh->zoom + 1;
  ch = abs(sh->measure.canvas.end.y - sh->measure.canvas.start.y) + sh->zoom + 1;

  sh->measure.canvas.w = cw;
  sh->measure.canvas.h = ch;
  sh->measure.shot.w = cw / sh->zoom;
  sh->measure.shot.h = ch / sh->zoom;

  if (sh->measure.shot.w == 1)
    sh->measure.length = sh->measure.shot.h;
  else if (sh->measure.shot.h == 1)
    sh->measure.length = sh->measure.shot.w;
  else
    sh->measure.length = sqrt(
      (sh->measure.shot.w * sh->measure.shot.w) +
      (sh->measure.shot.h * sh->measure.shot.h)
    );

  evas_object_move(sh->measure.box, cx, cy);
  evas_object_resize(sh->measure.box, cw, ch);
  evas_object_image_fill_set(sh->measure.box, 0, 0, cw, ch);
  evas_object_show(sh->measure.box);

  event = calloc(1, sizeof(Elicit_Shot_Event_Selection));
  event->w = sh->measure.shot.w;
  event->h = sh->measure.shot.h;
  event->length = sh->measure.length;
  EINA_LIST_FOREACH(sh->select_callbacks, l, cb)
    cb->func(cb->data, event);
  free(event);
}

static void
shot_select_end(Elicit_Shot *sh, int px, int py)
{
  sh->measuring = 0;
  if (sh->measure.length == 0)
    shot_selection_clear(sh);
}

static void
shot_selection_clear(Elicit_Shot *sh)
{
  Elicit_Shot_Callback *cb;
  Eina_List *l;
  evas_object_hide(sh->measure.box);
  EINA_LIST_FOREACH(sh->select_callbacks, l, cb)
    cb->func(cb->data, NULL);
}

static void
cb_evas_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *ev;
  Elicit_Shot *sh;

  sh = data;
  ev = event_info;

  if (ev->button == 3)
    shot_select_start(sh, ev->canvas.x, ev->canvas.y);
}

static void
cb_evas_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Move *ev;
  Elicit_Shot *sh;

  sh = data;
  ev = event_info;

  if (sh->measuring)
    shot_select_update(sh, ev->cur.canvas.x, ev->cur.canvas.y);
}

static void
cb_evas_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Up *ev;
  Elicit_Shot *sh;

  sh = data;
  ev = event_info;

  if (ev->button == 3)
    shot_select_end(sh, ev->canvas.x, ev->canvas.y);
}

static void
cb_evas_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Wheel *ev;
  Elicit_Shot *sh;

  sh = data;
  ev = event_info;

  elicit_shot_zoom_set(sh->smart_obj, sh->zoom - ev->z);
}

static void
elicit_shot_callback_add(Eina_List **cb_list, Elicit_Shot_Callback_Func func, void *data)
{
  Elicit_Shot_Callback *cb;
  cb = calloc(1, sizeof(Elicit_Shot_Callback));
  cb->func = func;
  cb->data = data;

  *cb_list = eina_list_append(*cb_list, cb);
}

void
elicit_shot_callback_del(Eina_List **cb_list, Elicit_Shot_Callback_Func func)
{
  Eina_List *l, *l_next;
  Elicit_Shot_Callback *cb;

  EINA_LIST_FOREACH_SAFE(*cb_list, l, l_next, cb)
  {
    if (cb->func == func)
      *cb_list = eina_list_remove_list(*cb_list, l);
  }
}

void
elicit_shot_callback_select_add(Evas_Object *obj, Elicit_Shot_Callback_Func func, void *data)
{
  Elicit_Shot *sh;
  sh = evas_object_smart_data_get(obj);
  elicit_shot_callback_add(&(sh->select_callbacks), func, data);
}

void
elicit_shot_callback_select_del(Evas_Object *obj, Elicit_Shot_Callback_Func func)
{
  Elicit_Shot *sh;
  sh = evas_object_smart_data_get(obj);
  elicit_shot_callback_del(&(sh->select_callbacks), func);
}

void
elicit_shot_callback_zoom_add(Evas_Object *obj, Elicit_Shot_Callback_Func func, void *data)
{
  Elicit_Shot *sh;
  sh = evas_object_smart_data_get(obj);
  elicit_shot_callback_add(&(sh->zoom_callbacks), func, data);
}

void
elicit_shot_callback_zoom_del(Evas_Object *obj, Elicit_Shot_Callback_Func func)
{
  Elicit_Shot *sh;
  sh = evas_object_smart_data_get(obj);
  elicit_shot_callback_del(&(sh->zoom_callbacks), func);
}

static void
_smart_init(void)
{
  static const Evas_Smart_Class sc =
  {
    "elicit_shot",
    EVAS_SMART_CLASS_VERSION,
    _smart_add,
    _smart_del,
    _smart_move,
    _smart_resize,
    _smart_show,
    _smart_hide,
    _smart_color_set,
    _smart_clip_set,
    _smart_clip_unset,
    NULL,
    NULL,
    NULL,
    NULL
  };

  if (_smart) return;
  _smart = evas_smart_class_new(&sc);
}

static void
_smart_add(Evas_Object *o)
{
  Elicit_Shot *sh;
  Evas *evas;
  const char *grid_file;

  sh = calloc(1, sizeof(Elicit_Shot));
  if (!sh) return;

  evas_object_smart_data_set(o, sh);

  evas = evas_object_evas_get(o);

  sh->smart_obj = o;
  sh->obj = evas_object_image_add(evas);
  evas_object_smart_member_add(sh->obj, o);

  grid_file = elicit_data_file_find("images/grid_cell.png");
  if (grid_file)
  {
    sh->grid = evas_object_image_add(evas);
    evas_object_pass_events_set(sh->grid, 1);
    evas_object_image_file_set(sh->grid, grid_file, "");
    evas_object_smart_member_add(sh->grid, o);
  }
  else
  {
    fprintf(stderr, "[Elicit] Error: could not find grid_cell.png");
  }

  sh->clip = evas_object_rectangle_add(evas);
  evas_object_clip_set(sh->obj, sh->clip);
  evas_object_clip_set(sh->grid, sh->clip);
  evas_object_smart_member_add(sh->clip, o);

  sh->event = evas_object_rectangle_add(evas);
  evas_object_color_set(sh->event, 0, 0, 0, 0);
  evas_object_repeat_events_set(sh->event, 1);
  evas_object_smart_member_add(sh->grid, o);
  evas_object_event_callback_add(sh->event, EVAS_CALLBACK_MOUSE_DOWN, cb_evas_mouse_down, sh);
  evas_object_event_callback_add(sh->event, EVAS_CALLBACK_MOUSE_MOVE, cb_evas_mouse_move, sh);
  evas_object_event_callback_add(sh->event, EVAS_CALLBACK_MOUSE_UP, cb_evas_mouse_up, sh);
  evas_object_event_callback_add(sh->event, EVAS_CALLBACK_MOUSE_WHEEL, cb_evas_mouse_wheel, sh);

  elicit_shot_zoom_set(o, 4);
}

static void
_smart_del(Evas_Object *o)
{
  Elicit_Shot *sh;
  Elicit_Shot_Callback *cb;

  sh = evas_object_smart_data_get(o);

  if (sh->obj) evas_object_del(sh->obj);
  if (sh->grid) evas_object_del(sh->grid);
  if (sh->clip) evas_object_del(sh->clip);
  if (sh->measure.box) evas_object_del(sh->measure.box);

  EINA_LIST_FREE(sh->select_callbacks, cb)
  {
    free(cb);
  }

  free(sh);
  evas_object_smart_data_set(o, NULL);
}

static void
_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Elicit_Shot *sh;

  sh = evas_object_smart_data_get(o);

  evas_object_move(sh->grid, x, y);
  evas_object_move(sh->obj, x, y);
  evas_object_move(sh->clip, x, y);
  evas_object_move(sh->event, x, y);
}

static void
_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Elicit_Shot *sh;

  sh = evas_object_smart_data_get(o);
  evas_object_resize(sh->obj, sh->iw * sh->zoom, sh->ih * sh->zoom);
  evas_object_resize(sh->grid, w, h);
  evas_object_resize(sh->clip, w, h);
  evas_object_resize(sh->event, w, h);

  sh->ow = w;
  sh->oh = h;
}

static void
_smart_show(Evas_Object *o)
{
  Elicit_Shot *sh;

  sh = evas_object_smart_data_get(o);

  evas_object_show(sh->obj);
  evas_object_show(sh->clip);
  evas_object_show(sh->event);
  if (sh->has_data && sh->grid_visible)
    evas_object_show(sh->grid);
  else
    evas_object_hide(sh->grid);
}

static void
_smart_hide(Evas_Object *o)
{
  Elicit_Shot *sh;

  sh = evas_object_smart_data_get(o);

  evas_object_hide(sh->obj);
  evas_object_hide(sh->grid);
  evas_object_hide(sh->clip);
  evas_object_hide(sh->event);
}

static void
_smart_color_set(Evas_Object *o, int r, int g, int b, int a)
{
  Elicit_Shot *sh;

  sh = evas_object_smart_data_get(o);

  evas_object_color_set(sh->clip, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Elicit_Shot *sh;
  
  sh = evas_object_smart_data_get(o);

  evas_object_clip_set(sh->clip, clip);
}

static void
_smart_clip_unset(Evas_Object *o)
{
  Elicit_Shot *sh;
  
  sh = evas_object_smart_data_get(o);

  evas_object_clip_unset(sh->clip);
}

