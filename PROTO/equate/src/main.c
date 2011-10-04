#include "Equate.h"

Equate          equate;

int
main(int argc, char *argv[], char *env[])
{
   int             nextarg = 1, parse_ret, found;
   char           *arg;

   elm_init(argc, argv);

   equate.conf.mode = EDJE;
   math_init();
   equate_init_gui(&equate, argc, argv);

   elm_run();

   elm_shutdown();

   return 0;
}
