/* options.c
 *
 * Copyright (C) 1999 Tom Gilbert
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "feh.h"

static void check_options (void);

void
init_parse_options (int argc, char **argv)
{
  static char stropts[] = "a:AbBcdD:f:FhHikmo:O:PrR:stTvVwWxy:z:";
  static struct option lopts[] = {
    /* actions and macros */
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {"booth", 0, 0, 'B'},
    /* toggles */
    {"montage", 0, 0, 'm'},
    {"index", 0, 0, 'i'},
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
    /* options with values */
    {"output", 1, 0, 'o'},
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
    {0, 0, 0, 0}
  };
  int optch = 0, cmdx = 0;

  D (("In init_parse_options\n"));

  /* For setting the command hint */
  cmdargc = argc;
  cmdargv = argv;

  /* Set default options */
  memset (&opt, 0, sizeof (fehoptions));
  opt.display = 1;
  opt.aspect = 1;
  opt.progressive = 1;
  opt.thumb_w = 60;
  opt.thumb_h = 60;

  /* Now to pass some optionarinos */
  while ((optch = getopt_long (argc, argv, stropts, lopts, &cmdx)) != EOF)
    {
      switch (optch)
	{
	case 0:
	  printf ("FIXME: Long option without short version\n");
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
	case 'i':
	  opt.index = 1;
	  break;
	case 't':
	  opt.thumbs = 1;
	  break;
	case 'V':
	  opt.verbose = 1;
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
	case 'P':
	  opt.progressive = 0;
	  break;
	case 'A':
	  opt.aspect = 0;
	  break;
	case 'B':
	  opt.full_screen = 1;
	  opt.slideshow_delay = 20;
	  break;
	case 'o':
	  opt.output = 1;
	  opt.output_file = optarg;
	  break;
	case 'O':
	  opt.output = 1;
	  opt.output_file = optarg;
	  opt.display = 0;
	  break;
	case 'f':
	  opt.font = optarg;
	  break;
	case 'T':
	  opt.title_font = optarg;
	  break;
	case 'b':
	  opt.bg = 1;
	  opt.bg_file = optarg;
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
	  printf ("FIXME! Default case reached\n");
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
  else
    show_mini_usage ();

  if (opt.randomize)
    {
      /* Randomize the filename order */
      filelist = filelist_randomize (filelist);
    }
  else
    {
      /* Let's reverse the list. Its back-to-front right now ;) */
      filelist = filelist_reverse (filelist);
    }

  check_options ();
}

static void
check_options (void)
{
  if (opt.montage && opt.index)
    {
      weprintf ("you can't use montage mode and index mode together.\n"
		"   Montage mode has been disabled");
      opt.montage = 0;
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

  if ( 0 && opt.draw_filename && !opt.full_screen)
    {
      weprintf
	("Filename drawing is reserved for full screen mode.");
      opt.draw_filename = 0;
    }
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
	   " - No loadable images specified. Use " PACKAGE
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
	   "  -h, --help                display this help and exit\n"
	   "  -v, --version             output version information and exit\n"
	   "  -V, --verbose             output useful information, progress bars, etc\n"
	   "  -r, --recursive           Recursively expand any directories in FILE to\n"
	   "                            the content of those directories. (Take it easy)\n"
	   "  -c, --randomize           When viewing multiple files in a slideshow,\n"
	   "                            randomise the file list before displaying\n"
	   "  -F, --full-screen         Make the window fullscreen\n"
	   "  -d, --draw-filename       In fullscreen mode, draw the filename top-right\n"
	   "  -w, --multiwindow         Disable slideshow mode. With this setting,\n"
	   "                            instead of opening multiple files in slideshow\n"
	   "                            mode, multiple windows will be opened.\n"
	   "  -x, --borderless          Create borderless windows\n"
	   "  -P, --noprogressive       Disable progressive loading and display of images\n"
	   "  -D, --slideshow-delay NUM For slideshow mode, specifies time delay (seconds)\n"
	   "                            between automatically changing slides.\n"
	   "  -R, --reload NUM          Use this option to tell feh to reload an image\n"
	   "                            after NUM seconds. Useful for viewing webcams\n"
	   "                            view http, or even on your local machine.\n"
	   "  -k, --keep-http           When viewing files using http, feh normally\n"
	   "                            deletes the local copies after viewing, or,\n"
	   "                            if caching, on exit. This option prevents this\n"
	   "                            so that you get to keep the local copies.\n"
	   "                            They will be in /tmp with \"feh\" in the name.\n"
	   "  -m, --montage             Enable montage mode. Montage mode creates a new\n"
	   "                            image consisting of a grid of thumbnails of the\n"
	   "                            images specified using FILE... When montage mode\n"
	   "                            is selected, certain other options become\n"
	   "                            available. See MONTAGE MODE OPTIONS\n"
	   "  -i, --index               Enable Index mode. Index mode is similar to\n"
	   "                            montage mode, and accepts the same options. It\n"
	   "                            creates an index print of thumbails, printing user-\n"
	   "                            defined information beneath each thumbnail. Index\n"
	   "                            mode enables certain other options, see INDEX MODE\n"
	   "                            OPTIONS\n"
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
	   "This program is free software.\n"
	   "Distributed under the GNU public license.\n"
	   "Copyright Tom Gilbert, 1999\n"
	   "Email bugs to <feh@tomgilbert.freeserve.co.uk>\n");
  exit (0);
}
