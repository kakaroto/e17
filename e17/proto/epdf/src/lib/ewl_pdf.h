#ifndef __EWL_PDF_H__
#define __EWL_PDF_H__

#include <Epdf.h>


/**
 * @file ewl_pdf.h
 *
 * @defgroup Ewl_Pdf Ewl Pdf
 *
 * @brief A PDF display Widget for the Ewl toolkit
 *
 * Provides a widget for displaying PDF files in an Ewl widget.
 * To use an Ewl_Pdf widget, create it with  ewl_pdf_new(), and
 * set the file with ewl_pdf_file_set(). For example:
 *
 * @code
 * #include <Ewl.h>
 * #include <ewl_pdf.h>
 *
 * Ewl_Widget *pdf;
 * char       *filename
 *
 * pdf = ewl_pdf_new ();
 * ewl_pdf_file_set (EWL_PDF (pdf), filename);
 * ewl_widget_show (pdf);
 * @endcode
 *
 * To change the page, use ewl_pdf_page_set() or ewl_pdf_page_next()
 * / ewl_pdf_page_previous().
 *
 * The orientation of the page can be changed with
 * ewl_pdf_orientation_set().
 *
 * @{
 */

/**
 * @themekey /pdf/file
 * @themekey /pdf/group
 */

/**
 * @def EWL_PDF_TYPE
 * The type name for the Ewl_Pdf widget
 */
#define EWL_PDF_TYPE "pdf"

/**
 * @typedef Ewl_Pdf
 * @brief An Ewl pdf widget is a simple widget that displays a PDF document
 */
typedef struct Ewl_Pdf Ewl_Pdf;

/**
 * @def EWL_PDF(pdf)
 * Typecase a pointer to an Ewl_Pdf widget
 */
#define EWL_PDF(pdf) ((Ewl_Pdf *) pdf)

/**
 * @struct Ewl_Pdf
 * @brief Inherits from Ewl_Widget and extends to provide a pdf widget
 */
struct Ewl_Pdf
{
        Ewl_Widget            widget;         /**< Inherit from Ewl_Widget */

        void                 *image;          /**< The evas object for the image */
        char                 *filename;       /**< The file name */
        int                   dirty;

        Epdf_Document        *pdf_document;   /**< The Epdf document */
        Epdf_Page            *pdf_page;       /**< The Epdf current page */
        Ecore_List           *pdf_index;      /**< The Epdf index */

        struct {
                void         *o;
                char         *text;
                Ecore_List   *list;
                int           page;
                int           is_case_sensitive;
                int           is_circular;
        }search;                              /**< The search members */

};

Ewl_Widget           *ewl_pdf_new(void);
int                   ewl_pdf_init(Ewl_Pdf *pdf);

int                   ewl_pdf_file_set(Ewl_Pdf *pdf, const char *filename);
const char           *ewl_pdf_file_get(Ewl_Pdf *pdf);

void                  ewl_pdf_page_set(Ewl_Pdf *pdf, int page);
int                   ewl_pdf_page_get(Ewl_Pdf *pdf);

void                  ewl_pdf_size_get(Ewl_Pdf *pdf, int *width, int *height);

void                  ewl_pdf_orientation_set(Ewl_Pdf *pdf, Epdf_Page_Orientation o);
Epdf_Page_Orientation ewl_pdf_orientation_get(Ewl_Pdf *pdf);

void                  ewl_pdf_scale_set(Ewl_Pdf *pdf, double hscale, double vscale);
void                  ewl_pdf_scale_get(Ewl_Pdf *pdf, double *hscale, double *vscale);

void                  ewl_pdf_page_next(Ewl_Pdf *pdf);
void                  ewl_pdf_page_previous(Ewl_Pdf *pdf);

void                  ewl_pdf_search_text_set(Ewl_Pdf *pdf, const char *text);
void                  ewl_pdf_search_is_case_sensitive(Ewl_Pdf *pdf, int is_case_sensitive);
int                   ewl_pdf_search_next(Ewl_Pdf *pdf);

Epdf_Document        *ewl_pdf_pdf_document_get(Ewl_Pdf *pdf);
Epdf_Page            *ewl_pdf_pdf_page_get(Ewl_Pdf *pdf);
Ecore_List           *ewl_pdf_pdf_index_get(Ewl_Pdf *pdf);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_pdf_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_pdf_reveal_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_pdf_obscure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_pdf_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data );

/**
 * @}
 */


#endif /* __EWL_PDF_H__ */
