
#ifndef MAIN_H
#define MAIN_H

int filter = 1;
int image_error;				/* 1 if Imlib couldn't load image */
int first = 1;					/* TRUE iff this is first image */
GdkEventMask event_mask = GDK_BUTTON_RELEASE_MASK;
int jumping;
char jcmd[100];
int jidx;
GdkWindow *parent;
gint w, h, win_x, win_y;
GdkCursor *cursor;
GdkWindowAttr attr;
GMainLoop *MainLoop;
GdkWindow *win;					/* Main window for windowed and fullscreen mode */
GdkPixmap **p;					/* Pixmap of the image to display */
GdkImlibImage *im;				/* Image */
GdkImlibColorModifier mod;		/* Image modifier (for brightness..) */
gint org_w, org_h;				/* Size of original image in pixels */
gint screen_x, screen_y;		/* Size of the screen in pixels */
GdkEvent *ev;					/* Event handler */
GdkGC *gc;						/* Background GC (black), also for statusbar font */
GdkColor color_blue;			/* for the error window/screen */
GdkColor color_bg;				/* default background */
GdkColor text_background_color;
int images;						/* Number of images in current collection */
char **image_names;				/* Filenames of the images */
int image_idx;					/* Index of current image displayed. 0 = 1st image */

gint center = CENTER;
gint user_bg_red;				/* default black */
gint user_bg_green;
gint user_bg_blue;
gint default_brightness = 256;
gint default_contrast = 256;
gint default_gamma = 256;
gint delay = 51;				/* delay in slideshow mode in seconds */
int random_order;				/* TRUE if random delay in slideshow */
int fullscreen;					/* TRUE if fullscreen mode */
int maxpect;					/* TRUE if autozoom (fit-to-screen) mode */
int statusbar = 1;				/* TRUE if statusbar is turned on (default) */
int slide;						/* 1=slide show running */
int scale_down;					/* resize down if image x/y > screen */
int to_root;					/* display on root (centered) */
int to_root_t;					/* display on root (tiled) */
int to_root_s;					/* display on root (stretched) */
int transparency;				/* transparency on/off */
int rotate_press;				/* image been rotated */
int return_press;				/* return pressed */
int do_grab;					/* grab keboard/pointer (default off) */
int bg_set;

char *color_names[] =
{
	"white", "black", "green",
	"blue", "red", "magenta",
	"cyan", "yellow", "pink",
	"purple", "brown", "orange",
  /* Note the color "evil" and it's similarity to MSgreen :) */
	"evil", "coral", "grey",
	"end"
};

char *color_numbers[] =
{
	"FFFFFF", "000000", "00FF00",
	"0000FF", "FF0000", "FF00FF",
	"00FFFF", "FFFF00", "FFB6C1",
	"9370DB", "8B4513", "FF8C00",
	"008080", "F08080", "BEBEBE"
};

char *image_extensions[] =
{
#ifdef HAVE_JPEG
	".jpg", ".jpeg",
#endif
#ifdef HAVE_GIF
	".gif",
#endif
#ifdef HAVE_TIFF
	".tif", ".tiff",
#endif
#ifdef HAVE_XPM
	".xpm",
#endif
#ifdef HAVE_XBM
	".xbm",
#endif
#ifdef HAVE_PNG
	".png", ".pjpeg",
#endif
#ifdef HAVE_PPM
	".ppm",
#endif
#ifdef HAVE_PNM
	".pnm",
#endif
#ifdef HAVE_PGM
	".pgm",
#endif
#ifdef HAVE_PCX
	".pcx",
#endif
#ifdef HAVE_BMP
	".bmp",
#endif
#ifdef HAVE_EIM
	".eim",
#endif
#ifdef HAVE_TGA
	".tga",
#endif
/* This "end" value is used to stop compares... don't remove! */
	"end"
};

#endif							/* MAIN_H */
