#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "Eyesight.h"
#include "envision.h"
#include "envision_win.h"
#include "envision_gui.h"

Eina_Bool
env_file_load(Envision *envision, const char *file)
{
  Evas_Object *o;
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

  env_gui_build(envision);

  o = eyesight_object_add(evas_object_evas_get(envision->gui.window));
  if (!eyesight_object_file_set(o, envision->file))
    {
      printf("erreur file set\n");
      return EINA_FALSE;
    }

  eyesight_object_page_render(o);
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  evas_object_size_hint_min_set(o, w, h);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, 0.0);
  evas_object_focus_set(o, EINA_TRUE);
  elm_win_resize_object_add(envision->gui.window, o);
  elm_object_content_set(envision->gui.scroller, o);
  evas_object_show(o);
  envision->obj = o;

  if (eyesight_object_page_count(envision->obj) > 1)
    env_gui_genlist_build(envision);

  env_win_title_set(envision);

  return EINA_TRUE;
}
