#include <gdk/gdk.h>
#include <gdk_imlib.h>
#include <unistd.h>
#include <stdlib.h>


#define ATTRIBUTES_MASK (GDK_WA_X | GDK_WA_Y)
#define VERSION "0.1"

#ifndef HAVE_RANDOM
extern long random();
extern int srandom(unsigned);
#endif

extern int filter;
extern int image_error;
extern int first;
extern GdkEventMask event_mask;
extern int jumping;
extern char jcmd[100];
extern int jidx;
extern GdkWindow *parent;
extern gint w, h, win_x, win_y;
extern GdkCursor *cursor;
extern GdkWindowAttr attr;
extern GMainLoop *MainLoop;
extern GdkWindow *win;
extern GdkPixmap **p;
extern GdkImlibImage *im;
extern GdkImlibColorModifier mod;
extern gint org_w, org_h;
extern gint screen_x, screen_y;
extern GdkEvent *ev;
extern GdkGC *gc;
extern GdkColor color_blue;
extern GdkColor color_bg;
extern GdkColor text_background_color;
extern int images;
extern char **image_names;
extern int image_idx;
extern char *color_names[], *color_numbers[], *image_extensions[];

extern gint center, wm_handle;
extern gint user_bg_red;
extern gint user_bg_green;
extern gint user_bg_blue;
extern gint default_brightness;
extern gint default_contrast;
extern gint default_gamma;
extern gint delay;
extern int random_order;
extern int fullscreen;
extern int maxpect;
extern int statusbar;
extern int slide;
extern int scale_down;
extern int to_root;
extern int to_root_t;
extern int to_root_s;
extern int transparency;
extern int rotate_press;
extern int return_press;
extern int do_grab;
extern int bg_set;

/* main.c */

void eplay_exit(int);
void eplay_load_image();
void eplay_handle_timer(gpointer);
void eplay_handle_event(GdkEvent *);

/* options.c */

int options_read(int, char **);

/* utils.c */

int move2trash(char *);
void jump2image(char *);
void finish(int);
void usage(char *, int);
void show_help(char *, int);
void set_bg_color(char *);

/* image.c */

void eplay_load_image();
void set_desktop_image();
void zoom_in(gint *, gint *, gint *, gint *);
void zoom_out(gint *, gint *, gint *, gint *);
void zoom_maxpect(gint *, gint *, gint *, gint *);
void reset_display_settings(gint *, gint *, gint *, gint *);
void update_image(char, gint, gint, gint, gint, GdkImlibColorModifier);

/* filter.c */

void filter_images(int *, char **);
int check_image(char *);
