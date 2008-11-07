#include "Elicit.h"

#include <string.h>

#define INCH 25.4 / 8
#define PICA INCH * 8 / 6
#define POINT PICA / 12

static void _elicit_ruler_unit_swallow(Elicit_Ruler_Units u);
static void _elicit_ruler_units_popup(void);
static void _elicit_ruler_units_popdown(void);
static void _elicit_ruler_cb_resize(Ecore_Evas *ee);
static void _elicit_ruler_cb_popup_resize(Ecore_Evas *ee);
static void _elicit_ruler_cb_close(Ecore_Evas *ee);
static void _elicit_ruler_cb_rotate(void *data, Evas_Object *o, const char *sig, const char *src);
static void _elicit_ruler_cb_selector(void *data, Evas_Object *o, const char *sig, const char *src);
static void _elicit_ruler_cb_units(void *data, Evas_Object *o, const char *sig, const char *src);
static void _elicit_lines_create(Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);
static void _smart_init(void);
static void _smart_add(Evas_Object *o);
static void _smart_del(Evas_Object *o);
static void _smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *o);
static void _smart_hide(Evas_Object *o);

static Evas_Smart *_rule_smart = NULL;
Elicit_Ruler *_ruler = NULL;

static const Evas_Smart_Class _smart_class = {
	"elicit_rule",
	EVAS_SMART_CLASS_VERSION,
	_smart_add,
	_smart_del,
	_smart_move,
	_smart_resize,
	_smart_show,
	_smart_hide,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

void
elicit_ruler_show(Elicit *el)
{
  if (!_ruler)
  {
    Evas_Object *o;
    const char *data_string;

    _ruler = calloc(1, sizeof(Elicit_Ruler));
    _ruler->el = el;
    _ruler->ee = ecore_evas_software_x11_new(0, 0, 0, 0, 256, 64);

    ecore_evas_title_set(_ruler->ee, "Elicit Ruler");
    ecore_evas_name_class_set(_ruler->ee, "Elicit_Ruler", "Elicit");

    _ruler->evas = ecore_evas_get(_ruler->ee);
    evas_font_path_append(_ruler->evas, DATADIR"/font");

    _ruler->ruler = edje_object_add(_ruler->evas);
    evas_object_name_set(_ruler->ruler, "ruler");
    evas_object_move(_ruler->ruler, 0, 0);
    evas_object_show(_ruler->ruler);

    _ruler->theme = elicit_theme_find(elicit_config_theme_get(el));
    if (!edje_object_file_set(_ruler->ruler, _ruler->theme, "ruler"))
    {
      printf("Error: can't set ruler theme to %s\n", _ruler->theme);
      return;
    }

    data_string = edje_object_data_get(_ruler->ruler, "line_color");
    if (!data_string || 4 != sscanf(data_string, "%d %d %d %d",
          &_ruler->r, &_ruler->g, &_ruler->b, &_ruler->a))
    {
      _ruler->r = _ruler->g = _ruler->b = 0;
      _ruler->a = 255;
    }

    ecore_evas_callback_resize_set(_ruler->ee, _elicit_ruler_cb_resize);
    ecore_evas_borderless_set(_ruler->ee, 1);
    if (ecore_x_screen_is_composited(0))
      ecore_evas_alpha_set(_ruler->ee, 1);
    else
      ecore_evas_shaped_set(_ruler->ee, 1);

    ecore_evas_callback_delete_request_set(el->ee, _elicit_ruler_cb_close);

    _ruler->draggie = esmart_draggies_new(_ruler->ee);
    esmart_draggies_button_set(_ruler->draggie, 1);
    evas_object_layer_set(_ruler->draggie, -2);
    evas_object_name_set(_ruler->draggie, "ruler_draggie");
    edje_object_part_swallow(_ruler->ruler, "draggie", _ruler->draggie);
    evas_object_show(_ruler->draggie);

    _ruler->resize = esmart_resize_new(_ruler->ee);
    esmart_resize_button_set(_ruler->resize, 1);
    esmart_resize_type_set(_ruler->resize, ESMART_RESIZE_RIGHT);
    evas_object_layer_set(_ruler->resize, -1);
    evas_object_name_set(_ruler->resize, "ruler_resize");
    edje_object_part_swallow(_ruler->ruler, "resize", _ruler->resize);
    evas_object_show(_ruler->resize);


    _elicit_ruler_cb_resize(_ruler->ee);

    _smart_init();
    o = _ruler->smart = evas_object_smart_add(_ruler->evas, _rule_smart);
    evas_object_name_set(o, "rule");
    edje_object_part_swallow(_ruler->ruler, "rule", o);
    evas_object_show(o);

    _elicit_ruler_unit_swallow(ELICIT_RULER_UNITS_PX);

    edje_object_signal_callback_add(_ruler->ruler, "elicit,ruler,rotate", "ruler", _elicit_ruler_cb_rotate, o);
    edje_object_signal_callback_add(_ruler->ruler, "elicit,ruler,selector", "ruler", _elicit_ruler_cb_selector, o);
  }

  _elicit_ruler_cb_resize(_ruler->ee);
  ecore_evas_show(_ruler->ee);

  el->flags.ruler = 1;
}

void
elicit_ruler_hide(Elicit *el)
{
  ecore_evas_hide(_ruler->ee);
  el->flags.ruler = 0;
}

static void
_elicit_ruler_unit_swallow(Elicit_Ruler_Units u)
{
  Evas_Object *s;
  int ret;


  s = edje_object_part_swallow_get(_ruler->ruler, "unit_selector");
  if (s)
    evas_object_del(s);

  s = edje_object_add(_ruler->evas);
  switch(u)
  {
    case ELICIT_RULER_UNITS_PX:
      ret = edje_object_file_set(s, _ruler->theme, "ruler/px");
      break;
    case ELICIT_RULER_UNITS_CM:
      ret = edje_object_file_set(s, _ruler->theme, "ruler/cm");
      break;
    case ELICIT_RULER_UNITS_IN:
      ret = edje_object_file_set(s, _ruler->theme, "ruler/in");
      break;
    case ELICIT_RULER_UNITS_PC:
      ret = edje_object_file_set(s, _ruler->theme, "ruler/pc");
      break;
    case ELICIT_RULER_UNITS_PT:
      ret = edje_object_file_set(s, _ruler->theme, "ruler/pt");
      break;
  }
  if (ret)
  {
    edje_object_part_swallow(_ruler->ruler, "unit_selector", s);
    evas_object_show(s);
  }
  else evas_object_del(s);
}

static void
_elicit_ruler_units_popup(void)
{
  int x, y, w, h, px, py, pw, ph, ow, oh;

  if (_ruler->popup) return;

  if (!_ruler->popup_ee)
  {
    Evas *evas;
    Evas_Object *o;

    _ruler->popup_ee = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);

    ecore_evas_title_set(_ruler->popup_ee, "Elicit Ruler Popup");
    ecore_evas_name_class_set(_ruler->popup_ee, "Elicit_Ruler Popup", "Elicit");
    ecore_evas_borderless_set(_ruler->popup_ee, 1);
    if (ecore_x_screen_is_composited(0))
      ecore_evas_alpha_set(_ruler->popup_ee, 1);
    else
      ecore_evas_shaped_set(_ruler->popup_ee, 1);
    ecore_evas_callback_resize_set(_ruler->popup_ee, _elicit_ruler_cb_popup_resize);

    evas = ecore_evas_get(_ruler->popup_ee);
    evas_font_path_append(evas, DATADIR"/font");

    _ruler->popup_o = edje_object_add(evas);
    evas_object_name_set(_ruler->popup_o, "popup");
    edje_object_file_set(_ruler->popup_o, _ruler->theme, "ruler/popup");
    evas_object_move(_ruler->popup_o, 0, 0);
    evas_object_show(_ruler->popup_o);

    edje_object_signal_callback_add(_ruler->popup_o, "elicit,ruler,units,*", "ruler", _elicit_ruler_cb_units, NULL);
  }

  ecore_evas_show(_ruler->popup_ee);

  ecore_evas_geometry_get(_ruler->ee, &x, &y, &w, &h);
  edje_object_part_geometry_get(_ruler->ruler, "unit_selector", &px, &py, &pw, &ph);
  edje_object_size_min_get(_ruler->popup_o, &ow, &oh);
  if (!ow && !oh)
    edje_object_size_min_calc(_ruler->popup_o, &ow, &oh);

  ecore_evas_move_resize(_ruler->popup_ee, x + px + (pw - ow) / 2,
      y + py < oh ? y + py + ph : y + py - oh, ow, oh);

  _ruler->popup = 1;
}

static void
_elicit_ruler_units_popdown(void)
{
  if (!_ruler->popup) return;

  ecore_evas_hide(_ruler->popup_ee);

  _ruler->popup = 0;
}

static void 
_elicit_ruler_cb_resize(Ecore_Evas *ee)
{
  int w, h;

  ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
  evas_object_resize(_ruler->ruler, w, h);
}

static void 
_elicit_ruler_cb_popup_resize(Ecore_Evas *ee)
{
  int w, h;

  ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
  evas_object_resize(_ruler->popup_o, w, h);
}

static void
_elicit_ruler_cb_close(Ecore_Evas *ee)
{
  elicit_ruler_hide(_ruler->el);
}

static void
_elicit_ruler_cb_rotate(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Evas_Object *rule;
  int w, h;

  rule = data;

  _elicit_ruler_units_popdown();

  switch(_ruler->orient)
  {
    case (ELICIT_RULER_ORIENT_TOP):
      esmart_resize_type_set(_ruler->resize, ESMART_RESIZE_BOTTOM);
      _ruler->orient = ELICIT_RULER_ORIENT_RIGHT;
      edje_object_signal_emit(_ruler->ruler, "elicit,ruler,orient,right", "Elicit");
      break;
    case (ELICIT_RULER_ORIENT_RIGHT):
      esmart_resize_type_set(_ruler->resize, ESMART_RESIZE_RIGHT);
      _ruler->orient = ELICIT_RULER_ORIENT_BOTTOM;
      edje_object_signal_emit(_ruler->ruler, "elicit,ruler,orient,bottom", "Elicit");
      break;
    case (ELICIT_RULER_ORIENT_BOTTOM):
      esmart_resize_type_set(_ruler->resize, ESMART_RESIZE_BOTTOM);
      _ruler->orient = ELICIT_RULER_ORIENT_LEFT;
      edje_object_signal_emit(_ruler->ruler, "elicit,ruler,orient,left", "Elicit");
      break;
    case (ELICIT_RULER_ORIENT_LEFT):
      esmart_resize_type_set(_ruler->resize, ESMART_RESIZE_RIGHT);
      _ruler->orient = ELICIT_RULER_ORIENT_TOP;
      edje_object_signal_emit(_ruler->ruler, "elicit,ruler,orient,top", "Elicit");
      break;
  }

  ecore_evas_geometry_get(_ruler->ee, NULL, NULL, &w, &h);
  ecore_evas_resize(_ruler->ee, h, w);
}

static void
_elicit_ruler_cb_selector(void *data, Evas_Object *o, const char *sig, const char *src)
{
  if (_ruler->popup)
    _elicit_ruler_units_popdown();
  else
    _elicit_ruler_units_popup();
}

static void
_elicit_ruler_cb_units(void *data, Evas_Object *o, const char *sig, const char *src)
{
  if (elicit_glob_match(sig, "*,px"))
    _ruler->units = ELICIT_RULER_UNITS_PX;
  else if (elicit_glob_match(sig, "*,cm"))
    _ruler->units = ELICIT_RULER_UNITS_CM;
  else if (elicit_glob_match(sig, "*,in"))
    _ruler->units = ELICIT_RULER_UNITS_IN;
  else if (elicit_glob_match(sig, "*,pc"))
    _ruler->units = ELICIT_RULER_UNITS_PC;
  else if (elicit_glob_match(sig, "*,pt"))
    _ruler->units = ELICIT_RULER_UNITS_PT;

  _smart_del(_ruler->smart);
  _smart_add(_ruler->smart);
  _smart_show(_ruler->smart);

  _elicit_ruler_unit_swallow(_ruler->units);
  _elicit_ruler_units_popdown();
}

static void
_elicit_lines_create(Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
  int i, length, broad, top, major, minor, step;
  float dpmm, multi;

  dpmm = ((float) ecore_x_dpi_get()) / 25.4;

  switch(_ruler->orient)
  {
    case (ELICIT_RULER_ORIENT_TOP):
      length = w;
      broad = h;
      break;
    case (ELICIT_RULER_ORIENT_RIGHT):
      length = h;
      broad = w;
      break;
    case (ELICIT_RULER_ORIENT_BOTTOM):
      length = w;
      broad = h;
      break;
    case (ELICIT_RULER_ORIENT_LEFT):
      length = h;
      broad = w;
      break;
  }

  switch(_ruler->units)
  {
    case (ELICIT_RULER_UNITS_PX):
      top = 100;
      major = 50;
      minor = 10;
      step = 2;
      multi = 1;
      break;
    case (ELICIT_RULER_UNITS_CM):
      length /= dpmm;
      top = 10;
      major = 10;
      minor = 5;
      step = 1;
      multi = dpmm;
      break;
    case (ELICIT_RULER_UNITS_IN):
      length /= dpmm * INCH;
      top = 8;
      major = 8;
      minor = 4;
      step = 1;
      multi = dpmm * INCH;
      break;
    case (ELICIT_RULER_UNITS_PC):
      length /= dpmm * PICA;
      top = 6;
      major = 6;
      minor = 3;
      step = 1;
      multi = dpmm * PICA;
      break;
    case (ELICIT_RULER_UNITS_PT):
      length /= dpmm * POINT;
      top = 72;
      major = 36;
      minor = 6;
      step = 6;
      multi = dpmm * POINT;
      break;
  }

  /* Pixels case */
  for (i = 0; i < length; i += step)
  {
    Evas_Object *line;
    int line_height;

    if (!(i % major))
      line_height = broad / 3;
    else if (!(i % minor))
      line_height = broad / 3 - 6;
    else
      line_height = broad / 3 - 12;

    line = evas_object_line_add(_ruler->evas);
    switch(_ruler->orient)
    {
      case (ELICIT_RULER_ORIENT_TOP):
        evas_object_line_xy_set(line, x + i * multi, y, x + i * multi, line_height);
        break;
      case (ELICIT_RULER_ORIENT_RIGHT):
        evas_object_line_xy_set(line, x + w - line_height, y + i * multi, x + w, y + i * multi);
        break;
      case (ELICIT_RULER_ORIENT_BOTTOM):
        evas_object_line_xy_set(line, x + i * multi, y + h - line_height, x + i * multi, y + h);
        break;
      case (ELICIT_RULER_ORIENT_LEFT):
        evas_object_line_xy_set(line, x, y + i * multi, x + line_height, y + i * multi);
        break;
    }
    evas_object_color_set(line, _ruler->r, _ruler->g, _ruler->b, _ruler->a);

    evas_object_smart_member_add(line, _ruler->smart);

    if (i && !(i % top))
    {
      Evas_Object *label;
      char str[5];

      label = edje_object_add(_ruler->evas);
      edje_object_file_set(label, _ruler->theme, "ruler/label");
      switch(_ruler->units)
      {
        case (ELICIT_RULER_UNITS_PX):
        case (ELICIT_RULER_UNITS_PC):
        case (ELICIT_RULER_UNITS_PT):
          snprintf(str, 5, "%d", i);
          break;
        case (ELICIT_RULER_UNITS_CM):
          snprintf(str, 5, "%d", i / 10);
          break;
        case (ELICIT_RULER_UNITS_IN):
          snprintf(str, 5, "%d", i / 8);
          break;
      }
      edje_object_part_text_set(label, "ruler.label", str);

      switch(_ruler->orient)
      {
        case (ELICIT_RULER_ORIENT_TOP):
          evas_object_move(label, x + i * multi, line_height + 10);
          break;
        case (ELICIT_RULER_ORIENT_RIGHT):
          /* XXX: -3 so that the text does not go into the line */
          evas_object_move(label, x + w - line_height - 10, y + i * multi - 3);
          break;
        case (ELICIT_RULER_ORIENT_BOTTOM):
          evas_object_move(label, x + i * multi, y + h - line_height - 10);
          break;
        case (ELICIT_RULER_ORIENT_LEFT):
          evas_object_move(label, x + line_height + 10, y + i * multi - 3);
          break;
      }
      evas_object_smart_member_add(label, _ruler->smart);
    }
  } 
}

static void
_smart_init(void)
{
   if (_rule_smart) return;
   _rule_smart = evas_smart_class_new(&_smart_class);
}

static void
_smart_add(Evas_Object *o)
{
  int x, y, w, h;

  edje_object_part_geometry_get(_ruler->ruler, "rule", &x, &y, &w, &h);
  _elicit_lines_create(x, y, w, h);
}

static void
_smart_del(Evas_Object *o)
{
  Eina_List *lines, *l;

  lines = evas_object_smart_members_get(o);
  for (l = lines; l; l = l->next)
  {
    evas_object_del(l->data);
  }

  eina_list_free(lines);
}

static void _smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  int w, h;

  edje_object_part_geometry_get(_ruler->ruler, "rule", NULL, NULL, &w, &h);
  _smart_del(o);
  _elicit_lines_create(x, y, w, h);
}

static void _smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  int x, y;

  edje_object_part_geometry_get(_ruler->ruler, "rule", &x, &y, NULL, NULL);
  _smart_del(o);
  _elicit_lines_create(x, y, w, h);
}

static void _smart_show(Evas_Object *o)
{
  Eina_List *lines, *l;

  lines = evas_object_smart_members_get(o);
  for (l = lines; l; l = l->next)
  {
    evas_object_show(l->data);
  }

  eina_list_free(lines);
}

static void _smart_hide(Evas_Object *o)
{
  Eina_List *lines, *l;

  lines = evas_object_smart_members_get(o);
  for (l = lines; l; l = l->next)
  {
    evas_object_hide(l->data);
  }

  eina_list_free(lines);
}

