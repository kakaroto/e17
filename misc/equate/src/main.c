#include "Equate.h"

int
main (int argc, char *argv[], char *env[])
{

  math_init();

  ewl_init (&argc, argv);

  draw_interface ();

  return 0;
}
