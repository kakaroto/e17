#include "Elicit.h"

void elicit_theme_load_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_theme_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source);

int
elicit_themes_init(Elicit *el)
{
  el->themes.cont = esmart_container_new(el->evas); 
  esmart_container_direction_set(el->themes.cont, CONTAINER_DIRECTION_VERTICAL);
  esmart_container_spacing_set(el->themes.cont, 2);
  esmart_container_padding_set(el->themes.cont, 5, 5, 5, 5);
  edje_object_part_swallow(el->gui, "theme.container", el->themes.cont);
  edje_object_part_drag_value_set(el->gui, "theme.scroll.bar", 0.001, 0.001);

  evas_object_show(el->themes.cont);
  {
    Evas_Object *o;

    o = edje_object_add(el->evas);
    edje_object_file_set(o, elicit_theme_find(elicit_config_theme_get()), "theme");
    evas_object_resize(o, 100, 10);
    evas_object_data_set(o, "theme", strdup("winter"));
    evas_object_data_set(o, "elicit", el);
    edje_object_part_text_set(o, "theme.label", "Not Yet");
/*    edje_object_signal_callback_add(o, "elicit,theme,load", "*", elicit_theme_load_cb, NULL);*/
    evas_object_show(o);
    esmart_container_element_append(el->themes.cont, o);

#if 0
    o = edje_object_add(el->evas);
    edje_object_file_set(o, elicit_theme_find(elicit_config_theme_get()), "theme");
    evas_object_resize(o, 100, 10);
    edje_object_part_text_set(o, "theme.label", "spring");
    evas_object_data_set(o, "theme", strdup("spring"));
    evas_object_data_set(o, "elicit", el);
    edje_object_signal_callback_add(o, "elicit,theme,load", "*", elicit_theme_load_cb, NULL);
    evas_object_show(o);
    esmart_container_element_append(el->themes.cont, o);
#endif
  }
}

void
elicit_themes_free(Elicit *el)
{
  /* fixme: free 'em up! */
}


void
elicit_theme_load_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  char *th;

  el = evas_object_data_get(o, "elicit");
  th = evas_object_data_get(o, "theme");

  elicit_ui_theme_set(el, th, "elicit");
}


void
elicit_theme_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  char *th;

  el = evas_object_data_get(o, "elicit");
  th = evas_object_data_get(o, "theme");

  edje_object_part_text_set(el->gui, "theme.label", th);
}


