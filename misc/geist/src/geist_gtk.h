
#include "geist.h"
#include "geist_document.h"
void geist_document_render_to_gtk_window(geist_document * doc,

                                         GtkWidget * win);
void geist_document_render_to_gtk_window_partial(geist_document * doc,
                                                 GtkWidget * win, int x,
                                                 int y, int w, int h);
