#ifndef GEIST_DOCUMENT_H
#define GEIST_DOCUMENT_H

#include "geist.h"
#include "geist_list.h"
#include "geist_object.h"
#include "geist_layer.h"
#include "geist_imlib.h"
#include "geist_document_gtk.h"
#include "geist_fill.h"

#define GEIST_DOCUMENT(d) ((geist_document *) d)

struct __geist_document
{
   char *name;
   char *filename;
   geist_list *layers;
   geist_fill *bg_fill;
   int w;
   int h;
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
void geist_document_render_updates(geist_document * d);
void geist_document_render_pmap(geist_document * doc);
void geist_document_render_pmap_partial(geist_document * doc, int x, int y,
                                        int w, int h);
void geist_document_render_selection(geist_document * doc);
geist_list *geist_document_get_selected_list(geist_document * doc);
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
