/* main.c
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
#include "timers.h"
#include "options.h"

char **cmdargv = NULL;
int cmdargc = 0;
int call_level = 0;

int
main(int argc, char **argv)
{
   D_ENTER;
   atexit(feh_clean_exit);

   init_parse_options(argc, argv);

   init_x_and_imlib();

   if (opt.montage)
      init_montage_mode();
   else if (opt.index)
      init_index_mode();
   else if (opt.collage)
      init_collage_mode();
   else if (opt.multiwindow)
      init_multiwindow_mode();
   else if (opt.list || opt.customlist)
      init_list_mode();
   else if (opt.loadables)
      init_loadables_mode();
   else if (opt.unloadables)
      init_unloadables_mode();
   else
   {
      /* Slideshow mode is now the default. Because it's spiffy */
      opt.slideshow = 1;
      init_slideshow_mode();
   }

   /* main event loop */
   while (feh_main_iteration(1));

   D_RETURN(0);
}


/* Return 0 to stop iterating, 1 if ok to continue. */
int
feh_main_iteration(int block)
{
   static int first = 1;
   static int xfd = 0;
   static int fdsize = 0;
   static double pt = 0.0;
   XEvent ev;
   struct timeval tval;
   fd_set fdset;
   int count = 0;
   double t1 = 0.0, t2 = 0.0;
   fehtimer ft;

   D_ENTER;

   if (window_num == 0)
      D_RETURN(0);

   if (first)
   {
      /* Only need to set these up the first time */
      xfd = ConnectionNumber(disp);
      fdsize = xfd + 1;
      pt = feh_get_time();
      first = 0;
   }

   /* Timers */
   t1 = feh_get_time();
   t2 = t1 - pt;
   pt = t1;
   while (XPending(disp))
   {
      XNextEvent(disp, &ev);
      feh_handle_event(&ev);
      if (window_num == 0)
         D_RETURN(0);
   }
   XFlush(disp);

   feh_redraw_menus();

   FD_ZERO(&fdset);
   FD_SET(xfd, &fdset);

   /* Timers */
   ft = first_timer;
   /* Don't do timers if we're zooming */
   if (ft && !opt.zoom_mode)
   {
      D(("There are timers in the queue\n"));
      if (ft->just_added)
      {
         D(("The first timer has just been added\n"));
         D(("ft->in = %f\n", ft->in));
         ft->just_added = 0;
         t1 = ft->in;
      }
      else
      {
         D(("The first timer was not just added\n"));
         t1 = ft->in - t2;
         if (t1 < 0.0)
            t1 = 0.0;
         ft->in = t1;
      }
      D(("I next need to action a timer in %f seconds\n", t1));
      /* Only do a blocking select if there's a timer due, or no events
         waiting */
      if (t1 == 0.0 || (block && !XPending(disp)))
      {
         tval.tv_sec = (long) t1;
         tval.tv_usec = (long) ((t1 - ((double) tval.tv_sec)) * 1000000);
         if (tval.tv_sec < 0)
            tval.tv_sec = 0;
         if (tval.tv_usec <= 1000)
            tval.tv_usec = 1000;
         errno = 0;
         D(("Performing blocking select - waiting for timer or event\n"));
         count = select(fdsize, &fdset, NULL, NULL, &tval);
         if ((count < 0)
             && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF)))
            eprintf("Connection to X display lost");
         if ((ft) && (count == 0))
         {
            /* This means the timer is due to be executed. If count was > 0,
               that would mean an X event had woken us, we're not interested
               in that */
            feh_handle_timer();
         }
      }
   }
   else
   {
      /* Don't block if there are events in the queue. That's a bit rude ;-) */
      if (block && !XPending(disp))
      {
         errno = 0;
         D(("Performing blocking select - no timers, or zooming\n"));
         count = select(fdsize, &fdset, NULL, NULL, NULL);
         if ((count < 0)
             && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF)))
            eprintf("Connection to X display lost");
      }
   }
   if (window_num == 0)
      D_RETURN(0);
   D_RETURN(1);
}

void
feh_handle_event(XEvent * ev)
{
   winwidget winwid = NULL;

   D_ENTER;

   switch (ev->type)
   {
     case KeyPress:
        D(("Received KeyPress event\n"));
        while (XCheckTypedWindowEvent(disp, ev->xkey.window, KeyPress, ev));
        handle_keypress_event(ev, ev->xkey.window);
        break;
     case ButtonPress:
        D(("Received ButtonPress event\n"));
        /* hide the menus and get the heck out if it's a mouse-click on the
           cover */
        if (ev->xbutton.window == menu_cover)
        {
           feh_menu_hide(menu_root);
           break;
        }
        switch (ev->xbutton.button)
        {
          case 1:
             D(("Button 1 Press event\n"));
             winwid = winwidget_get_from_window(ev->xbutton.window);
             if ((winwid != NULL) && (winwid->type == WIN_TYPE_SLIDESHOW))
                slideshow_change_image(winwid, SLIDE_NEXT);
             break;
          case 2:
             D(("Button 2 Press event\n"));
             if (!opt.full_screen)
             {
                winwid = winwidget_get_from_window(ev->xbutton.window);
                if (winwid != NULL)
                {
                   D(("Enabling zoom mode\n"));
                   opt.zoom_mode = 1;
                   winwid->zoom_mode = 1;
                   winwid->zoom = 1.0;
                   winwidget_render_image(winwid, 0);
                   winwid->zx = ev->xbutton.x;
                   winwid->zy = ev->xbutton.y;
                   if (winwid->zx > winwid->im_w)
                      winwid->zx = winwid->im_w;
                   if (winwid->zy > winwid->im_h)
                      winwid->zy = winwid->im_h;
                   if (opt.draw_filename)
                      feh_draw_filename(winwid);
                }
             }
             break;
          case 3:
             D(("Button 3 Press event\n"));
             winwid = winwidget_get_from_window(ev->xbutton.window);
             if (winwid != NULL)
             {
                int x, y, b;
                unsigned int c;
                Window r;

                if (!opt.no_menus)
                {
                   if (!menu_main)
                      feh_menu_init();
                   if (winwid->type == WIN_TYPE_ABOUT)
                   {
                      /* winwidget_destroy(winwid); */
                      XQueryPointer(disp, winwid->win, &r, &r, &x, &y, &b, &b,
                                    &c);
                      feh_menu_show_at_xy(menu_close, winwid, x, y);
                   }
                   else
                   {
                      XQueryPointer(disp, winwid->win, &r, &r, &x, &y, &b, &b,
                                    &c);
                      feh_menu_show_at_xy(menu_main, winwid, x, y);
                   }
                }
             }
             break;
          case 4:
             D(("Button 4 Press event\n"));
             winwid = winwidget_get_from_window(ev->xbutton.window);
             if ((winwid != NULL) && (winwid->type == WIN_TYPE_SLIDESHOW))
                slideshow_change_image(winwid, SLIDE_PREV);
             break;
          case 5:
             D(("Button 5 Press event\n"));
             winwid = winwidget_get_from_window(ev->xbutton.window);
             if ((winwid != NULL) && (winwid->type == WIN_TYPE_SLIDESHOW))
                slideshow_change_image(winwid, SLIDE_NEXT);
             break;
          default:
             D(("Received other ButtonPress event\n"));
             break;
        }
        break;
     case ButtonRelease:
        D(("Received ButtonRelease event\n"));
        if (menu_root)
        {
           /* if menus are open, close them, and execute action if needed */

           if (ev->xbutton.window == menu_cover)
              feh_menu_hide(menu_root);
           else if (menu_root)
           {
              feh_menu *m;

              if (ev->xbutton.window == menu_cover)
                 feh_menu_hide(menu_root);
              else if ((m = feh_menu_get_from_window(ev->xbutton.window)))
              {
                 feh_menu_item *i = NULL;

                 i = feh_menu_find_selected(m);
                 /* watch out for this. I put it this way around so the menu
                    goes away *before* we perform the action, if we start
                    freeing menus on hiding, it will break ;-) */
                 feh_menu_hide(menu_root);
                 feh_main_iteration(0);
                 if ((i) && (i->func))
                    (i->func) (m, i, i->data);
              }
           }
           break;
        }
        switch (ev->xbutton.button)
        {
          case 1:
             D(("Button 1 Release event\n"));
             break;
          case 2:
             D(("Button 2 Release event\n"));
             if (!opt.full_screen)
             {
                winwid = winwidget_get_from_window(ev->xbutton.window);
                if (winwid != NULL)
                {
                   winwid->zoom_mode = 0;
                   opt.zoom_mode = 0;
                   feh_smooth_image(winwid);
                   if (opt.draw_filename)
                      feh_draw_filename(winwid);
                }
             }
             break;
          case 3:
             D(("Button 3 Release event\n"));
             if (opt.no_menus)
                winwidget_destroy_all();
             break;
          default:
             break;
        }
        break;
     case ConfigureNotify:
        D(("Got ConfigureNotify\n"));
        while (XCheckTypedWindowEvent
               (disp, ev->xconfigure.window, ConfigureNotify, ev));
        if (!menu_root)
        {
           winwidget w = winwidget_get_from_window(ev->xconfigure.window);

           if (w)
           {
              D(
                ("configure size %dx%d\n", ev->xconfigure.width,
                 ev->xconfigure.height));
              if ((w->w != ev->xconfigure.width)
                  || (w->h != ev->xconfigure.height))
              {
                 w->w = ev->xconfigure.width;
                 w->h = ev->xconfigure.height;
                 w->had_resize = 1;
                 winwidget_render_image(w, 0);
              }
           }
        }
        break;
     case EnterNotify:
        D(("Got EnterNotify event\n"));

        break;
     case LeaveNotify:
        D(("Got LeaveNotify event\n"));
        if ((menu_root) && (ev->xcrossing.window == menu_root->win))
        {
           feh_menu_item *ii;

           D(("It is for a menu\n"));
           for (ii = menu_root->items; ii; ii = ii->next)
           {
              if (MENU_ITEM_IS_SELECTED(ii))
              {
                 D(("Unselecting menu\n"));
                 MENU_ITEM_SET_NORMAL(ii);
                 menu_root->updates =
                    imlib_update_append_rect(menu_root->updates, ii->x, ii->y,
                                             ii->w, ii->h);
                 menu_root->needs_redraw = 1;
              }
           }
           feh_raise_all_menus();
        }
        break;
     case MotionNotify:
        if (menu_root)
        {
           feh_menu *m;
           feh_menu_item *selected_item, *mouseover_item;

           D(("motion notify with menus open\n"));
           while (XCheckTypedWindowEvent
                  (disp, ev->xmotion.window, MotionNotify, ev));

           if (ev->xmotion.window == menu_cover)
              break;
           else if ((m = feh_menu_get_from_window(ev->xmotion.window)))
           {
              selected_item = feh_menu_find_selected(m);
              mouseover_item =
                 feh_menu_find_at_xy(m, ev->xmotion.x, ev->xmotion.y);
              if (selected_item != mouseover_item)
              {
                 D(("selecting a menu item\n"));
                 if (selected_item)
                    feh_menu_deselect_selected(m);
                 if ((mouseover_item)
                     && ((mouseover_item->func) || (mouseover_item->submenu)
                         || (mouseover_item->func_gen_sub)))
                    feh_menu_select(m, mouseover_item);
              }
           }
        }
        else if (opt.zoom_mode)
        {
           while (XCheckTypedWindowEvent
                  (disp, ev->xmotion.window, MotionNotify, ev));
           /* If zoom mode is set, then a window needs zooming, 'cos button 2 
              is pressed */
           winwid = winwidget_get_from_window(ev->xmotion.window);
           if (winwid != NULL)
           {
              if (winwid->zoom_mode)
              {
                 int sx, sy, sw, sh, dx, dy, dw, dh;

                 imlib_context_set_anti_alias(0);
                 imlib_context_set_dither(0);
                 imlib_context_set_blend(0);

                 winwid->zoom =
                    ((double) ev->xmotion.x - (double) winwid->zx) / 32.0;
                 if (winwid->zoom < 0)
                    winwid->zoom =
                       1.0 +
                       ((winwid->zoom * 32.0) / ((double) (winwid->zx + 1)));
                 else
                    winwid->zoom += 1.0;
                 if (winwid->zoom < 0.0001)
                    winwid->zoom = 0.0001;
                 if (winwid->zoom > 1.0)
                 {
                    dx = 0;
                    dy = 0;
                    dw = winwid->w;
                    dh = winwid->h;

                    sx = winwid->zx - (winwid->zx / winwid->zoom);
                    sy = winwid->zy - (winwid->zy / winwid->zoom);
                    sw = winwid->w / winwid->zoom;
                    sh = winwid->h / winwid->zoom;
                    if (imlib_image_has_alpha()
                        || ((winwid->w > winwid->im_w)
                            || (winwid->h > winwid->im_h)))
                       feh_draw_checks(winwid);
                 }
                 else
                 {
                    dx = winwid->zx - (winwid->zx * winwid->zoom);
                    dy = winwid->zy - (winwid->zy * winwid->zoom);
                    dw = winwid->w * winwid->zoom;
                    dh = winwid->h * winwid->zoom;

                    sx = 0;
                    sy = 0;
                    sw = winwid->w;
                    sh = winwid->h;
                    feh_draw_checks(winwid);
                 }
                 imlib_context_set_drawable(winwid->bg_pmap);
                 imlib_context_set_image(winwid->im);
                 if (imlib_image_has_alpha())
                    imlib_context_set_blend(1);
                 imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh,
                                                             dx, dy, dw, dh);
                 XSetWindowBackgroundPixmap(disp, winwid->win,
                                            winwid->bg_pmap);
                 XClearWindow(disp, winwid->win);
                 XFlush(disp);
              }
           }
        }
        else
        {
           while (XCheckTypedWindowEvent
                  (disp, ev->xmotion.window, MotionNotify, ev));
           winwid = winwidget_get_from_window(ev->xmotion.window);
           if (winwid != NULL)
           {
              if (winwid->type == WIN_TYPE_ABOUT)
              {
                 Imlib_Image *im2, *temp;
                 imlib_context_set_image(winwid->im);
                 im2 = imlib_clone_image();
                 imlib_context_set_image(im2);
                 imlib_apply_filter("bump_map_point(x=[],y=[],map=[];",
                                    &ev->xmotion.x, &ev->xmotion.y,
                                    winwid->file);
                 temp = winwid->im;
                 winwid->im = im2;
                 winwidget_render_image(winwid, 0);
                 winwid->im = temp;
                 imlib_context_set_image(im2);
                 imlib_free_image_and_decache();
              }
           }
        }
        break;
     case ClientMessage:
        if (ev->xclient.format == 32
            && ev->xclient.data.l[0] == (signed) wmDeleteWindow)
        {
           winwid = winwidget_get_from_window(ev->xclient.window);
           if (winwid != NULL)
              winwidget_destroy(winwid);
        }
        break;
     default:
        break;
   }
   D_RETURN_;
}

void
feh_smooth_image(winwidget w)
{
   int sx, sy, sw, sh, dx, dy, dw, dh;

   D_ENTER;

   if (w->zoom > 1.0)
   {
      dx = 0;
      dy = 0;
      dw = w->w;
      dh = w->h;

      sx = w->zx - (w->zx / w->zoom);
      sy = w->zy - (w->zy / w->zoom);
      sw = w->w / w->zoom;
      sh = w->h / w->zoom;
   }
   else
   {
      dw = w->w * w->zoom;
      dh = w->h * w->zoom;

      dx = w->zx - (w->zx * w->zoom);
      dy = w->zy - (w->zy * w->zoom);

      sx = 0;
      sy = 0;

      sw = w->w;
      sh = w->h;
   }
   imlib_context_set_anti_alias(1);
   imlib_context_set_dither(1);
   imlib_context_set_blend(0);
   imlib_context_set_drawable(w->bg_pmap);
   feh_draw_checks(w);
   imlib_context_set_image(w->im);
   if (imlib_image_has_alpha())
      imlib_context_set_blend(1);
   imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh, dx, dy, dw,
                                               dh);
   XSetWindowBackgroundPixmap(disp, w->win, w->bg_pmap);
   XClearWindow(disp, w->win);
   imlib_context_set_anti_alias(0);
   D_RETURN_;
}

void
feh_clean_exit(void)
{
   D_ENTER;

   if (!opt.keep_http)
      delete_rm_files();

   if (opt.filelistfile)
      feh_write_filelist(filelist, opt.filelistfile);

   D_RETURN_;
}
