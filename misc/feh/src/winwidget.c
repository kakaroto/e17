/* winwidget.c

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

#include "feh.h"
#include "feh_list.h"
#include "filelist.h"
#include "winwidget.h"
#include "options.h"

static void winwidget_unregister(winwidget win);
static void winwidget_register(winwidget win);
static winwidget winwidget_allocate(void);

int window_num = 0;             /* For window list */
winwidget *windows = NULL;      /* List of windows to loop though */

static winwidget
winwidget_allocate(void)
{
   winwidget ret = NULL;

   D_ENTER(4);
   ret = emalloc(sizeof(_winwidget));

   ret->win = 0;
   ret->w = 0;
   ret->h = 0;
   ret->im_w = 0;
   ret->im_h = 0;
   ret->im_angle = 0;
   ret->bg_pmap = 0;
   ret->im = NULL;
   ret->name = NULL;
   ret->file = NULL;
   ret->type = WIN_TYPE_UNSET;
   ret->visible = 0;

   /* Zoom stuff */
   ret->mode = MODE_NORMAL;

   ret->gc = None;

   /* New stuff */
   ret->im_x = 0;
   ret->im_y = 0;
   ret->zoom = 1.0;

   ret->click_offset_x = 0;
   ret->click_offset_y = 0;
   ret->im_click_offset_x = 0;
   ret->im_click_offset_y = 0;
   ret->has_rotated = 0;

   D_RETURN(4, ret);
}

winwidget
winwidget_create_from_image(Imlib_Image im, char *name, char type)
{
   winwidget ret = NULL;

   D_ENTER(4);

   if (im == NULL)
      D_RETURN(4, NULL);

   ret = winwidget_allocate();
   ret->type = type;

   ret->im = im;
   ret->w = ret->im_w = feh_imlib_image_get_width(ret->im);
   ret->h = ret->im_h = feh_imlib_image_get_height(ret->im);

   if (name)
      ret->name = estrdup(name);
   else
      ret->name = estrdup(PACKAGE);

   winwidget_create_window(ret, ret->w, ret->h);
   winwidget_render_image(ret, 1, 1);

   D_RETURN(4, ret);
}

winwidget
winwidget_create_from_file(feh_list * list, char *name, char type)
{
   winwidget ret = NULL;
   feh_file *file = FEH_FILE(list->data);

   D_ENTER(4);

   if (!file || !file->filename)
      D_RETURN(4, NULL);

   ret = winwidget_allocate();
   ret->file = list;
   ret->type = type;
   if (name)
      ret->name = estrdup(name);
   else
      ret->name = estrdup(file->filename);

   if (winwidget_loadimage(ret, file) == 0)
   {
      winwidget_destroy(ret);
      D_RETURN(4, NULL);
   }

   if (!ret->win)
   {
      ret->w = ret->im_w = feh_imlib_image_get_width(ret->im);
      ret->h = ret->im_h = feh_imlib_image_get_height(ret->im);
      D(3,
        ("image is %dx%d pixels, format %s\n", ret->w, ret->h,
         feh_imlib_image_format(ret->im)));
      winwidget_create_window(ret, ret->w, ret->h);
      winwidget_render_image(ret, 1, 1);
   }

   D_RETURN(4, ret);
}

void
winwidget_create_window(winwidget ret, int w, int h)
{
   XSetWindowAttributes attr;
   XClassHint *xch;
   MWMHints mwmhints;
   Atom prop = None;

   D_ENTER(4);

   if (opt.full_screen)
   {
      w = scr->width;
      h = scr->height;
   }
   else if(opt.geom)
   {
      w = opt.geom_w;
      h = opt.geom_h;
   }
   else
   {
      if (w > scr->width)
         w = scr->width;
      if (h > scr->height)
         h = scr->height;
   }

   ret->w = w;
   ret->h = h;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.colormap = cm;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   attr.event_mask =
      StructureNotifyMask | ButtonPressMask | ButtonReleaseMask |
      PointerMotionMask | EnterWindowMask | LeaveWindowMask | KeyPressMask |
      KeyReleaseMask | ButtonMotionMask | ExposureMask | FocusChangeMask |
      PropertyChangeMask | VisibilityChangeMask;

   if (opt.borderless || opt.full_screen)
   {
      prop = XInternAtom(disp, "_MOTIF_WM_HINTS", True);
      if (prop == None)
      {
         weprintf("Window Manager does not support MWM hints. "
                  "To get a borderless window I have to bypass your wm.");
         attr.override_redirect = True;
         mwmhints.flags = 0;
      }
      else
      {
         mwmhints.flags = MWM_HINTS_DECORATIONS;
         mwmhints.decorations = 0;
      }
   }
   else
      mwmhints.flags = 0;

   ret->win =
      XCreateWindow(disp, DefaultRootWindow(disp), 0, 0, w, h, 0, depth,
                    InputOutput, vis,
                    CWOverrideRedirect | CWSaveUnder | CWBackingStore |
                    CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
                    &attr);

   if (mwmhints.flags)
   {
      XChangeProperty(disp, ret->win, prop, prop, 32, PropModeReplace,
                      (unsigned char *) &mwmhints, PROP_MWM_HINTS_ELEMENTS);
   }

   XSetWMProtocols(disp, ret->win, &wmDeleteWindow, 1);
   winwidget_update_title(ret);
   xch = XAllocClassHint();
   xch->res_name = "feh";
   xch->res_class = "feh";
   XSetClassHint(disp, ret->win, xch);
   XFree(xch);

   /* Size hints */
   if (opt.full_screen)
   {
      XSizeHints xsz;

      xsz.flags = USPosition;
      xsz.x = 0;
      xsz.y = 0;
      XSetWMNormalHints(disp, ret->win, &xsz);
   }

   /* set the icon name property */
   XSetIconName(disp, ret->win, "feh");
   /* set the command hint */
   XSetCommand(disp, ret->win, cmdargv, cmdargc);

   winwidget_register(ret);
   D_RETURN_(4);
}

void
winwidget_update_title(winwidget ret)
{
   D_ENTER(4);
   D(4, ("winwid->name = %s\n", ret->name));
   if (ret->name)
      XStoreName(disp, ret->win, ret->name);
   else
      XStoreName(disp, ret->win, "feh");
   XFlush(disp);
   D_RETURN_(4);
}

void
winwidget_setup_pixmaps(winwidget winwid)
{
   D_ENTER(4);

   if (opt.full_screen)
   {
      if (!(winwid->bg_pmap))
      {
         if (winwid->gc == None)
         {
            XGCValues gcval;

            gcval.foreground = BlackPixel(disp, DefaultScreen(disp));
            winwid->gc = XCreateGC(disp, winwid->win, GCForeground, &gcval);
         }
         winwid->bg_pmap =
            XCreatePixmap(disp, winwid->win, scr->width, scr->height, depth);
      }
      XFillRectangle(disp, winwid->bg_pmap, winwid->gc, 0, 0, scr->width,
                     scr->height);
   }
   else
   {
      if (!winwid->bg_pmap || winwid->had_resize)
      {
         D(4,
           ("recreating background pixmap (%dx%d)\n", winwid->w, winwid->h));
         if (winwid->bg_pmap)
            XFreePixmap(disp, winwid->bg_pmap);

         if (winwid->w == 0)
            winwid->w = 1;
         if (winwid->h == 0)
            winwid->h = 1;
         winwid->bg_pmap =
            XCreatePixmap(disp, winwid->win, winwid->w, winwid->h, depth);
         winwid->had_resize = 0;
      }
   }
   D_RETURN_(4);
}

void
winwidget_render_image(winwidget winwid, int resize, int alias)
{
   int sx, sy, sw, sh, dx, dy, dw, dh;
   int calc_w, calc_h;

   D_ENTER(4);

   if (!opt.full_screen && resize)
   {
      winwidget_resize(winwid, winwid->im_w, winwid->im_h);
      winwidget_reset_image(winwid);
   }

   /* bounds checks for panning */
   if (winwid->im_x > winwid->w)
      winwid->im_x = winwid->w;
   if (winwid->im_y > winwid->h)
      winwid->im_y = winwid->h;

   winwidget_setup_pixmaps(winwid);

   if (!opt.full_screen
       && ((feh_imlib_image_has_alpha(winwid->im))
           || (winwid->im_x || winwid->im_y) || (winwid->zoom != 1.0)
           || (winwid->w > winwid->im_w || winwid->h > winwid->im_h)
           || (winwid->has_rotated)))
      feh_draw_checks(winwid);


   if (!opt.full_screen && opt.scale_down
       && ((winwid->w < winwid->im_w) || (winwid->h < winwid->im_h)))
   {
      D(2, ("scaling down image\n"));

      feh_calc_needed_zoom(&(winwid->zoom), winwid->im_w, winwid->im_h,
                           winwid->w, winwid->h);
      winwidget_resize(winwid, winwid->im_w * winwid->zoom,
                       winwid->im_h * winwid->zoom);
   }

   if (resize && (opt.full_screen || opt.geom))
   {
      int smaller;              /* Is the image smaller than screen? */
      int max_w, max_h;
      if(opt.full_screen)
      {
         max_w = scr->width;
         max_h = scr->height;
      }
      else if(opt.geom)
      {
         max_w = opt.geom_w;
         max_h = opt.geom_h;
      }

      D(4, ("Calculating for fullscreen/fixed geom render\n"));
      smaller = ((winwid->im_w < max_w) && (winwid->im_h < max_h));

      if (!smaller || opt.auto_zoom)
      {
         double ratio = 0.0;

         /* Image is larger than the screen (so want's shrinking), or it's
            smaller but wants expanding to fill it */
         ratio =
            feh_calc_needed_zoom(&(winwid->zoom), winwid->im_w, winwid->im_h,
                                 max_w, max_h);
         if (ratio > 1.0)
         {
            /* height is the factor */
            winwid->im_x = 0;
            winwid->im_y =
               ((int) (max_h - (winwid->im_h * winwid->zoom))) >> 1;
         }
         else
         {
            /* width is the factor */
            winwid->im_x =
               ((int) (max_w - (winwid->im_w * winwid->zoom))) >> 1;
            winwid->im_y = 0;
         }
      }
      else
      {
         /* Just center the image in the window */
         winwid->zoom = 1.0;
         winwid->im_x = (max_w - winwid->im_w) >> 1;
         winwid->im_y = (max_h - winwid->im_h) >> 1;
      }
   }

   /* Now we ensure only to render the area we're looking at */
   dx = winwid->im_x;
   dy = winwid->im_y;
   if (dx < 0)
      dx = 0;
   if (dy < 0)
      dy = 0;

   if (winwid->im_x < 0)
   {
      if (winwid->zoom < 1.0)
         sx = 0 - (winwid->im_x * winwid->zoom);
      else
         sx = 0 - (winwid->im_x / winwid->zoom);
   }
   else
      sx = 0;

   if (winwid->im_y < 0)
   {
      if (winwid->zoom < 1.0)
         sy = 0 - (winwid->im_y * winwid->zoom);
      else
         sy = 0 - (winwid->im_y / winwid->zoom);
   }
   else
      sy = 0;
   calc_w = winwid->im_w * winwid->zoom;
   calc_h = winwid->im_h * winwid->zoom;
   dw = (winwid->w - winwid->im_x);
   dh = (winwid->h - winwid->im_y);
   if (calc_w < dw)
      dw = calc_w;
   if (calc_h < dh)
      dh = calc_h;
   if (dw > winwid->w)
      dw = winwid->w;
   if (dh > winwid->h)
      dh = winwid->h;

   sw = dw / winwid->zoom;
   sh = dh / winwid->zoom;

   D(5,
     ("sx: %d sy: %d sw: %d sh: %d dx: %d dy: %d dw: %d dh: %d zoom: %f\n",
      sx, sy, sw, sh, dx, dy, dw, dh, winwid->zoom));

   D(5, ("winwidget_render(): winwid->im_angle = %f\n", winwid->im_angle));
   if (winwid->has_rotated)
      feh_imlib_render_image_part_on_drawable_at_size_with_rotation
         (winwid->bg_pmap, winwid->im, sx, sy, sw, sh, dx, dy, dw, dh,
          winwid->im_angle, 1, 1, alias);
   else
      feh_imlib_render_image_part_on_drawable_at_size(winwid->bg_pmap,
                                                      winwid->im, sx, sy, sw,
                                                      sh, dx, dy, dw, dh, 1,
                                                      feh_imlib_image_has_alpha
                                                      (winwid->im), alias);

   XSetWindowBackgroundPixmap(disp, winwid->win, winwid->bg_pmap);
   XClearWindow(disp, winwid->win);
   D_RETURN_(4);
}

double
feh_calc_needed_zoom(double *zoom, int orig_w, int orig_h, int dest_w,
                     int dest_h)
{
   double ratio = 0.0;

   D_ENTER(4);

   ratio = ((double) orig_w / orig_h) / ((double) dest_w / dest_h);

   if (ratio > 1.0)
      *zoom = ((double) dest_w / orig_w);
   else if (ratio != 1.0)
      *zoom = ((double) dest_h / orig_h);
   else
      *zoom = 1.0;

   D_RETURN(4, ratio);
}

Pixmap feh_create_checks(void)
{
   static Pixmap checks_pmap = None;
   Imlib_Image checks = NULL;

   D_ENTER(4);
   if (checks_pmap == None)
   {
      int onoff, x, y;

      checks = imlib_create_image(16, 16);

      if (!checks)
         eprintf
            ("Unable to create a teeny weeny imlib image. I detect problems");

      for (y = 0; y < 16; y += 8)
      {
         onoff = (y / 8) & 0x1;
         for (x = 0; x < 16; x += 8)
         {
            if (onoff)
               feh_imlib_image_fill_rectangle(checks, x, y, 8, 8, 144, 144,
                                              144, 255);
            else
               feh_imlib_image_fill_rectangle(checks, x, y, 8, 8, 100, 100,
                                              100, 255);
            onoff++;
            if (onoff == 2)
               onoff = 0;
         }
      }
      checks_pmap = XCreatePixmap(disp, root, 16, 16, depth);
      feh_imlib_render_image_on_drawable(checks_pmap, checks, 0, 0, 1, 0, 0);
   }
   D_RETURN(4, checks_pmap);
}

void
winwidget_clear_background(winwidget w)
{
   D_ENTER(4);
   XSetWindowBackgroundPixmap(disp, w->win, feh_create_checks());
   D_RETURN_(4);
}

void
feh_draw_checks(winwidget win)
{
   static GC gc = None;
   XGCValues gcval;

   D_ENTER(4);
   if (gc == None)
   {
      gcval.tile = feh_create_checks();
      gcval.fill_style = FillTiled;
      gc = XCreateGC(disp, win->win, GCTile | GCFillStyle, &gcval);
   }
   XFillRectangle(disp, win->bg_pmap, gc, 0, 0, win->w, win->h);
   D_RETURN_(4);
}

void
winwidget_destroy(winwidget winwid)
{
   D_ENTER(4);
   if (winwid->win)
   {
      winwidget_unregister(winwid);
      XDestroyWindow(disp, winwid->win);
   }
   if (winwid->bg_pmap)
      XFreePixmap(disp, winwid->bg_pmap);
   if (winwid->name)
      free(winwid->name);
   if ((winwid->type == WIN_TYPE_ABOUT) && winwid->file)
   {
      feh_file_free(FEH_FILE(winwid->file->data));
      free(winwid->file);
   }
   if (winwid->im)
      feh_imlib_free_image_and_decache(winwid->im);
   free(winwid);
   D_RETURN_(4);
}

void
winwidget_destroy_all(void)
{
   int i;

   D_ENTER(4);
   /* Have to DESCEND the list here, 'cos of the way _unregister works */
   for (i = window_num - 1; i >= 0; i--)
      winwidget_destroy(windows[i]);
   D_RETURN_(4);
}

winwidget winwidget_get_first_window_of_type(unsigned int type)
{
   int i;

   D_ENTER(4);
   for (i = 0; i < window_num; i++)
      if (windows[i]->type == type)
         D_RETURN(4, windows[i]);
   D_RETURN(4, NULL);
}

int
winwidget_loadimage(winwidget winwid, feh_file * file)
{
   D_ENTER(4);
   D(4, ("filename %s\n", file->filename));
   progwin = winwid;
   D_RETURN(4, feh_load_image(&(winwid->im), file));
}

void
winwidget_show(winwidget winwid)
{
   XEvent ev;

   D_ENTER(4);

   /* feh_debug_print_winwid(winwid); */
   if (!winwid->visible)
   {
      XMapWindow(disp, winwid->win);
      /* wait for the window to map */
      D(4, ("Waiting for window to map\n"));
      XMaskEvent(disp, StructureNotifyMask, &ev);
      D(4, ("Window mapped\n"));
      winwid->visible = 1;
   }
   D_RETURN_(4);
}

void
winwidget_resize(winwidget winwid, int w, int h)
{
   D_ENTER(4);
   if(opt.geom)
   {
      winwidget_clear_background(winwid);
      winwid->had_resize = 1;
      return;
   }
   if (winwid && ((winwid->w != w) || (winwid->h != h)))
   {
      D(4, ("Really doing a resize\n"));
      /* winwidget_clear_background(winwid); */
      winwid->w = (w > scr->width) ? scr->width : w;
      winwid->h = (h > scr->height) ? scr->height : h;
      XResizeWindow(disp, winwid->win, winwid->w, winwid->h);
      winwid->had_resize = 1;
   }
   else
   {
      D(4, ("No resize actually needed\n"));
   }
   D_RETURN_(4);
}

void
winwidget_hide(winwidget winwid)
{
   D_ENTER(4);
   XUnmapWindow(disp, winwid->win);
   winwid->visible = 0;
   D_RETURN_(4);
}

static void
winwidget_register(winwidget win)
{
   D_ENTER(4);
   D(5, ("window %p\n", win));
   window_num++;
   if (windows)
      windows = erealloc(windows, window_num * sizeof(winwidget));
   else
      windows = emalloc(window_num * sizeof(winwidget));
   windows[window_num - 1] = win;

   XSaveContext(disp, win->win, xid_context, (XPointer) win);
   D_RETURN_(4);
}

static void
winwidget_unregister(winwidget win)
{
   int i, j;

   D_ENTER(4);
   for (i = 0; i < window_num; i++)
   {
      if (windows[i] == win)
      {
         for (j = i; j < window_num - 1; j++)
            windows[j] = windows[j + 1];
         window_num--;
         if (window_num > 0)
            windows = erealloc(windows, window_num * sizeof(winwidget));
         else
         {
            free(windows);
            windows = NULL;
         }
      }
   }
   XDeleteContext(disp, win->win, xid_context);
   D_RETURN_(4);
}

winwidget
winwidget_get_from_window(Window win)
{
   winwidget ret = NULL;

   D_ENTER(4);
   if (XFindContext(disp, win, xid_context, (XPointer *) & ret) != XCNOENT)
      D_RETURN(4, ret);
   D_RETURN(4, NULL);
}

void
winwidget_rename(winwidget winwid, char *newname)
{
   D_ENTER(4);
   if (winwid->name)
      free(winwid->name);
   winwid->name = estrdup(newname);
   winwidget_update_title(winwid);
   D_RETURN_(4);
}

void
winwidget_free_image(winwidget w)
{
   D_ENTER(4);
   if (w->im)
      feh_imlib_free_image_and_decache(w->im);
   w->im = NULL;
   w->im_w = 0;
   w->im_h = 0;
   D_RETURN_(4);
}

void
feh_debug_print_winwid(winwidget w)
{
   printf("winwid_debug:\n" "winwid = %p\n" "win = %ld\n" "w = %d\n"
          "h = %d\n" "im_w = %d\n" "im_h = %d\n" "im_angle = %f\n"
          "type = %d\n" "had_resize = %d\n" "im = %p\n" "GC = %p\n"
          "pixmap = %ld\n" "name = %s\n" "file = %p\n" "mode = %d\n"
          "im_x = %d\n" "im_y = %d\n" "zoom = %f\n" "click_offset_x = %d\n"
          "click_offset_y = %d\n" "im_click_offset_x = %d\n"
          "im_click_offset_y = %d\n" "has_rotated = %d\n", w, w->win, w->w,
          w->h, w->im_w, w->im_h, w->im_angle, w->type, w->had_resize, w->im,
          w->gc, w->bg_pmap, w->name, w->file, w->mode, w->im_x, w->im_y,
          w->zoom, w->click_offset_x, w->click_offset_y, w->im_click_offset_x,
          w->im_click_offset_y, w->has_rotated);
}

void
winwidget_reset_image(winwidget winwid)
{
   D_ENTER(4);
   winwid->zoom = 1.0;
   winwid->im_x = 0;
   winwid->im_y = 0;
   winwid->im_angle = 0.0;
   winwid->has_rotated = 0;
   D_RETURN_(4);
}

void
winwidget_sanitise_offsets(winwidget winwid)
{
   int far_left, far_top;
   int min_x, max_x, max_y, min_y;

   D_ENTER(4);

   far_left = winwid->w - (winwid->im_w * winwid->zoom);
   far_top = winwid->h - (winwid->im_h * winwid->zoom);

   if ((winwid->im_w * winwid->zoom) > winwid->w)
   {
      min_x = far_left;
      max_x = 0;
   }
   else
   {
      min_x = 0;
      max_x = far_left;
   }
   if ((winwid->im_h * winwid->zoom) > winwid->h)
   {
      min_y = far_top;
      max_y = 0;
   }
   else
   {
      min_y = 0;
      max_y = far_top;
   }
   if (winwid->im_x > max_x)
      winwid->im_x = max_x;
   if (winwid->im_x < min_x)
      winwid->im_x = min_x;
   if (winwid->im_y > max_y)
      winwid->im_y = max_y;
   if (winwid->im_y < min_y)
      winwid->im_y = min_y;

   D_RETURN_(4);
}
