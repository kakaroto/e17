#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "Eyesight.h"
#include "envision.h"
#include "envision_win.h"


Evas_Object *
env_win_add()
{
  char buf[4096];
  Evas_Object *win;
  Evas_Object *o;

  win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
  elm_win_autodel_set(win, EINA_TRUE);

  elm_win_title_set(win, "Envision");
  elm_win_icon_name_set(win, "Envision");

  o = evas_object_image_add(evas_object_evas_get(win));
  snprintf(buf, sizeof(buf), "%s/images/document.png",
           elm_app_data_dir_get());
  evas_object_image_file_set(o, buf, NULL);
  elm_win_icon_object_set(win, o);

  return win;
}

void
env_win_title_set(Envision *envision)
{
  char buf[256];
  char *str;

#if _WIN32
  str = strrchr(envision->file, '\\');
#else
  str = strrchr(envision->file, '/');
#endif
  if (!str)
    str = envision->file;
  else
    str++;
  snprintf(buf, sizeof(buf) - 1, "Envision - %s (%d/%d)",
           str, envision->page_nbr + 1,
           eyesight_object_page_count(envision->obj));
  elm_win_title_set(envision->win, buf);
}
