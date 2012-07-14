#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "Eyesight.h"
#include "envision.h"

#define __UNUSED__

typedef struct _Genlist_Item
{
  Elm_Object_Item *item;
  Envision *envision;
  int page;
  Eina_Bool selected : 1;
} Genlist_Item;

static Elm_Genlist_Item_Class itc;

static char *
_env_genlist_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   const Genlist_Item *tit = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "page %d", tit->page);
   return strdup(buf);
}

static Evas_Object *
_env_genlist_content_get(void *data, Evas_Object *obj, const char *part)
{
  const Genlist_Item *tit = data;
  Evas_Object *o;
  void *doc;
  Eyesight_Backend eb;
  int w;
  int h;

  o = eyesight_object_add(evas_object_evas_get(tit->envision->win));
  if (!eyesight_object_file_set(o, tit->envision->file))
    {
      printf("erreur file set\n");
      return EINA_FALSE;
    }

  eyesight_object_page_set(o, tit->page);
  eyesight_object_page_scale_set(o, 0.24, 0.24);
  eyesight_object_page_render (o);
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  evas_object_size_hint_min_set(o, w, h);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_focus_set(o, EINA_TRUE);
  evas_object_show(o);
  elm_win_resize_object_add(tit->envision->win, o);

   return o;
}

static Eina_Bool
_env_genlist_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

static void
gl_sel(void *data, Evas_Object *obj, void *event_info)
{
   printf("sel item data [%p] on genlist obj [%p], item pointer [%p]\n", data, obj, event_info);
}

static Evas_Object *
_env_genlist_get(Envision *envision)
{
  Genlist_Item tit[3];
  Evas_Object *gl;

  gl = elm_genlist_add(envision->win);
  evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(gl);

  itc.item_style     = "default";
  itc.func.text_get = _env_genlist_text_get;
  itc.func.content_get  = _env_genlist_content_get;
  itc.func.state_get = _env_genlist_state_get;
  itc.func.del       = NULL;

  tit[0].envision = envision;
  tit[0].page = 1;
  tit[0].selected = EINA_TRUE;
  tit[0].item = elm_genlist_item_append(gl, &itc,
                                        &(tit[0]), NULL,
                                        ELM_GENLIST_ITEM_NONE, gl_sel, NULL);

  tit[1].envision = envision;
  tit[1].page = 2;
  tit[1].selected = EINA_FALSE;
  tit[1].item = elm_genlist_item_append(gl, &itc,
                                        &(tit[1]), NULL,
                                        ELM_GENLIST_ITEM_NONE, gl_sel, NULL);

  tit[2].envision = envision;
  tit[2].page = 3;
  tit[2].selected = EINA_FALSE;
  tit[2].item = elm_genlist_item_append(gl, &itc,
                                        &(tit[2]), NULL,
                                        ELM_GENLIST_ITEM_NONE, gl_sel, NULL);
  return gl;
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
  if (!eyesight_object_file_set(o, envision->file))
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

  {
    char buf[4096];
    Evas_Object *gl;

/*     snprintf(buf, sizeof(buf) - 1, "%s/themes/%s", */
/*              elm_app_data_dir_get(), "default.edj"); */
/*     o = edje_object_add(evas_object_evas_get(envision->win)); */
/*     evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
/*     evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL); */
/*     edje_object_file_set(o, buf, "envision/thumbs"); */
/*     elm_win_resize_object_add(envision->win, o); */
/*     evas_object_show(o); */

    gl = _env_genlist_get(envision);
    evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
/*     edje_object_part_swallow(o, "envision.swallow.content", gl); */
    elm_win_resize_object_add(envision->win, o);
    evas_object_show(gl);
  }

  env_win_title_set(envision);

  return EINA_TRUE;
}
