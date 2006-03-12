#ifndef __POPPLER_PRIVATE_H__
#define __POPPLER_PRIVATE_H__


#include <Ecore_Data.h>


class PDFDoc;
class Dict;
class FontInfoScanner;
class Object;
class Page;
class TextOutputDev;
class Gfx;
class LinkAction;

typedef struct _Evas_Poppler_Page_Transition Evas_Poppler_Page_Transition;
typedef struct _Evas_Poppler_Document Evas_Poppler_Document;


struct _Evas_Poppler_Font_Info
{
  char                       *font_name;
  char                       *font_path;
  unsigned char               is_embedded;
  unsigned char               is_subset;
  Evas_Poppler_Font_Info_Type type;
};

struct _Evas_Poppler_Page_Transition
{
  Object                                *transition;

  Evas_Poppler_Page_Transition_Type      type;
  int                                    duration;
  Evas_Poppler_Page_Transition_Alignment alignment;
  Evas_Poppler_Page_Transition_Direction direction;
  int                                    angle;
  double                                 scale;
  unsigned char                          is_rectangular;
};

struct _Evas_Poppler_Page
{
  Evas_Poppler_Document        *doc;
  int                           index;
  ::Page                       *page;
  Evas_Poppler_Page_Transition *transition;
  TextOutputDev                *text_dev;
  Gfx                          *gfx;
};

struct _Evas_Poppler_Document
{
  PDFDoc          *pdfdoc;
  unsigned char    locked;
  Dict            *dict;
  FontInfoScanner *scanner;
};

struct _Evas_Poppler_Index_Item
{
  char                         *title;
  LinkAction                   *action;
  Ecore_List                   *children;
};

Evas_Poppler_Page_Transition *evas_poppler_page_transition_new (const Object *data);
PDFDoc *evas_poppler_document_doc_get (Evas_Poppler_Document *document);


#endif /* __POPPLER_PRIVATE_H__ */
