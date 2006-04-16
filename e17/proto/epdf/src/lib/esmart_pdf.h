#ifndef __ESMART_PDF_H__
#define __ESMART_PDF_H__


#include "Epdf.h"

typedef struct _Smart_Pdf Smart_Pdf;

struct _Smart_Pdf
{
  Evas_Object *obj;
  char        *filename;
  int          page;

  Evas_Poppler_Document *pdf_document;
  Evas_Poppler_Page     *pdf_page;
  Ecore_List            *pdf_index;
  Evas_Poppler_Page_Orientation orientation;
};

Evas_Object           *poppler_object_add (Evas *evas);
Evas_Bool              poppler_object_init (Evas_Object *obj);
void                   poppler_object_file_set (Evas_Object *obj, const char *filename);
const char            *poppler_object_file_get (Evas_Object *obj);
void                   poppler_object_page_set (Evas_Object *obj, int page);
double                 poppler_object_position_get(Evas_Object *obj);
Evas_Poppler_Document *poppler_pdf_document_get (Evas_Object *obj);
Evas_Poppler_Page     *poppler_pdf_page_get (Evas_Object *obj);
Ecore_List            *poppler_pdf_index_get (Evas_Object *obj);
void                   poppler_pdf_orientation_set (Evas_Object *obj, Evas_Poppler_Page_Orientation o);
Evas_Poppler_Page_Orientation poppler_pdf_orientation_get (Evas_Object *obj);

#endif /* __ESMART_PDF_H__ */
