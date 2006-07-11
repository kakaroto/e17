/********* some functions stolen from imlib2 and modified :) ***********/

#include "Elicit.h"
#include "util.h"

#include <string.h>

void elicit_color_rgb_to_hsv(int rr, int gg, int bb, double *hh, double *ss, double *vv);
void elicit_color_hsv_to_rgb(double hh, double ss, double vv, int *rr, int *gg, int *bb);
char * elicit_color_rgb_to_hex(int rr, int gg, int bb);
void elicit_color_hex_to_rgb(char *hex, int *rr, int *gg, int *bb);

void
elicit_util_color_at_pointer_get(int *r, int *g, int *b)
{
  Imlib_Image *im;
  Imlib_Color col;
  int x, y;
  int tr;
  Window dummy;

  /* where are we pointing? */
  XQueryPointer(ecore_x_display_get(), RootWindow(ecore_x_display_get(),0), &dummy, &dummy, &tr, &tr, &x, &y, &tr);

  /* setup the imlib context */
  imlib_context_set_display(ecore_x_display_get());
  imlib_context_set_drawable(RootWindow(ecore_x_display_get(),0));
  imlib_context_set_visual( DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get() ) ));

  /* get the color of the current pixel */
  im = imlib_create_image_from_drawable(0, x-1, y-1, 1, 1, 0);
  imlib_context_set_image(im);
  imlib_image_query_pixel(0, 0, &col);

  //evas_object_color_set(swatch.obj, col.red, col.green, col.blue, 255);
  /* set the color values */
  if (r) *r = col.red;
  if (g) *g = col.green;
  if (b) *b = col.blue;

  /* update the other formats */
  /*
  _rgb_to_hsv();
  _rgb_to_hex();

  elicit_ui_update_text(el);
  changed = 1;
  */
  imlib_free_image(); 

}

void
elicit_util_shoot(Evas_Object *shot, int w, int h)
{
  Imlib_Image *im;
  int x, y;
  int px, py;
  int dw, dh;
  int tr;
  Evas_Coord sw, sh;
  Window dummy;

  XQueryPointer(ecore_x_display_get(), RootWindow(ecore_x_display_get(),0), &dummy, &dummy, &tr, &tr, &px, &py, &tr);

  x = px - .5 * w;
  y = py - .5 * h;

  ecore_x_window_size_get(RootWindow(ecore_x_display_get(),0), &dw, &dh);
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x + w > dw) x = dw - w;
  if (y + h > dh) y = dh - h;

  /* setup the imlib context */
  imlib_context_set_display(ecore_x_display_get());
  imlib_context_set_drawable(RootWindow(ecore_x_display_get(),0));
  imlib_context_set_visual( DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get() ) ));

  /* copy the correct part of the screen */
  im = imlib_create_image_from_drawable(0, x, y, w, h, 1);
  imlib_context_set_image(im);
  imlib_image_set_format("argb");

  /* get the object ready, copy the data in */
  evas_object_image_alpha_set(shot, 0);
  evas_object_image_size_set(shot, w, h);
  evas_object_image_smooth_scale_set(shot, 0);

  evas_object_image_data_copy_set(shot, imlib_image_get_data_for_reading_only());
  
  /* tell evas that we changed part of the image data */
  evas_object_image_data_update_add(shot, 0, 0, w, h);

  /* set it to fill the whole object */
  evas_object_geometry_get(shot, NULL, NULL, &sw, &sh);
  evas_object_image_fill_set(shot, 0, 0, sw, sh);

  imlib_free_image();
}

void
elicit_util_colors_set_from_rgb(Elicit *el)
{
    elicit_color_rgb_to_hsv(el->color.r, el->color.g, el->color.b,
                            &(el->color.h), &(el->color.s), &(el->color.v));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
}

void
elicit_util_colors_set_from_hsv(Elicit *el)
{
    elicit_color_hsv_to_rgb(el->color.h, el->color.s, el->color.v,
                            &(el->color.r), &(el->color.g), &(el->color.b));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
}

void
elicit_color_rgb_to_hsv(int rr, int gg, int bb, double *hh, double *ss, double *vv)
{
   int r, g, b;
   int f;
   float i,j,k,max,min,d;
   float h = 0, s = 0, v = 0;

   r = rr;
   g = gg;
   b = bb;

   i = ((float)r)/255.0;
   j = ((float)g)/255.0;
   k = ((float)b)/255.0;

   f = 0;
   max = min = i;
   if (j>max) { max = j; f = 1; } else min = j;
   if (k>max) { max = k; f = 2; } else if (k<min) min = k;
   d = max - min;

   v = max;
   if (max!=0) s = d/max; else s = 0;
   if (s==0)
      h = 0;
   else
   {
      switch (f)
      {
         case 0:
           h = (j - k)/d;
           break;
         case 1:
           h = 2 + (k - i)/d;
           break;
         case 2:
           h = 4 + (i - j)/d;
           break;
      }
      h *= 60.0;
      if (h<0) h += 360.0;
   }

   if (hh) *hh = h;
   if (ss) *ss = s;
   if (vv) *vv = v;

}

void
elicit_color_hsv_to_rgb(double hh, double ss, double vv, int *rr, int *gg, int *bb)
{
   int i,p,q,t;
   float vs,vsf;
   int r = 0, g = 0, b = 0;
   float h, s, v;

   h = hh;
   s = ss;
   v = vv;

   i = (int)(v*255.0);
   if (s==0)
      r = g = b = i;
   else
   {
      if (h==360) h = 0;
      h = h/60.0;
      vs = v * s;
                vsf = vs * (h - (int)h);
      p = (int)(255.0 * (v - vs));
      q = (int)(255.0 * (v - vsf));
      t = (int)(255.0 * (v - vs + vsf));
      switch ((int)h)
      {
         case 0:
           r = i;
                          g = t;
                          b = p;
                          break;
         case 1:
           r = q;
                          g = i;
                          b = p;
           break;
         case 2:
           r = p;
                          g = i;
                          b = t;
           break;
         case 3:
           r = p;
                          g = q;
                          b = i;
           break;
         case 4:
           r = t;
                          g = p;
                          b = i;
           break;
         case 5:
           r = i;
                          g = p;
                          b = q;
           break;
      }
   }

   if (rr) *rr = r;
   if (gg) *gg = g;
   if (bb) *bb = b;
} 

char * 
elicit_color_rgb_to_hex(int rr, int gg, int bb)
{
  char buf[10];

  snprintf(buf, 10, "#%.2x%.2x%.2x", rr, gg, bb);
  return (char *)strdup(buf);
}

void 
elicit_color_hex_to_rgb(char *hex, int *rr, int *gg, int *bb)
{
  unsigned int r = 0, g = 0, b = 0;
  int len;
  int num;

  if (!hex) return;

  len = strlen(hex);
  if (len == 7) hex++;
  else if (len != 6) return;

  num = sscanf(hex, "%02x%02x%02x", &r, &g, &b);
  if (3 == num)
  {
    if (rr) *rr = r;
    if (gg) *gg = g;
    if (bb) *bb = b;
  }
}

int
elicit_glob_match(const char *str, const char *glob)
{
   if (!strcmp(glob, "*")) return 1;
   if (!fnmatch(glob, str, 0)) return 1;
   return 0;
}

char *
elicit_theme_find(const char *name)
{
  static char eet[PATH_MAX + 1];
  struct stat st;

  snprintf(eet, sizeof(eet),
           "%s/.e/apps/"PACKAGE"/""themes/%s.edj",
           getenv("HOME"), name);
           
  if (!stat(eet, &st))
    return eet;

  snprintf(eet, sizeof(eet), DATADIR"/themes/%s.edj", name);
  return stat(eet, &st) ? NULL : eet;
}

void
elicit_util_shot_save(Elicit *el, const char *filename)
{
  Imlib_Image *im;
  int iw, ih;
  DATA32 *data;

  elicit_zoom_data_get(el->shot, (void **)&data, &iw, &ih);

  im = imlib_create_image_using_copied_data(iw, ih, data);
  imlib_context_set_image(im);
  imlib_save_image(filename);
  imlib_free_image();

}

