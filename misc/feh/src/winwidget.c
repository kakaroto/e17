/* winwidget.c
 *
 * Copyright (C) 2000 Tom Gilbert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "feh.h"
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

   D_ENTER;
   ret = emalloc(sizeof(_winwidget));

   ret->win = 0;
   ret->w = 0;
   ret->h = 0;
   ret->im_w = 0;
   ret->im_h = 0;
   ret->bg_pmap = 0;
   ret->im = NULL;
   ret->name = NULL;
   ret->file = NULL;
   ret->type = WIN_TYPE_UNSET;

   /* Zoom stuff */
   ret->mode = MODE_NORMAL;

   ret->gc = None;

   /* New stuff */
   ret->im_x = 0;
   ret->im_y = 0;
   ret->zoom_percent = 100;

   D_RETURN(ret);
}

winwidget winwidget_create_from_image(Imlib_Image im, char *name, char type)
{
   winwidget ret = NULL;

   D_ENTER;

   if (im == NULL)
      D_RETURN(NULL);

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

   D_RETURN(ret);
}

winwidget winwidget_create_from_file(feh_file * file, char *name, char type)
{
   winwidget ret = NULL;

   D_ENTER;

   if (!file || !file->filename)
      D_RETURN(NULL);

   ret = winwidget_allocate();
   ret->type = type;
   if (name)
      ret->name = estrdup(name);
   else
      ret->name = estrdup(file->filename);

   if (opt.progressive)
   {
      D(("Progressive loading enabled\n"));
      progwin = ret;
      imlib_context_set_progress_function(progressive_load_cb);
      imlib_context_set_progress_granularity(PROGRESS_GRANULARITY);
   }

   if (winwidget_loadimage(ret, file) == 0)
   {
      if (opt.progressive)
         winwidget_destroy(ret);
      D_RETURN(NULL);
   }

   if (!opt.progressive)
   {
      ret->w = ret->im_w = feh_imlib_image_get_width(ret->im);
      ret->h = ret->im_h = feh_imlib_image_get_height(ret->im);
      D(
        ("image is %dx%d pixels, format %s\n", ret->w, ret->h,
         feh_imlib_image_format(ret->im)));
      winwidget_create_window(ret, ret->w, ret->h);
      winwidget_render_image(ret, 1, 1);
   }

   D_RETURN(ret);
}

void
winwidget_create_window(winwidget ret, int w, int h)
{
   XSetWindowAttributes attr;
   XClassHint *xch;
   MWMHints mwmhints;
   Atom prop = None;

   D_ENTER;

   if (opt.full_screen)
   {
      w = scr->width;
      h = scr->height;
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
   D_RETURN_;
}

void
winwidget_update_title(winwidget ret)
{
   D_ENTER;
   if (ret->name)
      XStoreName(disp, ret->win, ret->name);
   else
      XStoreName(disp, ret->win, "feh");
   D_RETURN_;
}

void
winwidget_setup_pixmaps(winwidget winwid)
{
   D_ENTER;

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
         D(("recreating background pixmap (%dx%d)\n", winwid->w, winwid->h));
         if (winwid->bg_pmap)
            XFreePixmap(disp, winwid->bg_pmap);

         winwid->bg_pmap =
            XCreatePixmap(disp, winwid->win, winwid->w, winwid->h, depth);
         winwid->had_resize = 0;
      }
   }
   D_RETURN_;
}

void
winwidget_render_image(winwidget winwid, int resize, int alias)
{
   int x = 0, y = 0;
   int www, hhh;
   int need_resize = 0;

   D_ENTER;

   if (!opt.full_screen && resize)
   {
      winwidget_clear_background(winwid);
      winwidget_resize(winwid, winwid->im_w, winwid->im_h);
   }

   winwidget_setup_pixmaps(winwid);

   if (!opt.full_screen
       && ((feh_imlib_image_has_alpha(winwid->im))
           || (winwid->im_x || winwid->im_y)))
      feh_draw_checks(winwid);

   if (opt.full_screen)
   {
      int smaller;              /* Is the image smaller than screen? */

      D(("Calculating for fullscreen render\n"));
      smaller = ((winwid->im_w < scr->width) && (winwid->im_h < scr->height));

      if (opt.auto_zoom && !(smaller && !opt.stretch))
      {
         www = scr->width;
         hhh = scr->height;

         /* FIXME Very similar aspect code exists in montage.c */

         if (opt.aspect)
         {
            double ratio = 0.0;

            ratio =
               ((double) winwid->im_w / winwid->im_h) / ((double) www / hhh);

            if (ratio > 1.0)
            {
               hhh /= ratio;
               y = (scr->height - hhh) >> 1;
            }
            else if (ratio != 1.0)
            {
               www *= ratio;
               x = (scr->width - www) >> 1;
            }
         }
         feh_imlib_render_image_on_drawable_at_size(winwid->bg_pmap,
                                                    winwid->im, x, y, www,
                                                    hhh, 1,
                                                    feh_imlib_image_has_alpha
                                                    (winwid->im), alias);
      }
      else
      {
         x = (scr->width - winwid->im_w) >> 1;
         y = (scr->height - winwid->im_h) >> 1;
         feh_imlib_render_image_on_drawable(winwid->bg_pmap, winwid->im, x, y,
                                            1,
                                            feh_imlib_image_has_alpha
                                            (winwid->im), alias);
      }
   }
   else
   {
      /* resize window if the image size has changed */
      D(("rendering image normally\n"));
      feh_imlib_render_image_on_drawable_at_size(winwid->bg_pmap, winwid->im,
                                                 winwid->im_x, winwid->im_y,
                                                 winwid->im_w *
                                                 PERCENT(winwid->
                                                         zoom_percent),
                                                 winwid->im_h *
                                                 PERCENT(winwid->
                                                         zoom_percent), 1,
                                                 feh_imlib_image_has_alpha
                                                 (winwid->im), alias);
   }
   if (need_resize)
      winwidget_resize(winwid, winwid->im_w, winwid->im_h);

   XSetWindowBackgroundPixmap(disp, winwid->win, winwid->bg_pmap);
   XClearWindow(disp, winwid->win);
   D_RETURN_;
}

Pixmap
feh_create_checks(void)
{
   static Pixmap checks_pmap = None;
   Imlib_Image checks = NULL;

   D_ENTER;
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
   D_RETURN(checks_pmap);
}

void
winwidget_clear_background(winwidget w)
{
   D_ENTER;
   XSetWindowBackgroundPixmap(disp, w->win, feh_create_checks());
   D_RETURN_;
}

void
feh_draw_checks(winwidget win)
{
   static GC gc = None;
   XGCValues gcval;

   D_ENTER;

   if (gc == None)
   {
      gcval.tile = feh_create_checks();
      gcval.fill_style = FillTiled;
      gc = XCreateGC(disp, win->win, GCTile | GCFillStyle, &gcval);
   }
   XFillRectangle(disp, win->bg_pmap, gc, 0, 0, win->w, win->h);
   D_RETURN_;
}

void
winwidget_destroy(winwidget winwid)
{
   D_ENTER;
   winwidget_unregister(winwid);
   if (winwid->win)
      XDestroyWindow(disp, winwid->win);
   if (winwid->bg_pmap)
      XFreePixmap(disp, winwid->bg_pmap);
   if (winwid->name)
      free(winwid->name);
   if ((winwid->type == WIN_TYPE_ABOUT) && winwid->file)
      feh_file_free(winwid->file);
   if (winwid->im)
      feh_imlib_free_image_and_decache(winwid->im);
   free(winwid);
   D_RETURN_;
}

void
winwidget_destroy_all(void)
{
   int i;

   D_ENTER;
   /* Have to DESCEND the list here, 'cos of the way _unregister works */
   for (i = window_num - 1; i >= 0; i--)
      winwidget_destroy(windows[i]);
   D_RETURN_;
}

int
winwidget_loadimage(winwidget winwid, feh_file * file)
{
   D_ENTER;
   D(("filename %s\n", file->filename));
   D_RETURN(feh_load_image(&(winwid->im), file));
}

void
winwidget_show(winwidget winwid)
{
   XEvent ev;

   D_ENTER;
   XMapWindow(disp, winwid->win);
   /* wait for the window to map */
   D(("Waiting for window to map\n"));
   XMaskEvent(disp, StructureNotifyMask, &ev);
   D(("Window mapped\n"));
   D_RETURN_;
}

void
winwidget_resize(winwidget winwid, int w, int h)
{
   D_ENTER;
   if (winwid && ((winwid->w != w) || (winwid->h != h)))
   {
      D(("Really doing a resize\n"));
      XResizeWindow(disp, winwid->win, w, h);
      winwid->w = w;
      winwid->h = h;
      winwid->had_resize = 1;
   }
   else
   {
      D(("No resize actually needed\n"));
   }
   D_RETURN_;
}

void
winwidget_hide(winwidget winwid)
{
   D_ENTER;
   XUnmapWindow(disp, winwid->win);
   D_RETURN_;
}

static void
winwidget_register(winwidget win)
{
   D_ENTER;
   D(("window %p\n", win));
   window_num++;
   if (windows)
      windows = erealloc(windows, window_num * sizeof(winwidget));
   else
      windows = emalloc(window_num * sizeof(winwidget));
   windows[window_num - 1] = win;

   XSaveContext(disp, win->win, xid_context, (XPointer) win);
   D_RETURN_;
}

static void
winwidget_unregister(winwidget win)
{
   int i, j;

   D_ENTER;
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
   D_RETURN_;
}

winwidget winwidget_get_from_window(Window win)
{
   winwidget ret = NULL;

   D_ENTER;
   if (XFindContext(disp, win, xid_context, (XPointer *) & ret) != XCNOENT)
      D_RETURN(ret);
   D_RETURN(NULL);
}

void
winwidget_rename(winwidget winwid, char *newname)
{
   if (winwid->name)
      free(winwid->name);
   winwid->name = newname;
   winwidget_update_title(winwid);
}

void
winwidget_free_image(winwidget w)
{
   if (w->im)
      feh_imlib_free_image_and_decache(w->im);
   w->im = NULL;
   w->im_w = 0;
   w->im_h = 0;
}
