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

#include "gozer.h"
#include "options.h"

static void gozer_parse_option_array(int argc, char **argv);
static void gozer_parse_options_internal(void);
static void gozer_parse_rc_file(char *file, int user_defined);
gozeroptions opt;

void
init_parse_options(int argc, char **argv)
{
   char *home, *rcfile;
   struct stat ignored;

   /* Set default options */
   memset(&opt, 0, sizeof(gozeroptions));

   opt.line_spacing = 1;
   opt.bg_a = 255;
   opt.bg_g = 255;
   opt.bg_b = 255;
   opt.bg_r = 255;
   opt.fn_a = 255;
   opt.justification = JUST_LEFT;
   opt.font = estrdup("helmetr/16");
   opt.pipe = 0;
   opt.bg_image = NULL;
   opt.bg_resize = 0;
   opt.bg_tile = 0;
   opt.bg_scale = 0;

   home = getenv("HOME");
   if (!home)
      weprintf("D'oh! Please define HOME in your environment!"
               "It would really help me out in loading your rc file...\n");
   rcfile = estrjoin("/", home, ".gozerrc", NULL);
   if (stat(rcfile, &ignored) < 0)
	  gozer_parse_rc_file("/etc/gozerrc", 0);
   else
	  gozer_parse_rc_file(rcfile, 0);
   free(rcfile);

   /* Parse the cmdline args */
   gozer_parse_option_array(argc, argv);

   if (opt.rc_file)
      gozer_parse_rc_file(opt.rc_file, 1);

   gozer_parse_options_internal();
}

static void
gozer_parse_option_array(int argc, char **argv)
{
   static char stropts[] = "b:f:F:G:hj:l:oOpPr:s:t:T:vw:x:y:012";
   static struct option lopts[] = {
      /* actions */
      {"help", 0, 0, 'h'},
      {"version", 0, 0, 'v'},
      /* toggles */
      {"stdout", 0, 0, 'o'},
      {"cgi", 0, 0, 'O'},
      /* options */
      {"background", 1, 0, 'b'},
      {"foreground", 1, 0, 'f'},
      {"font", 1, 0, 'F'},
      {"bg-file", 1, 0, 'G'},
      {"bg-nocrop", 0, 0, '0'},
      {"bg-scale", 0, 0, '1'},
      {"bg-tile", 0, 0, '2'},
      {"justification", 1, 0, 'j'},
      {"line-spacing", 1, 0, 'l'},
      {"fontpath", 1, 0, 'p'},
      {"pipe", 0, 0, 'P'},
      {"rc-file", 1, 0, 'r'},
      {"style", 1, 0, 's'},
      {"text", 1, 0, 't'},
      {"textfile", 1, 0, 'T'},
      {"wrap", 1, 0, 'w'},
      {"x-offset", 1, 0, 'x'},
      {"y-offset", 1, 0, 'y'},
      {0, 0, 0, 0}
   };
   int optch = 0, cmdx = 0;

   /* Now to parse some optionarinos */
   while ((optch = getopt_long_only(argc, argv, stropts, lopts, &cmdx)) != EOF)
   {
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
        case 'b':
           opt.background_col = estrdup(optarg);
           break;
        case 'f':
           opt.font_col = estrdup(optarg);
           break;
        case 'F':
           opt.font = estrdup(optarg);
           break;
        case 'G':
           opt.bg_image = estrdup(optarg);
           break;
        case 'j':
           if (!strcasecmp(optarg, "left"))
              opt.justification = JUST_LEFT;
           else if (!strcasecmp(optarg, "right"))
              opt.justification = JUST_RIGHT;
           else if (!strcasecmp(optarg, "center"))
              opt.justification = JUST_CENTER;
           else if (!strcasecmp(optarg, "block"))
              opt.justification = JUST_BLOCK;
           else
              weprintf("unknown justification setting %s ignored\n", optarg);
           break;
        case 'l':
           opt.line_spacing = atoi(optarg);
           break;
        case 'o':
           opt.to_stdout = 1;
           break;
        case 'O':
           opt.to_stdout = 1;
           opt.cgi = 1;
           break;
        case 'p':
           gib_imlib_parse_fontpath(optarg);
           break;
		case 'P':
           opt.pipe = 1;
           break;
        case 'r':
           opt.rc_file = estrdup(optarg);
           break;
        case 's':
           opt.stylefile = estrdup(optarg);
           break;
        case 't':
           opt.text = estrdup(optarg);
           break;
        case 'T':
           opt.textfile = estrdup(optarg);
           break;
        case 'w':
           opt.wrap_width = atoi(optarg);
           break;
        case 'x':
           opt.x = atoi(optarg);
           break;
        case 'y':
           opt.y = atoi(optarg);
           break;
		case '0':
           opt.bg_resize = 1;
           opt.bg_scale = 0;
           opt.bg_tile = 0;
           break;
		case '1':
           opt.bg_resize = 0;
           opt.bg_scale = 1;
           opt.bg_tile = 0;
           break;
		case '2':
           opt.bg_resize = 0;
           opt.bg_scale = 0;
           opt.bg_tile = 1;
           break;
        default:
           break;
      }
   }

   /* Now the leftovers, which must be files */
   if (optind < argc)
   {
      while (optind < argc)
         if (!opt.output_file)
            opt.output_file = estrdup(argv[optind++]);
         else
            weprintf("unrecognised option %s\n", argv[optind++]);
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
   printf("Usage: " PACKAGE " [OPTIONS] file\nUse " PACKAGE
          " --help for detailed usage information\n");
   exit(0);
}


void
show_usage(void)
{
   fprintf(stdout,
           "Usage: " PACKAGE " [OPTIONS] file\n"
           " Where file is the target file for the rendered text.\n"
           " See man " PACKAGE " for more detailed information.\n"
           " Options can also be specified in an rc file , allowing you\n"
           " to save commonly used options (such as fontpaths) for\n"
           " reuse. The rc file $HOME/.gozerrc is always read first for default\n"
           " options, and other rc files can be specified on the commandline.\n"
           " See RC FILES for information on syntax.\n"
           " -h, --help               display this help and exit.\n"
           " -v, --version            output version information and exit.\n"
           " -b, --background COL     use COL as the background color,\n"
           "                          see COLOUR_DEFINITIONS for syntax,\n"
           "                          default is transparent (0,0,0,0).\n"
           " -f, --foreground COL     use COL as the foreground color. Default is\n"
           "                          white (255,255,255,255).\n"
           " -F, --font STR           draw using font STR. Size is specified with the\n"
           "                          name, eg \"arial/12\" for 12pt arial, default\n"
           "                          is helmetr/16 (a font supplied by gozer).\n"
           " -G, --bg-file FILE       load image FILE and draw the text on it\n"
           " -j, --justification STR  justify text, allowed values for STR are left,\n"
           "                          right, center or block. Default is left.\n"
           " -l, --line-spacing INT   separate multiple lines by INT pixels, default 1.\n"
           " -o, --stdout             send image data to STDOUT (for redirection)\n"
           " -O, --cgi                send image data to STDOUT with headers for CGI\n"
           "                          For both of these you still need to supply an\n"
           "                          output file, so gozer knows what image type to save\n"
           "                          as, but the file will be deleted automatically\n"
           " -p, --fontpath STR       colon seperated list of font directories to\n"
           "                          search for fonts in. Best used in the RCFILE\n"
           "                          to save typing it each time.\n"
           " -P, --pipe               Pipe input text from standard in instead of\n"
           "                          reading it from a file\n"
           " -r, --rc-file STR        use STR as an rc file to read for extra options.\n"
           " -s, --style STR          use STR as a file to load a fontstyle from,\n"
           "                          see FONTSTYLES for syntax.\n"
           " -t, --text STR           use STR as text to draw.\n"
           " -T, --textfile STR       read text to draw from file STR.\n"
           " -w, --wrap INT           wordwrap the text to INT pixels.\n" "\n"
           " -x, --x-offset INT       Offset text horizontally from top left by INT pixels.\n" "\n"
           " -y, --y-offset INT       Offset text vertically from top left by INT pixels.\n" "\n"
           " -0, --bg-nocrop          set output size to background image size\n"
           " -1, --bg-scale           scale background image\n"
           " -2, --bg-tile            tile background image\n"
           " RC FILES\n"
           " rc file syntax is simple. The '#' character at  the  start\n"
           " of  a  line  denotes that the line is a comment, otherwise\n"
           " lines are started with the name of a long option from  the\n"
           " commandline  (without  prefixing  the --), some whitespace\n"
           " and the value of the option. E.g.\n"
           " fontpath /usr/share/truetype:/usr/local/share/truetype\n"
           " NOTE that options in the rc file OVERRIDE those  from  the\n"
           " commandline.\n" "\n" " COLOUR_DEFINITIONS\n"
           " You can define colours in 4 ways. Using the html style:\n"
           " #RRGGBB (in which case alpha defaults to 255),\n" " #RRGGBBAA,\n"
           " or an alternative style:\n"
           " r,g,b or r,g,b,a (no spaces between commas please).\n"
           " eg for white text, either use #ffffff, #ffffffff, \"255,255,255\" or\n"
           " \"255,255,255,255\". For red, #ff0000, #ff0000ff or \"255,0,0\" etc.\n"
           "\n" " FONTSTYLES\n"
           " fontstyles can be defined in the EFM syntax (for compatibility only, these\n"
           " are not as powerful), or the recommended syntax defined here.\n"
           " The first line contains the line:\n" " #Style\n"
           " The second, an optional style name,\n" " #NAME mystyle\n"
           " There follows a list of layers. Each is described by this rule:\n"
           " RED GREEN BLUE ALPHA X_OFFSET Y_OFFSET\n"
           " The special values or 0,0,0,0 for red, green, blue and alpha specify the\n"
           " positioning of the actual text in it's selected colour.\n"
           " For example, the following style defines a very simple shadow for text:\n"
           " #Style\n" " #NAME shadow\n" " 0 0 0 128 -3 -3\n" " 0 0 0 0 0 0\n"
           "\n"
           "This program is free software - see the file COPYING for licensing info.\n"
           "Copyright Tom Gilbert 2000\n"
           "Email bugs to <gozer_sucks@linuxbrit.co.uk>\n");
   exit(0);
}

static void
gozer_parse_options_internal(void)
{
   if (opt.background_col)
      gib_imlib_parse_color(opt.background_col, &opt.bg_r, &opt.bg_g, &opt.bg_b,
                      &opt.bg_a);
   if (opt.font_col)
      gib_imlib_parse_color(opt.font_col, &opt.fn_r, &opt.fn_g, &opt.fn_b,
                      &opt.fn_a);
}


static void
gozer_parse_rc_file(char *file, int user_defined)
{
   FILE *fp = NULL;
   char s[1024], s1[1024], s2[1024];

   if ((fp = fopen(file, "r")) == NULL)
   {
      if (user_defined)
         weprintf("couldn't load the specified rcfile %s:", opt.rc_file);
      return;
   }

   /* Oooh. We have an options file :) */
   for (; fgets(s, sizeof(s), fp);)
   {
      s1[0] = '\0';
      s2[0] = '\0';
      sscanf(s, "%s %[^\n]\n", (char *) &s1, (char *) &s2);
      if (!(*s1) || (!*s2) || (*s1 == '\n') || (*s1 == '#'))
         continue;
      if (!strcasecmp(s1, "fontpath"))
         gib_imlib_parse_fontpath(s2);
      else if (!strcasecmp(s1, "background"))
      {
         free(opt.background_col);
         opt.background_col = estrdup(s2);
      }
      else if (!strcasecmp(s1, "foreground"))
      {
         free(opt.font_col);
         opt.font_col = estrdup(s2);
      }
      else if (!strcasecmp(s1, "font"))
      {
         free(opt.font);
         opt.font = estrdup(s2);
      }
      else if (!strcasecmp(s1, "justification"))
      {
         if (!strcasecmp(s2, "left"))
            opt.justification = JUST_LEFT;
         else if (!strcasecmp(s2, "right"))
            opt.justification = JUST_RIGHT;
         else if (!strcasecmp(s2, "center"))
            opt.justification = JUST_CENTER;
         else if (!strcasecmp(s2, "block"))
            opt.justification = JUST_BLOCK;
         else
            weprintf("unknown justification setting %s ignored\n", s2);
      }
      else if (!strcasecmp(s1, "line-spacing"))
         opt.line_spacing = atoi(s2);
      else if (!strcasecmp(s1, "style"))
      {
         free(opt.stylefile);
         opt.stylefile = estrdup(s2);
      }
      else if (!strcasecmp(s1, "text"))
      {
         free(opt.text);
         opt.text = estrdup(s2);
      }
      else if (!strcasecmp(s1, "textfile"))
      {
         free(opt.textfile);
         opt.textfile = estrdup(s2);
      }
      else if (!strcasecmp(s1, "wrap"))
         opt.wrap_width = atoi(s2);
      else
         weprintf("unrecognised option %s in rc file %s\n", s2, opt.rc_file);
   }
   fclose(fp);
}
