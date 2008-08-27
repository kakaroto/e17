#ifndef __EWL_PS_H__
#define __EWL_PS_H__

#include <Eps.h>

/**
 * @file ewl_ps.h
 *
 * @defgroup Ewl_Ps Ewl Ps
 *
 * @brief A Postscript Widget for the Ewl toolkit
 *
 * Provides a widget for displaying Postscript files in an Ewl widget.
 * To use an Ewl_Ps widget, create it with  ewl_ps_new(), and
 * set the file with ewl_ps_file_set(). For example:
 *
 * @code
 * #include <Ewl.h>
 * #include <ewl_ps.h>
 *
 * Ewl_Widget *ps;
 * char       *filename
 *
 * ps = ewl_ps_new ();
 * ewl_ps_file_set (EWL_PS (ps), filename);
 * ewl_widget_show (ps);
 * @endcode
 *
 * To change the page, use ewl_ps_page_set() or ewl_ps_page_next()
 * / ewl_ps_page_previous().
 *
 * The orientation of the page can be changed with
 * ewl_ps_orientation_set().
 *
 * @{
 */

/**
 * @themekey /ps/file
 * @themekey /ps/group
 */

/**
 * @def EWL_PS_TYPE
 * The type name for the Ewl_Ps widget
 */
#define EWL_PS_TYPE "ps"

/**
 * The Ewl_Ps widget
 */
typedef struct Ewl_Ps Ewl_Ps;

/**
 * @def EWL_PS(ps)
 * Typecase a pointer to an Ewl_Ps widget
 */
#define EWL_PS(ps) ((Ewl_Ps *) ps)

/**
 * Inherits from Ewl_Widget and extends to provide a ps widget
 */
struct Ewl_Ps
{
        Ewl_Widget           widget;        /**< Inherit from Ewl_Widget */

        void                *image;         /**< The evas object for the image */
        char                *filename;      /**< The file name */
        int                  dirty;

        Eps_Document        *ps_document;   /**< The Eps document */
        Eps_Page            *ps_page;       /**< The Eps current page */

};

Ewl_Widget          *ewl_ps_new(void);
int                  ewl_ps_init(Ewl_Ps *ps);

int                  ewl_ps_file_set(Ewl_Ps *ps, const char *filename);
const char          *ewl_ps_file_get(Ewl_Ps *ps);

void                 ewl_ps_page_set(Ewl_Ps *ps, int page);
int                  ewl_ps_page_get(Ewl_Ps *ps);

void                 ewl_ps_size_get(Ewl_Ps *ps, int *width, int *height);

void                 ewl_ps_orientation_set(Ewl_Ps *ps, Eps_Page_Orientation o);
Eps_Page_Orientation ewl_ps_orientation_get(Ewl_Ps *ps);

void                 ewl_ps_scale_set(Ewl_Ps *ps, double hscale, double vscale);
void                 ewl_ps_scale_get(Ewl_Ps *ps, double *hscale, double *vscale);

void                 ewl_ps_page_next(Ewl_Ps *ps);
void                 ewl_ps_page_previous(Ewl_Ps *ps);

const Eps_Document  *ewl_ps_ps_document_get(Ewl_Ps *ps);
const Eps_Page      *ewl_ps_ps_page_get(Ewl_Ps *ps);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_ps_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_ps_reveal_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_ps_obscure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_ps_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data );

/**
 * @}
 */


#endif /* __EWL_PS_H__ */
