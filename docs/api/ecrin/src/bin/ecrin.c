#include <Ecore.h>
#include <Ewl.h>

#include "ecrin.h"
#include "gui_ewl.h"
#include "hash.h"
#include "parser.h"

int
ecrin_init (char         *filename,
            Ecrin_Toolkit toolkit)
{
  if (!ecrin_parse_init (filename))
    return 0;

  if (!ecrin_hash_init ())
    return 0;
  
  switch (toolkit)
    {
    case TOOLKIT_EWL:
      if (!ewl_init (NULL, NULL))
        {
          printf ("Could not init ewl. Exiting...\n");
          return 0;
        }
      break;
    default:
      break;
    }
  return 1;
}

void
ecrin_window_create (Ecrin_Toolkit toolkit)
{
  switch (toolkit)
    {
    case TOOLKIT_EWL:
      ecrin_ewl_window_create ();
      break;
    default:
      break;
    }
}



void
ecrin_shutdown (void)
{
}

int
main (int argc, char *argv[])
{
  Ecrin_Toolkit toolkit;
  char *filename = NULL;

  toolkit = TOOLKIT_EWL;

  if (argc > 1)
    {
      if ((strcmp (argv[1], "-h") == 0) ||
          (strcmp (argv[1], "--help") == 0))
        {
          printf ("Usage: ecrin [option] xml_file\n\n");
          printf ("Option:\n");
          printf ("  -h, --help: this help message\n");
          printf ("  -ewl      : use of ewl\n");
          printf ("\n");
          
          return 1;
        }
      else if (strcmp (argv[1], "-ewl") == 0)
        {
          toolkit = TOOLKIT_EWL;
          if (argc > 2)
            filename = argv[2];
          else
            {
              printf ("Bad usage\n");
              printf ("Run ecrin -h for the help\n");
              
              return 0;
            }
        }
      else
        filename = argv[1];
    }
  else
    {
      printf ("Bad usage\n");
      printf ("Run ecrin -h for the help\n");

      return 0;
    }
    
  if (!ecrin_init (filename,
                   toolkit))
    return 0;

  ecrin_parse_xml ();

  ecrin_window_create (toolkit);

  ewl_main();

  ecrin_shutdown ();

  return 1;
}
