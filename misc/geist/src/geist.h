#ifndef geist_H
#define geist_H


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
#include <gdk/gdkx.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "config.h"
#include "debug.h"
#include "structs.h"
#include "utils.h"

#define XY_IN_RECT(x, y, rx, ry, rw, rh) \
(((x) >= (rx)) && ((y) >= (ry)) && ((x) < ((rx) + (rw))) && ((y) < ((ry) + (rh))))

#define SPANS_COMMON(x1, w1, x2, w2) \
(!((((x2) + (w2)) <= (x1)) || ((x2) >= ((x1) + (w1)))))

#define RECTS_INTERSECT(x, y, w, h, xx, yy, ww, hh) \
((SPANS_COMMON((x), (w), (xx), (ww))) && (SPANS_COMMON((y), (h), (yy), (hh))))

#define HALF_SEL_WIDTH 2
#define HALF_SEL_HEIGHT 2 

#ifndef PACKAGE
#define PACKAGE "geist"
#endif

#ifndef VERSION
#define VERSION 0.0.1
#endif
   
extern int call_level;
extern gint obj_sel_handler, obj_unsel_handler;

struct {
unsigned char debug_level;
} opt;

void imlib_init(GtkWidget *area);

enum GeistObjectTypes {
	GEIST_TYPE_OBJECT,
	GEIST_TYPE_IMAGE,
	GEIST_TYPE_TEXT,
	GEIST_TYPE_RECT
};
typedef unsigned int geist_object_type;


/* Imlib stuff */
extern Display *disp;
extern Visual *vis;
extern Colormap cm;
extern int depth;
extern Window root;
extern Screen *scr;


extern GtkWidget *darea;



#endif
