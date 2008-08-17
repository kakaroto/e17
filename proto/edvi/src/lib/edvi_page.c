#include <stdlib.h>
#include <stdio.h>
#include "config.h"

#ifdef HAVE_DVILIB_2_9
#  include <libdvi29.h>
#else
#  include <dvi-2_6.h>
#endif /* HAVE_DVILIB_2_9 */

#include "edvi_enum.h"
#include "edvi_forward.h"
#include "edvi_private.h"
#include "edvi_main.h"
#include "edvi_framebuffer.h"


extern Edvi_Framebuffer _fb;

static char *
_edvi_format_string_get (const Edvi_Page *page)
{
  if (!page)
    return "A4";

  switch (page->format) {
  case EDVI_PAGE_FORMAT_A1:
    return "A1";
  case EDVI_PAGE_FORMAT_A2:
    return "A2";
  case EDVI_PAGE_FORMAT_A3:
    return "A3";
  case EDVI_PAGE_FORMAT_A4:
    return "A4";
  case EDVI_PAGE_FORMAT_A5:
    return "A5";
  case EDVI_PAGE_FORMAT_A6:
    return "A6";
  case EDVI_PAGE_FORMAT_A7:
    return "A7";
  case EDVI_PAGE_FORMAT_B1:
    return "B1";
  case EDVI_PAGE_FORMAT_B2:
    return "B2";
  case EDVI_PAGE_FORMAT_B3:
    return "B3";
  case EDVI_PAGE_FORMAT_B4:
    return "B4";
  case EDVI_PAGE_FORMAT_B5:
    return "B5";
  case EDVI_PAGE_FORMAT_B6:
    return "B6";
  case EDVI_PAGE_FORMAT_B7:
    return "B7";
  case EDVI_PAGE_FORMAT_LETTER:
    return "LETTER";
  case EDVI_PAGE_FORMAT_US:
    return "US";
  case EDVI_PAGE_FORMAT_LEGAL:
    return "LEGAL";
  default:
    return "A4";
  }
}

Edvi_Page *
edvi_page_new (Edvi_Document *document)
{
  Edvi_Page *page;

  if (!document)
    return NULL;

  page = (Edvi_Page *)malloc (sizeof (Edvi_Page));
  if (!page)
    return NULL;

  page->doc = document;
  page->page = 0;
  page->mag = 1.0;
  page->shrink = EDVI_DEFAULT_SHRINK * edvi_dpi_get () / (page->mag * EDVI_DEFAULT_DPI * edvi_aa_get ());
  page->offset_x = edvi_offset_x_get () * edvi_dpi_get () / page->shrink;
  page->offset_y = edvi_offset_y_get () * edvi_dpi_get () / page->shrink;
  page->format = EDVI_PAGE_FORMAT_A4;
  page->orientation = EDVI_PAGE_ORIENTATION_PORTRAIT;

  return page;
}

void
edvi_page_delete (Edvi_Page *page)
{
  if (!page)
    return;

  free (page);
}

void
edvi_page_render (const Edvi_Page *page, Edvi_Device *device, Evas_Object *o)
{
  Edvi_Framebuffer *fb;
  double            pw;
  double            ph;
  int               res;

  if (!page || ! device || !o)
    return;

  if (DVI_parse_paper_size (_edvi_format_string_get (page), &pw, &ph) < 0)
    return;
  edvi_device_evas_object_set (device, o);
  fb = edvi_framebuffer_new ((long)(pw * edvi_dpi_get () / (page->shrink * edvi_aa_get ())),
                             (long)(ph * edvi_dpi_get () / (page->shrink * edvi_aa_get ())),
                             edvi_aa_get ());
  edvi_device_framebuffer_set (device, fb);
  edvi_device_offset_set (device,
                          edvi_offset_x_get () * edvi_dpi_get () / page->shrink,
                          edvi_offset_y_get () * edvi_dpi_get () / page->shrink);
  edvi_device_draw_page_finished (0);
  res = DVI_DRAW_PAGE (page->doc->dvi_document, device->dvi_device, page->page + 1, page->shrink);
  if (res != DVI_DRAW_INTERRUPTED)
    edvi_device_draw_page_finished (1);
}

void
edvi_page_page_set (Edvi_Page *page, int p)
{
  if (!page)
    return;

  page->page = p;
}

int
edvi_page_page_get (const Edvi_Page *page)
{
  if (!page)
    return 0;

  return page->page;
}

void
edvi_page_size_get (const Edvi_Page *page, int *width, int *height)
{
  double dwidth;
  double dheight;
  int    w = 0;
  int    h = 0;

  if (page) {
    if (DVI_parse_paper_size (_edvi_format_string_get (page), &dwidth,
                              &dheight) >= 0) {
      if (page->orientation == EDVI_PAGE_ORIENTATION_PORTRAIT) {
        w = dwidth * edvi_dpi_get () / (page->shrink * edvi_aa_get ());
        h = dheight * edvi_dpi_get () / (page->shrink * edvi_aa_get ());
      }
      else {
        w = dheight * edvi_dpi_get () / (page->shrink * edvi_aa_get ());
        h = dwidth * edvi_dpi_get () / (page->shrink * edvi_aa_get ());
      }
    }
  }

  if (width) *width = w;
  if (height) *height = h;
}

void
edvi_page_mag_set (Edvi_Page *page,
                   double     mag)
{
  if (!page)
    return;

  if (page->mag != mag) {
    if (mag < 0.01)
      mag = 0.01;
    if (mag > 5.0)
      mag = 5.0;
    page->mag = mag;
    page->shrink = EDVI_DEFAULT_SHRINK * edvi_dpi_get () / (page->mag * EDVI_DEFAULT_DPI * edvi_aa_get ());
    page->offset_x = edvi_offset_x_get () * edvi_dpi_get () / page->shrink;
    page->offset_y = edvi_offset_y_get () * edvi_dpi_get () / page->shrink;
  }
}

double
edvi_page_mag_get (const  Edvi_Page *page)
{
  if (!page)
    return 0;

  return page->mag;
}

void
edvi_page_format_set (Edvi_Page       *page,
                      Edvi_Page_Format format)
{
  if (!page)
    return;

  if (page->format != format)
    page->format = format;
}

Edvi_Page_Format
edvi_page_format_get (const Edvi_Page *page)
{
  if (!page)
    return EDVI_PAGE_FORMAT_A4;

  return page->format;
}

void
edvi_page_orientation_set (Edvi_Page            *page,
                           Edvi_Page_Orientation orientation)
{
  if (!page)
    return;

  if (page->orientation != orientation)
    page->orientation = orientation;
}

Edvi_Page_Orientation
edvi_page_orientation_get (const Edvi_Page *page)
{
  if (!page)
    return EDVI_PAGE_ORIENTATION_PORTRAIT;

  return page->orientation;
}
