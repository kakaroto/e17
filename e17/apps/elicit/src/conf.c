#include "Elicit.h"

static int elicit_config_listener(const char *key, const Ecore_Config_Type type, const int tag, void *data, void *bundle);

int
elicit_config_init(Elicit *el)
{
  ecore_config_init("elicit");

  elicit_config_load(el);
  ecore_config_listen("theme", "/settings/theme", elicit_config_listener, 0, el);
  return 1;
}

void
elicit_config_load()
{
  char buf[PATH_MAX + 1];
  struct stat st;
  
  snprintf(buf, sizeof(buf), "%s/.e/apps/%s", getenv("HOME"), __app_name);
  if (stat(buf, &st))
    mkdir(buf, S_IRUSR | S_IWUSR | S_IXUSR);

  snprintf(buf, sizeof(buf), "%s/.e/apps/%s/config.db", getenv("HOME"), __app_name);
  if (stat(buf, &st))
  {
  /*
    ecore_config_set_int("/color/r", 255);
    ecore_config_set_int("/color/g", 255);
    ecore_config_set_int("/color/b", 255);
    ecore_config_set_string("/settings/theme", "winter");
  */
    elicit_config_color_set(255, 255, 255);
    elicit_config_zoom_set(4.0);
    elicit_config_zoom_max_set(20.0);
    elicit_config_theme_set("winter");
    ecore_config_save();
  }
  else
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
  ecore_config_exit();
}

char *
elicit_config_theme_get()
{
  char *theme = ecore_config_get_string("/settings/theme");
  return theme;
}

void
elicit_config_theme_set(char *name)
{
  ecore_config_set_string("/settings/theme", name);
}

void
elicit_config_zoom_set(double zoom)
{
  ecore_config_set_float("/settings/zoom", (float)zoom);
}

double
elicit_config_zoom_get()
{
  return (double)ecore_config_get_float("/settings/zoom");
}

void
elicit_config_zoom_max_set(double zoom_max)
{
  ecore_config_set_float("/settings/zoom_max", (float)zoom_max);
}

double
elicit_config_zoom_max_get()
{
  double max = (double)ecore_config_get_float("/settings/zoom_max");
  if (max < 10.0)
  {
    max = 10.0;
    ecore_config_set_float("/settings/zoom_max", (float)max);
  }
  return max;
}

void
elicit_config_color_get(int *r, int *g, int *b)
{
  if (r) *r = ecore_config_get_int("/color/r");
  if (g) *g = ecore_config_get_int("/color/g");
  if (b) *b = ecore_config_get_int("/color/b");
}

void
elicit_config_color_set(int r, int g, int b)
{
  ecore_config_set_int("/color/r", r);
  ecore_config_set_int("/color/g", g);
  ecore_config_set_int("/color/b", b);
}

static int
elicit_config_listener(const char *key, const Ecore_Config_Type type, const int tag, void *data, void *bundle)
{
  Elicit *el = data;
  switch (tag)
  {
    case ELICIT_CONF_TAG_THEME:
      if (el->gui)
      {
        elicit_ui_theme_set(el, ecore_config_get_string(key), "elicit");
      }
      break;
  }
  return 1;
}
