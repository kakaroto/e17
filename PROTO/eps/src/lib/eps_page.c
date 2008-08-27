#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Evas.h>

#include <libspectre/spectre.h>

#include "eps_enum.h"
#include "eps_private.h"
#include "eps_page.h"


Eps_Page *
eps_page_new (const Eps_Document *document)
{
   Eps_Page     *page;
   SpectreStatus status;

   if (!document)
     return NULL;

   page = (Eps_Page *)malloc (sizeof (Eps_Page));
   if (!page)
     return NULL;

   page->document = document->ps_doc;
   page->page = spectre_document_get_page(document->ps_doc, 0);

   status = spectre_page_status (page->page);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      free (page);
      return 0;
   }

   page->rc = spectre_render_context_new ();
   if (!page->rc) {
      free (page);
      return NULL;
   }

   spectre_render_context_set_rotation (page->rc, 0);
   spectre_render_context_set_scale (page->rc, 1.0, 1.0);

   return page;
}

void
eps_page_delete (Eps_Page *page)
{
   if (!page)
     return;

   spectre_render_context_free (page->rc);
   spectre_page_free (page->page);
   free (page);
}

void
eps_page_render (const Eps_Page *page,
                 Evas_Object    *o)
{
   unsigned char *data;
   unsigned char *d;
   unsigned int  *m = NULL;
   double         hscale;
   double         vscale;
   SpectreStatus  status;
   int            width;
   int            height;
   int            stride;
   int            yy;

   if (!page || !o)
     return;

   spectre_page_render (page->page, page->rc,
                        &data, &stride);
   status = spectre_page_status (page->page);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return;
   }

   eps_page_scale_get (page, &hscale, &vscale);
   eps_page_size_get (page, &width, &height);

   width *= hscale;
   height *= vscale;

   evas_object_image_size_set (o, width, height);
   evas_object_image_fill_set (o, 0, 0, width, height);
   m = (unsigned int *)evas_object_image_data_get (o, 1);
   if (!m) {
      free (data);
      return;
   }

   if (stride == 4 * width)
     memcpy(m, data, height * stride);
   else {
      d = data;
      for (yy = 0; yy < height; d += stride, m += width, ++yy) {
         memcpy (m, d, width * 4);
      }
   }
   evas_object_image_data_update_add (o, 0, 0, width, height);
   evas_object_resize (o, width, height);

   free (data);
}

void
eps_page_render_slice (const Eps_Page *page,
                       Evas_Object    *o,
                       int             x,
                       int             y,
                       int             width,
                       int             height)
{
   unsigned char *data;
   unsigned char *d;
   unsigned int  *m = NULL;
   double         hscale;
   double         vscale;
   SpectreStatus  status;
   int            stride;
   int            yy;

   if (!page || !o || (width <= 0) || (height <= 0))
     return;

   spectre_page_render_slice (page->page, page->rc,
                              x, y, width, height,
                              &data, &stride);
   status = spectre_page_status (page->page);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("aie !!%d  %d %d %d %d\n", status, x, y, width, height);
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return;
   }

   eps_page_scale_get (page, &hscale, &vscale);

   width *= hscale;
   height *= vscale;

   evas_object_image_size_set (o, width, height);
   evas_object_image_fill_set (o, 0, 0, width, height);
   m = (unsigned int *)evas_object_image_data_get (o, 1);
   if (!m) {
      free (data);
      return;
   }

   d = data + y * stride + x;
   for (yy = 0; yy < height; d += stride, m += width, ++yy) {
      memcpy (m, d, width * 4);
   }
   evas_object_image_data_update_add (o, 0, 0, width, height);
   evas_object_resize (o, width, height);

   free (data);
}

void
eps_page_page_set (Eps_Page *page, int p)
{
   SpectrePage  *new_page;
   SpectreStatus status;

   if (!page)
     return;

   new_page = spectre_document_get_page(page->document, p);

   status = spectre_page_status (new_page);
   if (status != SPECTRE_STATUS_SUCCESS) {
      printf ("[eps] %s\n", spectre_status_to_string (status));
      return;
   }

   spectre_page_free (page->page);
   page->page = new_page;
}

int
eps_page_page_get (const Eps_Page *page)
{
   if (!page)
     return 0;

   return spectre_page_get_index (page->page);
}

const char *
eps_page_label_get (const Eps_Page *page)
{
   if (!page)
     return NULL;

   return spectre_page_get_label (page->page);
}

void
eps_page_size_get (const Eps_Page *page, int *width, int *height)
{
   SpectreOrientation orientation;
   int                w = 0;
   int                h = 0;

  if (page && page->page) {
     spectre_page_get_size (page->page, &w, &h);
     orientation = spectre_page_get_orientation (page->page);
     switch (orientation) {
      case SPECTRE_ORIENTATION_REVERSE_PORTRAIT:
      case SPECTRE_ORIENTATION_PORTRAIT:
         if (width) *width = w;
         if (height) *height = h;
         return;
      case SPECTRE_ORIENTATION_LANDSCAPE:
      case SPECTRE_ORIENTATION_REVERSE_LANDSCAPE:
         if (width) *width = h;
         if (height) *height = w;
         return;
     }
  }

  if (width) *width = 0;
  if (height) *height = 0;
}

void
eps_page_scale_set (const Eps_Page *page, double hscale, double vscale)
{
   if (!page)
     return;

   spectre_render_context_set_scale (page->rc, hscale, vscale);
}

void
eps_page_scale_get (const Eps_Page *page, double *hscale, double *vscale)
{
   if (!page)
     {
        if (hscale) *hscale = 1.0;
        if (vscale) *vscale = 1.0;
     }

   spectre_render_context_get_scale (page->rc, hscale, vscale);
}

void
eps_page_orientation_set (const Eps_Page      *page,
                          Eps_Page_Orientation orientation)
{
   int angle;

   if (!page)
     return;

   switch (orientation) {
    case EPS_PAGE_ORIENTATION_LANDSCAPE:
       angle = 90;
       break;
    case EPS_PAGE_ORIENTATION_UPSIDEDOWN:
       angle = 180;
       break;
    case EPS_PAGE_ORIENTATION_SEASCAPE:
       angle = 270;
       break;
    case EPS_PAGE_ORIENTATION_PORTRAIT:
       angle = 0;
       break;
    /* should not be possible but we return EPS_PAGE_ORIENTATION_PORTRAIT in default case */
    default:
       angle = 0;
       break;
   }

   spectre_render_context_set_rotation (page->rc, angle);
}

Eps_Page_Orientation
eps_page_orientation_get (const Eps_Page *page)
{
   SpectreOrientation orientation;

   if (!page)
     return EPS_PAGE_ORIENTATION_PORTRAIT;

   orientation = spectre_page_get_orientation (page->page);
   switch (orientation) {
    case SPECTRE_ORIENTATION_LANDSCAPE:
       return EPS_PAGE_ORIENTATION_LANDSCAPE;
    case SPECTRE_ORIENTATION_REVERSE_PORTRAIT:
       return EPS_PAGE_ORIENTATION_UPSIDEDOWN;
    case SPECTRE_ORIENTATION_REVERSE_LANDSCAPE:
       return EPS_PAGE_ORIENTATION_SEASCAPE;
    case SPECTRE_ORIENTATION_PORTRAIT:
       return EPS_PAGE_ORIENTATION_PORTRAIT;
    /* should not be possible but we return EPS_PAGE_ORIENTATION_PORTRAIT in default case */
    default:
       return EPS_PAGE_ORIENTATION_PORTRAIT;
   }
}
