#include "Elicit.h"

#include <string.h>

void _elicit_shots_update_scroll_bar(Elicit *el);

int
elicit_shots_init(Elicit *el)
{
  if (edje_object_part_exists(el->gui, "shot.container"))
  {
    char *dir;

    el->shots.cont = esmart_container_new(el->evas); 
  
    dir = (char *)edje_object_data_get(el->gui, "shots.direction");
    if (dir && (dir[0] == 'h' || dir[0] == 'H'))
      el->shots.dir = CONTAINER_DIRECTION_HORIZONTAL;
    else
      el->shots.dir = CONTAINER_DIRECTION_VERTICAL;
      
    esmart_container_direction_set(el->shots.cont, el->shots.dir);

    esmart_container_spacing_set(el->shots.cont, 3);
    esmart_container_padding_set(el->shots.cont, 5, 5, 5, 5);
    evas_object_show(el->shots.cont);
    edje_object_part_swallow(el->gui, "shot.container", el->shots.cont);
    edje_object_part_drag_value_set(el->gui, "shot.scroll.bar", 0.001, 0.001);
  
    elicit_shots_load(el);
  
    ecore_idle_enterer_add(elicit_shots_scroll_idler, el);
  }

  return 1;
}

void
elicit_shots_shutdown(Elicit *el)
{
  Evas_List *l;

  if (el->shots.cont){
    for (l = esmart_container_elements_get(el->shots.cont); l; l = l->next)
    {
      Evas_Object *o = l->data;
      Elicit_Shot *sh = evas_object_data_get(o, "shot");
      elicit_shot_free(sh);
    }
    /*
    edje_object_part_unswallow(el->gui, el->shots.cont);
    evas_object_del(el->shots.cont);
    el->shots.cont = NULL;
    */
  }
}


void
elicit_shots_save(Elicit *el)
{
  Evas_List *l;
  E_DB_File *db;
  int i = 0;
  int old_num = 0, cleanup = 0;
  char buf[PATH_MAX];

  snprintf(buf, PATH_MAX, "%s/.e/apps/%s/shots.db", getenv("HOME"), el->app_name);

  /* open for reading first (to make sure the file exists) */
  db = e_db_open_read(buf);
  if (db)
  {
    cleanup = e_db_int_get(db, "/shots/num", &old_num);
    e_db_close(db);
  }

  db = e_db_open(buf);
  
  if (!db) return;

  for (l = esmart_container_elements_get(el->shots.cont); l; l = l->next, i++)
  {
    Evas_Object *obj;
    Elicit_Shot *sh;
    void *data;
    int iw, ih;

    obj = (Evas_Object *)(l->data);
    sh = evas_object_data_get(obj, "shot");

    snprintf(buf, PATH_MAX, "/shots/%d/name", i);
    e_db_str_set(db, buf, sh->name);
  
    evas_object_image_size_get(sh->shot, &iw, &ih);
    data = evas_object_image_data_get(sh->shot, 1);

    snprintf(buf, PATH_MAX, "/shots/%d/w", i);
    e_db_int_set(db, buf, iw);

    snprintf(buf, PATH_MAX, "/shots/%d/h", i);
    e_db_int_set(db, buf, ih);

    snprintf(buf, PATH_MAX, "/shots/%d/shot", i);
    e_db_data_set(db, buf, data, sizeof(data)*iw*ih);
  }
  e_db_int_set(db, "/shots/num", i);

  if (cleanup)
  {
    for(; i < old_num; i++)
    {
      snprintf(buf, PATH_MAX, "/shots/%d/name", i);
      e_db_data_del(db, buf);
   
      snprintf(buf, PATH_MAX, "/shots/%d/shot", i);
      e_db_data_del(db, buf);

      snprintf(buf, PATH_MAX, "/shots/%d/w", i);
      e_db_data_del(db, buf);

      snprintf(buf, PATH_MAX, "/shots/%d/h", i);
      e_db_data_del(db, buf);
    }
  }
  
  e_db_close(db);
  e_db_flush();
}

void
elicit_shots_load(Elicit *el)
{
  E_DB_File *db;
  int num, ok;
  int i;
  char buf[PATH_MAX];

  snprintf(buf, PATH_MAX, "%s/.e/apps/%s/shots.db", getenv("HOME"), el->app_name);

  db = e_db_open_read(buf);
  if (db)
    ok = e_db_int_get(db, "/shots/num", &num);
  else return;
 
  for (i = 0; i < num; i++)
  {
    Elicit_Shot *sh;
    int iw = 0, ih = 0;
    void *data = NULL;
    int size = 0;
    Evas_Coord mw, mh;
    char *theme;
  
    sh = calloc(1, sizeof(Elicit_Shot));
  
    snprintf(buf, PATH_MAX, "/shots/%d/name", i);
    sh->name = e_db_str_get(db, buf);
     
    snprintf(buf, PATH_MAX, "/shots/%d/w", i);
    e_db_int_get(db, buf, &iw);
  
    snprintf(buf, PATH_MAX, "/shots/%d/h", i);
    e_db_int_get(db, buf, &ih);
  
    snprintf(buf, PATH_MAX, "/shots/%d/shot", i);
    data = e_db_data_get(db, buf, &size);
  
    sh->obj = edje_object_add(el->evas);
    sh->shot = evas_object_image_add(el->evas);
 
    theme = elicit_config_theme_get(el);
    edje_object_file_set(sh->obj, 
                         elicit_theme_find(theme),
                         "shot");
    free(theme);
    edje_object_size_min_get(sh->obj, &mw, &mh);
    if (mw != 0 && mh != 0)
      evas_object_resize(sh->obj, mw, mh);
    else
      evas_object_resize(sh->obj, 20, 20);
    
    evas_object_show(sh->obj);
    evas_object_data_set(sh->obj, "shot", sh);
    evas_object_data_set(sh->obj, "elicit", el);
  
    edje_object_signal_callback_add(sh->obj, "elicit,shot,load", "", elicit_shot_load_cb, sh);
    edje_object_signal_callback_add(sh->obj, "elicit,shot,del", "", elicit_shot_del_cb, sh);
    edje_object_signal_callback_add(sh->obj, "elicit,shot,name,show", "", elicit_shot_name_show_cb, sh);
  
    evas_object_image_size_set(sh->shot, iw, ih);
    evas_object_image_data_copy_set(sh->shot, data);
    free(data);
    evas_object_pass_events_set(sh->shot, 1);
    evas_object_show(sh->shot);
    edje_object_part_swallow(sh->obj, "shot", sh->shot);
    esmart_container_element_append(el->shots.cont, sh->obj);
  }

  el->shots.length = esmart_container_elements_length_get(el->shots.cont);
  
  e_db_close(db);
}


void
elicit_shot_free(Elicit_Shot *sh)
{

  if (sh->shot) {
    edje_object_part_unswallow(sh->obj, sh->shot);
    evas_object_del(sh->shot);
  }
  if (sh->obj) evas_object_del(sh->obj);
  if (sh->name) free(sh->name);
}

void
elicit_shot_save_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el = data;
  Elicit_Shot *sh;
  int iw, ih;
  Evas_Coord mw, mh;
  double length;
  Evas_Coord w, h;
  char *theme;

  /* don't save an empty shot */
  if (!el->flags.shot_taken) return;

  sh = calloc(1, sizeof(Elicit_Shot));

  sh->obj = edje_object_add(el->evas);
  sh->shot = evas_object_image_add(el->evas);
  sh->name = strdup("Unnamed");

  theme = elicit_config_theme_get(el);
  edje_object_file_set(sh->obj, 
                       elicit_theme_find(theme),
                       "shot");

  free(theme);
  edje_object_size_min_get(sh->obj, &mw, &mh);

  if (mw != 0 && mh != 0)
    evas_object_resize(sh->obj, mw, mh);
  else
    evas_object_resize(sh->obj, 20, 20);
  evas_object_show(sh->obj);
  evas_object_data_set(sh->obj, "shot", sh);
  evas_object_data_set(sh->obj, "elicit", el);

  edje_object_signal_callback_add(sh->obj, "elicit,shot,load", "", elicit_shot_load_cb, sh);
  edje_object_signal_callback_add(sh->obj, "elicit,shot,del", "", elicit_shot_del_cb, sh);
  edje_object_signal_callback_add(sh->obj, "elicit,shot,name,show", "", elicit_shot_name_show_cb, sh);

  evas_object_image_size_get(el->shot, &iw, &ih);
  evas_object_image_size_set(sh->shot, iw, ih);
  evas_object_image_data_copy_set(sh->shot, evas_object_image_data_get(el->shot, 1));
  evas_object_pass_events_set(sh->shot, 1);
  evas_object_show(sh->shot);
  edje_object_part_swallow(sh->obj, "shot", sh->shot);
  esmart_container_element_append(el->shots.cont, sh->obj);

  /* scroll to the end of the list */
  length = esmart_container_elements_length_get(el->shots.cont);
  el->shots.length = length;
  evas_object_geometry_get(el->shots.cont, NULL, NULL, &w, &h);
  if (el->shots.dir == CONTAINER_DIRECTION_HORIZONTAL && length > w)
  {
    esmart_container_scroll_offset_set(el->shots.cont,
                                       w - length - 10 );
    _elicit_shots_update_scroll_bar(el);
  }
  else if (el->shots.dir == CONTAINER_DIRECTION_VERTICAL && length > h)
  {
    esmart_container_scroll_offset_set(el->shots.cont,
                                       h - length - 10 );
    _elicit_shots_update_scroll_bar(el);
  }


  elicit_shots_save(el);
}

void
elicit_shot_load_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  Elicit_Shot *sh;
  int iw, ih;

  el = evas_object_data_get(o, "elicit");
  sh = evas_object_data_get(o, "shot");

  evas_object_image_size_get(sh->shot, &iw, &ih);
  evas_object_image_size_set(el->shot, iw, ih);
  evas_object_image_data_copy_set(el->shot, evas_object_image_data_get(sh->shot, 1));
  evas_object_image_smooth_scale_set(el->shot, 0);
  evas_object_image_data_update_add(el->shot, 0, 0, iw, ih);

  /* FIXME: does the image data need to be deleted somehow with all this copying? */
  elicit_ui_update(el);
}

void
elicit_shot_del_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  Elicit_Shot *sh;
  Evas_Coord w, h;
  int offset;

  el = evas_object_data_get(o, "elicit");
  sh = evas_object_data_get(o, "shot");

  esmart_container_element_remove(el->shots.cont, sh->obj);
  elicit_shot_free(sh);
  
  el->shots.length = esmart_container_elements_length_get(el->shots.cont);

  evas_object_geometry_get(el->shots.cont, NULL, NULL, &w, &h);
  offset = esmart_container_scroll_offset_get(el->shots.cont);
  if (el->shots.dir == CONTAINER_DIRECTION_VERTICAL && offset < h - el->shots.length - 10 && el->shots.length > h)
  {
    esmart_container_scroll_offset_set(el->shots.cont,
                                       h - el->shots.length - 10);
  }
  if (el->shots.dir == CONTAINER_DIRECTION_HORIZONTAL && offset < w - el->shots.length - 10 && el->shots.length > w)
  {
    esmart_container_scroll_offset_set(el->shots.cont,
                                       w - el->shots.length - 10);
  }
  _elicit_shots_update_scroll_bar(el);

  elicit_shots_save(el);
}

void
elicit_shot_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el;
  Elicit_Shot *sh;

  el = evas_object_data_get(o, "elicit");
  sh = evas_object_data_get(o, "shot");

  edje_object_part_text_set(el->gui, "shot.label", sh->name);
}

void
elicit_shot_scroll_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el = data;

  if (!strcmp(emission, "drag"))
  {
    double l = el->shots.length;
    Evas_Coord h;
    double vx, vy;

    evas_object_geometry_get(el->shots.cont, NULL, NULL, NULL, &h);
    if (l > h)
    {
      edje_object_part_drag_value_get(el->gui, source, &vx, &vy);
      esmart_container_scroll_offset_set(el->shots.cont, -vy*(l-h+10));
    }
  }
  else if (!fnmatch("elicit,shot,scroll,up*", emission, 0))
  {
    if (!strcmp(emission, "elicit,shot,scroll,up"))
    {
      esmart_container_scroll(el->shots.cont, 5);
      _elicit_shots_update_scroll_bar(el);
    }
    else if (!strcmp(emission, "elicit,shot,scroll,up,start"))
    {
      el->shots.scrolling = 1;
      esmart_container_scroll_start(el->shots.cont, 1);
    }
    else
    {
      esmart_container_scroll_stop(el->shots.cont);
    }
  }
  else
  {
    if (!strcmp(emission, "elicit,shot,scroll,down"))
    {
      esmart_container_scroll(el->shots.cont, -5);
      _elicit_shots_update_scroll_bar(el);
    }
    else if (!strcmp(emission, "elicit,shot,scroll,down,start"))
    {
      el->shots.scrolling = 1;
      esmart_container_scroll_start(el->shots.cont, -1);
    }
    else
    {
      esmart_container_scroll_stop(el->shots.cont);
    }
  }
}

int
elicit_shots_scroll_idler(void *data)
{
  Elicit *el = data;
  if (el->shots.scrolling)
  {
    _elicit_shots_update_scroll_bar(el);
  }
  return 1;
}

void
_elicit_shots_update_scroll_bar(Elicit *el)
{
    int val= esmart_container_scroll_offset_get(el->shots.cont);
    Evas_Coord h;

    evas_object_geometry_get(el->shots.cont, NULL, NULL, NULL, &h);
    edje_object_part_drag_value_set(el->gui, "shot.scroll.bar", 0, -(double)val / (el->shots.length - h + 10));
}
