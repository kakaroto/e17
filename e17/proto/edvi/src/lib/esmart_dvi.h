#ifndef __ESMART_DVI_H__
#define __ESMART_DVI_H__


#include "Edvi.h"


/**
 * @file ewl_dvi.h
 *
 * @defgroup Esmart_Dvi Esmart Dvi
 *
 * @brief An Evas smart object to display DVI document
 *
 * Provides a smart object for displaying DVI files in an Evas object.
 * Add an object to en evas with esmart_dvi_add(), and set the file
 * with esmart_dvi_file_set() and set the page with esmart_dvi_page_set().
 * For example:
 *
 * @code
 * #include <Evas.h>
 * #include <esmart_dvi.h>
 *
 * Evas        *evas;
 * Evas_Object *o;
 * char        *filename
 *
 * // get the evas
 *
 * o = esmart_dvi_add (evas);
 * if (!esmart_dvi_init (o)) {
 *   // manage the error
 * }
 *
 * esmart_dvi_file_set (o, argv[1]);
 * esmart_dvi_page_set (o, page_number);
 * evas_object_move (o, 0, 0);
 * evas_object_show (o);
 * @endcode
 *
 * To change the page, use esmart_dvi_page_set() or esmart_dvi_page_next()
 * / esmart_dvi_page_previous().
 *
 * The orientation of the page can be changed with
 * esmart_dvi_orientation_set().
 *
 * @{
 */


/**
 * @typedef Smart_Dvi
 * @brief A smart dvi object is a simple Evas smart object that displays a DVI document
 */
typedef struct _Smart_Dvi Smart_Dvi;

/**
 * @struct _Smart_Dvi
 * @brief Evas object structure to provide all the facilities of an Evas smart object
 */
struct _Smart_Dvi
{
  Evas_Object          *obj;            /**< The Evas object */
  char                 *filename;       /**< The file name */

  Edvi_Device          *dvi_device;     /**< The Edvi device */
  Edvi_Property        *dvi_property;   /**< The Edvi property */
  Edvi_Document        *dvi_document;   /**< The Edvi document */
  Edvi_Page            *dvi_page;       /**< The Edvi current page */
};

Evas_Object          *esmart_dvi_add (Evas *evas);
Evas_Bool             esmart_dvi_init (Evas_Object *obj);

int                   esmart_dvi_file_set (Evas_Object *obj, const char *filename);
const char           *esmart_dvi_file_get (Evas_Object *obj);

void                  esmart_dvi_page_set (Evas_Object *obj, int page);
int                   esmart_dvi_page_get (Evas_Object *obj);

void                  esmart_dvi_size_get (Evas_Object *obj, int *width, int *height);

void                  esmart_dvi_orientation_set (Evas_Object *obj, Edvi_Page_Orientation o);
Edvi_Page_Orientation esmart_dvi_orientation_get (Evas_Object *obj);

void                  esmart_dvi_mag_set (Evas_Object *obj, double mag);
double                esmart_dvi_mag_get (Evas_Object *obj);

void                  esmart_dvi_page_next (Evas_Object *obj);
void                  esmart_dvi_page_previous (Evas_Object *obj);

void                  esmart_dvi_render (Evas_Object *obj);

const Edvi_Document  *esmart_dvi_dvi_document_get (Evas_Object *obj);
const Edvi_Page      *esmart_dvi_dvi_page_get (Evas_Object *obj);

/**
 * @}
 */


#endif /* __ESMART_DVI_H__ */
