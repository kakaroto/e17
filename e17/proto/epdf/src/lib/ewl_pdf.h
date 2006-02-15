#ifndef __EWL_PDF_H__
#define __EWL_PDF_H__

#include <Evas_Poppler.h>

/**
 * @file ewl_pdf.h
 * @defgroup Ewl_Pdf Pdf: An Pdf Display Widget
 * Provides a widget for displaying evas loadable pdfs, and edjes.
 *
 * @{
 */

/**
 * @themekey /pdf/file
 * @themekey /pdf/group
 */

typedef struct Ewl_Pdf Ewl_Pdf;

#define EWL_PDF(pdf) ((Ewl_Pdf *) pdf)

/**
 * @struct Ewl_Pdf
 *
 */
struct Ewl_Pdf
{
	Ewl_Image       image;
	int             page;

        Evas_Poppler_Document  *pdf_document;
        Evas_Poppler_Page      *pdf_page;
        Ecore_List             *pdf_index;
};

Ewl_Widget            *ewl_pdf_new(void);
int                    ewl_pdf_init(Ewl_Pdf *pdf);
void                   ewl_pdf_file_set(Ewl_Pdf *pdf, char *filename);
void                   ewl_pdf_page_set(Ewl_Pdf *pdf, int page);
int                    ewl_pdf_page_get(Ewl_Pdf *pdf);
Evas_Poppler_Document *ewl_pdf_pdf_document_get (Ewl_Pdf *pdf);
Evas_Poppler_Page     *ewl_pdf_pdf_page_get (Ewl_Pdf *pdf);
Ecore_List            *ewl_pdf_pdf_index_get (Ewl_Pdf *pdf);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_pdf_reveal_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_pdf_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data );

/**
 * @}
 */

#endif				/* __EWL_PDF_H__ */
