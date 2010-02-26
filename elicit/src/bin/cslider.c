#include "cslider.h"

static Evas_Smart *_smart;

struct _Elicit_Cslider
{
  Evas_Object *gui;
  Evas_Object *spectrum;
  Color *color;
  Color_Type type;
};

static void _smart_init(void);
static void _smart_add(Evas_Object *o);
static void _smart_del(Evas_Object *o);
static void _smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *o);
static void _smart_hide(Evas_Object *o);
static void _smart_clip_set(Evas_Object *o, Evas_Object *clip);
static void _smart_clip_unset(Evas_Object *o);


static void cb_drag(void *data, Evas_Object *obj, const char *signal, const char *source);
static void cb_scroll(void *data, Evas_Object *obj, const char *signal, const char *source);
static void cb_color_changed(Color *color, void *data);

static char *cslider_labels[6] = { "R:", "G:", "B:", "H:", "S:", "V:" };

Evas_Object *
elicit_cslider_add(Evas *evas) 
{
  _smart_init();
  return evas_object_smart_add(evas, _smart);
}

int
elicit_cslider_theme_set(Evas_Object *o, const char *file, const char *group)
{
  Elicit_Cslider *cs;

  cs = evas_object_smart_data_get(o);

  if (cs->gui && cs->spectrum)
    edje_object_part_unswallow(cs->gui, cs->spectrum);

  if(!edje_object_file_set(cs->gui, file, group))
  {
    fprintf(stderr, "[Elicit] Error setting cslider theme (%s, %s)\n", file, group);
    return 0;
  }

  edje_object_signal_callback_add(cs->gui, "drag", "slider", cb_drag, cs);
  edje_object_signal_callback_add(cs->gui, "elicit,scroll,*", "*", cb_scroll, cs);

  if (edje_object_part_exists(cs->gui, "spectrum"))
  {
    if (!cs->spectrum)
    {
      cs->spectrum = evas_object_gradient_add(evas_object_evas_get(o));
      evas_object_gradient_angle_set(cs->spectrum, 270);
    }

    edje_object_part_swallow(cs->gui, "spectrum", cs->spectrum);
  }
  return 1;
}

void
elicit_cslider_color_set(Evas_Object *o, Color *color, Color_Type type)
{
  Elicit_Cslider *cs;

  cs = evas_object_smart_data_get(o);

  if (cs->color)
  {
    color_callback_changed_del(cs->color, cb_color_changed);
    color_unref(cs->color);
  }

  cs->color = color;
  color_ref(color);

  cs->type = type;

  if (edje_object_part_exists(cs->gui, "label")) {
    edje_object_part_text_set(cs->gui, "label", cslider_labels[cs->type]);
  }

  color_callback_changed_add(cs->color, cb_color_changed, cs);

}

static void
cb_drag(void *data, Evas_Object *obj, const char *signal, const char *source)
{
  Elicit_Cslider *cs = data;
  double val;

  if (edje_object_part_drag_dir_get(obj, source))
    edje_object_part_drag_value_get(obj, source, &val, NULL);
  else
    edje_object_part_drag_value_get(obj, source, NULL, &val);

  if (cs->type == COLOR_TYPE_RED)
    color_rgba_set(cs->color, (int)(255*val), -1, -1, -1);
  else if (cs->type == COLOR_TYPE_GREEN)
    color_rgba_set(cs->color, -1, (int)(255*val), -1, -1);
  else if (cs->type == COLOR_TYPE_BLUE)
    color_rgba_set(cs->color, -1, -1, (int)(255*val), -1);
  else if (cs->type == COLOR_TYPE_HUE)
    color_hsva_set(cs->color, 360*val, -1, -1, -1);
  else if (cs->type == COLOR_TYPE_SATURATION)
    color_hsva_set(cs->color, -1, val, -1, -1);
  else if (cs->type == COLOR_TYPE_VALUE)
    color_hsva_set(cs->color, -1, -1, val, -1);
}

static void
cb_scroll(void *data, Evas_Object *obj, const char *signal, const char *source)
{
  Elicit_Cslider *cs = data;

  int dir;
  int r, g, b;
  float h, s, v;

  if (!strcmp(signal+14, "up")) 
    dir = 1;
  else if (!strcmp(signal+14, "down"))
    dir = -1;
  else
    return;

  color_rgba_get(cs->color, &r, &g, &b, NULL);
  color_hsva_get(cs->color, &h, &s, &v, NULL);

  if (cs->type == COLOR_TYPE_RED)
    color_rgba_set(cs->color, r + dir, -1, -1, -1);
  else if (cs->type == COLOR_TYPE_GREEN)
    color_rgba_set(cs->color, -1, g + dir, -1, -1);
  else if (cs->type == COLOR_TYPE_BLUE)
    color_rgba_set(cs->color, -1, -1, b + dir, -1);
  else if (cs->type == COLOR_TYPE_HUE)
    color_hsva_set(cs->color, h + dir, -1, -1, -1);
  else if (cs->type == COLOR_TYPE_SATURATION)
    color_hsva_set(cs->color, -1, s + 0.01 * dir, -1, -1);
  else if (cs->type == COLOR_TYPE_VALUE)
    color_hsva_set(cs->color, -1, -1, v + 0.01 * dir, -1);

}

static void
cb_color_changed(Color *color, void *data)
{
  Elicit_Cslider *cs = data;

  char buf[20];

  double val;
  int r, g, b, a;
  float h, s, v;

  color_rgba_get(cs->color, &r, &g, &b, &a);
  color_hsva_get(cs->color, &h, &s, &v, NULL);

  if (cs->spectrum)
    evas_object_gradient_clear(cs->spectrum);

  switch(cs->type)
  {
    case COLOR_TYPE_RED:
      val = r/255.0;
      snprintf(buf, sizeof(buf), "%d", r);
      if (cs->spectrum)
      {
        evas_object_gradient_color_stop_add(cs->spectrum, 0, g, b, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, 255, g, b, 255, 1);
      }
      break;
    case COLOR_TYPE_GREEN:
      val = g/255.0;
      snprintf(buf, sizeof(buf), "%d", g);
      if (cs->spectrum)
      {
        evas_object_gradient_color_stop_add(cs->spectrum, r, 0, b, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, r, 255, b, 255, 1);
      }
      break;
    case COLOR_TYPE_BLUE:
      val = b/255.0;
      snprintf(buf, sizeof(buf), "%d", b);
      if (cs->spectrum)
      {
        evas_object_gradient_color_stop_add(cs->spectrum, r, g, 0, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, r, g, 255, 255, 1);
      }
      break;
    case COLOR_TYPE_HUE:
      val = h/360.0;
      snprintf(buf, sizeof(buf), "%d", (int)h);
      if (cs->spectrum)
      {
        int max, min;
        evas_color_hsv_to_rgb(0, s, v, &max, &min, NULL);
        evas_object_gradient_color_stop_add(cs->spectrum, max, min, min, 255, 0);
        evas_object_gradient_color_stop_add(cs->spectrum, max, max, min, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, min, max, min, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, min, max, max, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, min, min, max, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, max, min, max, 255, 1);
        evas_object_gradient_color_stop_add(cs->spectrum, max, min, min, 255, 1);
      }
      break;
    case COLOR_TYPE_SATURATION:
      val = s;
      snprintf(buf, sizeof(buf), "%.2f", val);
      if (cs->spectrum)
      {
        int r, g, b;
        evas_color_hsv_to_rgb(h, 0, v, &r, &g, &b);
        evas_object_gradient_color_stop_add(cs->spectrum, r, g, b, 255, 0);
        evas_color_hsv_to_rgb(h, 1, v, &r, &g, &b);
        evas_object_gradient_color_stop_add(cs->spectrum, r, g, b, 255, 1);
      }
      break;
    case COLOR_TYPE_VALUE:
      val = v;
      snprintf(buf, sizeof(buf), "%.2f", val);
      if (cs->spectrum)
      {
        int r, g, b;
        evas_color_hsv_to_rgb(h, s, 0, &r, &g, &b);
        evas_object_gradient_color_stop_add(cs->spectrum, r, g, b, 255, 0);
        evas_color_hsv_to_rgb(h, s, 1, &r, &g, &b);
        evas_object_gradient_color_stop_add(cs->spectrum, r, g, b, 255, 1);
      }
      break;
    default:
      return;
  }

  edje_object_part_text_set(cs->gui, "val", buf);
  edje_object_part_drag_value_set(cs->gui, "slider", val, val);
}



static void
_smart_init(void)
{
  static const Evas_Smart_Class sc =
  {
    "elicit_cslider",
    EVAS_SMART_CLASS_VERSION,
    _smart_add,
    _smart_del,
    _smart_move,
    _smart_resize,
    _smart_show,
    _smart_hide,
    NULL,
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
  Elicit_Cslider *cs;

  cs = calloc(1, sizeof(Elicit_Cslider));
  if (!cs) return;

  evas_object_smart_data_set(o, cs);

  cs->gui = edje_object_add(evas_object_evas_get(o));

  evas_object_smart_member_add(cs->gui, o);
}

static void
_smart_del(Evas_Object *o)
{
  Elicit_Cslider *cs;

  cs = evas_object_smart_data_get(o);

  if (cs->gui) evas_object_del(cs->gui);
  if (cs->spectrum) evas_object_del(cs->spectrum);
  color_unref(cs->color);

  free(cs);
  evas_object_smart_data_set(o, NULL);
}

static void
_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Elicit_Cslider *cs;

  cs = evas_object_smart_data_get(o);

  evas_object_move(cs->gui, x, y);
}

static void
_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Elicit_Cslider *cs;
  Evas_Coord sw, sh;

  cs = evas_object_smart_data_get(o);
  evas_object_resize(cs->gui, w, h);

  evas_object_geometry_get(cs->spectrum, NULL, NULL, &sw, &sh);
  evas_object_gradient_fill_set(cs->spectrum, 0, 0, sw, sh);
}

static void
_smart_show(Evas_Object *o)
{
  Elicit_Cslider *cs;

  cs = evas_object_smart_data_get(o);

  evas_object_show(cs->gui);
}

static void
_smart_hide(Evas_Object *o)
{
  Elicit_Cslider *cs;

  cs = evas_object_smart_data_get(o);

  evas_object_hide(cs->gui);
}

static void
_smart_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Elicit_Cslider *cs;
  
  cs = evas_object_smart_data_get(o);

  evas_object_clip_set(cs->gui, clip);
}

static void
_smart_clip_unset(Evas_Object *o)
{
  Elicit_Cslider *cs;
  
  cs = evas_object_smart_data_get(o);

  evas_object_clip_unset(cs->gui);
}

