#ifndef GEIST_DOCUMENT_GTK_H
#define GEIST_DOCUMENT_GTK_H

#include "geist.h"
#include "geist_document.h"
#include "geist_interface.h"

void geist_document_render_to_window(geist_document * doc);
void geist_document_render_to_window_partial(geist_document * doc, int x,
                                             int y, int w, int h);
void geist_document_save_as(geist_document * doc);
void geist_document_load(void);
void geist_document_resize_gtk(geist_document *doc, int w, int h);

#endif
