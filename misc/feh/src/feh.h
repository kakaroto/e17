/* feh.h
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


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "Imlib2.h"

#if 0
#define D(a) { printf("%s +%u : ",__FILE__,__LINE__); \
               printf a; fflush(stdout); }
#else
#define D(a) { /* No debug */; }
#endif

struct __thumbwidget
{
  Window win;
  char *filename;
  unsigned char hilited;
  unsigned char selected;
};
typedef struct __thumbwidget _thumbwidget;
typedef _thumbwidget *thumbwidget;

enum
{ WINWIDGET_SINGLE_IMAGE, WINWIDGET_MULITPLE_IMAGE, WINWIDGET_MONTAGE_IMAGE };

struct __winwidget
{
  Window win;
  int w;
  int h;
  unsigned char visible;
  unsigned char type;
  Imlib_Image *im;
  Pixmap bg_pmap;
  char *name;
};
typedef struct __winwidget _winwidget;
typedef _winwidget *winwidget;

typedef struct cmdlineoptions
{
  unsigned char multiwindow;
  unsigned char montage;
  unsigned char index;
  unsigned char thumbs;
  unsigned char slideshow;
  unsigned char recursive;
  unsigned char output;
  unsigned char verbose;
  unsigned char display;
  unsigned char bg;
  unsigned char alpha;
  unsigned char alpha_level;
  unsigned char aspect;
  unsigned char stretch;

  char *output_file;
  char *bg_file;

  int thumb_w;
  int thumb_h;
  int limit_w;
  int limit_h;
}
fehoptions;

void show_usage (void);
void show_version (void);
void main_loop (void);
winwidget winwidget_create_from_file (char *filename);
int winwidget_loadimage (winwidget winwid, char *filename);
void winwidget_show (winwidget winwid);
void winwidget_hide (winwidget winwid);
void init_x_and_imlib (void);
winwidget winwidget_get_from_window (Window win);
void winwidget_destroy (winwidget winwid);
void init_multiwindow_mode (void);
void init_parse_options (int argc, char **argv);
winwidget winwidget_create_from_image (Imlib_Image * im);
void init_montage_mode (void);
int feh_load_image (Imlib_Image ** im, char *filename);
void add_file_to_filelist_recursively (char *path);
void show_mini_usage (void);
void winwidget_destroy_all (void);

/* Imlib stuff */
extern Display *disp;
extern Visual *vis;
extern Colormap cm;
extern int depth;
extern Atom wmDeleteWindow;
/* Thumbnail sizes */
extern int window_num;		/* For window list */
extern winwidget *windows;	/* List of windows to loop though */
extern int cmdargc;
extern char **cmdargv;
extern fehoptions opt;
extern int file_num;
extern char **files;
