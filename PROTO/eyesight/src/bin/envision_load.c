#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "Eyesight.h"
#include "envision.h"

static char *
_env_backend_get(const char *file)
{
  char *backend;

  backend = strrchr(file, '.');
  if (!backend)
    return NULL;

  backend++;
  if (!strcmp(backend, "eps"))
    return "ps";

  if ((strcmp(backend, "bmp") == 0) ||
      (strcmp(backend, "gif") == 0) ||
      (strcmp(backend, "jpg") == 0) ||
      (strcmp(backend, "jpeg") == 0) ||
      (strcmp(backend, "png") == 0) ||
      (strcmp(backend, "ppm") == 0) ||
      (strcmp(backend, "tga") == 0) ||
      (strcmp(backend, "tiff") == 0) ||
      (strcmp(backend, "xpm") == 0) ||
      (strcmp(backend, "cba") == 0) ||
      (strcmp(backend, "cbr") == 0) ||
      (strcmp(backend, "cbt") == 0) ||
      (strcmp(backend, "cbz") == 0) ||
      (strcmp(backend, "cb7") == 0))
    return "img";

  return backend;
}

Eina_Bool
env_file_load(Envision *envision, const char *file)
{
  Eyesight_Backend eb;
  Evas_Object *o;
  void *doc;
  char *backend;
  char *tmp;
  int w;
  int h;

  if (!file || !*file)
    return EINA_FALSE;

  tmp = strdup(file);
  if (!tmp)
    return EINA_FALSE;

  if (envision->file)
    free(envision->file);

  envision->file = tmp;
#ifdef _WIN32
      while (*tmp)
        {
          if (*tmp == '/') *tmp = '\\';
          tmp++;
        }
#endif

  if (!envision->bg)
    {
      o = elm_bg_add(envision->win);
      evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      elm_win_resize_object_add(envision->win, o);
      evas_object_show(o);
      /* FIXME: modify theme for bg ? */
      envision->bg = o;
    }

  if (!envision->sc)
    {
      o = elm_scroller_add(envision->win);
      evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      elm_scroller_bounce_set(o, EINA_TRUE, EINA_TRUE);
      elm_win_resize_object_add(envision->win, o);
      evas_object_show(o);
      envision->sc = o;
    }

  o = eyesight_object_add(evas_object_evas_get(envision->win));
  backend = _env_backend_get(envision->file);
  eb = eyesight_object_init(o, backend);
  if (!eb)
    {
      printf("erreur init backend\n");
      return EINA_FALSE;
    }

  doc = eyesight_object_file_set(o, envision->file);
  if (!doc)
    {
      printf("erreur file set\n");
      return EINA_FALSE;
    }

  eyesight_object_page_render (o);
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  evas_object_size_hint_min_set(o, w, h);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, 0.0);
  evas_object_focus_set(o, EINA_TRUE);
  elm_object_content_set(envision->sc, o);
  evas_object_show(o);
  envision->obj = o;

  env_win_title_set(envision);

  return EINA_TRUE;
}
