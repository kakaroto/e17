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

#include "stalk.h"
#include "options.h"
#include "stalk_file.h"

static void stalk_parse_option_array(int argc, char **argv);
stalkoptions opt;

void
init_parse_options(int argc, char **argv)
{
   /* Set default options */
   memset(&opt, 0, sizeof(stalkoptions));
   opt.w = 800;
   opt.h = 150;
   opt.x = opt.y = -9999;
   opt.max_lines = 200;
   opt.fontstr = estrdup("arial/8");
   opt.fn_a = 255;
   opt.fn_r = 0;
   opt.fn_g = 0;
   opt.fn_b = 0;
   opt.bg_a = 255;
   opt.bg_r = 255;
   opt.bg_g = 255;
   opt.bg_b = 255;
   opt.wrap = 1;
   opt.trans = 0;

   /* Parse the cmdline args */
   stalk_parse_option_array(argc, argv);
}

static void
stalk_parse_option_array(int argc, char **argv)
{
   static char stropts[] = "c:C:f:g:hl:s:tvw+:";
   static struct option lopts[] = {
      /* actions */
      {"help", 0, 0, 'h'},
      {"version", 0, 0, 'v'},
      /* toggles */
      {"nowrap", 0, 0, 'w'},
      {"trans", 0, 0, 't'},
      /* settings */
      {"font", 1, 0, 'f'},
      {"geometry", 1, 0, 'g'},
      {"lines", 1, 0, 'l'},
      {"debug-level", 1, 0, '+'},
      {"style", 1, 0, 's'},
      {0, 0, 0, 0}
   };
   int optch = 0, cmdx = 0;

   /* Now to pass some optionarinos */
   while ((optch = getopt_long_only(argc, argv, stropts, lopts, &cmdx)) !=
          EOF)
   {
      switch (optch)
      {
        case 0:
           break;
        case 'c':
           gib_imlib_parse_color(optarg, &opt.fn_r, &opt.fn_g, &opt.fn_b,
                      &opt.fn_a);
           break;
        case 'C':
           gib_imlib_parse_color(optarg, &opt.bg_r, &opt.bg_g, &opt.bg_b,
                      &opt.bg_a);
           break;
        case 'f':
           if (opt.fontstr)
              free(opt.fontstr);
           opt.fontstr = estrdup(optarg);
           break;
        case 'g':
           XParseGeometry(optarg, &opt.x, &opt.y, &opt.w, &opt.h);
           break;
        case 'h':
           show_usage();
           break;
        case 'l':
           opt.max_lines = atoi(optarg);
           break;
        case 's':
           if (opt.stylefile)
              free(opt.stylefile);
           opt.stylefile = estrdup(optarg);
           break;
        case 't':
           opt.trans = 1;
           break;
        case 'v':
           show_version();
           break;
        case 'w':
           opt.wrap = 0;
           break;
        case '+':
           opt.debug_level = atoi(optarg);
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
         stalk_file *file = NULL;

         file = stalk_file_new(argv[optind++]);
         if (file)
            opt.files = gib_list_add_front(opt.files, file);
      }
   }

   /* So that we can safely be called again */
   optind = 1;
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
   printf("Usage : " PACKAGE " [OPTIONS]... FILE[s]\nUse " PACKAGE
          " --help for detailed usage information\n");
   exit(0);
}


void
show_usage(void)
{
   fprintf(stdout,
           "Usage : " PACKAGE " [OPTIONS]... FILE[s]\n"
           "  Where FILE is the target file for stalking.\n" "  See man "
           PACKAGE " for more details\n"
           "  -h, --help                display this help and exit\n"
           "  -v, --version             output version information and exit\n"
           "\n"
           "This program is free software see the file COPYING for licensing info.\n"
           "Copyright Tom Gilbert 2000\n"
           "Email bugs to <stalk_sucks@linuxbrit.co.uk>\n");
   exit(0);
}
