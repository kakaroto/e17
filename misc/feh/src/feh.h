/* feh.h

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

#ifndef FEH_H
#define FEH_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/wait.h>
#include <Imlib2.h>

#include "config.h"
#include "structs.h"
#include "feh_imlib2.h"
#include "menu.h"

#include "utils.h"
#include "getopt.h"


#include "debug.h"

#define SLIDESHOW_RELOAD_MAX 4096


#ifndef __GNUC__
# define __attribute__(x)
#endif

#define XY_IN_RECT(x, y, rx, ry, rw, rh) \
(((x) >= (rx)) && ((y) >= (ry)) && ((x) < ((rx) + (rw))) && ((y) < ((ry) + (rh))))

#define DEFAULT_FONT "helmetr/10"
#define DEFAULT_MENU_FONT "helmetr/10"
#define DEFAULT_FONT_BIG "helmetr/14"
#define DEFAULT_FONT_TITLE "helmetr/22"

enum mode_type
{ MODE_NORMAL = 0, MODE_PAN, MODE_ZOOM, MODE_ROTATE, MODE_BLUR, MODE_NEXT };

enum bgmode_type
{ BG_MODE_NONE = 0, BG_MODE_TILE, BG_MODE_CENTER, BG_MODE_SEAMLESS, BG_MODE_SCALE };

enum slide_change
{ SLIDE_NEXT, SLIDE_PREV, SLIDE_FIRST, SLIDE_LAST, SLIDE_JUMP_FWD,
   SLIDE_JUMP_BACK
};
enum direction
{ FORWARD, BACK };

typedef void (*sighandler_t) (int);

void show_usage(void);
void show_version(void);
int feh_main_iteration(int block);
void feh_handle_event(XEvent * ev);
void init_x_and_imlib(void);
void init_multiwindow_mode(void);
void init_thumbnail_mode(void);
void init_collage_mode(void);
void init_index_mode(void);
void init_slideshow_mode(void);
void init_list_mode(void);
void init_loadables_mode(void);
void init_unloadables_mode(void);
void feh_clean_exit(void);
int feh_load_image(Imlib_Image * im, feh_file * file);
void show_mini_usage(void);
void slideshow_change_image(winwidget winwid, int change);
void slideshow_pause_toggle(winwidget w);
char *slideshow_create_name(feh_file * file);
char *chop_file_from_full_path(char *str);
void handle_keypress_event(XEvent * ev, Window win);
void feh_action_run(feh_file * file);
char *feh_printf(char *str, feh_file * file);
void feh_draw_zoom(winwidget w);

void feh_draw_checks(winwidget win);
void cb_slide_timer(void *data);
void cb_reload_timer(void *data);
char *feh_http_load_image(char *url);
int feh_load_image_char(Imlib_Image * im, char *filename);
void feh_draw_filename(winwidget w);
void feh_display_status(char stat);
void real_loadables_mode(int loadable);
void feh_reload_image(winwidget w, int resize);
void feh_filelist_image_remove(winwidget winwid, char do_delete);
char *feh_strip_hostname(char *url);
struct hostent *feh_gethostbyname(const char *name);
void slideshow_save_image(winwidget win);
void feh_edit_inplace_orient(winwidget w, int orientation);



/* Imlib stuff */
extern Display *disp;
extern Visual *vis;
extern Colormap cm;
extern int depth;
extern Atom wmDeleteWindow;

/* Thumbnail sizes */
extern int cmdargc;
extern char **cmdargv;
extern winwidget progwin;
extern Window root;
extern XContext xid_context;
extern Screen *scr;
extern unsigned char reset_output;
extern int call_level;
extern feh_menu *menu_main;
extern feh_menu *menu_close;
extern char *mode; /* label for the current mode */

#endif
