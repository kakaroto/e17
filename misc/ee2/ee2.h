/* header file for Electric Eyes 2 */
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <Imlib2.h>
#include <gdk/gdkx.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <math.h>

#include "config.h"
#include "file.h"
#include "dnd.h"

/* Macros and #defines */
#define CHECKS 16
#define SPLASHSCREEN DATA_DIR "/ee2-alpha.png"
#define DEBUG 1
#ifdef DEBUG
# define D(x)  do {printf(__FILE__ ", line %d, " __FUNCTION__ "():  ", __LINE__); printf x;} while (0)
#else
# define D(x)  ((void) 0)
#endif

/* global widgets */
extern GtkWidget *MainWindow, *area, *RootMenu, *FileSel, *SaveSel, *ModWin,
  *ModList, *ModMdi, *BrWin, *BrClist, *area2, *infol;

/* Variables */
extern Imlib_Image *im, *bimg, *bg;
extern int imgw, imgh, winw, winh;
extern char currentimage[];

GdkPixmap *g_pixmap;
GdkBitmap *g_bitmap;

char cache[255];

/* stuff for nolan's mapping code */
typedef struct pixel_data
{
  int mappedx;
  int mappedy;
} mapped_pixels;
void sphere_map_image(GtkWidget *widget, GdkEvent *event, gpointer data);
mapped_pixels *calc_mapping(int im_width, int im_height, int x, int y);

/* functions */
void about_init(void);
void about_show(void);
void about_hide(void);

void browser_init(void);
void browser_show(void);
void browser_hide(void);

void webcam_init(void);
void webcam_show(void);
void webcam_hide(void);

void mod_init(void);
void mod_show(void);
void mod_hide(void);
void mod_cb(gpointer);
void mod_b(GtkWidget *widget, gint r, gint c, GdkEventButton *event, gpointer data);

void fx_init(void);
void fx_blur(void);
void fx_sharpen(void);

void redm_init(void);

void grm_init(void);

void blm_init(void);

void am_init(void);

void brite_init(void);
void brite_cha(GtkAdjustment *a);
void brite_chb(GtkAdjustment *a);
void brite_b(void);
void brite_c(void);

void scale_init(void);

void check_cache(void);
int check_cache_file(char *cache_file);

void AddList(char *foo);

void menus_init(void);

void ee2_init(int, char **);
void LoadImage(char *imagetoload);
void DrawChecks(void);
void Checks(int image_h, int image_w);
void DrawImage(Imlib_Image * im, int w, int h);
void prev_draw(Imlib_Image *im, int w, int h);
void check_callback(GtkWidget * widget, gpointer data);
void CloseWindow(GtkWidget * widget, gpointer data);
void CloseFileSel(GtkWidget * widget, gpointer data);
void FileOpen(GtkWidget * widget, GtkFileSelection * fs);
void SaveImage(GtkWidget * widget, gpointer data);
void CloseSaveSel(GtkWidget * widget, gpointer data);
void SaveImageAs(GtkWidget * widget, GtkFileSelection * fs);
gint ButtonPressed(GtkWidget * widget, GdkEvent * event, gpointer data);
gboolean a_config(GtkWidget * widget, GdkEventConfigure * event, gpointer user_data);
gboolean b_config(GtkWidget * widget, GdkEventConfigure * event, gpointer user_data);
void browser_sel(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data);
