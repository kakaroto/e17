#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "Eyesight.h"
#include "envision.h"
#include "envision_win.h"

#define ENV_SCALE_STEP 1.414213562

Eina_Bool
env_input_key_cb(void *data, int type, void *event)
{
  Ecore_Event_Key *ev;
  Envision *envision;
  Eina_Bool is_scaled = EINA_FALSE;

  ev = (Ecore_Event_Key *)event;
  envision = (Envision *)data;

  if (type == ECORE_EVENT_KEY_UP)
    {
      if (!strcmp(ev->keyname, "q"))
        {
          elm_exit();
          return ECORE_CALLBACK_DONE;
        }
      if (!strcmp(ev->keyname, "p"))
        {
          if (envision->page_nbr > 0) envision->page_nbr--;
        }
      if (!strcmp(ev->keyname, "n"))
        {
          if (envision->page_nbr < (eyesight_object_page_count(envision->obj) - 1)) envision->page_nbr++;
        }
      if (!strcmp(ev->keyname, "asterisk"))
        {
          envision->scale *= ENV_SCALE_STEP;
          is_scaled = EINA_TRUE;
        }
      if (!strcmp(ev->keyname, "slash"))
        {
          envision->scale /= ENV_SCALE_STEP;
          is_scaled = EINA_TRUE;
        }
    }

  if (envision->page_nbr != eyesight_object_page_get(envision->obj))
    {
      eyesight_object_page_set(envision->obj, envision->page_nbr);
      eyesight_object_page_render(envision->obj);
      env_win_title_set(envision);
    }
  else if (is_scaled)
    {
      eyesight_object_page_scale_set(envision->obj,
                                     envision->scale, envision->scale);
      eyesight_object_page_render(envision->obj);
    }

  return ECORE_CALLBACK_PASS_ON;
}

static void
_env_file_select_cb(void        *data,
                    Evas_Object *obj,
                    void        *event_info)
{
  Envision *envision;
  const char *selected;

  envision = (Envision *)data;
  selected = event_info;

  if (selected)
    {
      if (!env_file_load(envision, selected))
        evas_object_del(obj);
      envision->start_with_file = 1;
    }
  else
    evas_object_del(obj);
}

Eina_Bool
env_input_mouse_cb(void *data, int type, void *event)
{
  Ecore_Event_Mouse_Button *ev;
  Envision *envision;

  ev = (Ecore_Event_Mouse_Button *)event;
  envision = (Envision *)data;

  if (type == ECORE_EVENT_MOUSE_BUTTON_UP)
    {
      if (ev->buttons == 3)
        {
          if (!envision->start_with_file)
            {
              Evas_Object *fs;

              fs = elm_fileselector_add(envision->win);
              elm_fileselector_is_save_set(fs, EINA_TRUE);
              elm_fileselector_expandable_set(fs, EINA_FALSE);
              elm_fileselector_path_set(fs, getenv("HOME"));
              evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
              evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
              elm_win_resize_object_add(envision->win, fs);
              evas_object_show(fs);
              evas_object_smart_callback_add(fs, "done", _env_file_select_cb, envision);
            }
        }
    }

  return ECORE_CALLBACK_PASS_ON;
}
