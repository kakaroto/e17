#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "Eyesight.h"
#include "envision.h"

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
   const Genlist_Item *item = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "page %d", item->page);
   return strdup(buf);
}

static Evas_Object *
_env_genlist_content_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
  const Genlist_Item *item = data;
  Evas_Object *o;
  int w;
  int h;

  o = eyesight_object_add(evas_object_evas_get(item->envision->gui.window));
  if (!eyesight_object_file_set(o, item->envision->file))
    {
      printf("erreur file set\n");
      return NULL;
    }

  eyesight_object_page_set(o, item->page);
  eyesight_object_page_scale_set(o, 0.24, 0.24);
  eyesight_object_page_render (o);
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  evas_object_size_hint_min_set(o, w, h);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_focus_set(o, EINA_TRUE);
  evas_object_show(o);
  elm_win_resize_object_add(item->envision->gui.window, o);

  return o;
}

static Eina_Bool
_env_genlist_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

static void
_env_genlist_select_cb(void *data, Evas_Object *obj, void *event_info)
{
  Genlist_Item *item;
  Envision *envision;

  item = (Genlist_Item *)data;
  envision = item->envision;
  eyesight_object_page_set(envision->obj, item->page);
  eyesight_object_page_render(envision->obj);
}

void
env_gui_build(Envision *envision)
{
  Evas_Object *o;

  if (envision->gui_built)
    return;

  o = elm_bg_add(envision->gui.window);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(envision->gui.window, o);
  evas_object_show(o);
  /* FIXME: modify theme for bg ? */
  envision->gui.background = o;

  o = elm_box_add(envision->gui.window);
  elm_box_horizontal_set(o, EINA_TRUE);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(envision->gui.window, o);
  evas_object_show(o);
  envision->gui.box = o;

  o = elm_scroller_add(envision->gui.window);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_scroller_bounce_set(o, EINA_TRUE, EINA_TRUE);
  elm_box_pack_end(envision->gui.box, o);
  evas_object_show(o);
  envision->gui.scroller = o;

  envision->gui_built = EINA_TRUE;
}

void
env_gui_genlist_build(Envision *envision)
{
  Genlist_Item *items;
  Evas_Object *o;
  int page_count;
  int i;

  o = elm_genlist_add(envision->gui.window);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_pack_start(envision->gui.box, o);
  evas_object_show(o);
  envision->gui.genlist = o;

  itc.item_style     = "default";
  itc.func.text_get = _env_genlist_text_get;
  itc.func.content_get  = _env_genlist_content_get;
  itc.func.state_get = _env_genlist_state_get;
  itc.func.del       = NULL;

  page_count = eyesight_object_page_count(envision->obj);
  items = (Genlist_Item *)malloc(page_count * sizeof(Genlist_Item));
  for (i = 0; i < page_count; i++)
    {
      items[i].envision = envision;
      items[i].page = i;
      items[i].selected = EINA_TRUE;
      items[i].item = elm_genlist_item_append(envision->gui.genlist, &itc,
                                              items + i, NULL,
                                              ELM_GENLIST_ITEM_NONE, _env_genlist_select_cb, items + i);
    }
}
