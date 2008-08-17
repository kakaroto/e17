/* alb_document.h

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

#ifndef ALB_DOCUMENT_H
#define ALB_DOCUMENT_H

#include "alb.h"
#include "alb_object.h"
#include "alb_layer.h"
#include "alb_document_gtk.h"
#include "alb_fill.h"

#define ALB_DOCUMENT(d) ((alb_document *) d)

struct __alb_document {
  char *name;
  char *filename;
  gib_list *layers;
  alb_fill *bg_fill;
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

alb_document *alb_document_new(int w,
                               int h);
void alb_document_free(alb_document * l);
void alb_document_render(alb_document * document);
void alb_document_render_partial(alb_document * document,
                                 int x,
                                 int y,
                                 int w,
                                 int h);

void alb_document_add_object(alb_document * doc,
                             alb_object * obj);
void alb_document_add_layer(alb_document * doc);
alb_object *alb_document_find_clicked_object(alb_document * doc,
                                             int x,
                                             int y);
void alb_document_render_updates(alb_document * d,
                                 unsigned char clear);
void alb_document_render_pmap(alb_document * doc);
void alb_document_render_pmap_partial(alb_document * doc,
                                      int x,
                                      int y,
                                      int w,
                                      int h);
void alb_document_render_selection(alb_document * doc);
gib_list *alb_document_get_selected_list(alb_document * doc);
void alb_document_unselect_all(alb_document * doc);
void alb_document_render_selection_partial(alb_document * doc,
                                           int x,
                                           int y,
                                           int w,
                                           int h);
void alb_document_remove_object(alb_document * d,
                                alb_object * obj);
void alb_document_dirty_selection(alb_document * doc);
void alb_document_render_full(alb_document * d,
                              int selection);
void alb_document_rename(alb_document * d,
                         char *name);
int alb_document_save_imlib(alb_document * doc,
                            char *filename);
int alb_document_save(alb_document * doc,
                      char *filename);
void
  alb_document_resize(alb_document * doc,
                      int w,
                      int h);
void
  alb_document_dirty_area(alb_document * doc,
                          int x,
                          int y,
                          int w,
                          int h);
void alb_document_tesselate(alb_document *doc);

#endif
