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

int
main(int argc, char **argv)
{
   Imlib_Image image;
   Imlib_Load_Error err;

   init_parse_options(argc, argv);

   init_x_and_imlib();

   if (!opt.output_file)
      opt.output_file = estrdup("%Y-%m-%d-%H%M%S_$wx$h_scrot.png");

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
   imlib_image_attach_data_value("quality", NULL, opt.quality, NULL);

   gib_imlib_save_image_with_error_return(image, opt.output_file, &err);
   if (err)
      eprintf("Saving to file %s failed\n", opt.output_file);
   if (opt.exec)
      scrot_exec_app(image);
   gib_imlib_free_image_and_decache(image);

   return 0;
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
         fflush(stdout);
      }
      else
         sleep(opt.delay);
   }
}

Imlib_Image
scrot_grab_shot(void)
{
   Imlib_Image im;

   XBell(disp, 0);
   im = gib_imlib_create_image_from_drawable(root, 0, 0, 0, scr->width, scr->height, 1);

   return im;
}

void
scrot_exec_app(Imlib_Image im)
{
   char *execstr;

   execstr = im_printf(opt.exec, opt.output_file, im);
   system(execstr);
   exit(0);
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
   XSync(disp, True);


   if (done < 2)
   {
      scrot_do_delay();
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
      }
      else
      {
         Window child;
         XWindowAttributes attr;
         int stat;

         /* else it's a window click */
         /* get geometry of window and use that */
         /* get windowmanager frame of window */
         if (target != root)
         {
            unsigned int d, x;
            int status;

            status =
               XGetGeometry(disp, target, &root, &x, &x, &d, &d, &d, &d);
            if (status != 0)
            {
               Window rt, *children, parent;

               for (;;)
               {
                  /* Find window manager frame. */
                  status =
                     XQueryTree(disp, target, &rt, &parent, &children, &d);
                  if (status && (children != None))
                     XFree((char *) children);
                  if (!status || (parent == None) || (parent == rt))
                     break;
                  target = parent;
               }
               /* Get client window. */
               if (!opt.border)
                  target = scrot_get_client_window(disp, target);
               XRaiseWindow(disp, target);
            }
         }
         stat = XGetWindowAttributes(disp, target, &attr);
         if ((stat == False) || (attr.map_state != IsViewable))
            return NULL;
         rw = attr.width;
         rh = attr.height;
         XTranslateCoordinates(disp, target, root, 0, 0, &rx, &ry, &child);
      }

      /* clip rectangle nicely */
      if (rx < 0)
      {
         rw += rx;
         rx = 0;
      }
      if (ry < 0)
      {
         rh += ry;
         ry = 0;
      }
      if ((rx + rw) > scr->width)
         rw = scr->width - rx;
      if ((ry + rh) > scr->height)
         rh = scr->height - ry;

      XBell(disp, 0);
      im = gib_imlib_create_image_from_drawable(root, 0, rx, ry, rw, rh, 1);
   }
   return im;
}

Window
scrot_get_window(Display * display, Window window, int x, int y)
{
   Window source, target;

   int status, x_offset, y_offset;

   source = root;
   target = window;
   if (window == None)
      window = root;
   while (1)
   {
      status =
         XTranslateCoordinates(display, source, window, x, y, &x_offset,
                               &y_offset, &target);
      if (status != True)
         break;
      if (target == None)
         break;
      source = window;
      window = target;
      x = x_offset;
      y = y_offset;
   }
   if (target == None)
      target = window;
   return (target);
}


char *
im_printf(char *str, char *filename, Imlib_Image im)
{
   char *c;
   char buf[20];
   char ret[4096];
   char strf[4096];
   char *tmp;
   struct stat st;
   time_t t;
   struct tm *tm;

   ret[0] = '\0';
   time(&t);
   tm = localtime(&t);
   strftime(strf, 4095, str, tm);

   for (c = strf; *c != '\0'; c++)
   {
      if (*c == '$')
      {
         c++;
         switch (*c)
         {
           case 'f':
              if (filename)
                 strcat(ret, filename);
              break;
           case 'n':
              if (filename)
              {
                 tmp = strrchr(filename, '/');
                 if (tmp)
                    strcat(ret, tmp + 1);
                 else
                    strcat(ret, filename);
              }
              break;
           case 'w':
              snprintf(buf, sizeof(buf), "%d", gib_imlib_image_get_width(im));
              strcat(ret, buf);
              break;
           case 'h':
              snprintf(buf, sizeof(buf), "%d", gib_imlib_image_get_height(im));
              strcat(ret, buf);
              break;
           case 's':
              if (filename)
              {
                 if (!stat(filename, &st))
                 {
                    int size;

                    size = st.st_size;
                    snprintf(buf, sizeof(buf), "%d", size);
                    strcat(ret, buf);
                 }
                 else
                    strcat(ret, "[err]");
              }
              break;
           case 'p':
              snprintf(buf, sizeof(buf), "%d",
                       gib_imlib_image_get_width(im) * gib_imlib_image_get_height(im));
              strcat(ret, buf);
              break;
           case 't':
              strcat(ret, gib_imlib_image_format(im));
              break;
           case '$':
              strcat(ret, "$");
              break;
           default:
              strncat(ret, c, 1);
              break;
         }
      }
      else if (*c == '\\')
      {
         c++;
         switch (*c)
         {
           case 'n':
              if (filename)
                 strcat(ret, "\n");
              break;
           default:
              strncat(ret, c, 1);
              break;
         }
      }
      else
         strncat(ret, c, 1);
   }
   return estrdup(ret);
}

Window scrot_get_client_window(Display * display, Window target)
{
   Atom state;
   Atom type = None;
   int format, status;
   unsigned char *data;
   unsigned long after, items;
   Window client;

   state = XInternAtom(display, "WM_STATE", True);
   if (state == None)
      return target;
   status =
      XGetWindowProperty(display, target, state, 0L, 0L, False,
                         (Atom) AnyPropertyType, &type, &format, &items,
                         &after, &data);
   if ((status == Success) && (type != None))
      return target;
   client = scrot_find_window_by_property(display, target, state);
   if (!client)
      return target;
   return client;
}

Window scrot_find_window_by_property(Display * display, const Window window,
                                     const Atom property)
{
   Atom type = None;
   int format, status;
   unsigned char *data;
   unsigned int i, number_children;
   unsigned long after, number_items;
   Window child = None, *children, parent, root;

   status =
      XQueryTree(display, window, &root, &parent, &children,
                 &number_children);
   if (!status)
      return None;
   for (i = 0; (i < number_children) && (child == None); i++)
   {
      status =
         XGetWindowProperty(display, children[i], property, 0L, 0L, False,
                            (Atom) AnyPropertyType, &type, &format,
                            &number_items, &after, &data);
      if (data)
         XFree(data);
      if ((status == Success) && (type != (Atom) NULL))
         child = children[i];
   }
   for (i = 0; (i < number_children) && (child == None); i++)
      child = scrot_find_window_by_property(display, children[i], property);
   if (children != None)
      XFree(children);
   return (child);
}
