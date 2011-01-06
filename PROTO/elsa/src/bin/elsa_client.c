#include "elsa_client.h"
#include <Ecore.h>
#include <Ecore_X.h>
/*
static void
_elsa_help() {
   fprintf(stderr, "Usage: %s [options...]\n", PACKAGE);
   fprintf(stderr, "\th: print this help message\n");
}
*/

int
elsa_client_main()
{
   fprintf(stderr, PACKAGE": client init\n");
   if (elsa_gui_init()) return 1;
   fprintf(stderr, PACKAGE": client run\n");
   elsa_connect_init();
   elm_run();
   elsa_connect_shutdown();
   fprintf(stderr, PACKAGE": client shutdown\n");
   elsa_gui_shutdown();
   return 0;
}

int
main(int argc, char **argv)
{
   eina_init();
   ecore_init();
   ecore_x_init(":0");
   elm_init(argc, argv);
   elsa_client_main();
   elm_shutdown();
   ecore_x_shutdown();
   ecore_shutdown();
   eina_shutdown();
}
