/* main.c

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

static void gozer_parse_option_array(int argc, char **argv, gozeroptions *opt);
static void gozer_parse_options_internal(gozeroptions *opt);
static void init_parse_options(int argc, char **argv, gozeroptions *opt);
static void show_usage(void);
static void show_version(void);
static void show_mini_usage(void);
static void gozer_stream_file(char *file, int headers);
static char *gozer_read_file(char *filename);
static char *gozer_read_pipe();

int
main(int argc, char **argv)
{
   Imlib_Image image;
   Imlib_Load_Error err;
   gozeroptions opt;

   init_parse_options(argc, argv, &opt);

   init_imlib();

   if (!opt.output_file)
   {
      weprintf("no output file specified");
      show_mini_usage();
   }

   if (!opt.text && opt.textfile)
      opt.text = gozer_read_file(opt.textfile);

   if (!opt.text && opt.pipe)
   {
      opt.text = gozer_read_pipe();
   }

   if (!opt.text)
   {
      weprintf("no text specified for rendering");
      show_mini_usage();
   }

   image = gozer_render(&opt);

   imlib_context_set_image(image);
   imlib_image_attach_data_value("quality", NULL, opt.quality, NULL);

   gib_imlib_save_image_with_error_return(image, opt.output_file, &err);
   if (err)
      eprintf("Saving to file %s failed", opt.output_file);
   if (opt.to_stdout)
   {
      gozer_stream_file(opt.output_file, opt.cgi);
      unlink(opt.output_file);
   }

   return 0;
}

static void
init_parse_options(int argc, char **argv, gozeroptions *opt)
{
   default_options(opt);

   /* Parse the cmdline args */
   gozer_parse_option_array(argc, argv, opt);

   if (opt->rc_file)
      gozer_parse_rc_file(opt->rc_file, 1, opt);

   gozer_parse_options_internal(opt);
}

static void
gozer_parse_option_array(int argc, char **argv, gozeroptions *opt)
{
   static char stropts[] = "b:f:F:G:hj:l:oOp:Pr:s:t:T:vw:x:y:012";
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
      {"quality", 1, 0, 'q'},
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
           opt->background_col = estrdup(optarg);
           break;
        case 'f':
           opt->font_col = estrdup(optarg);
           break;
        case 'F':
           opt->font = estrdup(optarg);
           break;
        case 'G':
           opt->bg_image = estrdup(optarg);
           break;
        case 'j':
           if (!strcasecmp(optarg, "left"))
              opt->justification = JUST_LEFT;
           else if (!strcasecmp(optarg, "right"))
              opt->justification = JUST_RIGHT;
           else if (!strcasecmp(optarg, "center"))
              opt->justification = JUST_CENTER;
           else if (!strcasecmp(optarg, "block"))
              opt->justification = JUST_BLOCK;
           else
              weprintf("unknown justification setting %s ignored\n", optarg);
           break;
        case 'l':
           opt->line_spacing = atoi(optarg);
           break;
        case 'o':
           opt->to_stdout = 1;
           break;
        case 'O':
           opt->to_stdout = 1;
           opt->cgi = 1;
           break;
        case 'p':
           gib_imlib_parse_fontpath(optarg);
           break;
	    	case 'P':
           opt->pipe = 1;
           break;
        case 'q':
           opt->quality = atoi(optarg);
           break;
        case 'r':
           opt->rc_file = estrdup(optarg);
           break;
        case 's':
           opt->stylefile = estrdup(optarg);
           break;
        case 't':
           opt->text = estrdup(optarg);
           break;
        case 'T':
           opt->textfile = estrdup(optarg);
           break;
        case 'w':
           opt->wrap_width = atoi(optarg);
           break;
        case 'x':
           opt->x = atoi(optarg);
           break;
        case 'y':
           opt->y = atoi(optarg);
           break;
		case '0':
           opt->bg_resize = 1;
           opt->bg_scale = 0;
           opt->bg_tile = 0;
           break;
		case '1':
           opt->bg_resize = 0;
           opt->bg_scale = 1;
           opt->bg_tile = 0;
           break;
		case '2':
           opt->bg_resize = 0;
           opt->bg_scale = 0;
           opt->bg_tile = 1;
           break;
        default:
           break;
      }
   }

   /* Now the leftovers, which must be files */
   if (optind < argc)
   {
      while (optind < argc)
         if (!opt->output_file)
            opt->output_file = estrdup(argv[optind++]);
         else
            weprintf("unrecognised option %s\n", argv[optind++]);
   }

   /* So that we can safely be called again */
   optind = 1;
}


static void
show_version(void)
{
   printf(PACKAGE " version " VERSION "\n");
   exit(0);
}

static void
show_mini_usage(void)
{
   printf("Usage: " PACKAGE " [OPTIONS] file\nUse " PACKAGE
          " --help for detailed usage information\n");
   exit(0);
}


static void
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
           " -q, --quality NUM        Image quality (1-100) high value means\n"
           "                          high size, low compression. Default: 75.\n"
           "                          For lossless compression formats, like png,\n"
           "                          low quality means high compression.\n"
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
gozer_parse_options_internal(gozeroptions *opt)
{
   if (opt->background_col)
      gib_imlib_parse_color(opt->background_col,
            &opt->bg_r, &opt->bg_g, &opt->bg_b, &opt->bg_a);
   if (opt->font_col)
      gib_imlib_parse_color(opt->font_col,
            &opt->fn_r, &opt->fn_g, &opt->fn_b, &opt->fn_a);
}

static void
gozer_stream_file(char *file, int headers)
{
   FILE *fp;
   char buf[10240];
   size_t count;

   if (headers)
   {
      /* print headers for user-agents */
      char *extension;

      extension = strrchr(file, '.');
      if (extension)
      {
         extension++;
         if (!strcasecmp(extension, "jpg"))
            printf("Content-type: image/jpeg\n");
         else
            printf("Content-type: image/%s\n", extension);
      }
      else
         weprintf("couldn't determine file extension for mime type setting");
      printf("\n");
   }

   fp = fopen(file, "r");
   if (!fp)
   {
      weprintf("couldn't open file %s for streaming\n", file);
      return;
   }
   while ((count = fread(buf, 1, sizeof buf, fp)))
   {
      fwrite(buf, 1, count, stdout);
   }
   fclose(fp);
}

static char *
gozer_read_file(char *filename)
{
   FILE *fp;
   struct stat st;
   char *text;

   if (stat(filename, &st) == -1)
   {
      weprintf("couldn't stat file %s :", filename);
      return NULL;
   }

   fp = fopen(filename, "r");

   if (!fp)
   {
      weprintf("couldn't open file %s :", filename);
      return NULL;
   }

   text = malloc(st.st_size + 1);
   fread(text, 1, st.st_size, fp);
   text[st.st_size] = '\0';

   fclose(fp);

   return text;
}

static char *
gozer_read_pipe()
{
   char buf[PIPE_BUF_MAX] = "", buf2[1023] = "", *text = NULL;
   int len = 0;

   while (fgets(buf2, 1023, stdin))
   {
      len += strlen(buf2);
      if (len < PIPE_BUF_MAX)
      {
         strncat(buf, buf2, 1023);
      }
      else
      {
         weprintf("Truncating oversized pipe buffer at %d bytes.", len);
      }
   }
   text = _estrdup(buf);
   return text;
}
