#include "Equate.h"

#include <Ecore_Config.h>

Ecore_Config_Bundle *props;
Equate          equate;

void
print_usage(void)
{
   printf("Equate - a calculator for Enlightenment\n");
   printf("Version 0.0.4 (Dec 8 2003)\n");
   printf("(c)2003 by HandyAndE.\n");
   printf("Usage: equate [options]\n\n");
   printf("Supported Options:\n");
   printf("  -h, --help              Print this help text\n");
   printf("  -e, --exec        <str> Execute an equation and exit\n");
   printf("Display modes:\n");
   printf("  -b, --basic             Use Equate in basic mode (default)\n");
   printf("  -s, --scientific        Use Equate in scientific mode\n");
   printf
      ("  -t, --theme       [str] Use themed (edje) mode and load specified theme\n");
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
   if (props)
      ecore_config_save(props);
   ecore_config_exit();
   exit(0);
}

/********************************
 * listeners                    *
 ********************************/
int
gui_listener(const char *key, const Ecore_Config_Type type, const int tag,
             void *data, void *bundle)
{
   switch (tag) {
   case 0:
      equate.conf.mode = ecore_config_get_int(bundle, key);
      break;
   case 1:
      equate.conf.theme = ecore_config_get_string(bundle, key);
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
   int             nextarg = 1;
   char           *arg, *tmp;
   Ecore_Config_Server *conf_srv;

   equate.conf.mode = DEFAULT;

   if (conf_srv = ecore_config_init("equate")) {
      props = ecore_config_bundle_new(conf_srv, "settings");
      /* this controls our defaults */
      ecore_config_default_int_bound(props, "/settings/mode", BASIC, 0, 3, 1);
      ecore_config_default_string(props, "/settings/theme", "equate");

      ecore_config_load(props);
      equate.conf.mode = ecore_config_get_int(props, "/settings/mode");
      equate.conf.theme = ecore_config_get_string(props, "/settings/theme");

   } else {
      /* in case ecore_config fails to init */
      equate.conf.mode = BASIC;
      equate.conf.theme = "equate";
   }

   while (nextarg < argc) {
      arg = argv[nextarg];
      if (!strcmp(arg, "--scientific") || !strcmp(arg, "-s")) {
         equate.conf.mode = SCI;
         ecore_config_set_int(props, "/settings/mode", SCI);
      } else if (!strcmp(arg, "--basic") || !strcmp(arg, "-b")) {
         equate.conf.mode = BASIC;
         ecore_config_set_int(props, "/settings/mode", BASIC);
      } else if (!strcmp(arg, "--theme") || !strcmp(arg, "-t")) {
         equate.conf.mode = EDJE;
         ecore_config_set_int(props, "/settings/mode", EDJE);
         tmp = argv[++nextarg];
         if (tmp) {
            equate.conf.theme = tmp;
            ecore_config_set_string(props, "/settings/theme",
                                    equate.conf.theme);
         }
      } else if (!strcmp(arg, "--exec") || !strcmp(arg, "-e"))
         exec(argv[++nextarg]);



      else if (!strcmp(arg, "--help") || !strcmp(arg, "-h"))
         print_usage();
      else {
         printf("Unrecognised option \"%s\"\n\n", arg);
         print_usage();
      }
      nextarg++;
   }

   ecore_config_listen(props, "gui_mode", "/settings/mode", gui_listener, 0,
                       NULL);
   ecore_config_listen(props, "gui_theme", "/settings/theme", gui_listener, 1,
                       NULL);

   equate_init(&equate);
   equate_init_gui(&equate, argc, argv);
   return 0;
}
