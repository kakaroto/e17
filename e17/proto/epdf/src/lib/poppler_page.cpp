#include <stdlib.h>

#include <Evas.h>
#include <Ecore_Data.h>

#include <PDFDoc.h>
#include <Page.h>
#include <Gfx.h>
#include <TextOutputDev.h>
#include <UGooString.h>
#include <SplashOutputDev.h>
#include <splash/SplashBitmap.h>

#include "poppler_enum.h"
#include "poppler_private.h"
#include "poppler_page.h"
#include "poppler_page_transition.h"


Epdf_Page *
epdf_page_new (const Epdf_Document *doc, int index)
{
  Epdf_Page *page;

  if (!doc)
    return NULL;

  page = (Epdf_Page *)malloc (sizeof (Epdf_Page));
  if (!page)
    return NULL;

  page->page = doc->pdfdoc->getCatalog()->getPage(index + 1);
  if (!page->page) {
    free (page);
    return NULL;
  }

  page->doc = (Epdf_Document *)doc;
  page->index = index;
  page->transition = NULL;
  page->text_dev = NULL;
  page->gfx = NULL;

  return page;
}

void
epdf_page_delete (Epdf_Page *page)
{
  if (!page)
    return;

  if (page->gfx)
    delete page->gfx;
  if (page->text_dev)
    delete page->text_dev;
  epdf_page_transition_delete (page->transition);
  free (page);
}

void
epdf_page_render (Epdf_Page *page, Evas_Object *o, Epdf_Page_Orientation orientation, int x, int y, int w, int h, double hscale, double vscale)
{
  SplashOutputDev *output_dev;
  SplashColor      white;
  SplashColorPtr   color_ptr;
  Epdf_Document   *doc;
  unsigned int    *m = NULL;
  unsigned int     val;
  int              offset = 0;
  int              rotate;
  int              width;
  int              height;

  white[0] = 255;
  white[1] = 255;
  white[2] = 255;

  doc = page->doc;

  output_dev = new SplashOutputDev(splashModeRGB8, 4, gFalse, white);
  output_dev->startDoc(doc->pdfdoc->getXRef ());
  switch (orientation) {
  case EPDF_PAGE_ORIENTATION_LANDSCAPE:
    rotate = 90;
    break;
  case EPDF_PAGE_ORIENTATION_UPSIDEDOWN:
    rotate = 180;
    break;
  case EPDF_PAGE_ORIENTATION_SEASCAPE:
    rotate = 270;
    break;
  case EPDF_PAGE_ORIENTATION_PORTRAIT:
  default:
    rotate = 0;
    break;
  }
  page->page->displaySlice (output_dev, 72.0 * hscale, 72.0 * vscale,
                            rotate,
                            false, false,
                            x, y, w, h,
                            NULL,
                            doc->pdfdoc->getCatalog ());
  color_ptr = output_dev->getBitmap ()->getDataPtr ();

  width = output_dev->getBitmap()->getWidth();
  height = output_dev->getBitmap()->getHeight();

  evas_object_image_size_set(o, width, height);
  evas_object_image_fill_set(o, 0, 0, width, height);

  m = (unsigned int *)evas_object_image_data_get(o, 1);
  if (!m)
    goto sortie;

  memcpy (m, color_ptr, height * width * 4);
  evas_object_image_data_update_add(o, 0, 0, width, height);
  evas_object_resize(o, width, height);

 sortie:
  delete output_dev;
}

int
epdf_page_number_get (Epdf_Page *page)
{
  if (!page)
    return -1;

  return page->index;
}

static TextOutputDev *
epdf_page_text_output_dev_get (Epdf_Page *page)
{
  if (!page)
    return NULL;

  if (!page->text_dev) {
    page->text_dev = new TextOutputDev (NULL, 1, 0, 0);

    page->gfx = page->page->createGfx (page->text_dev,
                                       72.0, 72.0, 0,
                                       0, /* useMediaBox */
                                       1, /* Crop */
                                       -1, -1, -1, -1,
                                       NULL, /* links */
                                       page->doc->pdfdoc->getCatalog (),
                                       NULL, NULL, NULL, NULL);

    page->page->display(page->gfx);

    page->text_dev->endPage();
  }

  return page->text_dev;
}

char *
epdf_page_text_get (Epdf_Page *page, Epdf_Rectangle r)
{
  TextOutputDev *text_dev;
  PDFDoc        *doc;
  GooString     *sel_text;
  double         height, y1, y2;
  char          *result;
  PDFRectangle   pdf_selection;

  if (!page)
    return NULL;

  text_dev = epdf_page_text_output_dev_get (page);
  height = epdf_page_height_get (page);

  pdf_selection.x1 = r.x1;
  pdf_selection.y1 = r.y1;
  pdf_selection.x2 = r.x2;
  pdf_selection.y2 = r.y2;

  sel_text = new GooString;
  sel_text = text_dev->getSelectionText (&pdf_selection);
  result = strdup (sel_text->getCString ());
  delete sel_text;

  return result;
}

Ecore_List *
epdf_page_text_find (Epdf_Page    *page,
                     const char   *text,
                     unsigned char is_case_sensitive)
{
  Epdf_Rectangle *match;
  TextOutputDev  *output_dev;
  Ecore_List     *matches = NULL;
  double          xMin, yMin, xMax, yMax;
  int             length;
  int             height;
  UGooString      utext(text);

  if (!page || !text)
    return NULL;

  length = strlen (text);

  output_dev = new TextOutputDev (NULL, 1, 0, 0);

  height = epdf_page_height_get (page);
  page->page->display (output_dev, 72, 72, 0, 0,
		       1, NULL,
		       page->doc->pdfdoc->getCatalog());

  xMin = 0;
  yMin = 0;
#warning you probably want to add backwards as parameters
  while (output_dev->findText (utext.unicode (), utext.getLength (),
			       0, 1, // startAtTop, stopAtBottom
			       1, 0, // startAtLast, stopAtLast
			       is_case_sensitive, 0, // caseSensitive, backwards
			       &xMin, &yMin, &xMax, &yMax)) {
    if (!matches)
      matches = ecore_list_new ();
    ecore_list_set_free_cb (matches, ECORE_FREE_CB (free));
    match = (Epdf_Rectangle *)malloc (sizeof (Epdf_Rectangle));
    match->x1 = xMin;
    match->y1 = yMin;//height - yMax;
    match->x2 = xMax;
    match->y2 = yMax;//height - yMin;
    ecore_list_append (matches, match);
  }

  delete output_dev;

  return matches;
}

Epdf_Page_Transition *
epdf_page_transition_get (Epdf_Page *page)
{
  if (!page)
    return NULL;

  return page->transition;
}

int
epdf_page_width_get (Epdf_Page *page)
{
  int rotate;

  if (!page)
    return 0;

  rotate = page->page->getRotate ();
  if (rotate == 90 || rotate == 270)
    return (int)page->page->getMediaHeight ();
  else
    return (int)page->page->getMediaWidth ();
}

int
epdf_page_height_get (Epdf_Page *page)
{
  int rotate;

  if (!page)
    return 0;

  rotate = page->page->getRotate ();
  if (rotate == 90 || rotate == 270)
    return (int)page->page->getMediaWidth ();
  else
    return (int)page->page->getMediaHeight ();
}

Epdf_Page_Orientation
epdf_page_orientation_get (Epdf_Page *page)
{
  int rotation;

  if (!page)
    return EPDF_PAGE_ORIENTATION_LANDSCAPE;

  rotation = page->page->getRotate();;
  switch (rotation) {
  case 90:
    return EPDF_PAGE_ORIENTATION_LANDSCAPE;
  case 180:
    return EPDF_PAGE_ORIENTATION_UPSIDEDOWN;
  case 270:
    return EPDF_PAGE_ORIENTATION_SEASCAPE;
  default:
    return EPDF_PAGE_ORIENTATION_PORTRAIT;
  }
}

const char *
epdf_page_orientation_name_get (Epdf_Page *page)
{
  int rotation;

  if (!page)
    return "Unknown";

  rotation = page->page->getRotate();;
  switch (rotation) {
  case 90:
    return "Landscape";
  case 180:
    return "Upside Down";
  case 270:
    return "Seascape";
  default:
    return "Portrait";
  }
}
