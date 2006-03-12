#include <stdlib.h>
#include <string.h>

#include <GlobalParams.h>
#include <PDFDoc.h>
#include <Outline.h>
#include <GfxFont.h>
#include <FontInfo.h>
#include <ErrorCodes.h>
#include <PDFDocEncoding.h>
#include <UnicodeMap.h>
#include <UGooString.h>

#include "poppler_enum.h"
#include "poppler_private.h"
#include "poppler_fontinfo.h"
#include "poppler_page.h"
#include "poppler_document.h"

Evas_Poppler_Document *
evas_poppler_document_new (const char *filename)
{
  Evas_Poppler_Document *doc;

  if (!filename)
    return NULL;

  doc = (Evas_Poppler_Document *)malloc (sizeof (Evas_Poppler_Document));
  if (!doc)
    return NULL;

  if (!globalParams)
    globalParams = new GlobalParams("/etc/xpdfrc");

  doc->pdfdoc = new PDFDoc (new GooString (filename), NULL);
  if (doc->pdfdoc->isOk() || doc->pdfdoc->getErrorCode() == errEncrypted)
    {
      Object         obj;

      if (doc->pdfdoc->getErrorCode() == errEncrypted)
        doc->locked = true;
      else
        doc->locked = false;
      doc->scanner = new FontInfoScanner(doc->pdfdoc);

      doc->pdfdoc->getDocInfo (&obj);
      if (!obj.isDict ())
        {
          obj.free ();
          free (doc);
          
          return NULL;
        }
      
      doc->dict = obj.getDict();

      return doc;
    }
  else
    {
      free (doc);

      return NULL;
    }
}

void
evas_poppler_document_delete (Evas_Poppler_Document *document)
{
  if (!document)
    return;

  delete document->pdfdoc;
  delete document->dict;
  delete document->scanner;
  free (document);
}

PDFDoc *
evas_poppler_document_doc_get (Evas_Poppler_Document *document)
{
  if (!document)
    {
      const char *file = NULL;
      return NULL;
    }
  return document->pdfdoc;
}

Evas_Poppler_Page *
evas_poppler_document_page_get (Evas_Poppler_Document *document, int index)
{
  if (!document)
    return NULL;

  return evas_poppler_page_new (document, index);
}

int
evas_poppler_document_page_count_get (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->getNumPages();
}

Evas_Poppler_Document_Page_Mode
evas_poppler_document_page_mode_get (Evas_Poppler_Document *document)
{
  if (!document || !document->pdfdoc ||
      !document->pdfdoc->getCatalog () || !document->pdfdoc->getCatalog ()->isOk ())
    return EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_NONE;

  switch (document->pdfdoc->getCatalog ()->getPageMode ()) {
    case Catalog::pageModeNone:
      return EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_NONE;
    case Catalog::pageModeOutlines:
      return EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_OUTLINES;
    case Catalog::pageModeThumbs:
      return EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_THUMBS;
    case Catalog::pageModeFullScreen:
      return EVAS_POPPLER_DOCUMENT_PAGE_MODE_FULLSCREEN;
    case Catalog::pageModeOC:
      return EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_OC;
    default:
      return EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_NONE;
  }
}

Evas_Poppler_Document_Page_Layout
evas_poppler_document_page_layout_get (Evas_Poppler_Document *document)
{
  if (!document || !document->pdfdoc ||
      !document->pdfdoc->getCatalog () || !document->pdfdoc->getCatalog ()->isOk ())
    return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_NONE;

  switch (document->pdfdoc->getCatalog ()->getPageLayout ()) {
    case Catalog::pageLayoutNone:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_NONE;
    case Catalog::pageLayoutSinglePage:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_SINGLE_PAGE;
    case Catalog::pageLayoutOneColumn:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_ONE_COLUMN;
    case Catalog::pageLayoutTwoColumnLeft:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_LEFT;
    case Catalog::pageLayoutTwoColumnRight:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
    case Catalog::pageLayoutTwoPageLeft:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_LEFT;
    case Catalog::pageLayoutTwoPageRight:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_RIGHT;
    default:
      return EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_NONE;
  }
}

unsigned char
evas_poppler_document_unlock (Evas_Poppler_Document *document, const char *password)
{
  if (!document)
    return 0;

  if (document->locked)
    {
      /* racier then it needs to be */
      GooString *pwd = new GooString(password);
      PDFDoc *pdfdoc = new PDFDoc(document->pdfdoc->getFileName(), pwd);
      delete pwd;
      if (pdfdoc->isOk())
        {

          document->pdfdoc = pdfdoc;
          document->locked = false;
        }
    }

  return document->locked;
}

unsigned char
evas_poppler_document_is_locked (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->locked;
}

const char *
evas_poppler_document_info_get (Evas_Poppler_Document *document, const char *data)
{
  return NULL;
}

unsigned char
evas_poppler_document_is_encrypted (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->isEncrypted();
}

unsigned char
evas_poppler_document_is_linearized (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->isLinearized();
}

unsigned char
evas_poppler_document_is_printable (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToPrint();
}

unsigned char
evas_poppler_document_is_changeable (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToChange();
}

unsigned char
evas_poppler_document_is_copyable (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToCopy();
}

unsigned char
evas_poppler_document_is_notable (Evas_Poppler_Document *document)
{
  if (!document)
    return 0;

  return document->pdfdoc->okToAddNotes();
}


double
evas_poppler_document_pdf_version_get (Evas_Poppler_Document *document)
{
  if (!document)
    return 0.0;

  return document->pdfdoc->getPDFVersion();
}

Ecore_List *
evas_poppler_document_fonts_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_scan_for_fonts (document,
                                               evas_poppler_document_page_count_get (document));
}

Ecore_List *
evas_poppler_document_scan_for_fonts (Evas_Poppler_Document *document, int page_count)
{
  Ecore_List *fonts;
  int         length;

  if (!document)
    return NULL;

  GooList *items = document->scanner->scan (page_count);
  if (!items)
    return NULL;

  length = items->getLength();
  fonts = ecore_list_new ();
  ecore_list_set_free_cb (fonts, ECORE_FREE_CB (evas_poppler_font_info_delete));

  for ( int i = 0; i < length; ++i )
    {
      Evas_Poppler_Font_Info_Type type;
      Evas_Poppler_Font_Info     *font;
      Evas_Poppler_Font_Info     *font2;
      char                       *font_name = NULL;
      char                       *font_path = NULL;
      int                         index = 0;

      if (((::FontInfo*)items->get(i))->getName())
        font_name = ((::FontInfo*)items->get(i))->getName()->getCString();

      if (((::FontInfo*)items->get(i))->getFile())
        font_path = ((::FontInfo*)items->get(i))->getFile()->getCString();

      type = (Evas_Poppler_Font_Info_Type)((::FontInfo*)items->get(i))->getType();
      font = evas_poppler_font_info_new (font_name, font_path,
                                         ((::FontInfo*)items->get(i))->getEmbedded(),
                                         ((::FontInfo*)items->get(i))->getSubset(),
                                         type);

      while ((font->font_name[index]) &&
             (font->font_name[index] != '+'))
        index++;
      if (font->font_name[index] == '\0')
        index = 0;
      else
        index++;
      font_name = strdup (font->font_name + index);
      font2 = evas_poppler_font_info_new (font_name,
                                          font_path,
                                          font->is_embedded,
                                          font->is_subset,
                                          font->type);
      ecore_list_append (fonts, font2);
      free (font_name);
      evas_poppler_font_info_delete (font);
    }

  deleteGooList (items, FontInfo);

  return fonts;
}

char *
evas_poppler_document_filename_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return document->pdfdoc->getFileName()->getCString();
}

static char *
evas_poppler_document_property_get (Evas_Poppler_Document *document, const char *property)
{
  Object     obj;
  UGooString prop_str(property);
  GooString *goo_string;
  char      *title = NULL;

  if (!document)
    return NULL;

  if (!document->dict->lookup (prop_str, &obj)->isString ())
    {
      obj.free ();

      return NULL;
    }

  goo_string = obj.getString ();
  /* Unicode */
  if (((goo_string->getChar (0) & 0xff) == 0xfe) &&
      ((goo_string->getChar (1) & 0xff) == 0xff))
    {
      printf ("Unicode !\n");
      title = NULL;
    }
  else
    {
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
evas_poppler_document_title_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_property_get (document, "Title");
}

char *
evas_poppler_document_author_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_property_get (document, "Author");
}

char *
evas_poppler_document_subject_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_property_get (document, "Subject");
}

char *
evas_poppler_document_keywords_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_property_get (document, "Keywords");
}

char *
evas_poppler_document_creator_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_property_get (document, "Creator");
}

char *
evas_poppler_document_producer_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_property_get (document, "Producer");
}

static char *
evas_poppler_document_date_get (Evas_Poppler_Document *document, const char *type)
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

  if (!document->dict->lookup ((char *)type, &obj)->isString ())
    {
      obj.free ();

      return NULL;
    }

  goo_string = obj.getString ();

  /* Unicode */
  if (((goo_string->getChar (0) & 0xff) == 0xfe) &&
      ((goo_string->getChar (1) & 0xff) == 0xff))
    {
      printf ("Unicode !\n");
      return NULL;
    }
  else
    {
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
      (strlen (date) > 14))
    {
      int century;
      int years_since_1900;

      scanned_items = sscanf (date, "%2d%3d%2d%2d%2d%2d%2d",
                              &century, &years_since_1900, &month, &day, &hour, &minute, &second);
      
      if (scanned_items != 7)
        {
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
evas_poppler_document_creation_date_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_date_get (document, "CreationDate");
}

char *
evas_poppler_document_mod_date_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  return evas_poppler_document_date_get (document, "ModDate");
}

const char *
evas_poppler_document_linearized_get (Evas_Poppler_Document *document)
{
  if (!document)
    return NULL;

  if (evas_poppler_document_is_linearized (document))
    return "yes";
  else
    return "no";
}

const char *
evas_poppler_document_page_mode_string_get (Evas_Poppler_Document *document)
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
evas_poppler_document_page_layout_string_get (Evas_Poppler_Document *document)
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
