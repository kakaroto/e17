#include "Equate.h"

#include <Ecore_Config.h>

Ecore_Config_Bundle *props;

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
   ewl_main_quit();
   if (props)
      ecore_config_save(props);
   ecore_config_exit();
}

int
main(int argc, char *argv[], char *env[])
{
   Equate          equate;
   int             nextarg = 1;
   char           *arg;
   Ecore_Config_Server *conf_srv;

   equate.conf.mode = DEFAULT;

   while (nextarg < argc) {
      arg = argv[nextarg];
      if (!strcmp(arg, "--scientific") || !strcmp(arg, "-s"))
         equate.conf.mode = SCI;
      else if (!strcmp(arg, "--basic") || !strcmp(arg, "-b"))
         equate.conf.mode = BASIC;
      else if (!strcmp(arg, "--theme") || !strcmp(arg, "-t")) {
         equate.conf.mode = EDJE;
         equate.conf.theme = argv[++nextarg];
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

   if (conf_srv = ecore_config_init("equate")) {
      props = ecore_config_bundle_new(conf_srv, "settings");
      /* this controls our defaults */
      ecore_config_default_int_bound(props, "/settings/mode", BASIC, 0, 3, 1);
      ecore_config_default_string(props, "/settings/theme", "equate");




      ecore_config_load(props);

      if (equate.conf.mode == DEFAULT)
         equate.conf.mode = ecore_config_get_int(props, "/settings/mode");
      if ((equate.conf.mode == EDJE) && (!equate.conf.theme))
         equate.conf.theme = ecore_config_get_string(props, "/settings/theme");
   } else {
      /* in case ecore_config fails to init */
      if (equate.conf.mode == DEFAULT)
         equate.conf.mode = BASIC;
      if ((equate.conf.mode == EDJE) && (!equate.conf.theme))
         equate.conf.theme = "equate";
   }
   equate_init(&equate);

   init_gui(&equate, argc, argv);

   return 0;
}
