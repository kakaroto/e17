/* options.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "scrot.h"
#include "options.h"

static void check_options(void);
static void feh_parse_option_array(int argc, char **argv);
scrotoptions opt;

void
init_parse_options(int argc, char **argv)
{
   D_ENTER(4);

   /* Set default options */
   memset(&opt, 0, sizeof(scrotoptions));

   D(3, ("About to parse commandline options\n"));
   /* Parse the cmdline args */
   feh_parse_option_array(argc, argv);

   D_RETURN_(4);
}

static void
feh_parse_option_array(int argc, char **argv)
{
   static char stropts[] = "cd:e:hsv+:";
   static struct option lopts[] = {
      /* actions */
      {"help", 0, 0, 'h'},                  /* okay */
      {"version", 0, 0, 'v'},               /* okay */
      {"count", 0, 0, 'c'},
      {"select", 0, 0, 's'},
      /* toggles */
      {"delay", 1, 0, 'd'},
      {"exec", 1, 0, 'e'},
      {"debug-level", 1, 0, '+'},
      {0, 0, 0, 0}
   };
   int optch = 0, cmdx = 0;

   D_ENTER(4);

   /* Now to pass some optionarinos */
   while ((optch = getopt_long(argc, argv, stropts, lopts, &cmdx)) != EOF)
   {
      D(5, ("Got option, getopt calls it %d, or %c\n", optch, optch));
      switch (optch)
      {
        case 0:
           break;
        case 'h':
           show_usage();
           break;
        case 'v':
           show_version();
           break;
        case 'd':
           opt.delay = atoi(optarg);
           break;
        case 'e':
           opt.exec = estrdup(optarg);
           break;
        case 's':
           opt.select = 1;
              break;
        case '+':
           opt.debug_level = atoi(optarg);
           break;
        case 'c':
           opt.countdown = 1;
           break;
        default:
           break;
      }
   }

   /* Now the leftovers, which must be files */
   if (optind < argc)
   {
      while (optind < argc)
      {
         /* If recursive is NOT set, but the only argument is a directory
            name, we grab all the files in there, but not subdirs */
         if (!opt.output_file)
            opt.output_file = argv[optind++];
         else
            weprintf("unrecognised option %s\n", argv[optind++]);
      }
   }

   /* So that we can safely be called again */
   optind = 1;
   D_RETURN_(4);
}


void
show_version(void)
{
   printf(PACKAGE " version " VERSION "\n");
   exit(0);
}

void
show_mini_usage(void)
{
   printf("Usage : " PACKAGE " [OPTIONS]... FILE\nUse " PACKAGE
          " --help for detailed usage information\n");
   exit(0);
}


void
show_usage(void)
{
   fprintf(stdout,
           "Usage : " PACKAGE " [OPTIONS]... FILE\n"
           "  Where a FILE is the target file for the screenshot.\n" "  man "
           PACKAGE " for more details\n"
           "  -h, --help                display this help and exit\n"
           "  -v, --version             output version information and exit\n"
           "  -c, --count               show a countdown before taking the shot\n"
           "  -d, --delay NUM           wait NUM seconds before taking a shot\n"
           "  -e, --exec APP            run APP on the resulting screenshot\n"
           "  -s, --select              interactively choose a window or rectnagle\n"
           "                            with the mouse\n"
           "\n"
           "This program is free software see the file COPYING for licensing info.\n"
           "Copyright Tom Gilbert 2000\n"
           "Email bugs to <scrot_sucks@linuxbrit.co.uk>\n");
   exit(0);
}
