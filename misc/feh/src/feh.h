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
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>
#include <Imlib2.h>
#include "utils.h"

#if 1
#define D(a) { printf("%s +%u : ",__FILE__,__LINE__); \
               printf a; fflush(stdout); }
#else
#define D(a) { ; }
#endif

#define CHECK_SIZE 160

struct __thumbwidget
{
  Window win;
  char *filename;
  unsigned char hilited;
  unsigned char selected;
};
typedef struct __thumbwidget _thumbwidget;
typedef _thumbwidget *thumbwidget;

typedef struct __menuwidget _menuwidget;
typedef _menuwidget *menuwidget;
struct __menuwidget
{
  Window win;
  void (*action) (void *data, menuwidget menu);
  char *label;
  Imlib_Image *im;
  Pixmap pm;
  int w;
  int h;
  unsigned char hilited;
  unsigned char visible;
};

enum winwidget_type
{ WINWIDGET_SINGLE_IMAGE, WINWIDGET_MULITPLE_IMAGE, WINWIDGET_MONTAGE_IMAGE,
  WINWIDGET_INDEX_IMAGE
};

enum slide_change
{ SLIDE_NEXT, SLIDE_PREV, SLIDE_FIRST, SLIDE_LAST };

enum bg_type
{
  SCALE, CENTER, FIT, TILE
};

struct __winwidget
{
  Window win;
  int w;
  int h;
  int im_w;
  int im_h;
  unsigned char visible;
  unsigned char type;
  Imlib_Image *im;
  Pixmap bg_pmap;
  char *name;

  /* Stuff for zooming */
  int zoom_mode;
  int zx;
  int zy;
  double zoom;
  int timeout;

  /* hoss - rectangle drawing stuff */
  int rectangle_drawing_mode;
  int rec_x;
  int rec_y;
  int rec_w;
  int rec_h;
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
  unsigned char progressive;

  char *output_file;
  char *bg_file;
  char *font;
  char *title_font;

  int thumb_w;
  int thumb_h;
  int limit_w;
  int limit_h;
  int cur_slide;
}
fehoptions;

void show_usage (void);
void show_version (void);
void main_loop (void);
void init_x_and_imlib (void);
void init_multiwindow_mode (void);
void init_parse_options (int argc, char **argv);
void init_montage_mode (void);
void init_index_mode (void);
void init_slideshow_mode (void);
int feh_load_image (Imlib_Image ** im, char *filename);
void add_file_to_filelist_recursively (char *path, unsigned char enough);
void show_mini_usage (void);
void slideshow_change_image (winwidget winwid, int change);
char *slideshow_create_name (char *filename);
char *chop_file_from_full_path (char *str);
void handle_keypress_event (XEvent * ev, Window win);

int winwidget_loadimage (winwidget winwid, char *filename);
void winwidget_show (winwidget winwid);
void winwidget_hide (winwidget winwid);
void winwidget_destroy_all (void);
void winwidget_render_image (winwidget winwid);
void winwidget_update_title (winwidget ret);
winwidget winwidget_get_from_window (Window win);
winwidget winwidget_create_from_file (char *filename, char *name);
winwidget winwidget_create_from_image (Imlib_Image * im, char *name);
void winwidget_destroy (winwidget winwid);
void progress (Imlib_Image im, char percent, int update_x, int update_y,
	       int update_w, int update_h);
void winwidget_create_window (winwidget ret, int w, int h);
void winwidget_rerender_image (winwidget winwid);
void feh_draw_checks (winwidget win);


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
extern winwidget progwin;
extern int actual_file_num;
extern Imlib_Image *checks;
extern int rectangles_on;
extern Window root;
extern XContext xid_context;
