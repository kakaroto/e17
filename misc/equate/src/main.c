#include "Equate.h"

int
main (int argc, char *argv[], char *env[])
{
  mode calc_mode = BASIC;
  if (argc > 1 && (!strcmp (argv[1], "--scientific")))
    calc_mode = SCI;

  ewl_init (&argc, argv);

  draw_interface (calc_mode);

  return 0;
}
