#include "Elicit.h"

static Evas_Smart *_smart;

static void _smart_init(void);
static void _smart_add(Evas_Object *o);
static void _smart_del(Evas_Object *o);
static void _smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *o);
static void _smart_hide(Evas_Object *o);
static void _smart_clip_set(Evas_Object *o, Evas_Object *clip_o);
static void _smart_clip_unset(Evas_Object *o);


char * spectrum_names[ELICIT_SPECTRUM_COUNT] = {
  "red-spectrum",
  "green-spectrum",
  "blue-spectrum",
  "hue-spectrum",
  "sat-spectrum",
  "val-spectrum"
};

void
elicit_spectra_init(Elicit *el)
{
  Evas_Object *o;
  int i;

  for (i = 0; i < ELICIT_SPECTRUM_COUNT; i++) {
    if (edje_object_part_exists(el->gui, spectrum_names[i]))
    {
      o = elicit_spectrum_add(el->evas);
      elicit_spectrum_mode_set(o, (Elicit_Spectrum_Mode)i);
      evas_object_name_set(o, spectrum_names[i]);
      evas_object_show(o);
      edje_object_part_swallow(el->gui, spectrum_names[i], o);
    }
  }
  elicit_spectra_update(el);
}

void
elicit_spectra_update(Elicit *el)
{
  int i;
  for (i = 0; i < ELICIT_SPECTRUM_COUNT; i++) {
    Evas_Object *o = NULL;
    if ((o = evas_object_name_find(el->evas, spectrum_names[i]))) {
      elicit_spectrum_color_set(o, el->color.r, el->color.g, el->color.b, el->color.h, el->color.s, el->color.v);
    }
  }
}

void
elicit_spectra_shutdown(Elicit *el)
{
  int i;
  for (i = 0; i < ELICIT_SPECTRUM_COUNT; i++) {
    Evas_Object *o = NULL;
    if ((o = evas_object_name_find(el->evas, spectrum_names[i]))) {
      edje_object_part_unswallow(el->gui, o);
      evas_object_del(o);
    }
  }
}

Evas_Object *
elicit_spectrum_add(Evas *evas) 
{
  _smart_init();
  return evas_object_smart_add(evas, _smart);
  
}

void
elicit_spectrum_mode_set(Evas_Object *o, Elicit_Spectrum_Mode mode)
{
  Elicit_Spectrum *s;
  
  s = evas_object_smart_data_get(o);
  if (!s) return;

  s->mode = mode;
}

void
elicit_spectrum_color_set(Evas_Object *o, int r, int g, int b, int h, float s, float v)
{
  Elicit_Spectrum *sp;
  int min, max;
  
  sp = evas_object_smart_data_get(o);
  if (!sp) return;

  evas_object_gradient_clear(sp->grad);
  switch(sp->mode)
  {
    case ELICIT_SPECTRUM_R:
      evas_object_gradient_color_stop_add(sp->grad, 0, g, b, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, 255, g, b, 255, 1);
      break;
    case ELICIT_SPECTRUM_G:
      evas_object_gradient_color_stop_add(sp->grad, r, 0, b, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, r, 255, b, 255, 1);
      break;
    case ELICIT_SPECTRUM_B:
      evas_object_gradient_color_stop_add(sp->grad, r, g, 0, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, r, g, 255, 255, 1);
      break;
    case ELICIT_SPECTRUM_H:
/*
 * Color Stops:
 *   0 x n n
 *  60 x x n
 * 120 n x n
 * 180 n x x
 * 240 n n x
 * 300 x n x
 * 360 x n n
 */
      evas_color_hsv_to_rgb(0, s, v, &max, &min, NULL);

      evas_object_gradient_color_stop_add(sp->grad, max, min, min, 255, 0);
      evas_object_gradient_color_stop_add(sp->grad, max, max, min, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, min, max, min, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, min, max, max, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, min, min, max, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, max, min, max, 255, 1);
      evas_object_gradient_color_stop_add(sp->grad, max, min, min, 255, 1);
      break;
    case ELICIT_SPECTRUM_S:
      evas_color_hsv_to_rgb(h, 0, v, &r, &g, &b);
      evas_object_gradient_color_stop_add(sp->grad, r, g, b, 255, 0);
      evas_color_hsv_to_rgb(h, 1, v, &r, &g, &b);
      evas_object_gradient_color_stop_add(sp->grad, r, g, b, 255, 1);
      break;
    case ELICIT_SPECTRUM_V:
      evas_color_hsv_to_rgb(h, s, 0, &r, &g, &b);
      evas_object_gradient_color_stop_add(sp->grad, r, g, b, 255, 0);
      evas_color_hsv_to_rgb(h, s, 1, &r, &g, &b);
      evas_object_gradient_color_stop_add(sp->grad, r, g, b, 255, 1);
      break;
    default:
      break;
  }
}

static const Evas_Smart_Class _smart_class = {
	"elicit_spectrum",
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
	NULL
};

static void
_smart_init(void)
{
   if (_smart) return;
   _smart = evas_smart_class_new(&_smart_class);
}

static void
_smart_add(Evas_Object *o)
{
  Elicit_Spectrum *s;
  Evas *evas;

  s = calloc(1, sizeof(Elicit_Spectrum));
  if (!s) return;

  evas_object_smart_data_set(o, s);

  evas = evas_object_evas_get(o);

  s->grad = evas_object_gradient_add(evas);
  evas_object_gradient_angle_set(s->grad, 270);
  evas_object_gradient_color_stop_add(s->grad, 0, 255, 0, 255, 0);
  evas_object_gradient_color_stop_add(s->grad, 0, 0, 0, 255, 1);
  evas_object_smart_member_add(s->grad, o);
}

static void
_smart_del(Evas_Object *o)
{
  Elicit_Spectrum *s;

  s = evas_object_smart_data_get(o);
  if (!s) return;
  if (s->grad) evas_object_del(s->grad);
  free(s);
  evas_object_smart_data_set(o, NULL);

}

static void
_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Elicit_Spectrum *s;

  s = evas_object_smart_data_get(o);
  if (!s) return;

  evas_object_move(s->grad, x, y);
}

static void
_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Elicit_Spectrum *s;

  s = evas_object_smart_data_get(o);
  if (!s) return;
  evas_object_resize(s->grad, w, h);
  evas_object_gradient_fill_set(s->grad, 0, 0, w, h);
}

static void
_smart_show(Evas_Object *o)
{
  Elicit_Spectrum *s;

  s = evas_object_smart_data_get(o);
  if (!s) return;

  evas_object_show(s->grad);
}

static void
_smart_hide(Evas_Object *o)
{
  Elicit_Spectrum *s;

  s = evas_object_smart_data_get(o);
  if (!s) return;

  evas_object_hide(s->grad);
}

static void
_smart_clip_set(Evas_Object *o, Evas_Object *clip_o)
{
  Elicit_Spectrum *s;

  s = evas_object_smart_data_get(o);
  if (!s) return;

  evas_object_clip_set(s->grad, clip_o);
}

static void
_smart_clip_unset(Evas_Object *o)
{
  Elicit_Spectrum *s;

  s = evas_object_smart_data_get(o);
  if (!s) return;

  evas_object_clip_unset(s->grad);
}

