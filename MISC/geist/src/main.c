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

#include "geist.h"
#include "geist_document.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_document_gtk.h"
#include "geist_text.h"
#include "geist_rect.h"
#include "geist_line.h"
#include "geist_layer.h"
#include "geist_document_xml.h"
#include "geist_gtk_menu.h"
#include "geist_interface.h"

int call_level = 0;
gib_list *doc_list = NULL;

int
main(int argc, char *argv[])
{
   GtkWidget *obj_win, *doc_win;
   geist_document *doc;
   int err;

   opt.debug_level = 3;
   D_ENTER(3);

   gtk_init(&argc, &argv);

   obj_win = geist_create_object_list();
   gtk_widget_show(obj_win);
   imlib_init(obj_win);

   gtk_clist_freeze(GTK_CLIST(obj_list));

   if (argc > 1)
      doc = geist_document_load_xml(argv[1], &err);
   else
      doc = geist_document_load_xml(PREFIX "/share/geist/test.xml", &err);
   if (!doc)
      weprintf("failed to load demo xml file %s, did you make install?",
               PREFIX "/share/test.xml");
   else
   {
      doc_win = geist_gtk_new_document_window(doc);
      geist_document_render_full(doc, 1);
      gtk_widget_show(doc_win);
   }

   gtk_clist_thaw(GTK_CLIST(obj_list));
   gtk_main();
   D_RETURN(3, 0);
}
