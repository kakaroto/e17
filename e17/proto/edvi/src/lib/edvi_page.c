#include <stdlib.h>

#include <libdvi29.h>

#include "edvi_enum.h"
#include "edvi_forward.h"
#include "edvi_private.h"
#include "edvi.h"
#include "edvi_framebuffer.h"


extern Edvi_Framebuffer _fb;

static char *
_edvi_size_string_get (Edvi_Page *page)
{
  if (!page)
    return "A4";

  switch (page->size) {
  case EDVI_PAGE_SIZE_A1:
    return "A1";
  case EDVI_PAGE_SIZE_A2:
    return "A2";
  case EDVI_PAGE_SIZE_A3:
    return "A3";
  case EDVI_PAGE_SIZE_A4:
    return "A4";
  case EDVI_PAGE_SIZE_A5:
    return "A5";
  case EDVI_PAGE_SIZE_A6:
    return "A6";
  case EDVI_PAGE_SIZE_A7:
    return "A7";
  case EDVI_PAGE_SIZE_B1:
    return "B1";
  case EDVI_PAGE_SIZE_B2:
    return "B2";
  case EDVI_PAGE_SIZE_B3:
    return "B3";
  case EDVI_PAGE_SIZE_B4:
    return "B4";
  case EDVI_PAGE_SIZE_B5:
    return "B5";
  case EDVI_PAGE_SIZE_B6:
    return "B6";
  case EDVI_PAGE_SIZE_B7:
    return "B7";
  case EDVI_PAGE_SIZE_LETTER:
    return "LETTER";
  case EDVI_PAGE_SIZE_US:
    return "US";
  case EDVI_PAGE_SIZE_LEGAL:
    return "LEGAL";
  default:
    return "A4";
  }
}

Edvi_Page *
edvi_page_new (Edvi_Document *document, int index)
{
  Edvi_Page *page;

  if (!document)
    return NULL;

  page = (Edvi_Page *)malloc (sizeof (Edvi_Page));
  if (!page)
    return NULL;

  page->doc = document;
  page->page = index + 1;
  page->mag = 1.0;
  page->shrink = EDVI_DEFAULT_SHRINK * edvi_dpi_get () / (page->mag * EDVI_DEFAULT_DPI * edvi_aa_get ());
  page->offset_x = edvi_offset_x_get () * edvi_dpi_get () / page->shrink;
  page->offset_y = edvi_offset_y_get () * edvi_dpi_get () / page->shrink;
  page->size = EDVI_PAGE_SIZE_A4;
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
edvi_page_render (Edvi_Page *page, Edvi_Device *device, Evas_Object *o)
{
  Edvi_Framebuffer *fb;
  double            pw;
  double            ph;
  int               res;

  if (DVI_parse_paper_size (_edvi_size_string_get (page), &pw, &ph) < 0)
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
  res = DVI_DRAW_PAGE (page->doc->dvi_document, device->dvi_device, page->page, page->shrink);
  if (res != DVI_DRAW_INTERRUPTED)
    edvi_device_draw_page_finished (1);
}

int
edvi_page_number_get (Edvi_Page *page)
{
  if (!page)
    return -1;

  return page->page - 1;
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
edvi_page_mag_get (Edvi_Page *page)
{
  if (!page)
    return 0;

  return page->mag;
}

void
edvi_page_size_set (Edvi_Page     *page,
                    Edvi_Page_Size size)
{
  if (!page)
    return;

  if (page->size != size)
    page->size = size;
}

Edvi_Page_Size
edvi_page_size_get (Edvi_Page     *page)
{
  if (!page)
    return EDVI_PAGE_SIZE_A4;

  return page->size;
}

int
edvi_page_width_get (Edvi_Page *page)
{
  double dwidth;
  double dheight;

  if (DVI_parse_paper_size (_edvi_size_string_get (page), &dwidth,
                            &dheight) < 0) {
    return 0;
  }

  return (page->orientation == EDVI_PAGE_ORIENTATION_PORTRAIT ? dwidth : dheight) * edvi_dpi_get () / (page->shrink * edvi_aa_get ());
}

int
edvi_page_height_get (Edvi_Page *page)
{
  double dwidth;
  double dheight;

  if (DVI_parse_paper_size (_edvi_size_string_get (page), &dwidth, &dheight) < 0)
    return 0;

  return (page->orientation == EDVI_PAGE_ORIENTATION_PORTRAIT ? dheight : dwidth) * edvi_dpi_get () / (page->shrink * edvi_aa_get ());
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
edvi_page_orientation_get (Edvi_Page *page)
{
  if (!page)
    return EDVI_PAGE_ORIENTATION_PORTRAIT;
  
  return page->orientation;
}
