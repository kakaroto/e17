/** @file etk_pdf.h */
#ifndef _ETK_PDF_H_
#define _ETK_PDF_H_

#include <Epdf.h>
#include "etk_widget.h"

/**
 * @defgroup Etk_Pdf Etk_Pdf
 * @{
 */

/** @brief Gets the type of a pdf */
#define ETK_PDF_TYPE        (etk_pdf_type_get())
/** @brief Casts the object to an Etk_Pdf */
#define ETK_PDF(obj)        (ETK_OBJECT_CAST((obj), ETK_PDF_TYPE, Etk_Pdf))
/** @brief Check if the object is an Etk_Pdf */
#define ETK_IS_PDF(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_PDF_TYPE))

/**
 * @struct Etk_Pdf
 * @brief A pdf is a simple widget that just displays an pdf
 */
typedef struct _Etk_Pdf Etk_Pdf;

struct _Etk_Pdf
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget   widget;

   Evas_Object *pdf_object;
   char        *filename;
   int          page;

   Evas_Poppler_Document *pdf_document;
   Evas_Poppler_Page     *pdf_page;
   Ecore_List            *pdf_index;
};

Etk_Type *etk_pdf_type_get();
Etk_Widget *etk_pdf_new();

void                   etk_pdf_file_set(Etk_Pdf *pdf, const char *filename);
const char            *etk_pdf_file_get(Etk_Pdf *pdf);
void                   etk_pdf_page_set(Etk_Pdf *pdf, int page);
int                    etk_pdf_page_get(Etk_Pdf *pdf);
Evas_Poppler_Document *etk_pdf_pdf_document_get (Etk_Pdf *pdf);
Evas_Poppler_Page     *etk_pdf_pdf_page_get (Etk_Pdf *pdf);
Ecore_List            *etk_pdf_pdf_index_get (Etk_Pdf *pdf);

/** @} */

#endif
