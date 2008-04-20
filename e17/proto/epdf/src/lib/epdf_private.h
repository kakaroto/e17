#ifndef __EPDF_PRIVATE_H__
#define __EPDF_PRIVATE_H__


#include <Ecore_Data.h>


class PDFDoc;
class Dict;
class FontInfoScanner;
class Object;
class Page;
class TextOutputDev;
class Gfx;
class LinkAction;

typedef struct _Epdf_Page_Transition Epdf_Page_Transition;
typedef struct _Epdf_Document Epdf_Document;


struct _Epdf_Font_Info
{
  char               *font_name;
  char               *font_path;
  unsigned char       is_embedded;
  unsigned char       is_subset;
  Epdf_Font_Info_Type type;
};

struct _Epdf_Page_Transition
{
  Object                        *transition;

  Epdf_Page_Transition_Type      type;
  int                            duration;
  Epdf_Page_Transition_Alignment alignment;
  Epdf_Page_Transition_Direction direction;
  int                            angle;
  double                         scale;
  unsigned char                  is_rectangular;
};

struct _Epdf_Page
{
  Epdf_Document        *doc;
  int                   index;
  ::Page               *page;
  double                hscale;
  double                vscale;
  Epdf_Page_Orientation orientation;
  Epdf_Page_Transition *transition;
  TextOutputDev        *text_dev;
  Gfx                  *gfx;
};

struct _Epdf_Document
{
  PDFDoc          *pdfdoc;
  unsigned char    locked;
  Dict            *dict;
  FontInfoScanner *scanner;
};

struct _Epdf_Index_Item
{
  char       *title;
  LinkAction *action;
  Ecore_List *children;
};

struct _Epdf_Postscript
{
  PDFDoc          *pdfdoc;

  char            *filename;

  int              first_page;
  int              last_page;

  double           width;
  double           height;
  int              duplex :1;
};

Epdf_Page_Transition *epdf_page_transition_new (const Object *data);
PDFDoc *epdf_document_doc_get (Epdf_Document *document);


#endif /* __EPDF_PRIVATE_H__ */
