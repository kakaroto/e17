/** @file etk_ps.h */
#ifndef _ETK_PS_H_
#define _ETK_PS_H_

#include <Eps.h>
#include "etk_widget.h"


/**
 * @file etk_ps.h
 *
 * @defgroup Etk_Ps Etk Ps
 *
 * @brief A Postscript display Widget for the Etk toolkit
 *
 * Provides a widget for displaying Postscript files in an Etk widget.
 * To use an Etk_Ps widget, create it with  etk_ps_new(), and
 * set the file with etk_ps_file_set(). For example:
 *
 * @code
 * #include <Etk.h>
 * #include <etk_ps.h>
 *
 * Etk_Widget *ps;
 * char       *filename
 *
 * ps = etk_ps_new ();
 * etk_ps_file_set (ETK_PS (ps), filename);
 * etk_widget_show (ps);
 * @endcode
 *
 * To change the page, use etk_ps_page_set() or etk_ps_page_next()
 * / etk_ps_page_previous().
 *
 * The orientation of the page can be changed with
 * etk_ps_orientation_set().
 *
 * @{
 */

/** @brief Gets the type of a ps */
#define ETK_PS_TYPE        (etk_ps_type_get())
/** @brief Casts the object to an Etk_Ps */
#define ETK_PS(obj)        (ETK_OBJECT_CAST((obj), ETK_PS_TYPE, Etk_Ps))
/** @brief Check if the object is an Etk_Ps */
#define ETK_IS_PS(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_PS_TYPE))

/**
 * @typedef Etk_Ps
 * @brief A ps is a simple widget that just displays an ps
 */
typedef struct _Etk_Ps Etk_Ps;

/**
 * @struct _Etk_Ps
 * @brief Inherits from Etk_Widget and extends to provide a ps widget
 */
struct _Etk_Ps
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget           widget;         /**< Inherit from Ewl_Widget */

   Evas_Object         *ps_object;     /**< The evas object for the image */
   char                *filename;       /**< The file name */

   Eps_Document        *ps_document;   /**< The Epdf document */
   Eps_Page            *ps_page;       /**< The Epdf current page */
};

Etk_Type            *etk_ps_type_get();
Etk_Widget          *etk_ps_new(void);

int                  etk_ps_file_set(Etk_Ps *ps, const char *filename);
const char          *etk_ps_file_get(Etk_Ps *ps);

void                 etk_ps_page_set(Etk_Ps *ps, int page);
int                  etk_ps_page_get(Etk_Ps *ps);

void                 etk_ps_ps_size_get(Etk_Ps *ps, int *width, int *height);

void                 etk_ps_orientation_set(Etk_Ps *ps, Eps_Page_Orientation o);

Eps_Page_Orientation etk_ps_orientation_get(Etk_Ps *ps);

void                 etk_ps_scale_set(Etk_Ps *ps, double hscale, double vscale);
void                 etk_ps_scale_get(Etk_Ps *ps, double *hscale, double *vscale);

void                 etk_ps_page_next(Etk_Ps *ps);
void                 etk_ps_page_previous(Etk_Ps *ps);

const Eps_Document  *etk_ps_ps_document_get(Etk_Ps *ps);
const Eps_Page      *etk_ps_ps_page_get (Etk_Ps *ps);

/** @} */

#endif
