#ifndef _ETK_DVI_H_
#define _ETK_DVI_H_

#include <Edvi.h>
#include "etk_widget.h"


/**
 * @file etk_dvi.h
 * @defgroup Etk_Dvi Etk Dvi
 * @brief A DVI display Widget for the Etk toolkit
 *
 * Provides a widget for displaying DVI files in an Etk widget.
 * To use an Etk_Dwi widget, create it with  etk_dvi_new(), and
 * set the file with etk_dvi_file_set(). For example:
 *
 * @code
 * #include <Etk.h>
 * #include <etk_dvi.h>
 *
 * Etk_Widget *dvi;
 * char       *filename
 *
 * dvi = etk_dvi_new ();
 * etk_dvi_file_set (ETK_DVI (dvi), filename);
 * etk_widget_show (dvi);
 * @endcode
 *
 * To change the page, use etk_dvi_page_set() or etk_dvi_page_next()
 * / etk_dvi_page_previous().
 *
 * The orientation of the page can be changed with
 * etk_dvi_orientation_set().
 *
 * @{
 */


/** @brief Gets the type of a dvi */
#define ETK_DVI_TYPE        (etk_dvi_type_get())
/** @brief Casts the object to an Etk_Dvi */
#define ETK_DVI(obj)        (ETK_OBJECT_CAST((obj), ETK_DVI_TYPE, Etk_Dvi))
/** @brief Check if the object is an Etk_Dvi */
#define ETK_IS_DVI(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_DVI_TYPE))

/**
 * @typedef Etk_Dvi
 * @brief An Etk dvi widget is a simple widget that displays a DVI document
 */
typedef struct _Etk_Dvi Etk_Dvi;

/**
 * @struct _Etk_Dvi
 * @brief Inherits from Etk_Widget and extends to provide a dvi widget
 */
struct _Etk_Dvi
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget            widget;         /**< Inherit from Etk_Widget */

   Evas_Object          *dvi_object;     /**< The evas object for the image */
   char                 *filename;       /**< The file name */

   Edvi_Device          *dvi_device;     /**< The Edvi device */
   Edvi_Property        *dvi_property;   /**< The Edvi property */
   Edvi_Document        *dvi_document;   /**< The Edvi document */
   Edvi_Page            *dvi_page;       /**< The Edvi current page */
};

Etk_Type             *etk_dvi_type_get();
Etk_Widget           *etk_dvi_new();

void                  etk_dvi_file_set(Etk_Dvi *dvi, const char *filename);
const char           *etk_dvi_file_get(Etk_Dvi *dvi);
void                  etk_dvi_page_set(Etk_Dvi *dvi, int page);
int                   etk_dvi_page_get(Etk_Dvi *dvi);
const Edvi_Document  *etk_dvi_dvi_document_get (Etk_Dvi *dvi);
const Edvi_Page      *etk_dvi_dvi_page_get (Etk_Dvi *dvi);
void                  etk_dvi_size_get (Etk_Dvi *dvi, int *width, int *height);

void                  etk_dvi_orientation_set (Etk_Dvi *dvi, Edvi_Page_Orientation o);
Edvi_Page_Orientation etk_dvi_orientation_get (Etk_Dvi *dvi);

void                  etk_dvi_mag_set (Etk_Dvi *dvi, double mag);
double                etk_dvi_mag_get (Etk_Dvi *dvi);
void                  etk_dvi_page_next (Etk_Dvi *dvi);
void                  etk_dvi_page_previous (Etk_Dvi *dvi);

/**
 * @}
 */


#endif
