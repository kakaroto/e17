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

void
init_parse_options (int argc, char **argv)
{
  int i;

  D (("In init_parse_options\n"));

  /* For setting the command hint */
  cmdargc = argc;
  cmdargv = argv;

  /* Set default options */
  opt.multiwindow = 1;
  opt.montage = 0;
  opt.index = 0;
  opt.thumbs = 0;
  opt.slideshow = 0;
  opt.recursive = 0;
  opt.output = 0;
  opt.output_file = NULL;
  opt.limit_w = 0;
  opt.limit_h = 0;
  opt.verbose = 0;
  opt.display = 1;
  opt.bg = 0;
  opt.bg_file = NULL;
  opt.aspect = 1;
  opt.alpha = 0;
  opt.alpha_level = 0;
  opt.stretch = 0;
  opt.font = NULL;
  opt.title_font = NULL;

  opt.thumb_w = 60;
  opt.thumb_h = 60;
  opt.cur_slide = 0;

  for (i = 1; i < argc; i++)
    {
      if ((!strcmp (argv[i], "--help")) || (!strcmp (argv[i], "-h")))
	show_usage ();
      else if ((!strcmp (argv[i], "--version")) || (!strcmp (argv[i], "-v")))
	show_version ();
      else if ((!strcmp (argv[i], "--montage")) || (!strcmp (argv[i], "-m")))
	opt.montage = 1;
      else if ((!strcmp (argv[i], "--index")) || (!strcmp (argv[i], "-i")))
	opt.index = 1;
      else if ((!strcmp (argv[i], "--thumbs")) || (!strcmp (argv[i], "-t")))
	opt.thumbs = 1;
      else if ((!strcmp (argv[i], "--verbose")) || (!strcmp (argv[i], "-V")))
	opt.verbose = 1;
      else if ((!strcmp (argv[i], "--stretch")) || (!strcmp (argv[i], "-s")))
	opt.stretch = 1;
      else if ((!strcmp (argv[i], "--ignoreaspect"))
	       || (!strcmp (argv[i], "-A")))
	opt.aspect = 0;
      else if ((!strcmp (argv[i], "--slideshow"))
	       || (!strcmp (argv[i], "-S")))
	opt.slideshow = 1;
      else if ((!strcmp (argv[i], "--recursive"))
	       || (!strcmp (argv[i], "-r")))
	opt.recursive = 1;
      else if ((!strcmp (argv[i], "-o")) && (argc - i > 1))
	{
	  opt.output = 1;
	  opt.output_file = argv[++i];
	}
      else if ((!strcmp (argv[i], "-O")) && (argc - i > 1))
	{
	  opt.output = 1;
	  opt.output_file = argv[++i];
	  opt.display = 0;
	}
      else if ((!strcmp (argv[i], "-f")) && (argc - i > 1))
	{
	  opt.font = argv[++i];
	}
      else if ((!strcmp (argv[i], "--title-font")) && (argc - i > 1))
	{
	  opt.title_font = argv[++i];
	}
      else if ((!strcmp (argv[i], "--bg")) && (argc - i > 1))
	{
	  opt.bg = 1;
	  opt.bg_file = argv[++i];
	}
      else if ((!strcmp (argv[i], "--limit-width")) && (argc - i > 1))
	{
	  opt.limit_w = atoi (argv[++i]);
	}
      else if ((!strcmp (argv[i], "--limit-height")) && (argc - i > 1))
	{
	  opt.limit_h = atoi (argv[++i]);
	}
      else if ((!strcmp (argv[i], "--thumb-width")) && (argc - i > 1))
	{
	  opt.thumb_w = atoi (argv[++i]);
	}
      else if ((!strcmp (argv[i], "--thumb-height")) && (argc - i > 1))
	{
	  opt.thumb_h = atoi (argv[++i]);
	}
      else if ((!strcmp (argv[i], "--alpha")) && (argc - i > 1))
	{
	  opt.alpha = 1;
	  opt.alpha_level = atoi (argv[++i]);
	}
      else
	{
	  /* If recursive is NOT set, but the only argument is a
	   * directory name, we grab all the files in there, but not
	   * subdirs */
	  add_file_to_filelist_recursively (argv[i], 0);
	}
    }
  if (file_num == 0)
    show_mini_usage ();
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
	   "  -h, --help                display this help and exit\n"
	   "  -v, --version             output version information and exit\n"
	   "  -V, --verbose             output useful information, progress bars, etc\n"
	   "  -r, --recursive           Recursively expand any directories in FILE to\n"
	   "                            the content of those directories. (Take it easy)\n"
	   "  -S, --slideshow           Enable slideshow mode. With this setting, instead\n"
	   "                            of opening multiple windows for multiple image\n"
	   "                            files, one window will be opened. Press mouse\n"
	   "                            button one to flick through the images.\n"
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
	   "      --thumb-width NUM     Set thumbnail width in pixels\n"
	   "      --thumb-height NUM    Set thumbnail height in pixels\n"
	   "                            Thumbnails default to 20x20 pixels\n"
	   "      --limit-width NUM     Limit the width of the montage in pixels\n"
	   "      --limit-height NUM    Limit the height of the montage in pixels\n"
	   "                            These options can be used together (to define the\n"
	   "                            image size exactly), or separately. If only one is\n"
	   "                            specified, theother is calculated from the number\n"
	   "                            of files specified and the size of the thumbnails.\n"
	   "                            The default is to limit width to 800 pixels and\n"
	   "                            calculate the height\n"
	   "      --bg FILE             Use FILE as a background for your montage. With\n"
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
	   "     --title-font FONT      Use FONT to print a title on the index, if no\n"
	   "                            font is specified, a title will not be printed\n"
	   "\n"
	   "This program is free software.\n"
	   "Distributed under the GNU public license.\n"
	   "Copyright Tom Gilbert, 1999\n"
	   "Email bugs to <feh@tomgilbert.freeserve.co.uk>\n");
  exit (0);
}
