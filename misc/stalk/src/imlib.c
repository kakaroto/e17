/* imlib.c

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

Display *disp = NULL;
Visual *vis = NULL;
Screen *scr = NULL;
Colormap cm;
int depth;
Window root = 0;

void
imlib_init(void)
{
   disp = GDK_DISPLAY();
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));
   cm = DefaultColormap(disp, DefaultScreen(disp));
   root = RootWindow(disp, DefaultScreen(disp));
   scr = ScreenOfDisplay(disp, DefaultScreen(disp));

   imlib_context_set_display(disp);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
   imlib_context_set_color_modifier(NULL);
   imlib_context_set_operation(IMLIB_OP_COPY);

   imlib_add_path_to_font_path(".");
   imlib_add_path_to_font_path("./ttfonts");
   imlib_add_path_to_font_path("/usr/X11R6/lib/X11/fonts/TrueType");
   imlib_add_path_to_font_path(PREFIX "/share/stalk/fonts");

   opt.font = imlib_load_font(opt.fontstr);
   if(!opt.font)
      eprintf("couldn't load font %s", opt.fontstr);

   if (opt.stylefile)
   {
      opt.style = gib_style_new_from_ascii(opt.stylefile);
      if (!opt.style)
         weprintf("failed to load style %s", opt.stylefile);
   }

}
