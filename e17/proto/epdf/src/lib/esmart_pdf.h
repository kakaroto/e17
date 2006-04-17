#ifndef __ESMART_PDF_H__
#define __ESMART_PDF_H__


#include "Epdf.h"

typedef struct _Smart_Pdf Smart_Pdf;

struct _Smart_Pdf
{
  Evas_Object          *obj;
  char                 *filename;
  int                   page;

  Epdf_Document        *pdf_document;
  Epdf_Page            *pdf_page;
  Ecore_List           *pdf_index;
  Epdf_Page_Orientation orientation;
  double                hscale;
  double                vscale;
};

Evas_Object          *esmart_pdf_add (Evas *evas);
Evas_Bool             esmart_pdf_init (Evas_Object *obj);
void                  esmart_pdf_file_set (Evas_Object *obj, const char *filename);
const char           *esmart_pdf_file_get (Evas_Object *obj);
void                  esmart_pdf_page_set (Evas_Object *obj, int page);
double                esmart_pdf_position_get(Evas_Object *obj);
Epdf_Document        *esmart_pdf_pdf_document_get (Evas_Object *obj);
Epdf_Page            *esmart_pdf_pdf_page_get (Evas_Object *obj);
Ecore_List           *esmart_pdf_pdf_index_get (Evas_Object *obj);
void                  esmart_pdf_orientation_set (Evas_Object *obj, Epdf_Page_Orientation o);
Epdf_Page_Orientation esmart_pdf_orientation_get (Evas_Object *obj);

void                  esmart_pdf_scale_set (Evas_Object *obj, double hscale, double vscale);
void                  esmart_pdf_scale_get (Evas_Object *obj, double *hscale, double *vscale);

#endif /* __ESMART_PDF_H__ */
