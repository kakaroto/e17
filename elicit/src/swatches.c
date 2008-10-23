#include "Elicit.h"

#include <string.h>


void _elicit_swatches_update_scroll_bar(Elicit *el);
void _elicit_swatches_load_gpl(Elicit *el);
void _elicit_swatches_save_gpl(Elicit *el);

int
elicit_swatches_init(Elicit *el)
{
  if (edje_object_part_exists(el->gui, "swatch.container"))
  {
    char *dir;
    
    el->swatches.cont = esmart_container_new(el->evas); 
    esmart_container_move_button_set(el->swatches.cont, 2);
    esmart_container_callback_order_change_set(el->swatches.cont, (void (*)(void *data))elicit_swatches_save, el);

    dir = (char *)edje_object_data_get(el->gui, "swatches.direction");
    if (dir && (dir[0] == 'h' || dir[0] == 'H'))
      el->swatches.dir = CONTAINER_DIRECTION_HORIZONTAL;
    else
      el->swatches.dir = CONTAINER_DIRECTION_VERTICAL;
    esmart_container_direction_set(el->swatches.cont, el->swatches.dir);

    esmart_container_spacing_set(el->swatches.cont, 3);
    esmart_container_padding_set(el->swatches.cont, 5, 5, 5, 5);
    evas_object_show(el->swatches.cont);
    edje_object_part_swallow(el->gui, "swatch.container", el->swatches.cont);
    edje_object_part_drag_value_set(el->gui, "swatch.scroll.bar", 0.001, 0.001);
  
    elicit_swatches_load(el);
  
    ecore_idle_enterer_add(elicit_swatches_scroll_idler, el);
  }

  return 1;
}

void
elicit_swatches_shutdown(Elicit *el)
{
  Eina_List *l;
  
  if (el->swatches.cont)
  {
    for (l = esmart_container_elements_get(el->swatches.cont); l; l = l->next)
    {
      Evas_Object *o = l->data;
      Elicit_Swatch *sw = evas_object_data_get(o, "swatch");
      elicit_swatch_free(sw);
    }
    /*
    edje_object_part_unswallow(el->gui, el->swatches.cont);
    evas_object_del(el->swatches.cont);
    el->swatches.cont = NULL;
    */
  }
}

void
elicit_swatches_save(Elicit *el)
{
  _elicit_swatches_save_gpl(el);
}

void
_elicit_swatches_save_gpl(Elicit *el)
{
  Eina_List *l;
  FILE *f = NULL;
  char buf[PATH_MAX];

  snprintf(buf, PATH_MAX, "%s/.e/apps/%s/swatches.gpl", getenv("HOME"), el->app_name);

  f = fopen(buf, "w");
 
  if (!f) return;

  fprintf(f, "GIMP Palette\nName: Elicit\nColumns: 1\n#\n");
  for (l = esmart_container_elements_get(el->swatches.cont); l; l = l->next)
  {
    Evas_Object *obj;
    Elicit_Swatch *sw;

    obj = (Evas_Object *)(l->data);
    sw = evas_object_data_get(obj, "swatch");

    fprintf(f, "%-3d %-3d %-3d %s\n", sw->r, sw->g, sw->b, sw->name);
  }

  fclose(f);
}

void
elicit_swatches_load(Elicit *el)
{
  char buf[PATH_MAX];

  snprintf(buf, PATH_MAX, "%s/.e/apps/%s/swatches.gpl", getenv("HOME"), el->app_name);

  if (ecore_file_exists(buf))
    _elicit_swatches_load_gpl(el);
}

void
_elicit_swatches_load_gpl(Elicit *el)
{
  FILE *f;
  char buf[PATH_MAX];
  char *name;
  int r, g, b;

  snprintf(buf, PATH_MAX, "%s/.e/apps/%s/swatches.gpl", getenv("HOME"), el->app_name);

  f = fopen(buf, "r");
  if (!f) return;

  name = calloc(PATH_MAX, sizeof(char));
  if (!name) return;
 
  while(fgets(buf, PATH_MAX, f))
  {
    if (sscanf(buf, "%3d %3d %3d\t%s\n", &r, &g, &b, name) == 4)
    {
      elicit_swatch_new(el, name, r, g, b);
    }
  }

  el->swatches.length = esmart_container_elements_length_get(el->swatches.cont);
  free(name);
}

void
elicit_swatch_free(Elicit_Swatch *sw)
{
  if (sw->rect)
  {
    edje_object_part_unswallow(sw->obj, sw->rect);
    evas_object_del(sw->rect);
  }
  if (sw->obj) evas_object_del(sw->obj);
  if (sw->name) free(sw->name);
}

void
elicit_swatch_save_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el = data;
  Elicit_Swatch *sw;
  double length;
  Evas_Coord w, h;

  sw = elicit_swatch_new(el, el->color.hex, el->color.r, el->color.g, el->color.b);

  /* scroll to the end of the list */
  length = esmart_container_elements_length_get(el->swatches.cont);
  el->swatches.length = length;
  evas_object_geometry_get(el->swatches.cont, NULL, NULL, &w, &h);
  if (el->swatches.dir == CONTAINER_DIRECTION_HORIZONTAL && length > w)
  {
    esmart_container_scroll_offset_set(el->swatches.cont,
                                       w - length - 10 );
    _elicit_swatches_update_scroll_bar(el);
  }
  else if (el->swatches.dir == CONTAINER_DIRECTION_VERTICAL && length > h)
  {
    esmart_container_scroll_offset_set(el->swatches.cont,
                                       h - length - 10 );
    _elicit_swatches_update_scroll_bar(el);
  }

  elicit_swatches_save(el);
}

Elicit_Swatch *
elicit_swatch_new(Elicit *el, char *name, int r, int g, int b)
{
  Elicit_Swatch *sw;
  char *theme;
  Evas_Coord mw, mh;

  sw = calloc(1, sizeof(Elicit_Swatch));

  sw->obj = edje_object_add(el->evas);
  sw->rect = evas_object_rectangle_add(el->evas);
  sw->r = r;
  sw->g = g;
  sw->b = b;
  sw->name = strdup(name);

  theme = elicit_config_theme_get(el);
  edje_object_file_set(sw->obj, 
                       elicit_theme_find(theme),
                       "swatch");
  free(theme);

  edje_object_size_min_get(sw->obj, &mw, &mh);
  if (mw != 0 && mh != 0)
    evas_object_resize(sw->obj, mw, mh);
  else
    evas_object_resize(sw->obj, 12, 12);

  evas_object_show(sw->obj);
  evas_object_data_set(sw->obj, "swatch", sw);
  evas_object_data_set(sw->obj, "elicit", el);

  edje_object_signal_callback_add(sw->obj, "elicit,swatch,load", "", elicit_swatch_load_cb, sw);
  edje_object_signal_callback_add(sw->obj, "elicit,swatch,del", "", elicit_swatch_del_cb, sw);
  edje_object_signal_callback_add(sw->obj, "elicit,swatch,name,show", "", elicit_swatch_name_show_cb, sw);
  
  evas_object_color_set(sw->rect, sw->r, sw->g, sw->b, 255);
  evas_object_pass_events_set(sw->rect, 1);
  evas_object_show(sw->rect);
  edje_object_part_swallow(sw->obj, "swatch", sw->rect);
  esmart_container_element_append(el->swatches.cont, sw->obj);
  
  return sw;
}

void
elicit_swatch_load_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  Elicit_Swatch *sw;

  el = evas_object_data_get(o, "elicit");
  sw = evas_object_data_get(o, "swatch");

  el->color.r = sw->r;
  el->color.g = sw->g;
  el->color.b = sw->b;

  elicit_util_colors_set_from_rgb(el);

  elicit_ui_update(el);
}

void
elicit_swatch_del_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  Elicit_Swatch *sw;
  Evas_Coord w, h;
  int offset;

  el = evas_object_data_get(o, "elicit");
  sw = evas_object_data_get(o, "swatch");

  esmart_container_element_remove(el->swatches.cont, sw->obj);
  elicit_swatch_free(sw);

  el->swatches.length = esmart_container_elements_length_get(el->swatches.cont);

  evas_object_geometry_get(el->swatches.cont, NULL, NULL, &w, &h);
  offset = esmart_container_scroll_offset_get(el->swatches.cont);
  if (el->swatches.dir == CONTAINER_DIRECTION_VERTICAL && offset < h - el->swatches.length - 10 && el->swatches.length > h)
  {
    esmart_container_scroll_offset_set(el->swatches.cont,
                                       h - el->swatches.length - 10);
  }
  else if (el->swatches.dir == CONTAINER_DIRECTION_HORIZONTAL && offset < w - el->swatches.length - 10 && el->swatches.length > w)
  {
    esmart_container_scroll_offset_set(el->swatches.cont,
                                       w - el->swatches.length - 10);
  }
  _elicit_swatches_update_scroll_bar(el);

  elicit_swatches_save(el);
}

void
elicit_swatch_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  Elicit_Swatch *sw;

  el = evas_object_data_get(o, "elicit");
  sw = evas_object_data_get(o, "swatch");

  edje_object_part_text_set(el->gui, "swatch.label", sw->name);
}


void
elicit_swatch_scroll_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el = data;
  
  if (!strcmp(emission, "drag"))
  {
    double l = el->swatches.length;
    Evas_Coord h;
    double vx, vy;

    evas_object_geometry_get(el->swatches.cont, NULL, NULL, NULL, &h);
    if (l > h)
    {
      edje_object_part_drag_value_get(el->gui, source, &vx, &vy);
      esmart_container_scroll_offset_set(el->swatches.cont, -vy*(l-h+10));
    }
    else
    {
      edje_object_part_drag_value_set(el->gui, source, .001, .001);
    }
  }
  else if (!fnmatch("elicit,swatch,scroll,up*", emission, 0))
  {
    if (!strcmp(emission, "elicit,swatch,scroll,up"))
    {
      esmart_container_scroll(el->swatches.cont, 5);
      _elicit_swatches_update_scroll_bar(el);
    }
    else if (!strcmp(emission, "elicit,swatch,scroll,up,start"))
    {
      el->swatches.scrolling = 1;
      esmart_container_scroll_start(el->swatches.cont, 1);
    }
    else
    {
      el->swatches.scrolling = 0;
      esmart_container_scroll_stop(el->swatches.cont);
    }
  }
  else
  {
    if (!strcmp(emission, "elicit,swatch,scroll,down"))
    {
      esmart_container_scroll(el->swatches.cont, -5);
      _elicit_swatches_update_scroll_bar(el);
    }
    else if (!strcmp(emission, "elicit,swatch,scroll,down,start"))
    {
      el->swatches.scrolling = 1;
      esmart_container_scroll_start(el->swatches.cont, -1);
    }
    else
    {
      el->swatches.scrolling = 0;
      esmart_container_scroll_stop(el->swatches.cont);
    }
  }
}

int
elicit_swatches_scroll_idler(void *data)
{
  Elicit *el = data;
  if (el->swatches.scrolling)
  {
    _elicit_swatches_update_scroll_bar(el);
  }
  return 1;
}

void
_elicit_swatches_update_scroll_bar(Elicit *el)
{
    int val= esmart_container_scroll_offset_get(el->swatches.cont);
    Evas_Coord h;

    evas_object_geometry_get(el->swatches.cont, NULL, NULL, NULL, &h);
    edje_object_part_drag_value_set(el->gui, "swatch.scroll.bar", 0, -(double)val / (el->swatches.length - h + 10));
}
