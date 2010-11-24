#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef _WIN32
# include <winsock2.h>
#else
# include <arpa/inet.h>
#endif

#include <fitz.h>
#include <mupdf.h>

#include <Evas.h>

#include "Epdf.h"
#include "epdf_mupdf_private.h"

Epdf_Page *
epdf_page_new(const Epdf_Document *doc)
{
   Epdf_Page *page;

   if (!doc)
     return NULL;

   page = (Epdf_Page*)malloc(sizeof(Epdf_Page));
   if (!page)
     return NULL;

   page->doc = (Epdf_Document*)doc;
   page->page = NULL;
   page->index = 0;
   page->image = NULL;
   page->orientation = EPDF_PAGE_ORIENTATION_PORTRAIT;

   return page;
}

void
epdf_page_delete(Epdf_Page *page)
{
   if (!page)
     return;

   if (page->page)
     {
        pdf_droppage(page->page); 
        page->page = NULL;
     }

   free(page);
}

static fz_matrix
epdf_page_viewctm(Epdf_Page *page)
{
   Epdf_Document *doc;
   fz_matrix ctm;

   doc = page->doc;
   switch (page->orientation) {
   case EPDF_PAGE_ORIENTATION_LANDSCAPE:
     doc->rotate = 90;
     break;
   case EPDF_PAGE_ORIENTATION_UPSIDEDOWN:
     doc->rotate = 180;
     break;
   case EPDF_PAGE_ORIENTATION_SEASCAPE:
     doc->rotate = 270;
     break;
   case EPDF_PAGE_ORIENTATION_PORTRAIT:
   default:
     doc->rotate = 0;
     break;
   }

   ctm = fz_identity();
   ctm = fz_concat(ctm, fz_translate(0, -page->page->mediabox.y1));
   ctm = fz_concat(ctm, fz_scale(doc->zoom, -doc->zoom));
   ctm = fz_concat(ctm, fz_rotate(doc->rotate + page->page->rotate));

   return ctm;
}

static void
epdf_page_load(Epdf_Page *page)
{
   Epdf_Document *doc;
   fz_obj *obj;
   fz_error error;

   if (!page)
     return;

   if (page->page)
     pdf_droppage(page->page);
   page->page = nil;

   doc = page->doc;
   if (!doc)
     return;

   /* load page */
   obj = pdf_getpageobject(doc->xref, page->index);
   if (!obj)
     return;

   error = pdf_loadpage(&page->page, doc->xref, obj);
   if (error)
     return;
}

void
epdf_page_render(Epdf_Page *page, Evas_Object *o)
{
   double t1,t2;
   epdf_page_render_slice(page, o, 0, 0, -1, -1);
}

void
evas_object_callback_del(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   fz_pixmap *image;

   image = (fz_pixmap*)evas_object_data_get(obj, "epdf_image");
   if (image)
     {
        evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, evas_object_callback_del);
        evas_object_data_del(obj, "epdf_image");
        fz_droppixmap(image);
        image = nil;
     }
}

void
epdf_page_render_slice(Epdf_Page   *page,
                       Evas_Object *o,
                       int          x,
                       int          y,
                       int          w,
                       int          h)
{
   Epdf_Document *doc;
   fz_error error;
   fz_matrix ctm;
   fz_rect bbox;
   fz_pixmap *image;
   unsigned int *d;
   int width;
   int height;
   int i;

   if (!page)
     return;

   if (!page->page)
     epdf_page_load(page);

   doc = page->doc;
   if (!doc)
     return;

   /* drop old pixmap */
   image = (fz_pixmap*)evas_object_data_get(o, "epdf_image");
   if (image)
     {
        evas_object_event_callback_del(o, EVAS_CALLBACK_DEL, evas_object_callback_del);
        evas_object_data_del(o, "epdf_image");
        fz_droppixmap(image);
        image = nil;
     }

   ctm = epdf_page_viewctm(page);

   if (w < 0 || h < 0)
     {
        bbox = fz_transformaabb(ctm, page->page->mediabox);
     }
   else
     {
        bbox.x0 = x;
        bbox.x1 = x + w;
        bbox.y0 = y;
        bbox.y1 = y + h;
     }

   error = fz_rendertree(&image, doc->rast, page->page->tree,
                         ctm, fz_roundrect(bbox), 1);
   if (error || !image)
     return;

   width = image->w;
   height = image->h;

   evas_object_image_size_set(o, width, height);
   evas_object_image_data_set(o, image->samples);
   evas_object_image_fill_set(o, 0, 0, width, height);
   evas_object_image_data_update_add(o, 0, 0, width, height);
   evas_object_resize(o, width, height);

   d = evas_object_image_data_get(o, 1);
   for (i = 0; i < height * width; i++, d++)
     *d = htonl(*d);

   evas_object_data_set(o, "epdf_image", image);
   evas_object_event_callback_add(o, EVAS_CALLBACK_DEL, evas_object_callback_del, NULL);
}

void
epdf_page_page_set(Epdf_Page *page, int p)
{
   if (!page)
     return;

   page->index = p + 1;
   if (page->page)
     pdf_droppage(page->page);
   page->page = nil;
}

int
epdf_page_page_get(const Epdf_Page *page)
{
   if (!page)
     return -1;

   return page->index - 1;
}

void
epdf_page_size_get(const Epdf_Page *page, int *width, int *height)
{
   fz_rect mediabox;

   if (!page)
     {
        *width = 0;
        *height = 0;
        return;
     }

   if (!page->page)
     epdf_page_load((Epdf_Page*)page);

   mediabox = page->page->mediabox;

   *width = mediabox.x1 - mediabox.x0;
   *height = mediabox.y1 - mediabox.y0;
}

void
epdf_page_content_geometry_get(const Epdf_Page *page, int *x, int *y, int *width, int *height)
{
   fz_rect contentbox;

   if (!page)
     {
        *x = 0;
        *y = 0;
        *width = 0;
        *height = 0;
        return;
     }

   if (!page->page)
     epdf_page_load((Epdf_Page*)page);

   contentbox = fz_boundnode(page->page->tree->root, fz_identity());

   *x = contentbox.x0;
   *y = contentbox.y0;
   *width = contentbox.x1 - contentbox.x0;
   *height = contentbox.y1 - contentbox.y0;
}

void
epdf_page_scale_set(Epdf_Page *page,
                    double hscale,
                    double vscale)
{
   if (!page || !page->doc)
     return;

   page->doc->zoom = hscale;
}

void
epdf_page_scale_get(const Epdf_Page *page,
                    double          *hscale,
                    double          *vscale)
{
   if (!page || !page->doc)
     {
        *hscale = 0.0;
        *vscale = 0.0;
        return;
     }

   *hscale = *vscale = page->doc->zoom;
}

void
epdf_page_orientation_set (Epdf_Page            *page,
                           Epdf_Page_Orientation orientation)
{
   if (!page)
     return;

   if (page->orientation != orientation)
     page->orientation = orientation;
}

Epdf_Page_Orientation
epdf_page_orientation_get (const Epdf_Page *page)
{
   if (!page)
     return EPDF_PAGE_ORIENTATION_PORTRAIT;

   return page->orientation;
}

Eina_List *
epdf_page_text_find (const Epdf_Page *page,
                     const char      *text,
                     unsigned char    is_case_sensitive)
{
   return NULL;
}
