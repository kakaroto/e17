/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

/*
 * Copyright © 2003 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* xcompmgr code derived and converted to EFL by Ibukun Olumuyiwa
 * Above licensing rules apply.
 * Enlightenment, now with mo' bling-bling!
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include <e.h>
#include "e_mod_main.h"
#include "compmgr.h"

static Bling *bling;
static Config *config;

static Ecore_X_Display *dpy;
static int scr;
static Ecore_X_Window root;
static Ecore_X_Window reg;
static Ecore_X_Picture rootPicture;
static Ecore_X_Picture rootBuffer;
static Ecore_X_Picture blackPicture;
static Ecore_X_Picture transBlackPicture;
static Ecore_X_Picture rootTile;
static Ecore_X_Region allDamage;
static Bool clipChanged;

static Eina_List *wins = NULL;

#if HAS_NAME_WINDOW_PIXMAP
static Bool hasNamePixmap;
#endif
static int root_height, root_width;
static int composite_event, composite_error;
static int render_event, render_error;
static Bool synchronize;
static int composite_opcode;

static Conv *gaussianMap;

static CompMode compMode = CompSimple;

#if 0
static int shadowRadius = 12;
static int shadowOffsetX = 0;
int shadowOffsetY = 0;
double shadowOpacity = .75;
#endif
static XRenderColor shadowColor;

#if 0
double fade_in_step = 0.028;
double fade_out_step = 0.03;
int fade_delta = 10;
int fade_time = 0;
static int fade_steps = 0;
#endif
static Eina_List *fades = NULL;

#if 0
static Bool fadeWindows = False;
static Bool excludeDockShadows = False;
static Bool fadeTrans = False;
#endif

static Bool autoRedirect = False;

/* For shadow precomputation */
static int Gsize = -1;
static unsigned char *shadowCorner = NULL;
static unsigned char *shadowTop = NULL;

static Ecore_X_Rectangle *expose_rects = 0;
static int n_expose = 0;
static int size_expose = 0;

static Ecore_Event_Handler * _window_create_hnd = NULL;
static Ecore_Event_Handler * _window_configure_hnd = NULL;
static Ecore_Event_Handler * _window_destroy_hnd = NULL;
static Ecore_Event_Handler * _window_hide_hnd = NULL;
static Ecore_Event_Handler * _window_reparent_hnd = NULL;
static Ecore_Event_Handler * _window_show_hnd = NULL;
static Ecore_Event_Handler * _window_stack_hnd = NULL;
static Ecore_Event_Handler * _window_focus_in_hnd = NULL;
static Ecore_Event_Handler * _window_focus_out_hnd = NULL;
static Ecore_Event_Handler * _window_damage_hnd = NULL;
static Ecore_Event_Handler * _window_property_hnd = NULL;
static Ecore_Event_Handler * _damage_notify_hnd = NULL;

static Ecore_Idle_Enterer * idler;

static void composite_win_mode_determine(Win *w);
static double get_opacity_percent(Win *w, double def);
static Ecore_X_Region composite_win_extents (Win *w);
static int _composite_run_fades_cb(void *data);
static int composite_update(void *data);

static int
composite_x_error(Display *dpy, XErrorEvent *xerror)
{
   char buf[PATH_MAX];
   if(XGetErrorText(dpy, xerror->error_code, buf, PATH_MAX))
   {
      fprintf(stderr, "X ERROR %5d: %s\n", xerror->error_code, buf);
   }
   return 1;
}

static Fade *
composite_fade_find(Win * w)
{
   Eina_List *l;

   for (l = fades; l; l = l->next)
   {
      Fade *f = l->data;
      if (f->w == w)
         return f;
   }
   return NULL;
}

static void
composite_fade_dequeue(Fade * f)
{
   if (!f) return;
   f->w->isInFade = False;
   if (f->callback)
      (*f->callback) (f->w, f->gone);
   fades = eina_list_remove(fades, f);
   if (f->anim)
      ecore_animator_del(f->anim);
   /* E_FREE(f); */
}

static void
composite_fade_dequeue_win(Win * w)
{
   Fade *f = composite_fade_find(w);
   if (f) composite_fade_dequeue(f);
}

static int
composite_fade_is_valid(Ecore_X_Window id)
{
   E_Manager *man;
   Eina_List *l;
   char *name = NULL, *class = NULL;

   if (!id) return 0;
   if (!id || id == root) return 0;
   ecore_x_icccm_name_class_get(id, &name, &class);
   if (class && !strcmp(class, "Background_Window"))
   {
      E_FREE(name);
      E_FREE(class);
      return 0;
   }
   E_FREE(name);
   E_FREE(class);
   man = e_manager_current_get();
   if (id == man->win) return 0;
   for (l = man->containers; l; l = l->next)
   {
      E_Container *con = l->data;
      if (id == con->win) return 0;
   }
   return 1;
}

static void
composite_fade_set(Win *w, double start, double finish, double step,
                   void (*callback) (Win *w, Bool gone), Bool gone,
                   Bool exec_callback, Bool override)
{
   Fade *f;

   /* Don't fade desktop/root windows */
   if (!composite_fade_is_valid(w->id)) return;

   f = composite_fade_find(w);
   if (!f)
   {
      f = E_NEW(Fade, 1);
      f->w = w;
      f->start = f->cur = start;
      w->isInFade = True;
      fades = eina_list_prepend(fades, f);
      f->anim = ecore_animator_add(_composite_run_fades_cb, f);
   }
   else if (!override)
      return;
   else
   {
      if (exec_callback && f->callback)
         (*f->callback)(f->w, f->gone);
   }

   if (finish < 0) finish = 0;
   if (finish > 1) finish = 1;
   f->finish = finish;
   f->step = (f->cur < finish) ? step : -step;
   f->start_time = ecore_time_get();
   if (f->cur < finish)
      f->interval = (1/config->fx_fade_in_step * ecore_animator_frametime_get());
   else
      f->interval = (1/config->fx_fade_out_step * ecore_animator_frametime_get());
   f->callback = callback;
   f->gone = gone;
   w->opacity = f->cur * OPAQUE;
   composite_win_mode_determine(w);
   if (w->shadow)
   {
      XRenderFreePicture(dpy, w->shadow);
      w->shadow= None;
      w->extents = composite_win_extents(w);
   }
}

static int
_composite_run_fades_cb(void *data)
{
   Fade *f;
   Win *w;
   int f_continue = 1;
   double current_time, delta_time;

   f = data;
   if (!f) return 0;
   w = f->w;

   current_time = ecore_time_get();
   delta_time = current_time - f->start_time;
   if (delta_time >= f->interval)
      f->cur = f->finish;
   else
      f->cur = f->start + (delta_time/f->interval * (f->finish - f->start));
   if (f->cur >= 1)
      f->cur = 1;
   else if (f->cur < 0)
      f->cur = 0;
   w->opacity = f->cur * OPAQUE;
   if (f->step > 0)
   {
      if (f->cur >= f->finish)
      {
         w->opacity = f->finish * OPAQUE;
         composite_fade_dequeue(f);
         f = NULL;
         f_continue = 0;
      }
   }
   else
   {
      if (f->cur <= f->finish)
      {
         w->opacity = f->finish * OPAQUE;
         composite_fade_dequeue(f);
         f = NULL;
         f_continue = 0;
      }
   }
   composite_win_mode_determine(w);
   if (w->shadow)
   {
      XRenderFreePicture(dpy, w->shadow);
      w->shadow = None;
      w->extents = composite_win_extents(w);
   }
   return f_continue;
}

#define SHADOW_OFFSET_X	((-config->shadow_active_size * 7 / 5) - config->shadow_horz_offset * config->shadow_active_size / 100) * w->shadowSize
#define SHADOW_OFFSET_Y	((-config->shadow_active_size * 7 / 5) - config->shadow_vert_offset * config->shadow_active_size / 100) * w->shadowSize

static double
gaussian(double r, double x, double y)
{
   return ((1 / (sqrt(2 * M_PI * r))) *
           exp((-(x * x + y * y)) / (2 * r * r)));
}


static Conv *
make_gaussian_map(double r)
{
   Conv *c;
   int size = ((int) ceil((r * 3)) + 1) & ~1;
   int center = size / 2;
   int x, y;
   double t;
   double g;

   c = malloc(sizeof(Conv) + size * size * sizeof(double));
   c->size = size;
   c->data = (double *) (c + 1);
   t = 0.0;
   for (y = 0; y < size; y++)
      for (x = 0; x < size; x++)
      {
         g = gaussian(r, (double) (x - center), (double) (y - center));
         t += g;
         c->data[y * size + x] = g;
      }
/*    printf ("gaussian total %f\n", t); */
   for (y = 0; y < size; y++)
      for (x = 0; x < size; x++)
      {
         c->data[y * size + x] /= t;
      }
   return c;
}

/*
 * A picture will help
 *
 *	-center   0                width  width+center
 *  -center +-----+-------------------+-----+
 *	    |     |                   |     |
 *	    |     |                   |     |
 *        0 +-----+-------------------+-----+
 *	    |     |                   |     |
 *	    |     |                   |     |
 *	    |     |                   |     |
 *   height +-----+-------------------+-----+
 *	    |     |                   |     |
 * height+  |     |                   |     |
 *  center  +-----+-------------------+-----+
 */

static unsigned char
sum_gaussian(Conv * map, double opacity, int x, int y, int width, int height)
{
   int fx, fy;
   double *g_data;
   double *g_line = map->data;
   int g_size = map->size;
   int center = g_size / 2;
   int fx_start, fx_end;
   int fy_start, fy_end;
   double v;

   /* 
    * Compute set of filter values which are "in range",
    * that's the set with:
    *                                                                                             0 <= x + (fx-center) && x + (fx-center) < width &&
    *  0 <= y + (fy-center) && y + (fy-center) < height
    *
    *  0 <= x + (fx - center)                                                                     x + fx - center < width
    *  center - x <= fx                                                                           fx < width + center - x
    */

   fx_start = center - x;
   if (fx_start < 0)
      fx_start = 0;
   fx_end = width + center - x;
   if (fx_end > g_size)
      fx_end = g_size;

   fy_start = center - y;
   if (fy_start < 0)
      fy_start = 0;
   fy_end = height + center - y;
   if (fy_end > g_size)
      fy_end = g_size;

   g_line = g_line + fy_start * g_size + fx_start;

   v = 0;
   for (fy = fy_start; fy < fy_end; fy++)
   {
      g_data = g_line;
      g_line += g_size;

      for (fx = fx_start; fx < fx_end; fx++)
         v += *g_data++;
   }
   if (v > 1)
      v = 1;

   return ((unsigned char) (v * opacity * 255.0));
}

/* precompute shadow corners and sides to save time for large windows */
static void
presum_gaussian(Conv * map)
{
   int center = map->size / 2;
   int opacity, x, y;

   Gsize = map->size;

   if (shadowCorner)
      free((void *) shadowCorner);
   if (shadowTop)
      free((void *) shadowTop);

   shadowCorner = (unsigned char *) (malloc((Gsize + 1) * (Gsize + 1) * 26));
   shadowTop = (unsigned char *) (malloc((Gsize + 1) * 26));

   for (x = 0; x <= Gsize; x++)
   {
      shadowTop[25 * (Gsize + 1) + x] =
         sum_gaussian(map, 1, x - center, center, Gsize * 2, Gsize * 2);
      for (opacity = 0; opacity < 25; opacity++)
         shadowTop[opacity * (Gsize + 1) + x] =
            shadowTop[25 * (Gsize + 1) + x] * opacity / 25;
      for (y = 0; y <= x; y++)
      {
         shadowCorner[25 * (Gsize + 1) * (Gsize + 1) + y * (Gsize + 1) + x] =
            sum_gaussian(map, 1, x - center, y - center, Gsize * 2,
                         Gsize * 2);
         shadowCorner[25 * (Gsize + 1) * (Gsize + 1) + x * (Gsize + 1) + y] =
            shadowCorner[25 * (Gsize + 1) * (Gsize + 1) + y * (Gsize + 1) +
                         x];
         for (opacity = 0; opacity < 25; opacity++)
            shadowCorner[opacity * (Gsize + 1) * (Gsize + 1) +
                         y * (Gsize + 1) + x] =
               shadowCorner[opacity * (Gsize + 1) * (Gsize + 1) +
                            x * (Gsize + 1) + y] =
               shadowCorner[25 * (Gsize + 1) * (Gsize + 1) + y * (Gsize + 1) +
                            x] * opacity / 25;
      }
   }
}

static XImage *
make_shadow(double opacity, int width, int height)
{
   XImage *ximage;
   unsigned char *data;
   int gsize = gaussianMap->size;
   int ylimit, xlimit;
   int swidth = width + gsize;
   int sheight = height + gsize;
   int center = gsize / 2;
   int x, y;
   unsigned char d;
   int x_diff;
   int opacity_int = (int) (opacity * 25);
   data = malloc(swidth * sheight * sizeof(unsigned char));
   if (!data)
      return 0;
   ximage =
      XCreateImage(dpy, DefaultVisual(dpy, DefaultScreen(dpy)), 8, ZPixmap, 0,
                   (char *) data, swidth, sheight, 8,
                   swidth * sizeof(unsigned char));
   if (!ximage)
   {
      free(data);
      return 0;
   }
   /* 
    * Build the gaussian in sections
    */

   /* 
    * center (fill the complete data array)
    */
   if (Gsize > 0)
      d = shadowTop[opacity_int * (Gsize + 1) + Gsize];
   else
      d = sum_gaussian(gaussianMap, opacity, center, center, width, height);
   memset(data, d, sheight * swidth);

   /* 
    * corners
    */
   ylimit = gsize;
   if (ylimit > sheight / 2)
      ylimit = (sheight + 1) / 2;
   xlimit = gsize;
   if (xlimit > swidth / 2)
      xlimit = (swidth + 1) / 2;

   for (y = 0; y < ylimit; y++)
      for (x = 0; x < xlimit; x++)
      {
         if (xlimit == Gsize && ylimit == Gsize)
            d = shadowCorner[opacity_int * (Gsize + 1) * (Gsize + 1) +
                             y * (Gsize + 1) + x];
         else
            d = sum_gaussian(gaussianMap, opacity, x - center, y - center,
                             width, height);
         data[y * swidth + x] = d;
         data[(sheight - y - 1) * swidth + x] = d;
         data[(sheight - y - 1) * swidth + (swidth - x - 1)] = d;
         data[y * swidth + (swidth - x - 1)] = d;
      }

   /* 
    * top/bottom
    */
   x_diff = swidth - (gsize * 2);
   if (x_diff > 0 && ylimit > 0)
   {
      for (y = 0; y < ylimit; y++)
      {
         if (ylimit == Gsize)
            d = shadowTop[opacity_int * (Gsize + 1) + y];
         else
            d = sum_gaussian(gaussianMap, opacity, center, y - center, width,
                             height);
         memset(&data[y * swidth + gsize], d, x_diff);
         memset(&data[(sheight - y - 1) * swidth + gsize], d, x_diff);
      }
   }

   /* 
    * sides
    */

   for (x = 0; x < xlimit; x++)
   {
      if (xlimit == Gsize)
         d = shadowTop[opacity_int * (Gsize + 1) + x];
      else
         d = sum_gaussian(gaussianMap, opacity, x - center, center, width,
                          height);
      for (y = gsize; y < sheight - gsize; y++)
      {
         data[y * swidth + x] = d;
         data[y * swidth + (swidth - x - 1)] = d;
      }
   }

   return ximage;
}

static Ecore_X_Picture
shadow_picture(double opacity, Picture alpha_pict, int width, int height,
               int *wp, int *hp)
{
   XImage *shadowImage;
   Ecore_X_Pixmap shadowPixmap;
   Picture shadowPicture;
   Ecore_X_GC gc = NULL;

   shadowImage = make_shadow(opacity, width, height);
   if (!shadowImage)
      return None;
   shadowPixmap =
      ecore_x_pixmap_new(root, shadowImage->width, shadowImage->height, 8);
   if (!shadowPixmap)
   {
      XDestroyImage(shadowImage);
      return None;
   }

   shadowPicture =
      XRenderCreatePicture(dpy, shadowPixmap,
                           XRenderFindStandardFormat(dpy, PictStandardA8), 0,
                           0);
   if (!shadowPicture)
   {
      XDestroyImage(shadowImage);
      ecore_x_pixmap_del(shadowPixmap);
      return None;
   }

   gc = ecore_x_gc_new(shadowPixmap, 0, NULL);
   if (!gc)
   {
      XDestroyImage(shadowImage);
      ecore_x_pixmap_del(shadowPixmap);
      XRenderFreePicture(dpy, shadowPicture);
      return None;
   }

   XPutImage(dpy, shadowPixmap, gc, shadowImage, 0, 0, 0, 0,
             shadowImage->width, shadowImage->height);
   *wp = shadowImage->width;
   *hp = shadowImage->height;
   ecore_x_gc_del(gc);
   XDestroyImage(shadowImage);
   ecore_x_pixmap_del(shadowPixmap);
   return shadowPicture;
}

static Ecore_X_Picture
solid_picture(Bool argb, double a, double r, double g, double b)
{
   Ecore_X_Pixmap pixmap;
   Picture picture;
   XRenderPictureAttributes pa;
   XRenderColor c;

   pixmap = ecore_x_pixmap_new(root, 1, 1, argb ? 32 : 8);
   if (!pixmap)
      return None;

   pa.repeat = True;
   picture =
      XRenderCreatePicture(dpy, pixmap,
                           XRenderFindStandardFormat(dpy,
                                                     argb ? PictStandardARGB32
                                                     : PictStandardA8),
                           CPRepeat, &pa);
   if (!picture)
   {
      ecore_x_pixmap_del(pixmap);
      return None;
   }

   c.alpha = a * 0xffff;
   c.red = r * 0xffff;
   c.green = g * 0xffff;
   c.blue = b * 0xffff;
   XRenderFillRectangle(dpy, PictOpSrc, picture, &c, 0, 0, 1, 1);
   ecore_x_pixmap_del(pixmap);
   return picture;
}

static char *backgroundProps[] = {
   "_XROOTPMAP_ID",
   "_XSETROOT_ID",
   0,
};

static Ecore_X_Picture
root_tile(void)
{
   Ecore_X_Picture picture;
   Ecore_X_Pixmap pixmap;
   Bool fill = False;
   XRenderPictureAttributes pa;
   int p;

   pixmap = None;
   for (p = 0; backgroundProps[p]; p++)
   {
      if (ecore_x_window_prop_xid_get(root, 
                                      ecore_x_atom_get(backgroundProps[p]),
                                      AnyPropertyType,
                                      &pixmap, 4))
      {
         fill = False;
         break;
      }
   }
   if (!pixmap)
   {
      pixmap = ecore_x_pixmap_new(root, 1, 1, 0);
      fill = True;
   }
   pa.repeat = True;
   picture =
      XRenderCreatePicture(dpy, pixmap,
                           XRenderFindVisualFormat(dpy,
                                                   DefaultVisual(dpy, scr)),
                           CPRepeat, &pa);
   if (fill)
   {
      XRenderColor c;

      c.red = c.green = c.blue = 0x8080;
      c.alpha = 0xffff;
      XRenderFillRectangle(dpy, PictOpSrc, picture, &c, 0, 0, 1, 1);
   }
   return picture;
}

static void
paint_root(void)
{
   if (!rootTile)
      rootTile = root_tile();

   XRenderComposite(dpy, PictOpSrc, rootTile, None, rootBuffer, 0, 0, 0, 0, 0,
                    0, root_width, root_height);
}

static Ecore_X_Region
composite_win_extents(Win * w)
{
   Ecore_X_Rectangle r;
   E_Border *bd = NULL;
   Bool needs_shadow = True;

   /* Don't shadow shaped clients (until we have a function that
    * can compute shaped shadows) */
   bd = e_border_find_by_window(w->id);
   if (bd && bd->client.shaped)
      needs_shadow = False;
   
   r.x = w->a.x;
   r.y = w->a.y;
   r.width = w->a.w + w->a.border * 2;
   r.height = w->a.h + w->a.border * 2;
   if (compMode != CompSimple && w->shadowSize > 0 && needs_shadow
       && !(w->windowType == ECORE_X_WINDOW_TYPE_DOCK
            && !(config->shadow_dock_enable)))
   {
      if (compMode == CompServerShadows || w->mode != WINDOW_ARGB)
      {
         Ecore_X_Rectangle sr;

         if (compMode == CompServerShadows)
         {
            w->shadow_dx = 2;
            w->shadow_dy = 7;
            w->shadow_width = w->a.w;
            w->shadow_height = w->a.h;
         }
         else
         {
#if 0
            w->shadow_dx = SHADOW_OFFSET_X;
            w->shadow_dx = w->shadow_dx / 100;
            w->shadow_dy = SHADOW_OFFSET_Y;
            w->shadow_dy = w->shadow_dy / 100;
#endif
#if 1
            w->shadow_dx = config->shadow_horz_offset - config->shadow_active_size - 3;
            w->shadow_dy = config->shadow_vert_offset - config->shadow_active_size - 3;
#endif
            if (!w->shadow)
            {
               double opacity = config->shadow_opacity;

               if (w->shadowSize > 100)
                  opacity = opacity / (w->shadowSize * 0.01);
               if (w->mode == WINDOW_TRANS)
                  opacity =
                     opacity * ((double) w->opacity) / ((double) OPAQUE);
               w->shadow =
                  shadow_picture(opacity, w->alphaPict,
                                 w->a.w + w->a.border * 2 -
                                 2 * (config->shadow_active_size -
                                      (w->shadowSize *
                                       config->shadow_active_size / 100)),
                                 w->a.h + w->a.border * 2 -
                                 2 * (config->shadow_active_size -
                                      (w->shadowSize *
                                       config->shadow_active_size / 100)),
                                 &w->shadow_width, &w->shadow_height);
            }
         }
         sr.x = w->a.x + w->shadow_dx;
         sr.y = w->a.y + w->shadow_dy;
         sr.width = w->shadow_width;
         sr.height = w->shadow_height;
         if (sr.x < r.x)
         {
            r.width = (r.x + r.width) - sr.x;
            r.x = sr.x;
         }
         if (sr.y < r.y)
         {
            r.height = (r.y + r.height) - sr.y;
            r.y = sr.y;
         }
         if (sr.x + sr.width > r.x + r.width)
            r.width = sr.x + sr.width - r.x;
         if (sr.y + sr.height > r.y + r.height)
            r.height = sr.y + sr.height - r.y;
      }
   }
   return ecore_x_region_new(&r, 1);
}

static Ecore_X_Region
border_size(Win * w)
{
   Ecore_X_Region border;

   /* 
    * if window doesn't exist anymore,  this will generate an error
    * as well as not generate a region.  Perhaps a better XFixes
    * architecture would be to have a request that copies instead
    * of creates, that way you'd just end up with an empty region
    * instead of an invalid XID.
    */
   border = ecore_x_region_new_from_window(w->id, ECORE_X_REGION_BOUNDING);
   /* translate this */
   ecore_x_region_translate(border, w->a.x + w->a.border,
                            w->a.y + w->a.border);
   return border;
}

static void
composite_paint_all(Ecore_X_Region region)
{
   Win *w;
   Win *t = NULL;
   Eina_List *l;

   if (!region)
   {
      Ecore_X_Rectangle r;

      r.x = 0;
      r.y = 0;
      r.width = root_width;
      r.height = root_height;
      region = ecore_x_region_new(&r, 1);
   }
#if MONITOR_REPAINT
   rootBuffer = rootPicture;
#else
   if (!rootBuffer)
   {
      Ecore_X_Pixmap rootPixmap =
         ecore_x_pixmap_new(root, root_width, root_height, 0);

      rootBuffer =
         XRenderCreatePicture(dpy, rootPixmap,
                              XRenderFindVisualFormat(dpy,
                                                      DefaultVisual(dpy,
                                                                    scr)), 0,
                              0);
      ecore_x_pixmap_del(rootPixmap);
   }
#endif
   ecore_x_region_picture_clip_set(region, rootPicture, 0, 0);
#if MONITOR_REPAINT
   XRenderComposite(dpy, PictOpSrc, blackPicture, None, rootPicture, 0, 0, 0,
                    0, 0, 0, root_width, root_height);
#endif
#if DEBUG_REPAINT
   printf("paint:");
#endif
   for (l = wins; l; l = l->next)
   {
      w = l->data;
#if CAN_DO_USABLE
      if (!w->usable)
         continue;
#endif
      /* never painted, ignore it */
      if (!w->damaged)
         continue;

      /* skip invisible windows */
      if (w->a.x + w->a.w < 1 || w->a.y + w->a.h < 1
          || w->a.x >= root_width || w->a.y >= root_height)
         continue;

      if (!w->picture)
      {
         XRenderPictureAttributes pa;
         XRenderPictFormat *format;
         Drawable draw = w->id;

#if HAS_NAME_WINDOW_PIXMAP
         if (hasNamePixmap && !w->pixmap)
            w->pixmap = XCompositeNameWindowPixmap(dpy, w->id);
         if (w->pixmap)
            draw = w->pixmap;
#endif
         format = XRenderFindVisualFormat(dpy, w->a.visual);
         pa.subwindow_mode = IncludeInferiors;
         w->picture =
            XRenderCreatePicture(dpy, draw, format, CPSubwindowMode, &pa);
      }
#if DEBUG_REPAINT
      printf(" 0x%x", w->id);
#endif
      if (clipChanged)
      {
         if (w->borderSize)
         {
            ecore_x_region_del(w->borderSize);
            w->borderSize = None;
         }
         if (w->extents)
         {
            ecore_x_region_del(w->extents);
            w->extents = None;
         }
         if (w->borderClip)
         {
            ecore_x_region_del(w->borderClip);
            w->borderClip = None;
         }
      }
      if (!w->borderSize)
         w->borderSize = border_size(w);
      if (!w->extents)
         w->extents = composite_win_extents(w);
      if (w->mode == WINDOW_SOLID)
      {
         int x, y, wid, hei;

#if HAS_NAME_WINDOW_PIXMAP
         x = w->a.x;
         y = w->a.y;
         wid = w->a.w + w->a.border * 2;
         hei = w->a.h + w->a.border * 2;
#else
         x = w->a.x + w->a.border;
         y = w->a.y + w->a.border;
         wid = w->a.w;
         hei = w->a.h;
#endif
         ecore_x_region_picture_clip_set(region, rootBuffer, 0, 0);
         ecore_x_region_subtract(region, region, w->borderSize);
         XRenderComposite(dpy, PictOpSrc, w->picture, None, rootBuffer, 0, 0,
                          0, 0, x, y, wid, hei);
      }
      if (!w->borderClip)
      {
         w->borderClip = ecore_x_region_new(NULL, 0);
         ecore_x_region_copy(w->borderClip, region);
      }
      w->prev_trans = t;
      t = w;
   }
#if DEBUG_REPAINT
   printf("\n");
   fflush(stdout);
#endif
   ecore_x_region_picture_clip_set(region, rootBuffer, 0, 0);
   paint_root();
   for (w = t; w; w = w->prev_trans)
   {
      /* skip desktop */
      if (w->windowType == ECORE_X_WINDOW_TYPE_DESKTOP)
         continue;

      if (w->shadowSize > 0 && config->shadow_enable)
      {
         switch (compMode)
         {
           case CompSimple:
              break;
           case CompServerShadows:
              ecore_x_region_picture_clip_set(w->borderClip, rootBuffer, 0,
                                              0);
              if (w->opacity != OPAQUE && !w->shadowPict)
                 w->shadowPict =
                    solid_picture(True,
                                  (double) w->opacity / OPAQUE * 0.3, 0, 0,
                                  0);
              XRenderComposite(dpy, PictOpOver,
                               w->shadowPict ? w->
                               shadowPict : transBlackPicture, w->picture,
                               rootBuffer, 0, 0, 0, 0, w->a.x + w->shadow_dx,
                               w->a.y + w->shadow_dy, w->shadow_width,
                               w->shadow_height);
              break;
           case CompClientShadows:
              if (w->shadow && w->windowType != ECORE_X_WINDOW_TYPE_DESKTOP)
              {
                 ecore_x_region_picture_clip_set(w->borderClip, rootBuffer, 0,
                                                 0);
                 XRenderComposite(dpy, PictOpOver, blackPicture, w->shadow,
                                  rootBuffer, 0, 0, 0, 0,
                                  w->a.x + w->shadow_dx,
                                  w->a.y + w->shadow_dy, w->shadow_width,
                                  w->shadow_height);
              }
              break;
         }
      }
      if (w->opacity != OPAQUE && !w->alphaPict)
         w->alphaPict =
            solid_picture(False, (double) w->opacity / OPAQUE,
                          shadowColor.red, shadowColor.green,
                          shadowColor.blue);
      if (w->mode == WINDOW_TRANS)
      {
         int x, y, wid, hei;

         ecore_x_region_intersect(w->borderClip, w->borderClip,
                                  w->borderSize);
         ecore_x_region_picture_clip_set(w->borderClip, rootBuffer, 0, 0);
#if HAS_NAME_WINDOW_PIXMAP
         x = w->a.x;
         y = w->a.y;
         wid = w->a.w + w->a.border * 2;
         hei = w->a.h + w->a.border * 2;
#else
         x = w->a.x + w->a.border;
         y = w->a.y + w->a.border;
         wid = w->a.w;
         hei = w->a.h;
#endif
         XRenderComposite(dpy, PictOpOver, w->picture, w->alphaPict,
                          rootBuffer, 0, 0, 0, 0, x, y, wid, hei);
      }
      else if (w->mode == WINDOW_ARGB)
      {
         int x, y, wid, hei;

         ecore_x_region_intersect(w->borderClip, w->borderClip,
                                  w->borderSize);
         ecore_x_region_picture_clip_set(w->borderClip, rootBuffer, 0, 0);
#if HAS_NAME_WINDOW_PIXMAP
         x = w->a.x;
         y = w->a.y;
         wid = w->a.w + w->a.border * 2;
         hei = w->a.h + w->a.border * 2;
#else
         x = w->a.x + w->a.border;
         y = w->a.y + w->a.border;
         wid = w->a.w;
         hei = w->a.h;
#endif
         XRenderComposite(dpy, PictOpOver, w->picture, w->alphaPict,
                          rootBuffer, 0, 0, 0, 0, x, y, wid, hei);
      }
      ecore_x_region_del(w->borderClip);
      w->borderClip = None;
   }
   ecore_x_region_del(region);
   if (rootBuffer != rootPicture)
   {
      ecore_x_region_picture_clip_set(None, rootBuffer, 0, 0);
      XRenderComposite(dpy, PictOpSrc, rootBuffer, None, rootPicture, 0, 0, 0,
                       0, 0, 0, root_width, root_height);
   }
}

static void
composite_damage_add(Ecore_X_Region damage)
{
   if (allDamage)
   {
      ecore_x_region_combine(allDamage, allDamage, damage);
      ecore_x_region_del(damage);
   }
   else
      allDamage = damage;
}

static Win *
composite_win_find(Ecore_X_Window id)
{
   Eina_List *l;
   Win *w;

   EINA_LIST_FOREACH(wins, l, w)
   {
      if (w && w->id == id)
         return w;
   }
   return NULL;
}

static void
composite_win_repair(Win * w)
{
   Ecore_X_Region parts;

   if (!w->damaged)
   {
      parts = composite_win_extents(w);
      ecore_x_damage_subtract(w->damage, None, None);
   }
   else
   {
      Ecore_X_Region o;

      parts = ecore_x_region_new(NULL, 0);
      ecore_x_damage_subtract(w->damage, None, parts);
      ecore_x_region_translate(parts, w->a.x + w->a.border,
                               w->a.y + w->a.border);
      if (compMode == CompServerShadows)
      {
         o = ecore_x_region_new(NULL, 0);
         ecore_x_region_copy(o, parts);
         ecore_x_region_translate(o, w->shadow_dx, w->shadow_dy);
         ecore_x_region_combine(parts, parts, o);
         ecore_x_region_del(o);
      }
   }
   composite_damage_add(parts);
   w->damaged = 1;
}

static void
composite_win_map(Ecore_X_Window id, Bool fade)
{
   E_Border *bd = e_border_find_by_window(id);
   Win *w = composite_win_find(id);

   if (!w)
      return;

   w->a.visible = 1;
   w->a.viewable = 1;

   /* Set focus transparency */
   if (ecore_x_window_focus_get() == w->id)
   {
      w->focused = True;
      w->opacity = (unsigned int) ((double)config->trans_active_value/100.0 * OPAQUE);
   }
   else
   {
      w->focused = False;
      w->opacity = (unsigned int) ((double)config->trans_inactive_value/100.0 * OPAQUE);
   }

#if CAN_DO_USABLE
   w->damage_bounds.x = w->damage_bounds.y = 0;
   w->damage_bounds.width = w->damage_bounds.height = 0;
#endif
   w->damaged = 0;

   if ((!bd || (bd && !bd->fx.start.x && !bd->fx.start.y)) &&
	fade && config->fx_fade_enable)
      composite_fade_set(w, 0, get_opacity_percent(w, 1.0),
               config->fx_fade_in_step, 0, False, True, True);
}

static void
composite_win_finish_unmap(Win * w)
{
   w->damaged = 0;
#if CAN_DO_USABLE
   w->usable = False;
#endif
   if (w->extents != None)
   {
      composite_damage_add(w->extents);   /* destroys region */
      w->extents = None;
   }

#if HAS_NAME_WINDOW_PIXMAP
   if (w->pixmap)
   {
      XFreePixmap(dpy, w->pixmap);
      w->pixmap = None;
   }
#endif

   if (w->picture)
   {
      XRenderFreePicture(dpy, w->picture);
      w->picture = None;
   }

   if (w->borderSize)
   {
      ecore_x_region_del(w->borderSize);
      w->borderSize = None;
   }
   if (w->shadow)
   {
      XRenderFreePicture(dpy, w->shadow);
      w->shadow = None;
   }
   if (w->borderClip)
   {
      ecore_x_region_del(w->borderClip);
      w->borderClip = None;
   }

   clipChanged = True;
}

#if HAS_NAME_WINDOW_PIXMAP
static void
unmap_callback(Win * w, Bool gone)
{
   composite_win_finish_unmap(w);
}
#endif

static void
composite_win_unmap(Ecore_X_Window id, Bool fade)
{
   E_Border *bd = e_border_find_by_window(id);
   Win *w = composite_win_find(id);

   if (!w)
      return;
   w->a.visible = 0;
   w->a.viewable = 0;
   if (w->focused)
   {
      Ecore_X_Window nf = ecore_x_window_focus_get();
      Win *nf_win;

      w->focused = False;
      nf_win = composite_win_find(nf);
      if (nf_win)
      {
         nf_win->focused = True;
         if (composite_fade_is_valid(nf))
         {
            double opacity_pct;
            unsigned int opacity;
            opacity_pct = ((double) config->trans_active_value)/100.0;
            opacity = (unsigned int) (OPAQUE * opacity_pct);
            ecore_x_netwm_opacity_set(nf, opacity);
         }
      }
   }     
      
#if HAS_NAME_WINDOW_PIXMAP
   if ((!bd || (bd && !bd->fx.start.x && !bd->fx.start.y)) &&
	w->pixmap && fade && config->fx_fade_enable)
      composite_fade_set(w, w->opacity * 1.0 / OPAQUE, 0.0,
               config->fx_fade_out_step, unmap_callback, False, False, True);
   else
#endif
      composite_win_finish_unmap(w);
}

/* Get the opacity prop from window
   not found: default
   otherwise the value
 */
static unsigned int
get_opacity_prop(Win * w, unsigned int def)
{
   unsigned int i;

   int result = ecore_x_window_prop_card32_get(w->id,
                                               ECORE_X_ATOM_NET_WM_WINDOW_OPACITY,
                                               &i, 1);

   return (result > 0) ? i : def;
}

static unsigned int
get_shadow_prop(Win * w)
{
   unsigned int i;

   int result = ecore_x_window_prop_card32_get(w->id,
                                               ECORE_X_ATOM_NET_WM_WINDOW_SHADOW,
                                               &i, 1);

   return (result > 0) ? i : 100;
}

static unsigned int
composite_shade_prop_get(Win * w)
{
   unsigned int i;

   int result = ecore_x_window_prop_card32_get(w->id,
                                               ECORE_X_ATOM_NET_WM_WINDOW_SHADE,
                                               &i, 1);

   return (result > 0) ? i : 0;
}

/* Get the opacity property from the window in a percent format
   not found: default
   otherwise: the value
*/
static double
get_opacity_percent(Win * w, double def)
{
   if (w && w->isInFade)
   {
      Fade *f = composite_fade_find(w);

      return f->finish;
   }
   else
   {
      unsigned int opacity =
         get_opacity_prop(w, (unsigned int) (OPAQUE * def));
      return opacity * 1.0 / OPAQUE;
   }
}

/* determine mode for window all in one place.
   Future might check for menu flag and other cool things
*/

static void
composite_win_mode_determine(Win * w)
{
   int mode;
   XRenderPictFormat *format;

   /* if trans prop == -1 fall back on previous tests */

   if (w->alphaPict)
   {
      XRenderFreePicture(dpy, w->alphaPict);
      w->alphaPict = None;
   }
   if (w->shadowPict)
   {
      XRenderFreePicture(dpy, w->shadowPict);
      w->shadowPict = None;
   }

   if (w->a.input_only)
   {
      format = 0;
   }
   else
   {
      format = XRenderFindVisualFormat(dpy, w->a.visual);
   }

   if (format && format->type == PictTypeDirect && format->direct.alphaMask)
   {
      mode = WINDOW_ARGB;
   }
   else if (w->opacity != OPAQUE)
   {
      mode = WINDOW_TRANS;
   }
   else
   {
      mode = WINDOW_SOLID;
   }
   w->mode = mode;
   if (w->extents)
   {
      Ecore_X_Region damage;

      damage = ecore_x_region_new(NULL, 0);
      ecore_x_region_copy(damage, w->extents);
      composite_damage_add(damage);
   }
}

static Ecore_X_Window_Type
composite_win_determine_type(Ecore_X_Window w)
{
   Ecore_X_Window *children = NULL;
   int nchildren, i;
   Ecore_X_Window_Type type;

   ecore_x_netwm_window_type_get(w, &type);
   if (type != ECORE_X_WINDOW_TYPE_NORMAL)
      return type;

   if (!(children = ecore_x_window_children_get(w, &nchildren)))
      return ECORE_X_WINDOW_TYPE_NORMAL;

   for (i = 0; i < nchildren; i++)
   {
      type = composite_win_determine_type(children[i]);
      if (type != ECORE_X_WINDOW_TYPE_NORMAL)
         return type;
   }

   if (children)
      free((void *) children);

   return ECORE_X_WINDOW_TYPE_NORMAL;
}

static void
composite_win_add(Ecore_X_Window id, Ecore_X_Window prev)
{
   Win *new = E_NEW(Win, 1);
   Win *p;

   if (!new)
      return;
   new->id = id;
   if (!ecore_x_window_attributes_get(id, &new->a))
   {
      E_FREE(new);
      return;
   }
   new->damaged = 0;
#if CAN_DO_USABLE
   new->usable = False;
#endif
#if HAS_NAME_WINDOW_PIXMAP
   new->pixmap = None;
#endif
   new->picture = None;
   if (new->a.input_only)
   {
      new->damage_sequence = 0;
      new->damage = None;
   }
   else
   {
      new->damage_sequence = NextRequest(dpy);
      new->damage = ecore_x_damage_new(id, ECORE_X_DAMAGE_REPORT_NON_EMPTY);
   }
   new->isInFade = False;
   new->alphaPict = None;
   new->shadowPict = None;
   new->borderSize = None;
   new->extents = None;
   new->shadow = None;
   new->shadow_dx = 0;
   new->shadow_dy = 0;
   new->shadow_width = 0;
   new->shadow_height = 0;
   new->opacity = OPAQUE;
   new->shadowSize = 100;

   new->borderClip = None;
   new->prev_trans = NULL;

   /* moved mode setting to one place */
   new->opacity = get_opacity_prop(new, OPAQUE);
   new->shadowSize = get_shadow_prop(new);
   new->windowType = composite_win_determine_type(new->id);
   composite_win_mode_determine(new);

   p = composite_win_find(prev);
   if (p)
      wins = eina_list_prepend_relative(wins, new, p);
   else
      wins = eina_list_prepend(wins, new);
   
   /* Don't fade desktop/container windows */
#if 0
   ecore_x_icccm_name_class_get(new->id, &name, &class);
   new->fade = strcmp(class, "Background_Window");
#endif
   if (new->a.viewable)
      composite_win_map(id, true);
}

void
composite_win_restack(Win * w, Ecore_X_Window new_above)
{
   Ecore_X_Window old_above;
   Eina_List *l;

   l = eina_list_data_find_list(wins, w);

   if (l && l->next)
      old_above = ((Win *)l->next->data)->id;
   else
      old_above = None;
   if (old_above != new_above)
   {
      Win *prev = composite_win_find(new_above);
      if (!prev)
      {
         wins = eina_list_remove(wins, w);
         wins = eina_list_prepend(wins, w);
      }
      else
      {
         wins = eina_list_remove(wins, w);
         wins = eina_list_prepend_relative(wins, w, prev);
      }
   }
}

static void
composite_win_destroy_finish(Ecore_X_Window id, Bool gone)
{
   Win *w;
   Eina_List *l;

   for (l = wins; l; l = l->next)
   {
      w = l->data;
      if (w->id == id)
      {
         if (!gone)
            composite_win_finish_unmap(w);
         wins = eina_list_remove(wins, w);
         if (w->picture)
         {
            XRenderFreePicture(dpy, w->picture);
            w->picture = None;
         }
         if (w->alphaPict)
         {
            XRenderFreePicture(dpy, w->alphaPict);
            w->alphaPict = None;
         }
         if (w->shadowPict)
         {
            XRenderFreePicture(dpy, w->shadowPict);
            w->shadowPict = None;
         }
         if (w->damage != None)
         {
            ecore_x_damage_del(w->damage);
            w->damage = None;
         }
         composite_fade_dequeue_win(w);
         E_FREE(w);
         break;
      }
   }
}

#if HAS_NAME_WINDOW_PIXMAP
static void
_composite_destroy_cb(Win * w, Bool gone)
{
   composite_win_destroy_finish(w->id, gone);
}
#endif

static void
composite_win_destroy(Ecore_X_Window id, Bool gone, Bool fade)
{
   Win *w = composite_win_find(id);

#if HAS_NAME_WINDOW_PIXMAP
   if (w && w->pixmap && fade && config->fx_fade_enable)
      composite_fade_set(w, w->opacity * 1.0 / OPAQUE, 0.0,
               config->fx_fade_out_step, _composite_destroy_cb, gone, False,
               True);
   else
#endif
   {
      composite_win_destroy_finish(id, gone);
   }
}

/*
static void
dump_Win (Win *w)
{
    printf ("\t%08lx: %d x %d + %d + %d (%d)\n", w->id,
	    w->a.w, w->a.h, w->a.x, w->a.y, w->a.border);
}


static void
dump_wins (void)
{
    win	*w;

    printf ("windows:\n");
    for (w = list; w; w = w->next)
	dump_Win (w);
}
*/

static void
composite_expose_root(Ecore_X_Window root, Ecore_X_Rectangle * rects, int nrects)
{
   Ecore_X_Region region = ecore_x_region_new(rects, nrects);

   composite_damage_add(region);
}


static void
composite_shadow_color_set(const char *value)
{ 
   unsigned int tmp;
   char **res = NULL;

   tmp = strtoul(value, res, 16);
   if (!value || strlen(value) < 6 || strlen(value) > 8
       || (*(value + 1) == 'x' && strlen(value) < 8) || res != NULL)
   {
      shadowColor.red = 0;
      shadowColor.green = 0;
      shadowColor.blue = 0;
      printf
         ("wrong hexadecimal (use 0xXXXXXX or XXXXXX)! defaulting to black...\n");
      return;
   }
   shadowColor.blue = tmp & 0xff;
   tmp >>= 8;
   shadowColor.green = tmp & 0xff;
   tmp >>= 8;
   shadowColor.red = tmp & 0xff;
}

static int
composite_update(void *data)
{
   if (allDamage && !autoRedirect)
   {
      composite_paint_all(allDamage);
      XSync(dpy, False);
      allDamage = None;
      clipChanged = False;
   }
   return 1;
}


/* Ecore Event Handlers */
static int
_composite_event_window_create_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Create *e;
   e = ev;
   composite_win_add(e->win, 0);
   return 1;
}

static int
_composite_event_window_configure_cb(void *data, int type, void *ev)
{
   Display *dpy = ecore_x_display_get();
   Win *w;
   Ecore_X_Region damage = None;
   Ecore_X_Event_Window_Configure *e;

   e = ev;

   w = composite_win_find(e->win);

   if (!w)
   {
      if (e->win == root)
      {
         if (rootBuffer)
         {
            XRenderFreePicture(dpy, rootBuffer);
            rootBuffer = None;
         }
         root_width = e->w;
         root_height = e->h;
      }
      return 1;
   }
#if CAN_DO_USABLE
   if (w->usable)
#endif
   {
      damage = ecore_x_region_new(NULL, 0);
      if (w->extents != None)
         ecore_x_region_copy(damage, w->extents);
   }
   w->a.x = e->x;
   w->a.y = e->y;
   if (w->a.w != e->w || w->a.h != e->h)
   {
#if HAS_NAME_WINDOW_PIXMAP
      if (w->pixmap)
      {
         XFreePixmap(dpy, w->pixmap);
         w->pixmap = None;
         if (w->picture)
         {
            XRenderFreePicture(dpy, w->picture);
            w->picture = None;
         }
      }
#endif
      if (w->shadow)
      {
         XRenderFreePicture(dpy, w->shadow);
         w->shadow = None;
      }
   }
   w->a.w = e->w;
   w->a.h = e->h;
   w->a.border = e->border;
   w->a.override = e->override;
   composite_win_restack(w, e->abovewin);
   if (damage)
   {
      Ecore_X_Region extents = composite_win_extents(w);

      ecore_x_region_combine(damage, damage, extents);
      ecore_x_region_del(extents);
      composite_damage_add(damage);
   }
   clipChanged = True;
   return 1;
}

static int
_composite_event_window_destroy_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Destroy *e;

   e = ev;
   composite_win_destroy(e->win, true, true);
   return 1;
}

static int
_composite_event_window_hide_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Hide *e;

   e = ev;
   composite_win_unmap(e->win, true);
   return 1;
}

static int
_composite_event_window_reparent_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Reparent *e;
   E_Border *bd;

   e = ev;
   bd = e_border_find_by_window(e->win);
   if (e->parent == root)
      composite_win_add(e->win, 0);
   else
      composite_win_destroy(e->win, false, true);
   return 1;
}


static int
_composite_event_window_show_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Show *e;
   Win *w;

   e = ev;
   w = composite_win_find(e->win);
#if 0
   if (w)
      composite_win_map(e->win, w->fade);
   else
#endif
      composite_win_map(e->win, true);
   return 1;
}

static int
_composite_event_window_stack_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Stack *e;
   Win *w;
   Ecore_X_Window new_above;

   e = ev;
   w = composite_win_find(e->win);

   if (!w)
      return 1;
   if (e->detail == ECORE_X_WINDOW_STACK_ABOVE)
      new_above = ((Win *)(wins->data))->id;
   else
      new_above = None;
   composite_win_restack(w, new_above);
   clipChanged = True;
   return 1;
}

static int
_composite_event_window_focus_in_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Focus_In *e;
   Win *w;
   unsigned int opacity;
   double opacity_pct;

   e = ev;
   w = composite_win_find(e->win);
   if (!w) return 1;
   if (composite_fade_is_valid(e->win))
   {
      opacity_pct = ((double) config->trans_active_value)/100.0;
      opacity = (unsigned int) (OPAQUE * opacity_pct);
      ecore_x_netwm_opacity_set(e->win, opacity);
   }
   return 1;
}

static int
_composite_event_window_focus_out_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Focus_Out *e;
   Win *w;
   unsigned int opacity;
   double opacity_pct;

   e = ev;
   w = composite_win_find(e->win);
   if (!w) return 1;
   if (composite_fade_is_valid(e->win))
   {
      opacity_pct = ((double) config->trans_inactive_value)/100.0;
      opacity = (unsigned int) (OPAQUE * opacity_pct);
      ecore_x_netwm_opacity_set(e->win, opacity);
   }
   return 1;
}  

static int
_composite_event_window_expose_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Damage *e;

   e = ev;

   if (e->win == root)
   {
      int more = e->count + 1;

      if (n_expose == size_expose)
      {
         if (expose_rects)
         {
            expose_rects =
               realloc(expose_rects,
                       (size_expose + more) * sizeof(Ecore_X_Rectangle));
            size_expose += more;
         }
         else
         {
            expose_rects = malloc(more * sizeof(Ecore_X_Rectangle));
            size_expose = more;
         }
      }
      expose_rects[n_expose].x = e->y;
      expose_rects[n_expose].y = e->y;
      expose_rects[n_expose].width = e->w;
      expose_rects[n_expose].height = e->h;
      n_expose++;
      if (e->count == 0)
      {
         composite_expose_root(root, expose_rects, n_expose);
         n_expose = 0;
      }
   }

   return 1;
}

static int
_composite_event_window_property_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Window_Property *e;
   Bling *b;
   int p;

   e = ev;
   b = data;

   for (p = 0; backgroundProps[p]; p++)
   {
      if (e->atom == XInternAtom(dpy, backgroundProps[p], False))
      {
         if (rootTile)
         {
            ecore_x_window_area_expose(root, 0, 0, 0, 0);
            XRenderFreePicture(dpy, rootTile);
            rootTile = None;
            break;
         }
      }
   }

   /* Window set shade? */
   if (e->atom == ECORE_X_ATOM_NET_WM_WINDOW_SHADE)
   {
      Win *w = composite_win_find(e->win);
      if (!w) return 1;
      unsigned int tmp = composite_shade_prop_get(w);

      if (tmp)
      {
         if (tmp == 1)
         {
            w->preShadeOpacity = w->opacity;
            w->opacity = 0;
            composite_win_mode_determine(w);
         }
         else if (tmp == 2)
         {
            w->opacity = w->preShadeOpacity;
            composite_win_mode_determine(w);
         }
      }
      return 1;
   }
   else if (e->atom == ECORE_X_ATOM_NET_WM_WINDOW_OPACITY
            || e->atom == ECORE_X_ATOM_NET_WM_WINDOW_SHADOW)
   {
      Win *w = composite_win_find(e->win);
      if (!w) return 1;
      unsigned int oldShadowSize = w->shadowSize;
      unsigned int tmp;

      if (w)
      {
         if (e->atom == ECORE_X_ATOM_NET_WM_WINDOW_OPACITY)
         {
            tmp = get_opacity_prop(w, OPAQUE);
            if (tmp == w->opacity)
               return 1;
            if (b->config->fx_fade_opacity_enable)
            {
               composite_fade_set(w, w->opacity * 1.0 / OPAQUE, (tmp * 1.0) / OPAQUE,
                        b->config->fx_fade_out_step, 0, False, True, True);
               return 1;
            }
            else
               w->opacity = tmp;
         }
         else
         {
            tmp = get_shadow_prop(w);
            if (tmp == w->shadowSize)
               return 1;           /* Skip if shadow does not change */
            w->shadowSize = tmp;
         }

         if (w->shadow)
         {
            XRenderFreePicture(dpy, w->shadow);
            w->shadow = None;
         }

         /* Catch size changes on cleanup with composite_win_mode_determine */
         if (oldShadowSize < w->shadowSize)
         {
            w->extents = composite_win_extents(w);
            composite_win_mode_determine(w);
         }
         else
         {
            composite_win_mode_determine(w);
            w->extents = composite_win_extents(w);
         }
      }
   }

   return 1;
}

static int
_composite_event_damage_cb(void *data, int type, void *ev)
{
   Ecore_X_Event_Damage *e;
   Bling *b;
   Win *w;

   e = ev;
   b = data;
   w = composite_win_find(e->drawable);
   if (!w)
      return 1;
#if CAN_DO_USABLE
   if (!w->usable);
   {
      if (w->damage_bounds.width == 0 || w->damage_bounds.height == 0)
      {
         /* New damage rectangle */
         w->damage_bounds.x = e->area.x;
         w->damage_bounds.y = e->area.y;
         w->damage_bounds.width = e->area.width;
         w->damage_bounds.height = e->area.height;
      }
      else
      {
         /* Expand damage rectangle to cover new damage */
         if (e->area.x < w->damage_bounds.x)
         {
            w->damage_bounds.width += (w->damage_bounds.x - e->area.x);
            w->damage_bounds.x = e->area.x;
         }
         if (e->area.y < w->damage_bounds.y)
         {
            w->damage_bounds.height += (w->damage_bounds.y - e->area.y);
            w->damage_bounds.y = e->area.y;
         }
         if (e->area.x + e->area.width >
             w->damage_bounds.x + w->damage_bounds.width)
            w->damage_bounds.width =
               e->area.x + e->area.width - w->damage_bounds.x;
         if (e->area.y + e->area.height >
             w->damage_bounds.y + w->damage_bounds.height)
            w->damage_bounds.height =
               e->area.y + e->area.height - w->damage_bounds.y;
      }
      if (w->damage_bounds.x <= 0 && w->damage_bounds.y <= 0
          && w->a.w <= w->damage_bounds.x + w->damage_bounds.width
          && w->a.h <= w->damage_bounds.y + w->damage_bounds.height)
      {
         clipChanged = True;
         if (b->config->fx_fade_in_enable)
            composite_fade_set(w, 0, get_opacity_percent(w, 1.0),
                     b->config->fx_fade_in_step, 0, False, True, True);
         w->usable = True;
      }
   }
   if (w->usable)
#endif
      composite_win_repair(w);
   return 1;
}

int
composite_init(Bling *b)
{
   Ecore_X_Window *children;
   int nchildren;
   int i;
   XRenderPictureAttributes pa;
   int composite_major, composite_minor;

   bling = b;
   config = b->config;

   composite_shadow_color_set(config->shadow_color);

   compMode = CompClientShadows;	/* Not 
	   making 
	   this 
	   an 
	   option 
	 */
   autoRedirect = 0;

   dpy = ecore_x_display_get();
#if 0
   XSetErrorHandler(composite_x_error);
#endif
   if (synchronize)
      ecore_x_sync();
   scr = DefaultScreen(dpy);
   root = RootWindow(dpy, scr);

   if (!XRenderQueryExtension(dpy, &render_event, &render_error))
   {
      e_error_message_show("Unable to load Bling module:<br>"
                           "Your X server does not have the Render extension.<br>");
      return 0;
   }
   if (!XQueryExtension
       (dpy, COMPOSITE_NAME, &composite_opcode, &composite_event,
        &composite_error))
   {
      e_error_message_show("Unable to load Bling module:<br>"
                           "Your X server does not have the Composite extension.<br>"
                           "You may need to enable this manually in your X configuration.<br>");
      return 0;
   }
   XCompositeQueryVersion(dpy, &composite_major, &composite_minor);
#if HAS_NAME_WINDOW_PIXMAP
   if (composite_major > 0 || composite_minor >= 2)
      hasNamePixmap = True;
#endif

   pa.subwindow_mode = IncludeInferiors;

   if (compMode == CompClientShadows)
   {
      gaussianMap = make_gaussian_map(config->shadow_active_size);
      presum_gaussian(gaussianMap);
   }

   root_width = DisplayWidth(dpy, scr);
   root_height = DisplayHeight(dpy, scr);

   rootPicture =
      XRenderCreatePicture(dpy, root,
                           XRenderFindVisualFormat(dpy,
                                                   DefaultVisual(dpy, scr)),
                           CPSubwindowMode, &pa);
   blackPicture =
      solid_picture(True, 1, (double) (shadowColor.red) / 0xff,
                    (double) (shadowColor.green) / 0xff,
                    (double) (shadowColor.blue) / 0xff);
   if (compMode == CompServerShadows)
      transBlackPicture = solid_picture(True, 0.3, 0, 0, 0);
   allDamage = None;
   clipChanged = True;

   if (!reg)
     {
	Ecore_X_Atom a;
	char buf[16];

	snprintf(buf, sizeof(buf), "_NET_WM_CM_S%d", scr);
	a = XInternAtom(dpy, buf, False);
	reg = ecore_x_window_input_new(0, 0, 0, 1, 1);
	XSetSelectionOwner(dpy, a, reg, 0);
     }

   ecore_x_grab();
   if (autoRedirect)
      XCompositeRedirectSubwindows(dpy, root, CompositeRedirectAutomatic);
   else
   {
      printf("Composite: Manual Redirect Mode Enabling...\n");
      XCompositeRedirectSubwindows(dpy, root, CompositeRedirectManual);
      children = ecore_x_window_children_get(root, &nchildren);
      if (!children)
         return 0;              /* FIXME */
      for (i = 0; i < nchildren; i++)
         composite_win_add(children[i], i ? children[i - 1] : None);
      free(children);

      _window_create_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_CREATE, _composite_event_window_create_cb, b);
      _window_configure_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_CONFIGURE, _composite_event_window_configure_cb, b);
      _window_destroy_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, _composite_event_window_destroy_cb, b);
      _window_hide_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_HIDE, _composite_event_window_hide_cb, b);
      _window_reparent_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_REPARENT, _composite_event_window_reparent_cb, b);
      _window_show_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_SHOW, _composite_event_window_show_cb, b);
      _window_stack_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_STACK, _composite_event_window_stack_cb, b);
      _window_focus_in_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_IN, _composite_event_window_focus_in_cb, b);
      _window_focus_out_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_OUT, _composite_event_window_focus_out_cb, b);
      _window_damage_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DAMAGE, _composite_event_window_expose_cb, b);
      _window_property_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, _composite_event_window_property_cb, b);
      _damage_notify_hnd = ecore_event_handler_add(ECORE_X_EVENT_DAMAGE_NOTIFY, _composite_event_damage_cb, b);
   }
   ecore_x_ungrab();
   if (!autoRedirect)
      composite_paint_all(None);
   idler = ecore_idle_enterer_add(composite_update, NULL);

   return 1;
}

void
composite_shutdown(void)
{
   Eina_List *l;

   for (l = fades; l; l = l->next)
   {
      Fade *f = l->data;
      ecore_animator_del(f->anim);
      E_FREE(f);
   }

   ecore_idle_enterer_del(idler);
   ecore_event_handler_del(_window_create_hnd);
   ecore_event_handler_del(_window_configure_hnd);
   ecore_event_handler_del(_window_destroy_hnd);
   ecore_event_handler_del(_window_hide_hnd);
   ecore_event_handler_del(_window_reparent_hnd);
   ecore_event_handler_del(_window_show_hnd);
   ecore_event_handler_del(_window_stack_hnd);
   ecore_event_handler_del(_window_focus_in_hnd);
   ecore_event_handler_del(_window_focus_out_hnd);
   ecore_event_handler_del(_window_damage_hnd);
   ecore_event_handler_del(_window_property_hnd);
   ecore_event_handler_del(_damage_notify_hnd);

   ecore_x_window_del(reg);

   XCompositeUnredirectSubwindows(dpy, root, CompositeRedirectManual);
   XRenderFreePicture(dpy, rootPicture);
   XRenderFreePicture(dpy, blackPicture);
   XRenderFreePicture(dpy, rootBuffer);
   XRenderFreePicture(dpy, transBlackPicture);
   XRenderFreePicture(dpy, rootTile);
}
   
