#include <Edje.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "palette_view.h"

static Evas_Smart *pv_smart;
Evas_Smart_Class pv_class;

static void pv_init();
static void pv_add(Evas_Object *obj);
static void pv_del(Evas_Object *obj);
static void pv_show(Evas_Object *obj);
static void pv_hide(Evas_Object *obj);
static void pv_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void pv_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void pv_clip_set(Evas_Object *obj, Evas_Object *clip);
static void pv_clip_unset(Evas_Object *obj);

static void pv_layout(Evas_Object *obj);
static int pv_layout_timer(void *data);

static void cb_swatch_in(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void cb_swatch_out(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void cb_swatch_up(void *data, Evas *evas, Evas_Object *obj, void *event_info);

#undef API_ENTRY
#define API_ENTRY \
  Palette_View *pv; \
  pv = evas_object_smart_data_get(obj); \
  if (!pv) return

/** API **/
Evas_Object *
palette_view_add(Evas *evas)
{
  pv_init();
  return evas_object_smart_add(evas, pv_smart);
}

void
palette_view_palette_set(Evas_Object *obj, Palette *palette)
{
  API_ENTRY;

  pv->palette = palette;
  pv_layout(obj);
}

Palette *
palette_view_palette_get(Evas_Object *obj)
{
  API_ENTRY NULL;

  return pv->palette;
}

void
palette_view_changed(Evas_Object *obj)
{
  API_ENTRY;
  pv_layout(obj);
}

void
palette_view_theme_set(Evas_Object *obj, const char *file, const char *group)
{
  API_ENTRY;

  if (pv->theme.file) free(pv->theme.file);
  if (pv->theme.group) free(pv->theme.group);

  pv->theme.file = strdup(file);
  pv->theme.group = strdup(group);
  pv->theme.changed = 1;
}

void
palette_view_select(Evas_Object *obj, Color *c)
{
  API_ENTRY;

  if (pv->selected) color_unref(pv->selected);

  pv->selected = c;
  if (pv->selected) color_ref(pv->selected);

  palette_view_changed(obj);
}

Color *
palette_view_selected(Evas_Object *obj)
{
  API_ENTRY NULL;

  return pv->selected;
}

static void
pv_init()
{
  if (pv_smart) return;
  pv_class.name = "Palette_View";
  pv_class.version = EVAS_SMART_CLASS_VERSION;
  pv_class.add = pv_add;
  pv_class.del = pv_del;
  pv_class.show = pv_show;
  pv_class.hide = pv_hide;
  pv_class.move = pv_move;
  pv_class.resize = pv_resize;
  pv_class.clip_set = pv_clip_set;
  pv_class.clip_unset = pv_clip_unset;
  pv_smart = evas_smart_class_new(&pv_class);
}


static void
pv_add(Evas_Object *obj)
{
  Palette_View *pv;

  pv = calloc(1, sizeof(Palette_View));
  pv->smart_obj = obj;
  pv->rects = NULL;
  pv->size = 20;

  pv->selector = edje_object_add(evas_object_evas_get(obj));
  evas_object_smart_member_add(pv->selector, pv->smart_obj);

  evas_object_smart_data_set(obj, pv);
}

static void
pv_del(Evas_Object *obj)
{
  Eina_List *l;
  Evas_Object *rect;

  API_ENTRY;

  EINA_LIST_FOREACH(pv->rects, l, rect)
  {
    Color *c;
    c = evas_object_data_get(rect, "Color");
    color_unref(c);
    evas_object_del(rect);
  }
  eina_list_free(pv->rects);

  if (pv->selected) color_unref(pv->selected);
  if (pv->selector) evas_object_del(pv->selector);

  if (pv->theme.file) free(pv->theme.file);
  if (pv->theme.group) free(pv->theme.group);

  free(pv);
}

static void
pv_show(Evas_Object *obj)
{
  Eina_List *l;
  Evas_Object *rect;
  API_ENTRY;

  EINA_LIST_FOREACH(pv->rects, l, rect) {
    evas_object_show(rect);
  }
  if (pv->selected)
    evas_object_show(pv->selector);
}

static void
pv_hide(Evas_Object *obj)
{
  Eina_List *l;
  Evas_Object *rect;
  API_ENTRY;

  EINA_LIST_FOREACH(pv->rects, l, rect) {
    evas_object_hide(rect);
  }
  if (pv->selected)
    evas_object_hide(pv->selector);
}

static void
pv_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
  API_ENTRY;
  if (pv->x == x && pv->y == y) return;
  pv->x = x;
  pv->y = y;
  pv_layout(obj);
}

static void
pv_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
  API_ENTRY;
  if (pv->w == w && pv->h == h) return;
  pv->w = w;
  pv->h = h;
  pv_layout(obj);
}

static void
pv_clip_set(Evas_Object *obj, Evas_Object *clip)
{
  Eina_List *l;
  Evas_Object *rect;
  API_ENTRY;

  EINA_LIST_FOREACH(pv->rects, l, rect) {
    evas_object_clip_set(rect, clip);
  }
  evas_object_clip_set(pv->selector, clip);
}

static void
pv_clip_unset(Evas_Object *obj)
{
  Eina_List *l;
  Evas_Object *rect;
  API_ENTRY;

  EINA_LIST_FOREACH(pv->rects, l, rect) {
    evas_object_clip_unset(rect);
  }
  evas_object_clip_unset(pv->selector);
}

static void
pv_layout(Evas_Object *obj)
{
  API_ENTRY;
  if (pv->layout_timer) ecore_timer_del(pv->layout_timer);
  pv->layout_timer = ecore_timer_add(0.01, pv_layout_timer, pv);
}

static int
pv_layout_timer(void *data)
{
  Evas_Object *rect;
  Eina_List *colors, *lc, *lr, *lr_next;
  Color *c, *cc;
  int cols, i;
  Evas_Object *clip;
  int x, y, w, h, adj;
  Palette_View *pv;

  pv = data;
  if (!pv) return 0;

  if (pv->theme.changed)
    edje_object_file_set(pv->selector, pv->theme.file, "elicit.palette.selector");
  if (!pv->selected)
    evas_object_hide(pv->selector);

  x = y = w = h = adj = 0;

  lr = NULL;
  if (pv->palette && pv->theme.file && pv->theme.group)
  {
    clip = evas_object_clip_get(pv->smart_obj);

    colors = palette_colors_get(pv->palette);
    cols = palette_columns_get(pv->palette);
    if (cols == 0) cols = 1; //XXX use a global config value instead

    w = pv->w / cols;
    h = pv->size;

    adj = (pv->w - (w * cols)) / 2 + 1;

    i = 0;
    lr = pv->rects;
    EINA_LIST_FOREACH(colors, lc, c)
    {
      int r,g,b;
      int rw, rh;

      rect = eina_list_data_get(lr);
      if (!rect)
      {
        rect = edje_object_add(evas_object_evas_get(pv->smart_obj));
        evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_IN, cb_swatch_in, pv);
        evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_OUT, cb_swatch_out, pv);
        evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, cb_swatch_up, pv);
        if (clip) evas_object_clip_set(rect, clip);
        edje_object_file_set(rect, pv->theme.file, pv->theme.group);

        pv->rects = eina_list_append(pv->rects, rect);
        evas_object_smart_member_add(rect, pv->smart_obj);
      }
      else if (pv->theme.changed)
        edje_object_file_set(rect, pv->theme.file, pv->theme.group);

      color_rgba_get(c, &r, &g, &b, NULL);
      edje_object_color_class_set(rect, "palette.swatch", r, g, b, 255, 0, 0, 0, 0, 0, 0, 0, 0);

      x = pv->x + w * (i % cols) + (i % cols ? adj : 0);
      y = pv->y + h * (i / cols);

      rw = w + ((i % cols == 0) ? adj : (i % cols == cols - 1) ? adj + 1 : 0);
      rh = h;
      evas_object_move(rect, x, y);
      // left most gets corrected by 'adj', rightmost is corrected by adj + 1 to fill space (they are clipped, so going over is ok)
      evas_object_resize(rect, rw, rh);
      evas_object_show(rect);

      cc = evas_object_data_get(rect, "Color");
      if (cc) color_unref(cc);

      color_ref(c);
      evas_object_data_set(rect, "Color", c);

      /* move selector to proper locations */
      if (c == pv->selected)
      {
        evas_object_move(pv->selector, x, y);
        evas_object_resize(pv->selector, rw, rh);
        evas_object_show(pv->selector);
      }

      //printf("place %s at (%d,%d) %d x %d\n", color_hex_get(c, 1), x, y, w, h);
      i++;
      lr = eina_list_next(lr);
    } 
    evas_object_raise(pv->selector);
    evas_object_resize(pv->smart_obj, pv->w, y + h - pv->y);
  }

  // remove any leftover rects from a previous larger palette
  while (lr)
  {
    lr_next = eina_list_next(lr);
    rect = eina_list_data_get(lr);
    cc = evas_object_data_get(rect, "Color");
    if (cc) color_unref(cc);
    evas_object_smart_member_del(rect);
    evas_object_del(rect);
    pv->rects = eina_list_remove_list(pv->rects, lr);
    lr = lr_next;
  }

  pv->layout_timer = NULL;
  return 0;
}


static void
cb_swatch_in(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
  Color *c;
  c = evas_object_data_get(obj, "Color");
  if (!c) return;
}

static void
cb_swatch_out(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
  Color *c;
  c = evas_object_data_get(obj, "Color");
  if (!c) return;
}

static void
cb_swatch_up(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Up *ev;
  Palette_View *pv;
  Color *c;

  ev = event_info;
  pv = data; 
  c = evas_object_data_get(obj, "Color");
  if (!c) return;

  if (ev->button == 1)
  {
    Eina_List *l;
    Evas_Object *rect;
    EINA_LIST_FOREACH(pv->rects, l, rect)
    {
      edje_object_signal_emit(rect, "elicit,swatch,deselect", "elicit");
    }
    edje_object_signal_emit(obj, "elicit,swatch,select", "elicit");
    evas_object_raise(obj);

    palette_view_select(pv->smart_obj, c);
    evas_object_smart_callback_call(pv->smart_obj, "selected", c);
  }
  else if (ev->button == 3)
  {
    palette_color_remove(pv->palette, c);
    palette_view_changed(pv->smart_obj);
    palette_view_select(pv->smart_obj, NULL);
    evas_object_smart_callback_call(pv->smart_obj, "deleted", c);
  }
}

