/* main.c

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

#include "scrot.h"
#include "options.h"

int call_level = 0;

int
main(int argc, char **argv)
{
   char *tmp;
   Imlib_Image image;
   Imlib_Load_Error err;

   D_ENTER(4);

   init_parse_options(argc, argv);

   init_x_and_imlib();

   if (!opt.output_file)
      show_mini_usage();

   if (opt.select)
      image = scrot_sel_and_grab_image();
   else
   {
      scrot_do_delay();
      image = scrot_grab_shot();
   }

   if (!image)
      eprintf("no image grabbed");

   imlib_context_set_image(image);
   tmp = strrchr(opt.output_file, '.');
   if (tmp)
      imlib_image_set_format(tmp + 1);

   imlib_image_attach_data_value("quality", NULL, opt.quality, NULL);
   imlib_save_image_with_error_return(opt.output_file, &err);
   if (err)
      eprintf("Saving to file %s failed\n", opt.output_file);
   if (opt.exec)
      scrot_exec_app();

   D_RETURN(4, 0);
}

void
scrot_do_delay(void)
{
   if (opt.delay)
   {
      if (opt.countdown)
      {
         int i;

         printf("Taking shot in %d.. ", opt.delay);
         fflush(stdout);
         sleep(1);
         for (i = opt.delay - 1; i > 0; i--)
         {
            printf("%d.. ", i);
            fflush(stdout);
            sleep(1);
         }
         printf("0.\n");
      }
      else
      {
         sleep(opt.delay);
      }
   }
}

Imlib_Image
scrot_grab_shot(void)
{
   Imlib_Image im;

   D_ENTER(3);

   imlib_context_set_drawable(root);
   im = imlib_create_image_from_drawable(0, 0, 0, scr->width, scr->height, 1);

   D_RETURN(3, im);
}

void
scrot_exec_app(void)
{
   D_ENTER(3);

   execlp(opt.exec, opt.exec, opt.output_file, NULL);
   eprintf("exec of %s failed:", opt.exec);
   D_RETURN_(3);
}

Imlib_Image scrot_sel_and_grab_image(void)
{
   Imlib_Image im = NULL;
   static int xfd = 0;
   static int fdsize = 0;
   XEvent ev;
   fd_set fdset;
   int count = 0, done = 0;
   int rx = 0, ry = 0, rw = 0, rh = 0, btn_pressed = 0;
   int rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
   int dont_care;
   Window not_interested;
   Cursor cursor;
   Window target = None;
   GC gc;
   XGCValues gcval;

   xfd = ConnectionNumber(disp);
   fdsize = xfd + 1;

   cursor = XCreateFontCursor(disp, XC_left_ptr);

   gcval.foreground = XWhitePixel(disp, 0);
   gcval.function = GXxor;
   gcval.background = XBlackPixel(disp, 0);
   gcval.plane_mask = gcval.background ^ gcval.foreground;
   gcval.subwindow_mode = IncludeInferiors;

   gc =
      XCreateGC(disp, root,
                GCFunction | GCForeground | GCBackground | GCSubwindowMode,
                &gcval);

   if (
       (XGrabPointer
        (disp, root, False,
         ButtonMotionMask | ButtonPressMask | ButtonReleaseMask,
         GrabModeAsync, GrabModeAsync, root, cursor,
         CurrentTime) != GrabSuccess))
      eprintf("couldn't grab pointer:");

   if (
       (XGrabKeyboard
        (disp, root, False, GrabModeAsync, GrabModeAsync,
         CurrentTime) != GrabSuccess))
      eprintf("couldn't grab keyboard:");


   while (1)
   {
      /* handle events here */
      while (!done && XPending(disp))
      {
         XNextEvent(disp, &ev);
         switch (ev.type)
         {
           case MotionNotify:
              if (btn_pressed)
              {
                 D(2, ("Motion event with button pressed\n"));
                 if (rect_w)
                 {
                    /* re-draw the last rect to clear it */
                    XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w,
                                   rect_h);
                 }

                 rect_x = rx;
                 rect_y = ry;
                 rect_w = ev.xmotion.x - rect_x;
                 rect_h = ev.xmotion.y - rect_y;

                 if (rect_w < 0)
                 {
                    rect_x += rect_w;
                    rect_w = 0 - rect_w;
                 }
                 if (rect_h < 0)
                 {
                    rect_y += rect_h;
                    rect_h = 0 - rect_h;
                 }
                 /* draw rectangle */
                 XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w,
                                rect_h);
                 XFlush(disp);
              }
              break;
           case ButtonPress:
              btn_pressed = 1;
              rx = ev.xbutton.x;
              ry = ev.xbutton.y;
              target =
                 scrot_get_window(disp, ev.xbutton.subwindow, ev.xbutton.x,
                                  ev.xbutton.y);
              if (target == None)
                 target = root;
              break;
           case ButtonRelease:
              D(2, ("Button release event\n"));
              if (rect_w > 5)
              {
                 /* if a rect has been drawn, it's an area selection */
                 rw = ev.xbutton.x - rx;
                 rh = ev.xbutton.y - ry;

                 if (rw < 0)
                 {
                    rx += rw;
                    rw = 0 - rw;
                 }
                 if (rh < 0)
                 {
                    ry += rh;
                    rh = 0 - rh;
                 }
                 /* grab area */
                 imlib_context_set_drawable(root);
              }
              else
              {
                 /* else it's a window click */
                 /* get geometry of window and use that */
                 XGetGeometry(disp, target, &not_interested, &dont_care,
                              &dont_care, &rw, &rh, &dont_care, &dont_care);
                 rx = 0;
                 ry = 0;
                 imlib_context_set_drawable(target);
              }
              done = 1;
              break;
           case KeyPress:
              fprintf(stderr, "Key was pressed, aborting shot\n");
              done = 2;
              break;
           case KeyRelease:
              /* ignore */
              break;
           default:
              fprintf(stderr, "unexpected event %d\n", ev.type);
              break;
         }
      }
      if (done)
         break;

      /* now block some */
      FD_ZERO(&fdset);
      FD_SET(xfd, &fdset);
      errno = 0;
      count = select(fdsize, &fdset, NULL, NULL, NULL);
      if ((count < 0)
          && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF)))
         eprintf("Connection to X display lost");
   }
   if (rect_w)
   {
      XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
      XFlush(disp);
   }
   XUngrabPointer(disp, CurrentTime);
   XUngrabKeyboard(disp, CurrentTime);
   XFreeCursor(disp, cursor);
   XFreeGC(disp, gc);
   if (done < 2)
   {
      scrot_do_delay();
      im = imlib_create_image_from_drawable(0, rx, ry, rw, rh, 1);
   }
   return im;
}

Window
scrot_get_window(Display * display, Window window, int x, int y)
{
   Window source_window, target_window;

   int status, x_offset, y_offset;

   source_window = root;
   target_window = window;
   if (window == None)
      window = root;
   for (;;)
   {
      status =
         XTranslateCoordinates(display, source_window, window, x, y,
                               &x_offset, &y_offset, &target_window);
      if (status != True)
         break;
      if (target_window == None)
         break;
      source_window = window;
      window = target_window;
      x = x_offset;
      y = y_offset;
   }
   if (target_window == None)
      target_window = window;
   return (target_window);
}
