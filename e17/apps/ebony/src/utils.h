#ifndef _UTILS_H
#define _UTILS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Evas.h>
#include <Edb.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#define DRAW() \
{ \
   if (idle) gtk_idle_remove(idle); \
   idle = gtk_idle_add(redraw, NULL); \
}
extern int idle;
int redraw(void *data);
    
typedef struct _E_Background          E_Background;
typedef struct _E_Background_Layer    E_Background_Layer;

struct _E_Background_Layer
{
   int mode;
   int type;
   int inlined;
   struct {
      float x, y;
   } scroll;
   struct {
      float x, y;
   } pos;
   struct {
      float w, h;
      struct {
	 int w, h;
      } orig;
   } size, fill;
   char *color_class;
   char *file;
   float angle;
   struct {
      int r, g, b, a;
   } fg, bg;
   
   double x, y, w, h, fw, fh;
   
   Evas_Object obj;
   Imlib_Image image;
};

struct _E_Background
{
   Evas  evas;
   char *file;
   
   struct {
      int sx, sy;
      int w, h;
   } geom;
   
   Evas_List layers;
   
   Evas_Object base_obj;
};

extern Evas evas;
extern E_Background *background;

void
  e_update_background(E_Background *bg);
E_Background_Layer *
  e_get_layer_number(E_Background *bg, int num);
void
  e_setup_evas(Display *disp, Window win, Visual *vis, Colormap cmap, int w, int h);
void
  e_background_free(E_Background *bg);
E_Background *
  e_background_new(void);
E_Background *
  e_background_load(char *file);
void
  e_background_save(char *file, E_Background *bg);
void
  e_background_realize(E_Background *bg, Evas evas);
void
  e_background_set_scroll(E_Background *bg, int sx, int sy);
void
  e_background_set_size(E_Background *bg, int w, int h);
void
  e_background_set_color_class(E_Background *bg, char *cc, int r, int g, int b, int a);

#endif
