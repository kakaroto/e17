#ifndef __ESMART_PS_H__
#define __ESMART_PS_H__


#include "Eps.h"


/**
 * @file esmart_ps.h
 *
 * @defgroup Esmart_Ps Esmart Ps
 *
 * @brief An Evas smart object to display Postscript document
 *
 * Provides a smart object for displaying Postscript files in an Evas object.
 * Add an object to en evas with esmart_ps_add(), set the file
 * with esmart_ps_file_set() and set the page with esmart_ps_page_set().
 * Then, you can render the page on the Evas Object with esmart_ps_render().
 * For example:
 *
 * @code
 * #include <Evas.h>
 * #include <esmart_ps.h>
 *
 * Evas        *evas;
 * Evas_Object *o;
 * char        *filename
 *
 * // get the evas
 *
 * o = esmart_ps_add (evas);
 * if (!esmart_ps_init (o)) {
 *   // manage the error
 * }
 *
 * esmart_ps_file_set (o, argv[1]);
 * esmart_ps_page_set (o, page_number);
 * esmart_ps_render (o);
 * evas_object_move (o, 0, 0);
 * evas_object_show (o);
 * @endcode
 *
 * To change the page, use esmart_ps_page_set() or esmart_ps_page_next()
 * / esmart_ps_page_previous().
 *
 * The orientation of the page can be changed with
 * esmart_ps_orientation_set().
 *
 * @{
 */

/**
 * @typedef Smart_Ps
 * @brief A smart ps object is a simple Evas smart object that displays a Postscript document
 */
typedef struct _Smart_Ps Smart_Ps;

/**
 * @struct _Smart_Ps
 * @brief Evas object structure to provide all the facilities of an Evas smart object
 */
struct _Smart_Ps
{
   Evas_Object         *obj;            /**< The Evas object */
   char                *filename;       /**< The file name */

   Eps_Document        *ps_document;    /**< The Eps document */
   Eps_Page            *ps_page;        /**< The Eps current page */
};

Evas_Object         *esmart_ps_add (Evas *evas);
Evas_Bool            esmart_ps_init (Evas_Object *obj);

int                  esmart_ps_file_set (Evas_Object *obj, const char *filename);
const char          *esmart_ps_file_get (Evas_Object *obj);

void                 esmart_ps_page_set (Evas_Object *obj, int page);
int                  esmart_ps_page_get (Evas_Object *obj);

void                 esmart_ps_size_get(Evas_Object *obj, int *width, int *height);

void                 esmart_ps_orientation_set (Evas_Object *obj, Eps_Page_Orientation o);
Eps_Page_Orientation esmart_ps_orientation_get (Evas_Object *obj);

void                 esmart_ps_scale_set (Evas_Object *obj, double hscale, double vscale);
void                 esmart_ps_scale_get (Evas_Object *obj, double *hscale, double *vscale);

void                 esmart_ps_page_next (Evas_Object *obj);
void                 esmart_ps_page_previous (Evas_Object *obj);

void                 esmart_ps_render (Evas_Object *obj);

Eps_Document        *esmart_ps_ps_document_get (Evas_Object *obj);
Eps_Page            *esmart_ps_ps_page_get (Evas_Object *obj);

/**
 * @}
 */


#endif /* __ESMART_PS_H__ */
