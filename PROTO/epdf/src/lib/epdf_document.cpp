#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>

#include <PDFDoc.h>
#include <Outline.h>
#include <GfxFont.h>
#include <FontInfo.h>
#include <ErrorCodes.h>
#include <PDFDocEncoding.h>
#include <UnicodeMap.h>

#include "Epdf.h"
#include "epdf_private.h"


#if HAVE___ATTRIBUTE__
# define __UNUSED__ __attribute__((unused))
#else
# define __UNUSED__
#endif


Epdf_Document *
epdf_document_new (const char *filename)
{
  Epdf_Document *doc;

  if (!filename)
    return NULL;

  doc = (Epdf_Document *)malloc (sizeof (Epdf_Document));
  if (!doc)
    return NULL;

  doc->pdfdoc = new PDFDoc (new GooString (filename), NULL);
  if (doc->pdfdoc->isOk() || doc->pdfdoc->getErrorCode() == errEncrypted) {
    Object         obj;

    if (doc->pdfdoc->getErrorCode() == errEncrypted)
      doc->locked = true;
    else
      doc->locked = false;
    doc->scanner = new FontInfoScanner(doc->pdfdoc);

    doc->pdfdoc->getDocInfo (&obj);
    if (!obj.isDict ()) {
      obj.free ();
      free (doc);

      return NULL;
    }

    doc->dict = obj.getDict();

    return doc;
  }

  free (doc);

  return NULL;
}

void
epdf_document_delete (Epdf_Document *document)
{
  if (!document)
    return;

  delete document->pdfdoc;
  delete document->dict;
  delete document->scanner;
  free (document);
}

int
epdf_document_page_count_get (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->getNumPages();
}

Epdf_Document_Page_Mode
epdf_document_page_mode_get (const Epdf_Document *document)
{
  if (!document || !document->pdfdoc ||
      !document->pdfdoc->getCatalog () || !document->pdfdoc->getCatalog ()->isOk ())
    return EPDF_DOCUMENT_PAGE_MODE_USE_NONE;

  switch (document->pdfdoc->getCatalog ()->getPageMode ()) {
  case Catalog::pageModeNone:
    return EPDF_DOCUMENT_PAGE_MODE_USE_NONE;
  case Catalog::pageModeOutlines:
    return EPDF_DOCUMENT_PAGE_MODE_USE_OUTLINES;
  case Catalog::pageModeThumbs:
    return EPDF_DOCUMENT_PAGE_MODE_USE_THUMBS;
  case Catalog::pageModeFullScreen:
    return EPDF_DOCUMENT_PAGE_MODE_FULLSCREEN;
  case Catalog::pageModeOC:
    return EPDF_DOCUMENT_PAGE_MODE_USE_OC;
  default:
    return EPDF_DOCUMENT_PAGE_MODE_USE_NONE;
  }
}

Epdf_Document_Page_Layout
epdf_document_page_layout_get (Epdf_Document *document)
{
  if (!document || !document->pdfdoc ||
      !document->pdfdoc->getCatalog () || !document->pdfdoc->getCatalog ()->isOk ())
    return EPDF_DOCUMENT_PAGE_LAYOUT_NONE;

  switch (document->pdfdoc->getCatalog ()->getPageLayout ()) {
  case Catalog::pageLayoutNone:
    return EPDF_DOCUMENT_PAGE_LAYOUT_NONE;
  case Catalog::pageLayoutSinglePage:
    return EPDF_DOCUMENT_PAGE_LAYOUT_SINGLE_PAGE;
  case Catalog::pageLayoutOneColumn:
    return EPDF_DOCUMENT_PAGE_LAYOUT_ONE_COLUMN;
  case Catalog::pageLayoutTwoColumnLeft:
    return EPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_LEFT;
  case Catalog::pageLayoutTwoColumnRight:
    return EPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
  case Catalog::pageLayoutTwoPageLeft:
    return EPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_LEFT;
  case Catalog::pageLayoutTwoPageRight:
    return EPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_RIGHT;
  default:
    return EPDF_DOCUMENT_PAGE_LAYOUT_NONE;
  }
}

unsigned char
epdf_document_unlock (Epdf_Document *document, const char *password)
{
  if (!document)
    return 0;

  if (document->locked) {
    /* racier then it needs to be */
    GooString *pwd = new GooString(password);
    PDFDoc *pdfdoc = new PDFDoc(document->pdfdoc->getFileName(), pwd);
    delete pwd;
    if (pdfdoc->isOk()) {

      document->pdfdoc = pdfdoc;
      document->locked = false;
    }
  }

  return document->locked;
}

unsigned char
epdf_document_is_locked (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->locked;
}

const char *
epdf_document_info_get (const Epdf_Document *document __UNUSED__, const char *data __UNUSED__)
{
  return NULL;
}

unsigned char
epdf_document_is_encrypted (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->isEncrypted();
}

unsigned char
epdf_document_is_linearized (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->isLinearized();
}

unsigned char
epdf_document_is_printable (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToPrint();
}

unsigned char
epdf_document_is_changeable (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToChange();
}

unsigned char
epdf_document_is_copyable (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToCopy();
}

unsigned char
epdf_document_is_notable (const Epdf_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToAddNotes();
}


double
epdf_document_pdf_version_get (const Epdf_Document *document)
{
  if (!document)
    return 0.0;

  return document->pdfdoc->getPDFVersion();
}

Eina_List *
epdf_document_fonts_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_scan_for_fonts (document,
                                       epdf_document_page_count_get (document));
}

Eina_List *
epdf_document_scan_for_fonts (const Epdf_Document *document, int page_count)
{
  Eina_List *fonts = NULL;
  int        length;

  if (!document)
    return NULL;

  GooList *items = document->scanner->scan (page_count);
  if (!items)
    return NULL;

  length = items->getLength();

  for ( int i = 0; i < length; ++i ) {
    Epdf_Font_Info     *font;
    Epdf_Font_Info     *font2;
    FontInfo           *fi;
    char               *font_name = NULL;
    char               *font_path = NULL;
    int                 index = 0;

    fi = (::FontInfo*)(items->get(i));
    if (fi->getName())
      font_name = fi->getName()->getCString();

    if (fi->getFile())
      font_path = fi->getFile()->getCString();

    font = epdf_font_info_new (font_name, font_path,
                               fi->getEmbedded(),
                               fi->getSubset(),
                               (Epdf_Font_Info_Type)fi->getType());

//     printf ("font name : %s\n", font_name);
//     printf ("font name : %s\n", font->font_name);
//     while ((font->font_name[index]) &&
//            (font->font_name[index] != '+'))
//       index++;
//     if (font->font_name[index] == '\0')
//       index = 0;
//     else
//       index++;
//     font_name = strdup (font->font_name + index);
//     font2 = epdf_font_info_new (font_name,
//                                 font_path,
//                                 font->is_embedded,
//                                 font->is_subset,
//                                 font->type);
//     ecore_list_append (fonts, font2);
    fonts = eina_list_append (fonts, font);
//     free (font_name);
//     epdf_font_info_delete (font);
  }

  deleteGooList (items, FontInfo);

  return fonts;
}

const char *
epdf_document_filename_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return document->pdfdoc->getFileName()->getCString();
}

static char *
epdf_document_property_get (const Epdf_Document *document, const char *property)
{
  Object     obj;
  GooString *goo_string;
  char      *title = NULL;

  if (!document)
    return NULL;

  if (!document->dict->lookup ((char *)property, &obj)->isString ()) {
    obj.free ();

    return NULL;
  }

  goo_string = obj.getString ();
  /* Unicode */
  if (((goo_string->getChar (0) & 0xff) == 0xfe) &&
      ((goo_string->getChar (1) & 0xff) == 0xff)) {
    printf ("Unicode !\n");
    title = NULL;
  }
  else {
    char *temp;
    int   length;
    int   i;

    length = goo_string->getLength ();
    temp = (char *)malloc (sizeof (char) * (length + 1));
    for (i = 0; i < length; i++)
      temp[i] = pdfDocEncoding[(unsigned char)goo_string->getChar (i)];
    temp[length] = '\0';

    title = temp;
  }
  obj.free ();

  return title;
}

char *
epdf_document_title_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_property_get (document, "Title");
}

char *
epdf_document_author_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_property_get (document, "Author");
}

char *
epdf_document_subject_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_property_get (document, "Subject");
}

char *
epdf_document_keywords_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_property_get (document, "Keywords");
}

char *
epdf_document_creator_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_property_get (document, "Creator");
}

char *
epdf_document_producer_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_property_get (document, "Producer");
}

static char *
epdf_document_date_get (const Epdf_Document *document, const char *type)
{
  Object     obj;
  GooString *goo_string;
  char      *date = NULL;
  char       res[64];
  int        year;
  int        month;
  int        day;
  int        hour;
  int        minute;
  int        second;
  int        scanned_items;

  if (!document)
    return NULL;

  if (!document->dict->lookup ((char *)type, &obj)->isString ()) {
    obj.free ();

    return NULL;
  }

  goo_string = obj.getString ();

  /* Unicode */
  if (((goo_string->getChar (0) & 0xff) == 0xfe) &&
      ((goo_string->getChar (1) & 0xff) == 0xff)) {
    printf ("Unicode !\n");
    return NULL;
  }
  else {
    int length;

    length = goo_string->getLength ();
    date = (char *)malloc (sizeof (char) * (length + 1));
    if (!date)
      {
        obj.free ();

        return NULL;
      }
    memcpy (date, goo_string->getCString (), length + 1);
  }

  /* See PDF Reference 1.3, Section 3.8.2 for PDF Date representation */

  if (date [0] == 'D' && date [1] == ':')
    scanned_items = sscanf (date + 2, "%4d%2d%2d%2d%2d%2d",
                            &year, &month, &day, &hour, &minute, &second);
  else
    scanned_items = sscanf (date, "%4d%2d%2d%2d%2d%2d",
                            &year, &month, &day, &hour, &minute, &second);
  if (scanned_items != 6)
    {
      free (date);
      obj.free ();

      return NULL;
    }

  /* Workaround for y2k bug in Distiller 3, hoping that it won't
   * be used after y2.2k */
  if ((year < 1930) &&
      (strlen (date) > 14)) {
    int century;
    int years_since_1900;

    scanned_items = sscanf (date, "%2d%3d%2d%2d%2d%2d%2d",
                            &century, &years_since_1900, &month, &day, &hour, &minute, &second);

    if (scanned_items != 7) {
      free (date);
      obj.free ();

      return NULL;
    }

    year = century * 100 + years_since_1900;
  }

  snprintf (res, 64, "%d-%d-%d, %d:%d:%d", year, month, day, hour, minute, second);
  free (date);
  obj.free ();

  return strdup (res);
}

char *
epdf_document_creation_date_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_date_get (document, "CreationDate");
}

char *
epdf_document_mod_date_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  return epdf_document_date_get (document, "ModDate");
}

const char *
epdf_document_linearized_get (const Epdf_Document *document)
{
  if (!document)
    return NULL;

  if (epdf_document_is_linearized (document))
    return "yes";
  else
    return "no";
}

const char *
epdf_document_page_mode_string_get (const Epdf_Document *document)
{
  if (!document || !document->pdfdoc ||
      !document->pdfdoc->getCatalog () || !document->pdfdoc->getCatalog ()->isOk ())
    return "no document";

  switch (document->pdfdoc->getCatalog()->getPageMode()) {
  case Catalog::pageModeNone:
    return "none";
  case Catalog::pageModeOutlines:
    return "outlines";
  case Catalog::pageModeThumbs:
    return "thumbs";
  case Catalog::pageModeFullScreen:
    return "fullscreen";
  case Catalog::pageModeOC:
    return "OC";
  default:
    return "none";
  }
}

const char *
epdf_document_page_layout_string_get (const Epdf_Document *document)
{
  if (!document || !document->pdfdoc ||
      !document->pdfdoc->getCatalog () || !document->pdfdoc->getCatalog ()->isOk ())
    return "no document";

  switch (document->pdfdoc->getCatalog ()->getPageLayout ()) {
  case Catalog::pageLayoutNone:
    return "none";
  case Catalog::pageLayoutSinglePage:
    return "single page";
  case Catalog::pageLayoutOneColumn:
    return "one column";
  case Catalog::pageLayoutTwoColumnLeft:
    return "two column left";
  case Catalog::pageLayoutTwoColumnRight:
    return "two column right";
  case Catalog::pageLayoutTwoPageLeft:
    return "two page left";
  case Catalog::pageLayoutTwoPageRight:
    return "two page right";
  default:
    return "none";
  }
}
