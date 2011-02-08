#include "elsa_client.h"
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Getopt.h>
/*
static void
_elsa_help() {
   fprintf(stderr, "Usage: %s [options...]\n", PACKAGE);
   fprintf(stderr, "\th: print this help message\n");
}
*/
static const Ecore_Getopt options =
{
   "elsa_client",
   "%prog [options]",
   VERSION,
   "(C) 2011 Enlightenment, see AUTHORS.",
   "GPL, see COPYING",
   "Launch gui client.",
   EINA_TRUE,
   {
      ECORE_GETOPT_STORE_STR('d', "display", "specify the display to use"),
      ECORE_GETOPT_HELP ('h', "help"),
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_COPYRIGHT('R', "copyright"),
      ECORE_GETOPT_LICENSE('L', "license"),
      ECORE_GETOPT_SENTINEL
   }
};

int
elsa_client_main()
{
   fprintf(stderr, PACKAGE": client init\n");
   if (elsa_gui_init()) return EXIT_FAILURE;
   fprintf(stderr, PACKAGE": client run\n");
   elsa_connect_init();
   elm_run();
   elsa_connect_shutdown();
   fprintf(stderr, PACKAGE": client shutdown\n");
   elsa_gui_shutdown();
   return EXIT_SUCCESS;
}

int
main(int argc, char **argv)
{
   int args;
   unsigned char quit_option = 0;
   char *display = NULL;

   Ecore_Getopt_Value values[] =
     {
        ECORE_GETOPT_VALUE_STR(display),
        ECORE_GETOPT_VALUE_BOOL(quit_option),
        ECORE_GETOPT_VALUE_BOOL(quit_option),
        ECORE_GETOPT_VALUE_BOOL(quit_option),
        ECORE_GETOPT_VALUE_BOOL(quit_option)
     };
   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     return EXIT_FAILURE;
   if (quit_option)
     return EXIT_SUCCESS;
   if (!display)
     {
        printf("A display is required!\n");
        return EXIT_FAILURE;
     }
   eina_init();
   ecore_init();
   ecore_x_init(display);
   elm_init(argc, argv);
   elsa_client_main();
   elm_shutdown();
   ecore_x_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return EXIT_SUCCESS;
}
