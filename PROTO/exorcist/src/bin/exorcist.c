#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_TOOLKIT_EWL
#  include <Ewl.h>
#  include "ewl_main_window.h"
#endif

#ifdef HAVE_TOOLKIT_ETK
#  include <Etk.h>
#  include "etk_main_window.h"
#endif

typedef enum
{
  EXORCIST_TOOLKIT_EWL,
  EXORCIST_TOOLKIT_ETK
}Exorcist_Toolkit;

static void
exo_init (Exorcist_Toolkit toolkit, int argc, char *argv[])
{
  switch (toolkit) {
  case EXORCIST_TOOLKIT_EWL:
#ifdef HAVE_TOOLKIT_EWL
    if (!ewl_init (&argc, argv)) exit (-1);
#else
    printf ("Ewl toolkit is not installed\nExiting..");
    exit (-1);
#endif
    break;
  case EXORCIST_TOOLKIT_ETK:
#ifdef HAVE_TOOLKIT_ETK
    if (!etk_init ("ecore_evas_software_x11")) exit (-1);
#else
    printf ("Etk toolkit is not installed\nExiting..");
    exit (-1);
#endif
    break;
  default:
    break;
  }
}

static void
exo_main (Exorcist_Toolkit toolkit, char *filename)
{
  switch (toolkit) {
  case EXORCIST_TOOLKIT_EWL:
#ifdef HAVE_TOOLKIT_EWL
    exo_ewl_main_window (filename);
    ewl_main ();
#endif
    break;
  case EXORCIST_TOOLKIT_ETK:
#ifdef HAVE_TOOLKIT_ETK
    exo_etk_main_window (filename);
    etk_main ();
#endif
    break;
  default:
    break;
  }
}

static void
exo_info_print (char *file)
{
  printf ("\n");
  printf ("Usage: %s [option] [filename]\n", file);
  printf ("\n");
  printf ("Options:\n");
  printf ("   -h, --help     this help\n");
  printf ("   -ewl           use Ewl toolkit\n");
  printf ("   -etk           use Etk toolkit\n");
  printf ("\n");
}

int
main (int argc, char *argv[])
{
  char            *filename;
  Exorcist_Toolkit toolkit;
  int              i;

  filename = NULL;
  toolkit = EXORCIST_TOOLKIT_EWL;
  i = 1;
  while (i < argc) {
    if ((strcmp (argv[i], "-h") == 0) || (strcmp (argv[i], "--help") == 0))
      exo_info_print (argv[0]);
    else if (strcmp (argv[i], "-ewl") == 0)
      toolkit = EXORCIST_TOOLKIT_EWL;
    else if (strcmp (argv[i], "-etk") == 0)
      toolkit = EXORCIST_TOOLKIT_ETK;
    else
      filename = argv[i];
    i++;
  }

  exo_init (toolkit, argc, argv);
  exo_main (toolkit, filename);

  return 0;
}
