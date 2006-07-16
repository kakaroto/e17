#ifndef __EDVI_PAGE_H__
#define __EDVI_PAGE_H__


#include <Evas.h>

#include "edvi_forward.h"


Edvi_Page *edvi_page_new (Edvi_Document *document, int index);

void edvi_page_delete (Edvi_Page *page);

void edvi_page_render (Edvi_Page *page, Edvi_Device *device, Evas_Object *o);

void edvi_page_mag_set (Edvi_Page *page,
                       double     mag);

double edvi_page_mag_get (Edvi_Page *page);

void edvi_page_size_set (Edvi_Page     *page,
                         Edvi_Page_Size size);

Edvi_Page_Size edvi_page_size_get (Edvi_Page     *page);

int edvi_page_width_get (Edvi_Page *page);

int edvi_page_height_get (Edvi_Page *page);

void edvi_page_orientation_set (Edvi_Page            *page,
                                Edvi_Page_Orientation orientation);

Edvi_Page_Orientation edvi_page_orientation_get (Edvi_Page *page);


#endif /* __EDVI_PAGE_H__ */
