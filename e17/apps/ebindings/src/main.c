/**************************************************************************
 * main.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 10, 2001
 * Gets the ebindings ball rolling
 *************************************************************************/
#include "interface.h"
#include "ebindings.h"

void print_ebindings_version(void);

/* Pretty complex huh ? =) */
int
main(int argc, char *argv[])
{
   int i;
   GtkWidget *win;

   gtk_init(&argc, &argv);
   for (i = 0; i < argc; i++)
   {
      if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")
          || !strcmp(argv[i], "-V"))
      {
         print_ebindings_version();
      }
   }
   win = create_main_ebindings_window();

   gtk_main();

   return 0;
}

void
print_ebindings_version(void)
{
   printf("%s - %s\n", PACKAGE, VERSION);
   exit(0);
}
