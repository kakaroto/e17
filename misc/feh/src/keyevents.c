/* keyevents.c
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
#include "feh_list.h"
#include "filelist.h"
#include "winwidget.h"
#include "options.h"

void
handle_keypress_event(XEvent * ev, Window win)
{
   int len;
   static char kbuf[20];
   KeySym keysym;
   XKeyEvent *kev;
   winwidget winwid = NULL;

   D_ENTER;

   winwid = winwidget_get_from_window(win);
   if (winwid == NULL)
      D_RETURN_;

   kev = (XKeyEvent *) ev;
   len = XLookupString(&ev->xkey, (char *) kbuf, sizeof(kbuf), &keysym, NULL);

   switch (keysym)
   {
     case XK_Left:
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_PREV);
        break;
     case XK_Right:
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_NEXT);
        break;
     case XK_Page_Up:
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_JUMP_BACK);
        break;
     case XK_Page_Down:
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_JUMP_FWD);
        break;
     case XK_Delete:
        /* Holding ctrl gets you a filesystem deletion and removal from the * 
           filelist. Just DEL gets you filelist removal only. */
        if (kev->state & ControlMask)
           feh_filelist_image_remove(winwid, 1);
        else
           feh_filelist_image_remove(winwid, 0);
        break;
     case XK_Home:
     case XK_KP_Home:
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_FIRST);
        break;
     case XK_End:
     case XK_KP_End:
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_LAST);
        break;
     case XK_Return:
        if (opt.action)
        {
           if (opt.slideshow)
           {
              feh_action_run(FEH_FILE(winwid->file->data));
              slideshow_change_image(winwid, SLIDE_NEXT);
           }
           else if (opt.multiwindow)
           {
              feh_action_run(FEH_FILE(winwid->file->data));
              winwidget_destroy(winwid);
           }
        }
        break;
     default:
        break;
   }

   if (len <= 0 || len > (int) sizeof(kbuf))
      D_RETURN_;

   kbuf[len] = '\0';

   switch (*kbuf)
   {
     case 'n':
     case 'N':
     case ' ':
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_NEXT);
        break;
     case 'p':
     case 'P':
     case '\b':
        if (opt.slideshow)
           slideshow_change_image(winwid, SLIDE_PREV);
        break;
     case 'q':
     case 'Q':
        winwidget_destroy_all();
        break;
     case 'r':
     case 'R':
        feh_reload_image(winwid);
        break;
     case '=':
     case '+':
        if (opt.reload < SLIDESHOW_RELOAD_MAX)
           opt.reload++;
        else if (opt.verbose)
           weprintf("Cannot set RELOAD higher than %d seconds.", opt.reload);
        break;
     case '-':
     case '_':
        if (opt.reload > 1)
           opt.reload--;
        else if (opt.verbose)
           weprintf("Cannot set RELOAD lower than 1 second.");
        break;
     default:
        break;
   }
   D_RETURN_;
}
