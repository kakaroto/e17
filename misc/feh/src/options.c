/* options.c
 *
 * Copyright (C) 2000 Tom Gilbert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "feh.h"

static void check_options (void);
static void feh_parse_option_array (int argc, char **argv);
static void feh_parse_environment_options (void);
static void feh_check_theme_options (int arg, char **argv);
static void feh_parse_options_from_string (char *opts);
static void feh_load_options_for_theme (char *theme);
static char *theme;

void
init_parse_options (int argc, char **argv)
{
  D_ENTER;

  /* For setting the command hint on X windows */
  cmdargc = argc;
  cmdargv = argv;

  /* Set default options */
  memset (&opt, 0, sizeof (fehoptions));
  opt.display = 1;
  opt.aspect = 1;
  opt.progressive = 1;
  opt.slideshow_delay = -1;
  opt.thumb_w = 60;
  opt.thumb_h = 60;

  D (("About to parse env options (if any)\n"));
  /* Check for and parse any options in FEH_OPTIONS */
  feh_parse_environment_options ();

  D (("About to parse commandline options\n"));
  /* Parse the cmdline args */
  feh_parse_option_array (argc, argv);

  D (("About to check for theme configuration\n"));
  feh_check_theme_options (argc, argv);

  D (("Options parsed\n"));

  if (filelist_length (filelist) == 0)
    show_mini_usage ();

  check_options ();

  feh_prepare_filelist ();
  D_RETURN_;
}

static void
feh_check_theme_options (int arg, char **argv)
{
  D_ENTER;
  if (!theme)
    {
      /* This prevents screw up when running src/feh or ./feh */
      char *pos = strrchr (argv[0], '/');
      if (pos)
	theme = estrdup (pos + 1);
      else
	theme = estrdup (argv[0]);
    }
  D (("Theme name is %s\n", theme));

  feh_load_options_for_theme (theme);

  free (theme);
  D_RETURN_;
  arg = 0;
}

static void
feh_load_options_for_theme (char *theme)
{
  FILE *fp = NULL;
  char *home;
  char *rcpath;
  char s[1024], s1[1024], s2[1024];

  D_ENTER;

  home = getenv ("HOME");
  if (!home)
    weprintf
      ("D'oh! Please define HOME in your environment! It would really help me out...\n");
  else
    {
      rcpath = estrjoin ("/", home, ".fehrc", NULL);
      D (("Trying %s for config\n", rcpath));
      fp = fopen (rcpath, "r");
      free (rcpath);
    }
  if (!fp && ((fp = fopen ("/etc/fehrc", "r")) == NULL))
        D_RETURN_;

  /* Oooh. We have an options file :) */
  for (; fgets (s, sizeof (s), fp);)
    {
      s1[0] = '\0';
      s2[0] = '\0';
      sscanf (s, "%s %[^\n]\n", (char *) &s1, (char *) &s2);
      if (!(*s1) || (!*s2) || (*s1 == '\n') || (*s1 == '#'))
	continue;
      D (("Got theme/options pair %s/%s\n", s1, s2));
      if (!strcmp (s1, theme))
	{
	  D (("A match. Using options %s\n", s2));
	  feh_parse_options_from_string (s2);
	  break;
	}
    }
  fclose (fp);
  D_RETURN_;
}

static void
feh_parse_environment_options (void)
{
  char *opts;

  D_ENTER;

  if ((opts = getenv ("FEH_OPTIONS")) == NULL)
        D_RETURN_;

  /* We definitely have some options to parse */
  feh_parse_options_from_string (opts);
  D_RETURN_;
}

/* FIXME This function is a crufty bitch ;) */
static void
feh_parse_options_from_string (char *opts)
{
  char **list = NULL;
  int num = 0;
  char *s;
  char *t;
  int i = 0;

  D_ENTER;
  /* So we don't reinvent the wheel (not again, anyway), we use the
   * getopt_long function to do this parsing as well. This means it has to
   * look like the real argv ;)
   */
  list = malloc (sizeof (char *));
  list[num++] = estrdup (PACKAGE);

  for (s = opts, t = opts;; t++)
    {
      if (*t == ' ')
	{
	  *t = '\0';
	  num++;
	  list = erealloc (list, sizeof (char *) * num);
	  list[num - 1] = estrdup (s);
	  s = t + 1;
	}
      else if (*t == '\0')
	{
	  num++;
	  list = erealloc (list, sizeof (char *) * num);
	  list[num - 1] = estrdup (s);
	  break;
	}
    }

  feh_parse_option_array (num, list);

  for (i = 0; i < num; i++)
    if (list[i])
      free (list[i]);
  if (list)
    free (list);
  D_RETURN_;
}

static void
feh_parse_option_array (int argc, char **argv)
{
  static char stropts[] =
    "a:Ab:BcC:dD:f:FghH:iIklLmo:O:pPqrR:sS:tTuUvVwW:xX:y:z:";
  static struct option lopts[] = {
    /* actions and macros */
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {"booth", 0, 0, 'B'},
    /* toggles */
    {"montage", 0, 0, 'm'},
    {"collage", 0, 0, 'g'},
    {"index", 0, 0, 'i'},
    {"fullindex", 0, 0, 'I'},
    {"thumbs", 0, 0, 't'},
    {"verbose", 0, 0, 'V'},
    {"borderless", 0, 0, 'x'},
    {"keep-http", 0, 0, 'k'},
    {"stretch", 0, 0, 's'},
    {"multiwindow", 0, 0, 'w'},
    {"recursive", 0, 0, 'r'},
    {"randomize", 0, 0, 'c'},
    {"full-screen", 0, 0, 'F'},
    {"noprogressive", 0, 0, 'P'},
    {"ignoreaspect", 0, 0, 'A'},
    {"draw_filename", 0, 0, 'd'},
    {"list", 0, 0, 'l'},
    {"longlist", 0, 0, 'L'},
    {"quiet", 0, 0, 'q'},
    {"preload", 0, 0, 'p'},
    {"loadables", 0, 0, 'U'},
    {"unloadables", 0, 0, 'u'},
    /* options with values */
    {"output", 1, 0, 'o'},
    {"action", 1, 0, 'X'},
    {"output-only", 1, 0, 'O'},
    {"font", 1, 0, 'f'},
    {"title-font", 1, 0, 'T'},
    {"bg", 1, 0, 'b'},
    {"limit-width", 1, 0, 'W'},
    {"limit-height", 1, 0, 'H'},
    {"thumb-width", 1, 0, 'y'},
    {"thumb-height", 1, 0, 'z'},
    {"slideshow-delay", 1, 0, 'D'},
    {"reload", 1, 0, 'R'},
    {"alpha", 1, 0, 'a'},
    {"sort", 1, 0, 'S'},
    {"config", 1, 0, 'C'},
    {"fontpath", 1, 0, '='},
    {0, 0, 0, 0}
  };
  int optch = 0, cmdx = 0;

  D_ENTER;

  /* Now to pass some optionarinos */
  while ((optch = getopt_long (argc, argv, stropts, lopts, &cmdx)) != EOF)
    {
      D (("Got option, getopt calls it %d, or %c\n", optch, optch));
      switch (optch)
	{
	case 0:
	  break;
	case 'h':
	  show_usage ();
	  break;
	case 'v':
	  show_version ();
	  break;
	case 'm':
	  opt.montage = 1;
	  break;
	case 'g':
	  opt.collage = 1;
	  break;
	case 'i':
	  opt.index = 1;
	  opt.index_show_name = 1;
	  opt.index_show_size = 0;
	  opt.index_show_dim = 0;
	  break;
	case 'I':
	  opt.index = 1;
	  opt.index_show_name = 1;
	  opt.index_show_size = 1;
	  opt.index_show_dim = 1;
	  break;
	case 'l':
	  opt.list = 1;
	  break;
	case 'L':
	  opt.longlist = 1;
	  break;
	case 't':
	  opt.thumbs = 1;
	  break;
	case 'V':
	  opt.verbose = 1;
	  break;
	case 'q':
	  opt.quiet = 1;
	  break;
	case 'x':
	  opt.borderless = 1;
	  break;
	case 'k':
	  opt.keep_http = 1;
	  break;
	case 's':
	  opt.stretch = 1;
	  break;
	case 'w':
	  opt.multiwindow = 1;
	  break;
	case 'r':
	  opt.recursive = 1;
	  break;
	case 'c':
	  opt.randomize = 1;
	  break;
	case 'd':
	  opt.draw_filename = 1;
	  break;
	case 'F':
	  opt.full_screen = 1;
	  break;
	case 'U':
	  opt.loadables = 1;
	  break;
	case 'u':
	  opt.unloadables = 1;
	  break;
	case 'p':
	  opt.preload = 1;
	  break;
	case 'P':
	  opt.progressive = 0;
	  break;
	case 'A':
	  opt.aspect = 0;
	  break;
	case 'S':
	  if (!strcasecmp (optarg, "name"))
	    opt.sort = SORT_NAME;
	  else if (!strcasecmp (optarg, "filename"))
	    opt.sort = SORT_FILENAME;
	  else if (!strcasecmp (optarg, "width"))
	    opt.sort = SORT_WIDTH;
	  else if (!strcasecmp (optarg, "height"))
	    opt.sort = SORT_HEIGHT;
	  else if (!strcasecmp (optarg, "pixels"))
	    opt.sort = SORT_PIXELS;
	  else if (!strcasecmp (optarg, "size"))
	    opt.sort = SORT_SIZE;
	  else if (!strcasecmp (optarg, "format"))
	    opt.sort = SORT_FORMAT;
	  else if (!strcasecmp (optarg, "pixles"))
	    opt.sort = SORT_PIXELS;
	  else
	    {
	      weprintf
		("Unrecognised sort mode \"%s\". Defaulting to sort by filename",
		 optarg);
	      opt.sort = SORT_FILENAME;
	    }
	  break;
	case 'B':
	  opt.full_screen = 1;
	  opt.slideshow_delay = 20;
	  break;
	case 'o':
	  opt.output = 1;
	  opt.output_file = estrdup (optarg);
	  break;
	case 'O':
	  opt.output = 1;
	  opt.output_file = estrdup (optarg);
	  opt.display = 0;
	  break;
	case 'C':
	  theme = estrdup (optarg);
	  break;
	case '=':
	  opt.fontpath = estrdup (optarg);
	  break;
	case 'f':
	  opt.font = estrdup (optarg);
	  break;
	case 'T':
	  opt.title_font = estrdup (optarg);
	  break;
	case 'b':
	  opt.bg = 1;
	  opt.bg_file = estrdup (optarg);
	  break;
	case 'X':
	  opt.action = estrdup (optarg);
	  break;
	case 'W':
	  opt.limit_w = atoi (optarg);
	  break;
	case 'H':
	  opt.limit_h = atoi (optarg);
	  break;
	case 'y':
	  opt.thumb_w = atoi (optarg);
	  break;
	case 'z':
	  opt.thumb_h = atoi (optarg);
	  break;
	case 'D':
	  opt.slideshow_delay = atoi (optarg);
	  break;
	case 'R':
	  opt.reload = atoi (optarg);
	  break;
	case 'a':
	  opt.alpha = 1;
	  opt.alpha_level = atoi (optarg);
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
	  /* If recursive is NOT set, but the only argument is a
	   * directory name, we grab all the files in there, but not
	   * subdirs */
	  add_file_to_filelist_recursively (argv[optind++], FILELIST_FIRST);
	}
    }

  /* So that we can safely be called again */
  optind = 1;
  D_RETURN_;
}


static void
check_options (void)
{
  D_ENTER;
  if ((opt.montage + opt.index + opt.collage) > 1)
    {
      weprintf
	("you can't use montage mode, collage mode or index mode together.\n"
	 "   I'm going with index");
      opt.montage = 0;
      opt.collage = 0;
    }

  if (!(opt.montage || opt.index))
    {
      if (opt.font || opt.title_font)
	{
	  weprintf ("you can't use fonts without montage or index mode.\n"
		    "   The fonts you specified will be ignored");
	  opt.font = opt.title_font = NULL;
	}
    }

  if (opt.full_screen && opt.multiwindow)
    {
      weprintf
	("you shouldn't combine multiwindow mode with full-screen mode,\n"
	 "   Multiwindow mode has been disabled.");
      opt.multiwindow = 0;
    }

  if (opt.list
      && (opt.multiwindow || opt.montage || opt.index || opt.collage))
    {
      weprintf ("list mode can't be combined with other processing modes,\n"
		"   list mode disabled.");
      opt.list = 0;
    }

  if (opt.sort && opt.randomize)
    {
      weprintf ("You cant sort AND randomize the filelist...\n"
		"randomize mode has been unset\n");
      opt.randomize = 0;
    }

  if (opt.loadables && opt.unloadables)
    {
      weprintf ("You cant show loadables AND unloadables...\n"
		"you might as well use ls ;)\n"
		"loadables only will be shown\n");
      opt.unloadables = 0;
    }
  D_RETURN_;
}

void
show_version (void)
{
  printf (PACKAGE " version " VERSION "\n");
  exit (0);
}

void
show_mini_usage (void)
{
  fprintf (stdout,
	   PACKAGE
	   " - No loadable images specified.\nUse " PACKAGE
	   " --help for detailed usage information\n");
  exit (0);
}

void
show_usage (void)
{
  fprintf (stdout,
	   "Usage : "
	   PACKAGE
	   " [OPTION]... FILE...\n"
	   "  Where FILE is an imlib 2 readable image file.\n"
	   "  Multiple files are supported.\n"
	   "  Urls are supported. They must begin with http:// and you must have wget\n"
	   "  installed to download the files.\n"
	   "  Options can also be defined in the environment variable FEH_OPTIONS\n"
	   "  or in the feh configuration file. See man feh for more details\n"
	   "  -h, --help                display this help and exit\n"
	   "  -v, --version             output version information and exit\n"
	   "  -V, --verbose             output useful information, progress bars, etc\n"
	   "  -q, --quiet               Don't report non-fatal errors for failed loads\n"
	   "                            Verbose and quiet modes are not mutually exclusive,\n"
	   "                            the first controls informational messages, the\n"
	   "                            second only errors.\n"
	   "  -C THEME                  Load options from config file with name THEME\n"
	   "                            see man feh for more info\n"
	   "  -r, --recursive           Recursively expand any directories in FILE to\n"
	   "                            the content of those directories. (Take it easy)\n"
	   "  -c, --randomize           When viewing multiple files in a slideshow,\n"
	   "                            randomise the file list before displaying\n"
	   "  -p, --preload             Preload images. This doesn't mean hold them in\n"
	   "                            RAM, it means run through and eliminate unloadable\n"
	   "                            images first. Otherwise they will be removed as you\n"
	   "                            flick through.\n"
	   "  -F, --full-screen         Make the window fullscreen\n"
	   "  -w, --multiwindow         Disable slideshow mode. With this setting,\n"
	   "                            instead of opening multiple files in slideshow\n"
	   "                            mode, multiple windows will be opened.\n"
	   "  -x, --borderless          Create borderless windows\n"
	   "  -P, --noprogressive       Disable progressive loading and display of images\n"
	   "  -d, --draw-filename       Draw the filename at the top-left of the image\n"
	   "  -D, --slideshow-delay NUM For slideshow mode, specifies time delay (seconds)\n"
	   "                            between automatically changing slides.\n"
	   "  -R, --reload NUM          Use this option to tell feh to reload an image\n"
	   "                            after NUM seconds. Useful for viewing webcams\n"
	   "                            via http, or even on your local machine.\n"
	   "  -k, --keep-http           When viewing files using http, feh normally\n"
	   "                            deletes the local copies after viewing, or,\n"
	   "                            if caching, on exit. This option prevents this\n"
	   "                            so that you get to keep the local copies.\n"
	   "                            They will be in /tmp with \"feh\" in the name.\n"
	   "  -l, --list                Don't display images. Analyse them and display an\n"
	   "                            'ls' style listing. Useful in scripts hunt out\n"
	   "                            images of a certain size/resolution/type etc.\n"
	   "  -U, --loadable            Don't display images. Just print out their name\n"
	   "                            if imlib2 can successfully load them.\n"
	   "  -u, --unloadable          Don't display images. Just print out their name\n"
	   "                            if imlib2 can NOT successfully load them.\n"
	   "  -S, --sort SORT_TYPE      The file list may be sorted according to image\n"
	   "                            parameters. Allowed sort types are: name,\n"
	   "                            filename, width, height, pixels, size, format.\n"
	   "                            For sort modes other than name or filename, a\n"
	   "                            preload run will be necessary, causing a delay\n"
	   "                            proportional to the number of images in the list\n"
	   "  -X, --action ACTION       Specify a string as an action to perform when the\n"
	   "                            enter key is pressed in slideshow or multiwindow\n"
	   "                            modes. The action will be executed in a shell. Use\n"
	   "                            %%f to refer to the image filename, and %%n to refer\n"
	   "                            to it's name. Eg. -X \"mv %%f ~/images/%%n\"\n"
	   "                            In slideshow mode, the next image will be shown\n"
	   "                            after running the command, in multiwindow mode,\n"
	   "                            the window will be closed.\n"
	   "  -m, --montage             Enable montage mode. Montage mode creates a new\n"
	   "                            image consisting of a grid of thumbnails of the\n"
	   "                            images specified using FILE... When montage mode\n"
	   "                            is selected, certain other options become\n"
	   "                            available. See MONTAGE MODE OPTIONS\n"
	   "  -g, --collage             Same as montage mode, but the thumbnails are\n"
	   "                            distributed randomly. You must specify width and\n"
	   "                            height or supply a background image or both\n"
	   "  -i, --index               Enable Index mode. Index mode is similar to\n"
	   "                            montage mode, and accepts the same options. It\n"
	   "                            creates an index print of thumbails, printing the\n"
	   "                            images name beneath each thumbnail. Index mode\n"
	   "                            enables certain other options, see INDEX MODE\n"
	   "                            OPTIONS\n"
	   "  -I, --fullindex           Same as index mode, but below each thumbnail you\n"
	   "                            get image name, size and dimensions\n"
	   "  -B, --booth               Combines some options suitable for a nice\n"
	   "                            booth display mode. A fullscreen slideshow\n"
	   "                            with a slide change every 20 seconds...\n"
	   " MONTAGE MODE OPTIONS\n"
	   "  -A, --ignoreaspect        By default, the montage thumbnails will retain\n"
	   "                            their aspect ratios, while fitting in --thumb-width\n"
	   "                            and --thumb-height. This option will force them to\n"
	   "                            be the size set by --thumb-width and --thumb-height\n"
	   "                            This will prevent any whitespace in the final\n"
	   "                            montage\n"
	   "  -s, --stretch             Normally, if an image is smaller than the specified\n"
	   "                            thumbnail size, it will not be enlarged. If this\n"
	   "                            option is set, the image will be scaled up to fit\n"
	   "                            the thumnail size. (Aspect ratio will be maintained\n"
	   "                            unless --ignoreaspect is specified)\n"
	   "  -y, --thumb-width NUM     Set thumbnail width in pixels\n"
	   "  -z, --thumb-height NUM    Set thumbnail height in pixels\n"
	   "                            Thumbnails default to 20x20 pixels\n"
	   "  -W, --limit-width NUM     Limit the width of the montage in pixels\n"
	   "  -H, --limit-height NUM    Limit the height of the montage in pixels\n"
	   "                            These options can be used together (to define the\n"
	   "                            image size exactly), or separately. If only one is\n"
	   "                            specified, theother is calculated from the number\n"
	   "                            of files specified and the size of the thumbnails.\n"
	   "                            The default is to limit width to 800 pixels and\n"
	   "                            calculate the height\n"
	   "  -b, --bg FILE             Use FILE as a background for your montage. With\n"
	   "                            this option specified, the size of the montage will\n"
	   "                            default to the size of FILE if no size restrictions\n"
	   "                            are specified.\n"
	   "  -o FILE                   Save the created montage to FILE\n"
	   "  -O FILE                   Just save the created montage to FILE\n"
	   "                            WITHOUT displaying it (use in scripts)\n"
	   " INDEX MODE OPTIONS\n"
	   "  -f FONT                   Use FONT to print the information under each\n"
	   "                            thumbnail. FONT should be defined in the form\n"
	   "                            fontname/size(points). eg -f myfont/12\n"
	   "     --fontpath PATH        Specify an extra directory to look in for fonts\n"
	   "  -T,--title-font FONT      Use FONT to print a title on the index, if no\n"
	   "                            font is specified, a title will not be printed\n"
	   " SLIDESHOW KEYS\n"
	   " The default mode for viewing mulitple images is Slideshow mode\n"
	   " When viewing a slideshow, the following keys may be used:\n"
	   " p, P, <BACKSPACE>, <LEFT>  Goto previous slide\n"
	   " n, N, <SPACE>, <RIGHT>     Goto next slide\n"
	   " <HOME>                     Goto first slide\n"
	   " <END>                      Goto last slide\n"
	   " <CTRL+DELETE>              Delete the file currently being viewed\n"
	   " q, Q                       Quit the slideshow\n"
	   "\n"
	   " MOUSE ACTIONS\n"
	   " When viewing an image, mouse button 1 moves to the next image (slideshow\n"
	   " mode only), button 2 zooms (click and drag left->right to zoom in, right->\n"
	   " left to zoom out, click once to restore 1x zoom), and mouse button 3 closes\n"
	   " all open windows and ends the application\n"
	   "\n"
	   "See 'man feh' for more detailed information\n"
	   "\n"
	   "This program is free software see the man page for licensing info.\n"
	   "Copyright Tom Gilbert, 1999\n"
	   "Email bugs to <gilbertt@btinternet.com>\n");
  exit (0);
}
