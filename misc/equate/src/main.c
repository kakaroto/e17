#include "Equate.h"
#include <stdio.h>

void
print_usage(void)
{
   printf("Equate - a calculator for Enlightenment\n");
   printf("Version 0.0.1 (Dec 3 2003)\n");
   printf("(c)2003 by HandyAndE.\n");
   printf("Usage: equate [options]\n\n");
   printf("Supported Options:\n");
   printf("-h, --help              Print this help text\n");
   printf("-e, --exec        <str> Execute an equation and exit\n");
   printf("-b, --basic             Use Equate in basic mode (default)\n");
   printf("-s, --scientific        Use Equate in scientific mode\n");
}

int
main(int argc, char *argv[], char *env[])
{
   mode            calc_mode = BASIC;
   char           *exec = NULL;
   int             help = 0;
   int             nextarg = 1;
   char           *arg;

   while (nextarg < argc) {
      arg = argv[nextarg];
      if (!strcmp(arg, "--scientific") || !strcmp(arg, "-s"))
         calc_mode = SCI;
      else if (!strcmp(arg, "--basic") || !strcmp(arg, "-b"))
         calc_mode = BASIC;
      else if (!strcmp(arg, "--exec") || !strcmp(arg, "-e")) {
         exec = argv[++nextarg];
         if (!exec)
            printf("-e requires an extra parameter, none given\n");
      }


      else if (!strcmp(arg, "--help") || !strcmp(arg, "-h"))
         help = 1;
      else
         printf("Unrecognised option \"%s\", perhapse try --help?\n", arg);

      nextarg++;
   }

   if (help) {
      print_usage();
      return 0;
   }

   equate_init();
   if (exec) {
      equate_append(exec);

      printf("%.10g\n", equate_eval());
      return 0;
   }

   ewl_init(&argc, argv);

   draw_interface(calc_mode);

   return 0;
}
