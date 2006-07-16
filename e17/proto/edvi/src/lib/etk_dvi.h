/** @file etk_dvi.h */
#ifndef _ETK_DVI_H_
#define _ETK_DVI_H_

#include <Edvi.h>
#include "etk_widget.h"

/**
 * @defgroup Etk_Dvi Etk_Dvi
 * @{
 */

/** @brief Gets the type of a dvi */
#define ETK_DVI_TYPE        (etk_dvi_type_get())
/** @brief Casts the object to an Etk_Dvi */
#define ETK_DVI(obj)        (ETK_OBJECT_CAST((obj), ETK_DVI_TYPE, Etk_Dvi))
/** @brief Check if the object is an Etk_Dvi */
#define ETK_IS_DVI(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_DVI_TYPE))

/**
 * @struct Etk_Dvi
 * @brief A dvi is a simple widget that just displays an dvi
 */
typedef struct _Etk_Dvi Etk_Dvi;

struct _Etk_Dvi
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget            widget;

   Evas_Object          *dvi_object;
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

Etk_Type             *etk_dvi_type_get();
Etk_Widget           *etk_dvi_new();

void                  etk_dvi_file_set(Etk_Dvi *dvi, const char *filename);
const char           *etk_dvi_file_get(Etk_Dvi *dvi);
void                  etk_dvi_page_set(Etk_Dvi *dvi, int page);
int                   etk_dvi_page_get(Etk_Dvi *dvi);
Edvi_Document        *etk_dvi_dvi_document_get (Etk_Dvi *dvi);
Edvi_Page            *etk_dvi_dvi_page_get (Etk_Dvi *dvi);
void                  etk_dvi_size_get (Etk_Dvi *dvi, int *width, int *height);

void                  etk_dvi_orientation_set (Etk_Dvi *dvi, Edvi_Page_Orientation o);
Edvi_Page_Orientation etk_dvi_orientation_get (Etk_Dvi *dvi);

void                  etk_dvi_scale_set (Etk_Dvi *dvi, double hscale, double vscale);
void                  etk_dvi_scale_get (Etk_Dvi *dvi, double *hscale, double *vscale);
void                  etk_dvi_page_next (Etk_Dvi *dvi);
void                  etk_dvi_page_previous (Etk_Dvi *dvi);
void                  etk_dvi_page_page_length_set (Etk_Dvi *dvi, int page_length);
int                   etk_dvi_page_page_length_get (Etk_Dvi *dvi);
void                  etk_dvi_page_page_next (Etk_Dvi *dvi);
void                  etk_dvi_page_page_previous (Etk_Dvi *dvi);

/** @} */

#endif
