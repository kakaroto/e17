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
#include <sys/wait.h>
#include <Imlib2.h>
#include "utils.h"
#include "getopt.h"

#if 0
#define D(a) { printf("%s +%u : ",__FILE__,__LINE__); \
               printf a; fflush(stdout); }
#else
#define D(a) { ; }
#endif

#define CHECK_SIZE 160

#define SWAP(a, b)  ((int)(a) ^= (int)(b) ^= (int)(a) ^= (int)(b))

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
    SORT_SIZE, SORT_FORMAT };

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
  unsigned char multiwindow:1;
  unsigned char montage:1;
  unsigned char index:1;
  unsigned char thumbs:1;
  unsigned char slideshow:1;
  unsigned char recursive:1;
  unsigned char output:1;
  unsigned char verbose:1;
  unsigned char display:1;
  unsigned char bg:1;
  unsigned char alpha:1;
  unsigned char alpha_level:1;
  unsigned char aspect:1;
  unsigned char stretch:1;
  unsigned char progressive:1;
  unsigned char keep_http:1;
  unsigned char borderless:1;
  unsigned char randomize:1;
  unsigned char full_screen:1;
  unsigned char draw_filename:1;
  unsigned char list:1;
  unsigned char longlist:1;
  unsigned char quiet:1;
  unsigned char preload:1;

  char *output_file;
  char *bg_file;
  char *font;
  char *title_font;

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
void init_x_and_imlib (void);
void init_multiwindow_mode (void);
void init_parse_options (int argc, char **argv);
void init_montage_mode (void);
void init_index_mode (void);
void init_slideshow_mode (void);
void init_list_mode (void);
int feh_load_image (Imlib_Image ** im, feh_file *file);
void add_file_to_filelist_recursively (char *path, unsigned char level);
void feh_prepare_filelist (void);
void show_mini_usage (void);
void slideshow_change_image (winwidget winwid, int change);
char *slideshow_create_name (char *filename);
char *chop_file_from_full_path (char *str);
void handle_keypress_event (XEvent * ev, Window win);

int winwidget_loadimage (winwidget winwid, feh_file *filename);
void winwidget_show (winwidget winwid);
void winwidget_hide (winwidget winwid);
void winwidget_destroy_all (void);
void winwidget_render_image (winwidget winwid);
void winwidget_update_title (winwidget ret);
winwidget winwidget_get_from_window (Window win);
winwidget winwidget_create_from_file (feh_file *filename, char *name);
winwidget winwidget_create_from_image (Imlib_Image * im, char *name);
void winwidget_destroy (winwidget winwid);
void progress (Imlib_Image im, char percent, int update_x, int update_y,
	       int update_w, int update_h);
void winwidget_create_window (winwidget ret, int w, int h);
void winwidget_rerender_image (winwidget winwid);
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


feh_file *filelist_addtofront (feh_file *root, feh_file *newfile);
feh_file *filelist_newitem (char *filename);
feh_file *filelist_remove_file (feh_file *list, feh_file *file);
void feh_file_free (feh_file *file);
int filelist_length (feh_file *file);
feh_file *filelist_last (feh_file *file);
feh_file *filelist_first (feh_file *file);
feh_file *feh_file_rm_and_free (feh_file *list, feh_file *file);
int filelist_num (feh_file *list, feh_file *file);
feh_file *filelist_reverse (feh_file *list);
feh_file *filelist_randomize (feh_file *list);
typedef int (feh_compare_fn)(feh_file *file1, feh_file *file2);
feh_file_info *feh_file_info_new(void);
void feh_file_info_free(feh_file_info *info);
int feh_file_info_load (feh_file *file);
feh_file *feh_list_sort (feh_file *list, feh_compare_fn cmp);
feh_file *feh_list_sort_merge (feh_file *l1,
                                    feh_file *l2, feh_compare_fn cmp);
int feh_cmp_name (feh_file *file1, feh_file *file2); 
int feh_cmp_filename (feh_file *file1, feh_file *file2); 
int feh_cmp_width (feh_file *file1, feh_file *file2); 
int feh_cmp_height (feh_file *file1, feh_file *file2); 
int feh_cmp_pixels (feh_file *file1, feh_file *file2); 
int feh_cmp_size (feh_file *file1, feh_file *file2); 
int feh_cmp_format (feh_file *file1, feh_file *file2); 
feh_file *feh_file_info_preload(feh_file *list);


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
extern int actual_file_num;
extern Imlib_Image *checks;
extern int rectangles_on;
extern Window root;
extern XContext xid_context;
extern fehtimer first_timer;
extern feh_file *filelist;
extern feh_file *current_file;
extern Screen *scr;
