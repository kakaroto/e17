#include "Elicit.h"

#include <string.h>
#include <dirent.h>

void _elicit_themes_update_scroll_bar(Elicit *el);

int
elicit_themes_init(Elicit *el)
{
  if (edje_object_part_exists(el->gui, "theme.container"))
  {
    el->themes.cont = esmart_container_new(el->evas); 
    esmart_container_direction_set(el->themes.cont, CONTAINER_DIRECTION_VERTICAL);
    esmart_container_fill_policy_set(el->themes.cont, CONTAINER_FILL_POLICY_FILL_X);
    esmart_container_spacing_set(el->themes.cont, 2);
    esmart_container_padding_set(el->themes.cont, 5, 5, 5, 5);
    edje_object_part_swallow(el->gui, "theme.container", el->themes.cont);
    edje_object_part_drag_value_set(el->gui, "theme.scroll.bar", 0.001, 0.001);
  
    evas_object_show(el->themes.cont);
    {
      Evas_Object *o;
      Evas_List *themes, *l;

      themes = elicit_themes_list();
      
      for (l = themes; l; l = l->next)
      {
        o = edje_object_add(el->evas);
        edje_object_file_set(o, elicit_theme_find(elicit_config_theme_get()), "theme");
        evas_object_resize(o, 100, 10);
        edje_object_part_text_set(o, "theme.label", l->data);
        evas_object_data_set(o, "theme", l->data);
        evas_object_data_set(o, "elicit", el);
        evas_object_repeat_events_set(o, 1);
        edje_object_signal_callback_add(o, "elicit,theme,load", "*", elicit_theme_load_cb, NULL);
        evas_object_show(o);
        esmart_container_element_append(el->themes.cont, o);
      }

      evas_list_free(themes);

      el->themes.length = esmart_container_elements_length_get(el->themes.cont);

    }
  }

  ecore_idle_enterer_add(elicit_themes_scroll_idler, el);

  return 1;
}

void
elicit_themes_shutdown(Elicit *el)
{
  Evas_List *l;

  if(el->themes.cont)
  {
    for (l = esmart_container_elements_get(el->themes.cont); l; l = l->next)
    {
      Evas_Object *o = l->data;
      char *theme = evas_object_data_get(o, "theme");

      free(theme);
      esmart_container_element_remove(el->themes.cont, o);
      evas_object_del(o);
    }
   /* 
    evas_object_del(el->themes.cont);
    el->themes.cont = NULL;
  */
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

  esmart_container_element_remove(el->themes.cont, o);
  evas_object_del(o);

  elicit_shots_shutdown(el);
  elicit_swatches_shutdown(el);
  elicit_themes_shutdown(el);
  elicit_spectra_shutdown(el);

  evas_object_del(el->gui);
  
  el->gui = edje_object_add(el->evas);
  evas_object_name_set(el->gui, "gui");
  evas_object_move(el->gui, 0, 0);
  evas_object_show(el->gui);

  elicit_ui_theme_set(el, th, "elicit");

  elicit_shots_init(el);
  elicit_swatches_init(el);
  elicit_themes_init(el);
  elicit_spectra_init(el);
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

Evas_List *
elicit_themes_list()
{
  char *dirs = NULL, *ptr = NULL, *end = NULL, *dir = NULL;
  struct stat st;
  struct dirent *ent;
  DIR *dp;
  Evas_List *themes = NULL, *l = NULL;
  int theme_seen = 0;
  
  dirs = ecore_config_theme_search_path_get();
  ptr = dirs;
  end = dirs + strlen(dirs);
  dir = dirs;
  while (ptr && ptr < end)
  {
    while (*ptr != '|' && ptr < end)
      ptr++;
    if (ptr < end)
      *ptr = '\0';

    if (stat(dir, &st) != 0)
    {
      ptr ++;
      dir = ptr;
      continue;
    }
    dp = opendir((const char *) dir);
    while ((ent = readdir(dp)))
    {
      if (ent->d_name[0] == '.') continue;

      theme_seen = 0;
      for (l = themes; l; l = l->next)
      {
        if (!strcmp(l->data, ent->d_name))
        {
          theme_seen = 1;
          break;
        }
      }

      if (!theme_seen)
      {
        char *theme = strdup(ent->d_name);
        char *suff = theme + strlen(theme) - 4;
        if (!strcmp(suff, ".edj"))
        {
          *suff = '\0';
          themes = evas_list_append(themes, theme);
        }
        else free(theme);
      }
    }

    ptr++;
    dir = ptr;
  }
  free(dirs);

  return themes;
}


void
elicit_theme_scroll_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
  Elicit *el = data;

  if (!strcmp(emission, "drag"))
  {
    double l = el->themes.length;
    Evas_Coord h;
    double vx, vy;

    evas_object_geometry_get(el->themes.cont, NULL, NULL, NULL, &h);
    if (l > h)
    {
      edje_object_part_drag_value_get(el->gui, source, &vx, &vy);
      esmart_container_scroll_offset_set(el->themes.cont, -vy*(l-h+10));
    }
  }
  else if (!fnmatch("elicit,theme,scroll,up*", emission, 0))
  {
    if (!strcmp(emission, "elicit,theme,scroll,up"))
    {
      esmart_container_scroll(el->themes.cont, 5);
      _elicit_themes_update_scroll_bar(el);
    }
    else if (!strcmp(emission, "elicit,theme,scroll,up,start"))
    {
      el->themes.scrolling = 1;
      esmart_container_scroll_start(el->themes.cont, 1);
    }
    else
    {
      esmart_container_scroll_stop(el->themes.cont);
    }
  }
  else
  {
    if (!strcmp(emission, "elicit,theme,scroll,down"))
    {
      esmart_container_scroll(el->themes.cont, -5);
      _elicit_themes_update_scroll_bar(el);
    }
    else if (!strcmp(emission, "elicit,theme,scroll,down,start"))
    {
      el->themes.scrolling = 1;
      esmart_container_scroll_start(el->themes.cont, -1);
    }
    else
    {
      esmart_container_scroll_stop(el->themes.cont);
    }
  }
}

int
elicit_themes_scroll_idler(void *data)
{
  Elicit *el = data;

  if (el->themes.scrolling)
  {
    _elicit_themes_update_scroll_bar(el);
  }
  return 1;
}

void
_elicit_themes_update_scroll_bar(Elicit *el)
{
    int val= esmart_container_scroll_offset_get(el->themes.cont);
    Evas_Coord h;

    evas_object_geometry_get(el->themes.cont, NULL, NULL, NULL, &h);
    edje_object_part_drag_value_set(el->gui, "theme.scroll.bar", 0, -(double)val / (el->themes.length - h + 10));
}
