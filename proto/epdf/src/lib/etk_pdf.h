#ifndef _ETK_PDF_H_
#define _ETK_PDF_H_

#include <Epdf.h>
#include "etk_widget.h"


/**
 * @file etk_pdf.h
 *
 * @defgroup Etk_Pdf Etk Pdf
 *
 * @brief A PDF display Widget for the Etk toolkit
 *
 * Provides a widget for displaying PDF files in an Etk widget.
 * To use an Etk_Pdf widget, create it with  etk_pdf_new(), and
 * set the file with etk_pdf_file_set(). For example:
 *
 * @code
 * #include <Etk.h>
 * #include <etk_pdf.h>
 *
 * Etk_Widget *pdf;
 * char       *filename
 *
 * pdf = etk_pdf_new ();
 * etk_pdf_file_set (ETK_PDF (pdf), filename);
 * etk_widget_show (pdf);
 * @endcode
 *
 * To change the page, use etk_pdf_page_set() or etk_pdf_page_next()
 * / etk_pdf_page_previous().
 *
 * The orientation of the page can be changed with
 * etk_pdf_orientation_set().
 *
 * @{
 */


/** @brief Gets the type of a pdf */
#define ETK_PDF_TYPE        (etk_pdf_type_get())
/** @brief Casts the object to an Etk_Pdf */
#define ETK_PDF(obj)        (ETK_OBJECT_CAST((obj), ETK_PDF_TYPE, Etk_Pdf))
/** @brief Check if the object is an Etk_Pdf */
#define ETK_IS_PDF(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_PDF_TYPE))

/**
 * @typedef Etk_Pdf
 * @brief An Etk pdf widget is a simple widget that displays a PDF document
 */
typedef struct _Etk_Pdf Etk_Pdf;

/**
 * @struct _Etk_Pdf
 * @brief Inherits from Etk_Widget and extends to provide a pdf widget
 */
struct _Etk_Pdf
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget            widget;         /**< Inherit from Ewl_Widget */

   Evas_Object          *pdf_object;     /**< The evas object for the image */
   char                 *filename;       /**< The file name */

   Epdf_Document        *pdf_document;   /**< The Epdf document */
   Epdf_Page            *pdf_page;       /**< The Epdf current page */
   Ecore_List           *pdf_index;      /**< The Epdf index */

  struct {
     Evas_Object        *o;
     char               *text;
     Ecore_List         *list;
     int                 page;
     int                 is_case_sensitive;
     int                 is_circular;
  }search;                               /**< The search members */
};

Etk_Type             *etk_pdf_type_get();
Etk_Widget           *etk_pdf_new();

void                  etk_pdf_file_set(Etk_Pdf *pdf, const char *filename);
const char           *etk_pdf_file_get(Etk_Pdf *pdf);

void                  etk_pdf_page_set(Etk_Pdf *pdf, int page);
int                   etk_pdf_page_get(Etk_Pdf *pdf);

void                  etk_pdf_size_get(Etk_Pdf *pdf, int *width, int *height);

void                  etk_pdf_orientation_set(Etk_Pdf *pdf, Epdf_Page_Orientation o);
Epdf_Page_Orientation etk_pdf_orientation_get(Etk_Pdf *pdf);

void                  etk_pdf_scale_set(Etk_Pdf *pdf, double hscale, double vscale);
void                  etk_pdf_scale_get(Etk_Pdf *pdf, double *hscale, double *vscale);

void                  etk_pdf_page_next(Etk_Pdf *pdf);
void                  etk_pdf_page_previous(Etk_Pdf *pdf);

void                  etk_pdf_search_text_set(Etk_Pdf *pdf, const char *text);
void                  etk_pdf_search_is_case_sensitive(Etk_Pdf *pdf, int is_case_sensitive);
int                   etk_pdf_search_next(Etk_Pdf *pdf);

Epdf_Document        *etk_pdf_pdf_document_get(Etk_Pdf *pdf);
Epdf_Page            *etk_pdf_pdf_page_get(Etk_Pdf *pdf);
Ecore_List           *etk_pdf_pdf_index_get(Etk_Pdf *pdf);

/**
 * @}
 */


#endif
