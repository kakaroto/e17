#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "Eyesight.h"
#include "envision.h"
#include "envision_config.h"
#include "envision_input.h"
#include "envision_load.h"
#include "envision_win.h"

/********** Local **********/

int _envision_log_domain = -1;

static const Ecore_Getopt options = {
   PACKAGE_NAME,
   "%prog [options] file",
   PACKAGE_VERSION,
   "(C) 2012 Vincent Torri",
   "GPL v2",
   "Multi document viewer written with Enlightenment Foundation Libraries.",
   EINA_TRUE,
   {
      ECORE_GETOPT_STORE_STR('e', "engine", "ecore-evas engine to use"),
      ECORE_GETOPT_CALLBACK_NOARGS('E', "list-engines", "list ecore-evas engines",
                                   ecore_getopt_callback_ecore_evas_list_engines, NULL),
      ECORE_GETOPT_STORE_STR('t', "theme",
                             "Use the named edje theme or path to theme file."),
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_COPYRIGHT('C', "copyright"),
      ECORE_GETOPT_LICENSE('L', "license"),
      ECORE_GETOPT_HELP('h', "help"),
      ECORE_GETOPT_SENTINEL
   }
};

static void
_env_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_exit();
}

static Envision *
_env_new()
{
  Envision *e;

  e = (Envision *)calloc(1, sizeof(Envision));
  if (!e)
    return NULL;

  e->scale = 1.0;

  return e;
}

static void
_env_free(Envision *e)
{
  if (!e)
    return;

  if (e->file)
    free(e->file);
  free(e);
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
  char buf[4096];
  Evas *evas;
  Evas_Object *win;
  Evas_Object *o;
  Ecore_Event_Handler *handler_key_up;
  Ecore_Event_Handler *handler_mouse_up;
  Envision *envision;
  char *engine = NULL;
  char *theme = NULL;
  char *file = NULL;
  Eina_Bool quit_option = EINA_FALSE;
  int args;
  Ecore_Getopt_Value values[] = {
    ECORE_GETOPT_VALUE_STR(engine),
    ECORE_GETOPT_VALUE_BOOL(quit_option),
    ECORE_GETOPT_VALUE_STR(theme),
    ECORE_GETOPT_VALUE_BOOL(quit_option),
    ECORE_GETOPT_VALUE_BOOL(quit_option),
    ECORE_GETOPT_VALUE_BOOL(quit_option),
    ECORE_GETOPT_VALUE_BOOL(quit_option),
    ECORE_GETOPT_VALUE_NONE
  };

  if (!eyesight_init())
    {
      fprintf(stderr, "Could not initialize Eyesight\n");
      return EXIT_FAILURE;
    }

  ecore_event_init();

  args = ecore_getopt_parse(&options, values, argc, argv);
  if (args < 0)
    {
      ERR("Could not parse command line options.");
      goto failure;
    }
  if (quit_option) goto success;

  if (engine)
    elm_config_preferred_engine_set(engine);

  if (theme)
    {
#if 0
      char path[PATH_MAX];
      char name[PATH_MAX];

      if (eina_str_has_suffix(theme, ".edj"))
        eina_strlcpy(name, theme, sizeof(name));
      else
        snprintf(name, sizeof(name), "%s.edj", theme);

      if (strchr(name, '/'))
        eina_strlcpy(path, name, sizeof(path));
      else
        snprintf(path, sizeof(path), "%s/themes/%s",
                 elm_app_data_dir_get(), name);

      eina_stringshare_replace(&(config->theme), path);
      config->temporary = EINA_TRUE;
#endif
    }

  if (args == (argc - 1))
    {
      file = argv[args];
    }

  envision = _env_new();
  if (!envision)
    goto failure;

  handler_key_up = ecore_event_handler_add(ECORE_EVENT_KEY_UP,
                                           env_input_key_cb, envision);
  if (!handler_key_up)
    goto free_envision;

  handler_mouse_up = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,
                                             env_input_mouse_cb, envision);
  if (!handler_key_up)
    goto del_key_up;

  env_config_init();

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
  elm_app_compile_bin_dir_set(PACKAGE_BIN_DIR);
  elm_app_compile_data_dir_set(PACKAGE_DATA_DIR);
  elm_app_info_set(elm_main, PACKAGE_NAME, "themes/default.edj");

  win = env_win_add();
  evas_object_smart_callback_add(win, "delete,request", _env_win_del, NULL);
  envision->gui.window = win;

  evas = evas_object_evas_get(win);

  if (file)
    {
      if (!env_file_load(envision, file))
        elm_exit();
      envision->start_with_file = 1;
    }
  else
    {
      o = edje_object_add(evas);
      evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
      snprintf(buf, sizeof(buf) - 1, "%s/themes/%s",
               elm_app_data_dir_get(), "default.edj");
      if (!edje_object_file_set(o, buf, "e/init/splash"))
        {
          const char *errmsg;
          int err;

          err = edje_object_load_error_get(o);
          errmsg = edje_load_error_str(err);
          ERR("could not load 'e/init/splash' from %s: %s",
              buf, errmsg);
        }
      else
        {
          edje_object_part_text_set(o, "e.text.title", "Envision");
          edje_object_part_text_set(o, "e.text.version", PACKAGE_VERSION);
          edje_object_part_text_set(o, "e.text.status", "Right click to open file");
          evas_object_layer_set(o, -1);
          elm_win_resize_object_add(envision->gui.window, o);
          evas_object_show(o);
        }
    }

  evas_object_resize(win, 424, 600);
  evas_object_show(win);

  elm_run();
  env_config_shutdown();
  ecore_event_handler_del(handler_mouse_up);
  ecore_event_handler_del(handler_key_up);
  _env_free(envision);
 success:
  eyesight_shutdown();
  elm_shutdown();
  return EXIT_SUCCESS;

 del_key_up:
  ecore_event_handler_del(handler_key_up);
 free_envision:
  _env_free(envision);
 failure:
  eyesight_shutdown();

  return EXIT_FAILURE;
}
ELM_MAIN()
