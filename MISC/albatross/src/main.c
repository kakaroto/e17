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

#include "alb.h"
#include "alb_document.h"
#include "alb_object.h"
#include "alb_image.h"
#include "alb_document_gtk.h"
#include "alb_text.h"
#include "alb_rect.h"
#include "alb_line.h"
#include "alb_layer.h"
#include "alb_document_xml.h"
#include "alb_gtk_menu.h"
#include "alb_interface.h"

int call_level = 0;
gib_list *doc_list = NULL;
struct _state state;


int
main(int argc,
     char *argv[])
{
  opt.debug_level = 3;
  D_ENTER(3);

  gtk_init(&argc, &argv);

  /* obj_win = alb_create_object_list();
     gtk_widget_show(obj_win);
     gtk_clist_freeze(GTK_CLIST(obj_list));
     doc = alb_document_load_dir("/home/gilbertt/album/test");
     doc_win = alb_gtk_new_document_window(doc);
     alb_document_render_full(doc, 1);
     gtk_widget_show(doc_win);
     gtk_clist_thaw(GTK_CLIST(obj_list));
   */

  opt.album_dir = g_strjoin("/", getenv("HOME"), "album", NULL);
  /* HACK */
  mkdir(opt.album_dir, 0755);
  state.interface = alb_gtk_new_interface();
  imlib_init(state.interface);
  gtk_widget_show(state.interface);
  state.scratch = alb_document_new(900, 75);
  state.scratch->bg_fill->r = 255;
  state.scratch->bg_fill->g = 235;
  state.scratch->bg_fill->b = 200;
  state.scratch->darea = state.da_scratch;
  state.scratch->scrollwin = state.sw_scratch;
  alb_document_render_full(state.scratch, 1);

  alb_select_album(alb_get_album_from_row(0));

  gtk_main();
  D_RETURN(3, 0);
}
