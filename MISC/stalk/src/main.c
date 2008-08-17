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

#include "stalk.h"
#include "stalk_interface.h"
#include "stalk_file.h"
#include "stalk_line.h"
#include "stalk_window.h"

int call_level = 0;
gib_list *doc_list = NULL;
stalk_window *win = NULL;

static gboolean cb_get_file_updates(gpointer *data);
   
int
main(int argc, char *argv[])
{
   opt.debug_level = 3;
   opt.w = 400;
   opt.h = 100;

   gtk_init(&argc, &argv);
   init_parse_options(argc, argv);

   if(!opt.files)
      show_mini_usage();
   
   gtk_timeout_add(500, (GtkFunction)cb_get_file_updates, NULL);
   imlib_init();
   
   win = stalk_window_new(opt.x, opt.y, opt.w, opt.h);
   gtk_widget_show(win->win);

   gtk_main();
   return 0;
}

static gboolean cb_get_file_updates(gpointer *data)
{
   gib_list *l, *ll, *item;
   int error;
   int more_files = TRUE;
   int new_lines = FALSE;

   D(3,("checking files\n"));

   l = opt.files;
   while(l)
   {
      item = l;
      l = l->next;
      ll = stalk_file_get_new_lines(STALK_FILE(item->data), &error);
      if(ll)
      {
         opt.lines = gib_list_cat(opt.lines, ll);
         new_lines = TRUE;
      }
      else if(error != READ_ERROR_NONE)
      {
         weprintf("read error");
         /* unlink file from list and stop watching it */
         stalk_file_free(STALK_FILE(item->data));
         opt.files = gib_list_remove(opt.files, item);
         /* no more files? why watch? */
         if(gib_list_length(opt.files) == 0)
         {
            weprintf("no more files to watch");
            more_files = FALSE;
            break;
         }
      }
   }
   if(new_lines)
   {
      stalk_trim_lines();
      stalk_window_draw(win);
   }
   return more_files;
}

void stalk_trim_lines(void)
{
   int diff;
   gib_list *l, *item;

   diff = gib_list_length(opt.lines) - opt.max_lines;
   if(diff > 0)
   {
      l = gib_list_nth(opt.lines, diff - 1);
      opt.lines = l->next;
      opt.lines->prev = NULL;
      while(l)
      {
         item = l;
         l = l->prev;
         stalk_line_free(STALK_LINE(item->data));
         free(item);
      }
   }
   D(1,("line list length now %d\n", gib_list_length(opt.lines)));
}
