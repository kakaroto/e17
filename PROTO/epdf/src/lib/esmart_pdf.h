#ifndef __ESMART_PDF_H__
#define __ESMART_PDF_H__


#include "Epdf.h"


/**
 * @file esmart_pdf.h
 *
 * @defgroup Esmart_Pdf Esmart Pdf
 *
 * @brief An Evas smart object to display PDF document
 *
 * Provides a smart object for displaying PDF files in an Evas object.
 * Add an object to en evas with esmart_pdf_add(), set the file
 * with esmart_pdf_file_set() and set the page with esmart_pdf_page_set().
 * Then, you can render the page on the Evas Object with esmart_pdf_render().
 * For example:
 *
 * @code
 * #include <Evas.h>
 * #include <esmart_pdf.h>
 *
 * Evas        *evas;
 * Evas_Object *o;
 * char        *filename
 *
 * // get the evas
 *
 * o = esmart_pdf_add (evas);
 * if (!esmart_pdf_init (o)) {
 *   // manage the error
 * }
 *
 * esmart_pdf_file_set (o, argv[1]);
 * esmart_pdf_page_set (o, page_number);
 * esmart_pdf_render (o);
 * evas_object_move (o, 0, 0);
 * evas_object_show (o);
 * @endcode
 *
 * To change the page, use esmart_pdf_page_set() or esmart_pdf_page_next()
 * / esmart_pdf_page_previous().
 *
 * The orientation of the page can be changed with
 * esmart_pdf_orientation_set().
 *
 * @{
 */

/**
 * @typedef Smart_Pdf
 * @brief A smart pdf object is a simple Evas smart object that displays a PDF document
 */
typedef struct _Smart_Pdf Smart_Pdf;

/**
 * @struct _Smart_Pdf
 * @brief Evas object structure to provide all the facilities of an Evas smart object
 */
struct _Smart_Pdf
{
  Evas_Object          *obj;            /**< The Evas object */
  char                 *filename;       /**< The file name */

  Epdf_Document        *pdf_document;   /**< The Epdf document */
  Epdf_Page            *pdf_page;       /**< The Epdf current page */
  Eina_List            *pdf_index;      /**< The Epdf index */
};

Evas_Object          *esmart_pdf_add (Evas *evas);
Evas_Bool             esmart_pdf_init (Evas_Object *obj);

int                   esmart_pdf_file_set (Evas_Object *obj, const char *filename);
const char           *esmart_pdf_file_get (Evas_Object *obj);

void                  esmart_pdf_page_set (Evas_Object *obj, int page);
int                   esmart_pdf_page_get (Evas_Object *obj);

void                  esmart_pdf_size_get (Evas_Object *obj, int *width, int *height);

void                  esmart_pdf_orientation_set (Evas_Object *obj, Epdf_Page_Orientation o);
Epdf_Page_Orientation esmart_pdf_orientation_get (Evas_Object *obj);

void                  esmart_pdf_scale_set (Evas_Object *obj, double hscale, double vscale);
void                  esmart_pdf_scale_get (Evas_Object *obj, double *hscale, double *vscale);

void                  esmart_pdf_page_next (Evas_Object *obj);
void                  esmart_pdf_page_previous (Evas_Object *obj);

void                  esmart_pdf_render (Evas_Object *obj);

Epdf_Document        *esmart_pdf_pdf_document_get (Evas_Object *obj);
Epdf_Page            *esmart_pdf_pdf_page_get (Evas_Object *obj);
Eina_List            *esmart_pdf_pdf_index_get (Evas_Object *obj);

/**
 * @}
 */


#endif /* __ESMART_PDF_H__ */
