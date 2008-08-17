/* stalk_window.c

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

#include "stalk_window.h"
#include "stalk_interface.h"
#include "stalk_line.h"

stalk_window *
stalk_window_new(int x, int y, int w, int h)
{
   stalk_window *ret;

   ret = (stalk_window *) emalloc(sizeof(stalk_window));
   memset(ret, 0, sizeof(stalk_window));
   ret->im = imlib_create_image(w, h);
   ret->pmap = XCreatePixmap(disp, root, w, h, depth);
   ret->im_w = w;
   ret->im_h = h;
   stalk_create_main_window(ret);
   ret->x = 0;
   ret->y = 0;

   return ret;
}

void
stalk_window_set_background(stalk_window * win)
{
   Window xwin;

   if (!win->pmap)
      win->pmap = XCreatePixmap(disp, root, win->im_w, win->im_h, depth);
   gib_imlib_render_image_on_drawable(win->pmap, win->im, 0, 0, 1, 0, 0);

   gtk_widget_realize(win->darea);
   xwin = GDK_WINDOW_XWINDOW(win->darea->window);

   XSetWindowBackgroundPixmap(disp, xwin, win->pmap);

   XClearWindow(disp, xwin);
}

void
stalk_window_draw(stalk_window * win)
{
   gib_list *line, *sublines;
   int x, y;
   int line_spacing = 1;
   int line_height;
   int done = 0;
   int i;

   if (opt.trans)
   {
      /*
         if (win->im)
         gib_imlib_free_image(win->im);
         win->im = gib_imlib_clone_image(win->bg_im);
       */
      printf("implement me!\n");
   }
   else
   {
      /* blank the image */
      if (!win->im)
         win->im = imlib_create_image(win->im_w, win->im_h);
      gib_imlib_image_fill_rectangle(win->im, 0, 0, win->im_w, win->im_h, 255,
                                     255, 255, 255);
   }

   gib_imlib_get_text_size(opt.font, "ABCWZYygjlL/@~!^", opt.style, NULL,
                           &line_height, IMLIB_TEXT_TO_RIGHT);
   line_height += line_spacing;

   D(1, ("drawing text\n"));

   x = 0;
   y = win->im_h - line_height;

   line = gib_list_last(opt.lines);
   if (!line)
      return;
   if(win->offset)
   {
      i = win->offset;
      while(line && i--)
         line = line->prev;
   }

   while (line && !done)
   {
      /* split up this line into wrapped sublines */
      sublines =
         stalk_generate_wrapped_lines((stalk_line *) line->data,
                                      opt.wrap ? win->im_w : 0);
      if (sublines && !done)
      {
         gib_list *curline;

         curline = gib_list_last(sublines);
         while (curline && !done)
         {
            gib_imlib_text_draw(win->im, opt.font, opt.style, x, y,
                                (char *) curline->data, IMLIB_TEXT_TO_RIGHT,
                                opt.fn_r, opt.fn_g, opt.fn_b, opt.fn_a);
            if (y < 0)
            {
               done++;
               break;
            }
            y -= line_height;
            curline = curline->prev;
         }
         gib_list_free_and_data(sublines);
      }
      line = line->prev;
   }
   stalk_window_set_background(win);
}

gib_list *
stalk_generate_wrapped_lines(stalk_line * sline, int wrap_width)
{
   gib_list *ll, *list = NULL, *words, *lines;
   int w, line_width;
   int tw, th;
   char *p, *pp;
   char *line = NULL;
   char *temp;
   int space_width = 0, m_width = 0, t_width = 0, new_width = 0;

   p = stalk_line_get_string(sline);

   if (wrap_width)
   {
      gib_imlib_get_text_size(opt.font, "M M", opt.style, &t_width, NULL,
                              IMLIB_TEXT_TO_RIGHT);
      gib_imlib_get_text_size(opt.font, "M", opt.style, &m_width, NULL,
                              IMLIB_TEXT_TO_RIGHT);
      space_width = t_width - (2 * m_width);
      w = wrap_width;
      {
         line_width = 0;
         /* quick check to see if whole line fits okay */
         gib_imlib_get_text_size(opt.font, p, opt.style, &tw, &th,
                                 IMLIB_TEXT_TO_RIGHT);
         if (tw <= w)
            list = gib_list_add_end(list, estrdup(p));
         else if (strlen(p) == 0)
            list = gib_list_add_end(list, estrdup(""));
         else if (!strcmp(p, " "))
            list = gib_list_add_end(list, estrdup(" "));
         else
         {
            words = gib_string_split(p, " ");
            if (words)
            {
               ll = words;
               while (ll)
               {
                  pp = (char *) ll->data;
                  if (strcmp(pp, " "))
                  {
                     gib_imlib_get_text_size(opt.font, pp, opt.style, &tw,
                                             &th, IMLIB_TEXT_TO_RIGHT);
                     if (line_width == 0)
                        new_width = tw;
                     else
                        new_width = line_width + space_width + tw;
                     if (new_width <= w)
                     {
                        /* add word to line */
                        if (line)
                        {
                           int len;

                           len = strlen(line) + strlen(pp) + 2;
                           temp = emalloc(len);
                           snprintf(temp, len, "%s %s", line, pp);
                           free(line);
                           line = temp;
                        }
                        else
                           line = estrdup(pp);
                        line_width = new_width;
                     }
                     else if (line_width == 0)
                     {
                        /* can't fit single word in :/
                           increase width limit to width of word and jam the bastard
                           in anyhow */
                        w = tw;
                        line = estrdup(pp);
                        line_width = new_width;
                     }
                     else
                     {
                        /* finish this line, start next and add word there */
                        if (line)
                        {
                           list = gib_list_add_end(list, estrdup(line));
                           free(line);
                           line = NULL;
                        }
                        line = estrdup(pp);
                        line_width = tw;
                     }
                  }
                  ll = ll->next;
               }
               if (line)
               {
                  /* finish last line */
                  list = gib_list_add_end(list, estrdup(line));
                  free(line);
                  line = NULL;
                  line_width = 0;
               }
               gib_list_free_and_data(words);
            }
         }
      }
      lines = list;
   }
   else
      lines = gib_list_add_end(NULL, estrdup(p));
   free(p);
   return lines;
}

gboolean stalk_window_configure_event(GtkWidget * widget,
                                      GdkEventConfigure * event,
                                      gpointer * data)
{
   stalk_window *win;
   int need_bg_update = 0;

   win = STALK_WINDOW(data);
   if ((event->height != win->im_h) || (event->width != win->im_w))
   {
      gib_imlib_free_image(win->im);
      win->im = NULL;
      XFreePixmap(disp, win->pmap);
      win->pmap = 0;
      win->im_w = event->width;
      win->im_h = event->height;
      D(1, ("resized to %d,%d\n", event->width, event->height));
      need_bg_update = 1;
   }
   if ((event->x != win->x) || (event->y != win->y))
   {
      D(1, ("moved to %d,%d\n", event->x, event->y));
      win->x = event->x;
      win->y = event->y;
      /* need to update bg on move when transparent */
      if (opt.trans)
         need_bg_update = 1;
   }

   if (opt.trans && (need_bg_update || !win->bg_im))
   {
      /* grab new background */
      D(1,
        ("grabbing new bg image at %d,%d %dx%d\n", win->x, win->y, win->im_w,
         win->im_h));
      /*
         if (win->bg_im)
         gib_imlib_free_image(win->bg_im);
         d = stalk_get_desktop_pixmap();
         win->bg_im =
         gib_imlib_create_image_from_drawable(d, 0, win->x, win->y,
         win->im_w, win->im_h, 0);
       */
      printf("implement me!\n");
      stalk_window_draw(win);
   }

   return TRUE;
}

void
stalk_window_change_offset(stalk_window * win, int offset)
{
   if (!win)
      return;
   if (offset == win->offset)
      return;
   if(offset > opt.max_lines)
      offset = opt.max_lines;
   win->offset = offset;
   stalk_window_draw(win);
}
