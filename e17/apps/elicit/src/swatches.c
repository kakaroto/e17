#include "Elicit.h"


void _elicit_swatches_update_scroll_bar(Elicit *el);

int
elicit_swatches_init(Elicit *el)
{
  el->swatches.cont = esmart_container_new(el->evas); 
  esmart_container_direction_set(el->swatches.cont, CONTAINER_DIRECTION_VERTICAL);
  esmart_container_spacing_set(el->swatches.cont, 3);
  esmart_container_padding_set(el->swatches.cont, 5, 5, 5, 5);
  evas_object_show(el->swatches.cont);
  edje_object_part_swallow(el->gui, "swatch.container", el->swatches.cont);
  edje_object_part_drag_value_set(el->gui, "swatch.scroll.bar", 0.001, 0.001);

  elicit_swatches_load(el);

  ecore_idler_add(elicit_swatches_scroll_idler, el);
}

void
elicit_swatches_save(Elicit *el)
{
  Evas_List *l;
  E_DB_File *db;
  int i = 0;
  int cleanup = 0, old_num = 0;
  char buf[PATH_MAX];

  snprintf(buf, PATH_MAX, "%s/.e/apps/%s/swatches.db", getenv("HOME"), el->app_name);

  db = e_db_open_read(buf);
  if (db)
  {
    cleanup = e_db_int_get(db, "/swatches/num", &old_num);
    e_db_close(db);
  }

  db = e_db_open(buf);
  
  for (l = esmart_container_elements_get(el->swatches.cont); l; l = l->next)
  {
    Evas_Object *obj;
    Elicit_Swatch *sw;

    obj = (Evas_Object *)(l->data);
    sw = evas_object_data_get(obj, "swatch");

    snprintf(buf, PATH_MAX, "/swatches/%d/name", i);
    e_db_str_set(db, buf, sw->name);
    
    snprintf(buf, PATH_MAX, "/swatches/%d/r", i);
    e_db_int_set(db, buf, sw->r);
    snprintf(buf, PATH_MAX, "/swatches/%d/g", i);
    e_db_int_set(db, buf, sw->g);
    snprintf(buf, PATH_MAX, "/swatches/%d/b", i);
    e_db_int_set(db, buf, sw->b);

    i++;
  }
  
  e_db_int_set(db, "/swatches/num", i);

  if (cleanup)
  {
    for (; i < old_num; i++)
    {
      snprintf(buf, PATH_MAX, "/swatches/%d/name", i);
      e_db_data_del(db, buf);
      snprintf(buf, PATH_MAX, "/swatches/%d/r", i);
      e_db_data_del(db, buf);
      snprintf(buf, PATH_MAX, "/swatches/%d/g", i);
      e_db_data_del(db, buf);
      snprintf(buf, PATH_MAX, "/swatches/%d/b", i);
      e_db_data_del(db, buf);
    }
  }
  e_db_close(db);
  e_db_flush();
}

void
elicit_swatches_load(Elicit *el)
{
  Evas_List *l;
  E_DB_File *db;
  int num, ok;
  int i;
  int r, g, b;
  char buf[PATH_MAX];

  snprintf(buf, PATH_MAX, "%s/.e/apps/%s/swatches.db", getenv("HOME"), el->app_name);

  db = e_db_open_read(buf);
  if (db)
    ok = e_db_int_get(db, "/swatches/num", &num);
  else return;

  for (i = 0; i < num; i++)
  {
    Elicit_Swatch *sw;
    int iw = 0, ih = 0;
    Evas_Coord mw, mh;
  
    sw = calloc(1, sizeof(Elicit_Swatch));
  
    snprintf(buf, PATH_MAX, "/swatches/%d/name", i);
    sw->name = e_db_str_get(db, buf);
     
    snprintf(buf, PATH_MAX, "/swatches/%d/r", i);
    e_db_int_get(db, buf, &(sw->r));
    snprintf(buf, PATH_MAX, "/swatches/%d/g", i);
    e_db_int_get(db, buf, &(sw->g));
    snprintf(buf, PATH_MAX, "/swatches/%d/b", i);
    e_db_int_get(db, buf, &(sw->b));
  
    sw->obj = edje_object_add(el->evas);
    sw->rect = evas_object_rectangle_add(el->evas);
  
    edje_object_file_set(sw->obj, 
                         elicit_theme_find(elicit_config_theme_get(el)),
                         "swatch");
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
    evas_object_show(sw->rect);
    edje_object_part_swallow(sw->obj, "swatch", sw->rect);
    esmart_container_element_append(el->swatches.cont, sw->obj);
  }
}

void
elicit_swatch_free(Elicit_Swatch *sw)
{
  if (sw->rect) evas_object_del(sw->rect);
  if (sw->obj) evas_object_del(sw->obj);
  if (sw->name) free(sw->name);
}

void
elicit_swatch_save_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el = data;
  Elicit_Swatch *sw;
  Evas_Coord mw, mh;
  double length;
  Evas_Coord h;

  sw = calloc(1, sizeof(Elicit_Swatch));

  sw->obj = edje_object_add(el->evas);
  sw->rect = evas_object_rectangle_add(el->evas);
  sw->r = el->color.r;
  sw->g = el->color.g;
  sw->b = el->color.b;
  sw->name = strdup(el->color.hex);

  edje_object_file_set(sw->obj, 
                       elicit_theme_find(elicit_config_theme_get(el)),
                       "swatch");
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
  evas_object_show(sw->rect);
  edje_object_part_swallow(sw->obj, "swatch", sw->rect);
  esmart_container_element_append(el->swatches.cont, sw->obj);

  /* scroll to the end of the list */
  length = esmart_container_elements_length_get(el->swatches.cont);
  el->swatches.length = length;
  evas_object_geometry_get(el->swatches.cont, NULL, NULL, NULL, &h);
  if (length > h)
  {
    esmart_container_scroll_offset_set(el->swatches.cont,
                                       h - length - 10 );
    _elicit_swatches_update_scroll_bar(el);
  }

  elicit_swatches_save(el);
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

  elicit_ui_update(el);
}

void
elicit_swatch_del_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  Elicit_Swatch *sw;

  el = evas_object_data_get(o, "elicit");
  sw = evas_object_data_get(o, "swatch");

  esmart_container_element_remove(el->swatches.cont, sw->obj);
  elicit_swatch_free(sw);

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
  printf("swatch cb (%s)\n", emission);
  
  if (!strcmp(emission, "drag"))
  {
    double l = esmart_container_elements_length_get(el->swatches.cont);
    Evas_Coord h;
    double vx, vy;

    evas_object_geometry_get(el->swatches.cont, NULL, NULL, NULL, &h);
    edje_object_part_drag_value_get(el->gui, source, &vx, &vy);
    esmart_container_scroll_offset_set(el->swatches.cont, -vy*(l-h+10));
  }
  else if (!fnmatch("elicit,swatch,scroll,up*", emission, 0))
  {
    printf("up\n");
    if (!strcmp(emission, "elicit,swatch,scroll,up,start"))
    {
      el->swatches.length = esmart_container_elements_length_get(el->swatches.cont);
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
    printf("dn\n");
    if (!strcmp(emission, "elicit,swatch,scroll,down,start"))
    {
      el->swatches.length = esmart_container_elements_length_get(el->swatches.cont);
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
