#ifndef __ESMART_DVI_H__
#define __ESMART_DVI_H__


#include "Edvi.h"

typedef struct _Smart_Dvi Smart_Dvi;

struct _Smart_Dvi
{
  Evas_Object          *obj;
  char                 *filename;
  int                   page;
  int                   page_length;

  Edvi_Device          *dvi_device;
  Edvi_Property        *dvi_property;
  Edvi_Document        *dvi_document;
  Edvi_Page            *dvi_page;
  Edvi_Page_Orientation orientation;
  double                hscale;
  double                vscale;
};

Evas_Object          *esmart_dvi_add (Evas *evas);
Evas_Bool             esmart_dvi_init (Evas_Object *obj);
void                  esmart_dvi_file_set (Evas_Object *obj, const char *filename);
const char           *esmart_dvi_file_get (Evas_Object *obj);
void                  esmart_dvi_page_set (Evas_Object *obj, int page);
double                esmart_dvi_position_get(Evas_Object *obj);
Edvi_Document        *esmart_dvi_dvi_document_get (Evas_Object *obj);
Edvi_Page            *esmart_dvi_dvi_page_get (Evas_Object *obj);
void                  esmart_dvi_orientation_set (Evas_Object *obj, Edvi_Page_Orientation o);
Edvi_Page_Orientation esmart_dvi_orientation_get (Evas_Object *obj);

void                  esmart_dvi_scale_set (Evas_Object *obj, double hscale, double vscale);
void                  esmart_dvi_scale_get (Evas_Object *obj, double *hscale, double *vscale);
void                  esmart_dvi_page_next (Evas_Object *obj);
void                  esmart_dvi_page_previous (Evas_Object *obj);
void                  esmart_dvi_page_page_length_set (Evas_Object *obj, int page_length);
int                   esmart_dvi_page_page_length_get (Evas_Object *obj);
void                  esmart_dvi_page_page_next (Evas_Object *obj);
void                  esmart_dvi_page_page_previous (Evas_Object *obj);

#endif /* __ESMART_DVI_H__ */
