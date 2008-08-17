/* geist_document.h

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

#ifndef GEIST_DOCUMENT_H
#define GEIST_DOCUMENT_H

#include "geist.h"
#include "geist_object.h"
#include "geist_layer.h"
#include "geist_document_gtk.h"
#include "geist_fill.h"

#define GEIST_DOCUMENT(d) ((geist_document *) d)

struct __geist_document
{
   char *name;
   char *filename;
   gib_list *layers;
   geist_fill *bg_fill;
   int w;
   int h;
   int select_x;
   int select_y;
   int old_select_x;
   int old_select_y;
   int old_select_w;
   int old_select_h;
   Imlib_Image im;
   Pixmap pmap;
   Imlib_Updates up;
   GtkWidget *darea;
   GtkWidget *scrollwin;
   GtkWidget *win;
};

geist_document *geist_document_new(int w, int h);
void geist_document_free(geist_document * l);
void geist_document_render(geist_document * document);
void geist_document_render_partial(geist_document * document, int x, int y,
                                   int w, int h);

void geist_document_add_object(geist_document * doc, geist_object * obj);
void geist_document_add_layer(geist_document * doc);
geist_object *geist_document_find_clicked_object(geist_document * doc, int x,

                                                 int y);
void geist_document_render_updates(geist_document * d, unsigned char clear);
void geist_document_render_pmap(geist_document * doc);
void geist_document_render_pmap_partial(geist_document * doc, int x, int y,
                                        int w, int h);
void geist_document_render_selection(geist_document * doc);
gib_list *geist_document_get_selected_list(geist_document * doc);
void geist_document_unselect_all(geist_document * doc);
void geist_document_render_selection_partial(geist_document * doc, int x,
                                             int y, int w, int h);
void geist_document_remove_object(geist_document * d, geist_object * obj);
void geist_document_dirty_selection(geist_document * doc);
void geist_document_render_full(geist_document * d, int selection);
void geist_document_rename(geist_document *d, char *name);
void geist_document_reset_object_list(geist_document * d);
int geist_document_save_imlib(geist_document * doc, char *filename);
int geist_document_save(geist_document * doc, char *filename);
void
geist_document_resize(geist_document * doc, int w, int h);

#endif
