/* feh.h
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
#include <sys/wait.h>
#include <Imlib2.h>
#include "utils.h"
#include "getopt.h"

#if 0
#ifdef __GNUC__
#define D(a) \
  { \
      printf("%s +%u %s()     ",__FILE__,__LINE__,__FUNCTION__); \
      printf a; \
      fflush(stdout); \
  }
#define D_ENTER \
  { \
      printf("%s +%u %s() >>> Entering\n",__FILE__,__LINE__,__FUNCTION__); \
      fflush(stdout); \
  }
#define D_RETURN(a) \
  { \
      printf("%s +%u %s() <<< Leaving\n",__FILE__,__LINE__,__FUNCTION__); \
      fflush(stdout); \
      return (a); \
  }
#define D_RETURN_ \
  { \
      printf("%s +%u %s() <<< Leaving\n",__FILE__,__LINE__,__FUNCTION__); \
      fflush(stdout); \
      return; \
  }
#else
#define D(a) \
  { \
      printf("%s +%u : ",__FILE__,__LINE__); \
      printf a; \
      fflush(stdout); \
  }
#define D_ENTER
#define D_RETURN(a) \
  { \
      return(a); \
  }
#define D_RETURN_ \
  { \
      return; \
  }
#endif
#else
#define D(a)
#define D_ENTER
#define D_RETURN(a) \
  { \
      return (a); \
  }
#define D_RETURN_ \
  { \
      return; \
  }
#endif

#define CHECK_SIZE 160

#ifndef __GNUC__
# define __attribute__(x)
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

typedef struct __fehtimer _fehtimer;
typedef _fehtimer *fehtimer;
struct __fehtimer
{
  char *name;
  void (*func) (void *data);
  void *data;
  double in;
  char just_added;
  fehtimer next;
};

typedef struct __feh_file feh_file;
typedef struct __feh_file_info feh_file_info;

struct __feh_file
{
  char *filename;
  char *name;

  /* info stuff */
  feh_file_info *info;

  feh_file *next;
  feh_file *prev;
};

struct __feh_file_info
{
  int width;
  int height;
  int size;
  int pixels;
  unsigned char has_alpha;
  char *format;
  char *extension;
};

enum winwidget_type
{ WINWIDGET_SINGLE_IMAGE, WINWIDGET_MULITPLE_IMAGE, WINWIDGET_MONTAGE_IMAGE,
  WINWIDGET_INDEX_IMAGE
};

enum slide_change
{ SLIDE_NEXT, SLIDE_PREV, SLIDE_FIRST, SLIDE_LAST };

enum filelist_recuse
{ FILELIST_FIRST, FILELIST_CONTINUE, FILELIST_LAST };

enum sort_type
{ SORT_NONE, SORT_NAME, SORT_FILENAME, SORT_WIDTH, SORT_HEIGHT, SORT_PIXELS,
  SORT_SIZE, SORT_FORMAT
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
  GC gc;
  Pixmap bg_pmap;
  char *name;
  feh_file *file;

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
  unsigned char collage;
  unsigned char index;
  unsigned char index_show_name;
  unsigned char index_show_dim;
  unsigned char index_show_size;
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
  unsigned char keep_http;
  unsigned char borderless;
  unsigned char randomize;
  unsigned char full_screen;
  unsigned char draw_filename;
  unsigned char list;
  unsigned char longlist;
  unsigned char quiet;
  unsigned char preload;
  unsigned char loadables;
  unsigned char unloadables;
  unsigned char timeout;
  unsigned char zoom_mode;

  char *output_file;
  char *bg_file;
  char *font;
  char *title_font;
  char *action;
  char *fontpath;

  int thumb_w;
  int thumb_h;
  int limit_w;
  int limit_h;
  int slideshow_delay;
  int reload;
  int sort;
}
fehoptions;


void show_usage (void);
void show_version (void);
void main_loop (void);
void feh_handle_event (XEvent * ev);
void feh_smooth_image (winwidget w);
void init_x_and_imlib (void);
void init_multiwindow_mode (void);
void init_parse_options (int argc, char **argv);
void init_montage_mode (void);
void init_collage_mode (void);
void init_index_mode (void);
void init_slideshow_mode (void);
void init_list_mode (void);
void init_loadables_mode (void);
void init_unloadables_mode (void);
int feh_load_image (Imlib_Image ** im, feh_file * file);
void add_file_to_filelist_recursively (char *path, unsigned char level);
void feh_prepare_filelist (void);
void show_mini_usage (void);
void slideshow_change_image (winwidget winwid, int change);
char *slideshow_create_name (char *filename);
char *chop_file_from_full_path (char *str);
void handle_keypress_event (XEvent * ev, Window win);
void feh_action_run (winwidget w);
char *feh_printf (char *str, winwidget w);

int winwidget_loadimage (winwidget winwid, feh_file * filename);
void winwidget_show (winwidget winwid);
void winwidget_hide (winwidget winwid);
void winwidget_destroy_all (void);
void winwidget_render_image (winwidget winwid);
void winwidget_setup_pixmaps (winwidget winwid);
void winwidget_update_title (winwidget ret);
winwidget winwidget_get_from_window (Window win);
winwidget winwidget_create_from_file (feh_file * filename, char *name);
winwidget winwidget_create_from_image (Imlib_Image * im, char *name);
void winwidget_destroy (winwidget winwid);
void progressive_load_cb (Imlib_Image im, char percent, int update_x,
			  int update_y, int update_w, int update_h);
void winwidget_create_window (winwidget ret, int w, int h);
void feh_draw_checks (winwidget win);
void feh_handle_timer (void);
double feh_get_time (void);
void feh_remove_timer (char *name);
void feh_add_timer (void (*func) (void *data), void *data, double in,
		    char *name);
void feh_add_unique_timer (void (*func) (void *data), void *data, double in);
void cb_slide_timer (void *data);
void cb_reload_timer (void *data);
char *feh_http_load_image (char *url);
void add_file_to_rm_filelist (char *file);
void delete_rm_files (void);
int feh_load_image_char (Imlib_Image ** im, char *filename);
void feh_draw_filename (winwidget w);
void feh_display_status (char stat);
void real_loadables_mode (int loadable);


feh_file *filelist_addtofront (feh_file * root, feh_file * newfile);
feh_file *filelist_newitem (char *filename);
feh_file *filelist_remove_file (feh_file * list, feh_file * file);
void feh_file_free (feh_file * file);
int filelist_length (feh_file * file);
feh_file *filelist_last (feh_file * file);
feh_file *filelist_first (feh_file * file);
feh_file *feh_file_rm_and_free (feh_file * list, feh_file * file);
int filelist_num (feh_file * list, feh_file * file);
feh_file *filelist_reverse (feh_file * list);
feh_file *filelist_randomize (feh_file * list);
typedef int (feh_compare_fn) (feh_file * file1, feh_file * file2);
feh_file_info *feh_file_info_new (void);
void feh_file_info_free (feh_file_info * info);
int feh_file_info_load (feh_file * file);
feh_file *feh_list_sort (feh_file * list, feh_compare_fn cmp);
feh_file *feh_list_sort_merge (feh_file * l1,
			       feh_file * l2, feh_compare_fn cmp);
int feh_cmp_name (feh_file * file1, feh_file * file2);
int feh_cmp_filename (feh_file * file1, feh_file * file2);
int feh_cmp_width (feh_file * file1, feh_file * file2);
int feh_cmp_height (feh_file * file1, feh_file * file2);
int feh_cmp_pixels (feh_file * file1, feh_file * file2);
int feh_cmp_size (feh_file * file1, feh_file * file2);
int feh_cmp_format (feh_file * file1, feh_file * file2);
feh_file *feh_file_info_preload (feh_file * list);


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
extern winwidget progwin;
extern Imlib_Image *checks;
extern int rectangles_on;
extern Window root;
extern XContext xid_context;
extern fehtimer first_timer;
extern feh_file *filelist;
extern feh_file *current_file;
extern Screen *scr;
extern unsigned char reset_output;
