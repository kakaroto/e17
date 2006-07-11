#include "Elicit.h"

static int elicit_config_listener(const char *key, const Ecore_Config_Type type, const int tag, void *data);

int
elicit_config_init(Elicit *el)
{
  ecore_config_init("elicit");

  elicit_config_load();
  ecore_config_listen("theme", "/settings/theme", elicit_config_listener, 0, el);
  /* cheap way to create ~/.e/apps/elicit */
  elicit_config_save();
  return 1;
}

void
elicit_config_load()
{
  ecore_config_int_default("/color/r", 255);
  ecore_config_int_default("/color/g", 255);
  ecore_config_int_default("/color/b", 255);
  ecore_config_float_default("/settings/zoom", 4.0);
  ecore_config_float_default("/settings/zoom_max", 20.0);
  ecore_config_theme_default("/settings/theme", "winter");
  ecore_config_theme_preview_group_set("/settings/theme", "elicit");
  ecore_config_int_default("/colorclass/r", 229);
  ecore_config_int_default("/colorclass/g", 239);
  ecore_config_int_default("/colorclass/b", 255);
  ecore_config_string_default("/editor", "gimp-remote");
  ecore_config_load();
}

void
elicit_config_save()
{
  ecore_config_save();
}

void
elicit_config_shutdown()
{
  elicit_config_save();
  ecore_config_shutdown();
}

char *
elicit_config_theme_get()
{
  char *theme = ecore_config_theme_get("/settings/theme");
  return theme;
}

void
elicit_config_theme_set(char *name)
{
  if (!name) return;
  ecore_config_theme_set("/settings/theme", name);
}

void
elicit_config_zoom_set(double zoom)
{
  ecore_config_float_set("/settings/zoom", (float)zoom);
}

double
elicit_config_zoom_get()
{
  return (double)ecore_config_float_get("/settings/zoom");
}

void
elicit_config_zoom_max_set(double zoom_max)
{
  ecore_config_float_set("/settings/zoom_max", (float)zoom_max);
}

double
elicit_config_zoom_max_get()
{
  double max = (double)ecore_config_float_get("/settings/zoom_max");
  if (max < 10.0)
  {
    max = 10.0;
    ecore_config_float_set("/settings/zoom_max", (float)max);
  }
  return max;
}

void
elicit_config_color_get(int *r, int *g, int *b)
{
  if (r) *r = ecore_config_int_get("/color/r");
  if (g) *g = ecore_config_int_get("/color/g");
  if (b) *b = ecore_config_int_get("/color/b");
}

void
elicit_config_color_set(int r, int g, int b)
{
  ecore_config_int_set("/color/r", r);
  ecore_config_int_set("/color/g", g);
  ecore_config_int_set("/color/b", b);
}

void
elicit_config_colorclass_get(int *r, int *g, int *b)
{
  if (r) *r = ecore_config_int_get("/colorclass/r");
  if (g) *g = ecore_config_int_get("/colorclass/g");
  if (b) *b = ecore_config_int_get("/colorclass/b");
}

void
elicit_config_colorclass_set(int r, int g, int b)
{
  ecore_config_int_set("/colorclass/r", r);
  ecore_config_int_set("/colorclass/g", g);
  ecore_config_int_set("/colorclass/b", b);
}

char *
elicit_config_editor_get(void)
{
   return ecore_config_string_get("/editor");
}

void
elicit_config_editor_set(char *ed)
{
   if (!ed) return;
   ecore_config_string_set("/editor", ed);
}

int
elicit_config_grid_visible_get(void)
{
  return ecore_config_int_get("/show_grid");
}

void
elicit_config_grid_visible_set(int visible)
{
  ecore_config_int_set("/show_grid", visible);
}

static int
elicit_config_listener(const char *key, const Ecore_Config_Type type, const int tag, void *data)
{
  Elicit *el = data;
  switch (tag)
  {
    case ELICIT_CONF_TAG_THEME:
      if (el->gui)
      {
//        elicit_ui_theme_set(el, ecore_config_theme_get(key), "elicit");
      }
      break;
  }
  return 1;
}
