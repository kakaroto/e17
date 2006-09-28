#include "Equate.h"

#include <Ecore_Config.h>

Equate          equate;

void
equate_init(Equate * equate)
{
   math_init();
}

void
equate_quit(void)
{
   equate_quit_gui();
   ecore_config_save();
   ecore_config_shutdown();
   exit(0);
}

void
equate_arg_exec(char *val, void *data) {
  equate_append(val);
  printf("%.10g\n", equate_eval());
  ecore_config_shutdown();
  exit(0);
}

void
equate_arg_mode(char *val, void *data) {
  int mode;
  mode = (int) data;

  ecore_config_int_set("/settings/mode", mode);
}

/********************************
 * listeners                    *
 ********************************/
int
gui_listener(const char *key, const Ecore_Config_Type type, const int tag,
             void *data)
{
   switch (tag) {
   case 0:
      equate.conf.mode = ecore_config_int_get(key);
      break;
   case 1:
      equate.conf.theme = ecore_config_theme_get(key);
   }
   equate_update_gui(&equate);
   return 0;
}

/********************************
 * main                         * 
 ********************************/

int
main(int argc, char *argv[], char *env[])
{
   int             nextarg = 1, parse_ret, found;
   char           *arg;

   equate.conf.mode = DEFAULT;

   ecore_config_init("equate");
   ecore_config_app_describe("Equate - a calculator for Enlightenment\n\
Version 0.0.4 (Nov 30 2004)\n\
(c)2003 - 2004 by HandyAndE.\n\
Usage: equate [options]");
   ecore_config_theme_search_path_append(PACKAGE_DATA_DIR "/themes/");

   /* this controls our defaults */
   ecore_config_int_create_bound("/settings/mode", BASIC, 0, 3, 1, 'm', "mode",
      "The mode to start in, 1=basic, 2=sci, 3=edje");
   ecore_config_theme_create("/settings/theme", "equate", 't', "theme",
      "The name of the edje theme to use in mode 3");
   ecore_config_args_callback_str_add('e', "exec", 
                                      "Execute an equation on the command line",
				      &equate_arg_exec, NULL);
   ecore_config_args_callback_noarg_add('s', "scientific", 
                                        "Use Equate in scientific mode",
				        &equate_arg_mode, SCI);
   ecore_config_args_callback_noarg_add('b', "basic", 
                                        "Use Equate in basic mode (default)",
                                        &equate_arg_mode, BASIC);

   /* load and read our settings */
   ecore_config_load();

   ecore_app_args_set(argc, (const char **)argv);
   if ((parse_ret = ecore_config_args_parse())
       != ECORE_CONFIG_PARSE_CONTINUE) {
     ecore_config_shutdown();
     exit(0);
   }

   equate.conf.mode = ecore_config_int_get("/settings/mode");
   equate.conf.theme = ecore_config_theme_get("/settings/theme");

   equate_init(&equate);
   equate_init_gui(&equate, argc, argv);

   ecore_config_listen("gui_mode", "/settings/mode", gui_listener, 0, NULL);
   ecore_config_listen("gui_theme", "/settings/theme", gui_listener, 1, NULL);

   return 0;
}
