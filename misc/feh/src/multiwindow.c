/* multiwindow.c
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
#include "winwidget.h"
#include "timers.h"
#include "filelist.h"
#include "options.h"

void
init_multiwindow_mode(void)
{
   winwidget w = NULL;
   feh_file *file;

   D_ENTER;

   for (file = filelist; file; file = file->next)
   {
      char *s = NULL;
      int len = 0;

      len = strlen(PACKAGE " - ") + strlen(file->filename) + 1;
      s = emalloc(len);
      snprintf(s, len, PACKAGE " - %s", file->filename);

      if ((w = winwidget_create_from_file(file, s, WIN_TYPE_SINGLE)) != NULL)
      {
         if (opt.draw_filename)
            feh_draw_filename(w);
         if (!opt.progressive)
            winwidget_show(w);
         if (opt.reload > 0)
            feh_add_unique_timer(cb_reload_timer, w, opt.reload);
      }
      else
      {
         D(
           ("EEEK. Couldn't load image in multiwindow mode. "
            "I 'm not sure if this is a problem\n"));
      }
      free(s);
   }
   D_RETURN_;
}
