#ifndef __EWL_DVI_H__
#define __EWL_DVI_H__

#include <Edvi.h>


/**
 * @file ewl_dvi.h
 *
 * @defgroup Ewl_Dvi Ewl Dvi
 *
 * @brief A DVI display Widget for the Ewl toolkit
 *
 * Provides a widget for displaying DVI files in an Ewl widget.
 * To use an Ewl_Dwi widget, create it with  ewl_dvi_new(), and
 * set the file with ewl_dvi_file_set(). For example:
 *
 * @code
 * #include <Ewl.h>
 * #include <ewl_dvi.h>
 *
 * Ewl_Widget *dvi;
 * char       *filename
 *
 * dvi = ewl_dvi_new ();
 * ewl_dvi_file_set (EWL_DVI (dvi), filename);
 * ewl_widget_show (dvi);
 * @endcode
 *
 * To change the page, use ewl_dvi_page_set() or ewl_dvi_page_next()
 * / ewl_dvi_page_previous().
 *
 * The orientation of the page can be changed with
 * ewl_dvi_orientation_set().
 *
 * @{
 */

/**
 * @themekey /dvi/file
 * @themekey /dvi/group
 */

/**
 * @def EWL_DVI_TYPE
 * The type name for the Ewl_Dvi widget
 */
#define EWL_DVI_TYPE "dvi"

/**
 * @typedef Ewl_Dvi
 * @brief An Ewl dvi widget is a simple widget that displays a DVI document
 */
typedef struct Ewl_Dvi Ewl_Dvi;

/**
 * @def EWL_DVI(dvi)
 * Typecase a pointer to an Ewl_Dvi widget
 */
#define EWL_DVI(dvi) ((Ewl_Dvi *) dvi)

/**
 * @struct Ewl_Dvi
 * @brief Inherits from Ewl_Widget and extends to provide a dvi widget
 */
struct Ewl_Dvi
{
	Ewl_Widget            widget;         /**< Inherit from Ewl_Widget */

	void                 *image;          /**< The evas object for the image */
	char                 *filename;       /**< The file name */
        int                   dirty;

	Edvi_Device          *dvi_device;     /**< The Edvi device */
	Edvi_Property        *dvi_property;   /**< The Edvi property */
	Edvi_Document        *dvi_document;   /**< The Edvi document */
	Edvi_Page            *dvi_page;       /**< The Edvi current page */

};

Ewl_Widget           *ewl_dvi_new(void);
int                   ewl_dvi_init(Ewl_Dvi *dvi);

int                   ewl_dvi_file_set(Ewl_Dvi *dvi, const char *filename);
const char           *ewl_dvi_file_get(Ewl_Dvi *dvi);

void                  ewl_dvi_page_set(Ewl_Dvi *dvi, int page);
int                   ewl_dvi_page_get(Ewl_Dvi *dvi);

void                  ewl_dvi_size_get (Ewl_Dvi *dvi, int *width, int *height);

void                  ewl_dvi_orientation_set (Ewl_Dvi *dvi, Edvi_Page_Orientation o);
Edvi_Page_Orientation ewl_dvi_orientation_get (Ewl_Dvi *dvi);

void                  ewl_dvi_mag_set (Ewl_Dvi *dvi, double mag);
double                ewl_dvi_scale_get (Ewl_Dvi *dvi);

void                  ewl_dvi_page_next (Ewl_Dvi *dvi);
void                  ewl_dvi_page_previous (Ewl_Dvi *dvi);

const Edvi_Document  *ewl_dvi_dvi_document_get (Ewl_Dvi *dvi);
const Edvi_Page      *ewl_dvi_dvi_page_get (Ewl_Dvi *dvi);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_dvi_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_dvi_reveal_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_dvi_obscure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_dvi_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data );

/**
 * @}
 */


#endif /* __EWL_DVI_H__ */
