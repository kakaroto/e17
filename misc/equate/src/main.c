#include "Equate.h"

#include <Ecore_Config.h>

Equate          equate;

void
print_usage(void)
{
   printf(" -e, --exec\t <str> Execute an equation and exit\n");
   printf("Display modes:\n");
   printf(" -b, --basic\t       Use Equate in basic mode (default)\n");
   printf(" -s, --scientific      Use Equate in scientific mode\n");
   ecore_config_shutdown();
   exit(0);
}

void
exec(char *exe)
{
   if (!exe) {
      fprintf(stderr, "Error: --exec needs an addtional argument\n");
      exit(1);
   }

   equate_append(exe);
   printf("%.10g\n", equate_eval());
   ecore_config_shutdown();
   exit(0);
}

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
Version 0.0.4 (Dec 8 2003)\n\
(c)2003 by HandyAndE.\n\
Usage: equate [options]");

   /* this controls our defaults */
   ecore_config_int_create_bound("/settings/mode", BASIC, 0, 3, 1, 'm', "mode",
      "The mode to start in, 1=basic, 2=sci, 3=edje");
   ecore_config_theme_create("/settings/theme", "equate", 't', "theme",
      "The name of the edje theme to use in mode 3");

   /* load and read our settings */
   ecore_config_load();

   ecore_app_args_set(argc, (const char **)argv);
   if ((parse_ret = ecore_config_args_parse())
       == ECORE_CONFIG_PARSE_EXIT) {
// don't do this, as we have hooks to put in
//     ecore_config_shutdown();
//     exit(0);
      found = 0;
      while (nextarg < argc) {
         arg = argv[nextarg];
         if (!strcmp(arg, "--scientific") || !strcmp(arg, "-s")) {
            ecore_config_int_set("/settings/mode", SCI);
            found = 1;
         } else if (!strcmp(arg, "--basic") || !strcmp(arg, "-b")) {
            ecore_config_int_set("/settings/mode", BASIC);
            found = 1;
         } else if (!strcmp(arg, "--exec") || !strcmp(arg, "-e"))
            exec(argv[++nextarg]);
         nextarg++;
      }

      if (!found) {
         // we were probably told to quit due to an unrecognised option
         ecore_config_shutdown();
         exit(0);
      }
   }
   if (parse_ret == ECORE_CONFIG_PARSE_HELP) {
      print_usage();
   }

   equate.conf.mode = ecore_config_int_get("/settings/mode");
   equate.conf.theme = ecore_config_theme_get("/settings/theme");

   ecore_config_listen("gui_mode", "/settings/mode", gui_listener, 0, NULL);
   ecore_config_listen("gui_theme", "/settings/theme", gui_listener, 1, NULL);

   equate_init(&equate);
   equate_init_gui(&equate, argc, argv);
   return 0;
}
