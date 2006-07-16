#ifndef __EWL_DVI_H__
#define __EWL_DVI_H__

#include <Edvi.h>

/**
 * @file ewl_dvi.h
 * @defgroup Ewl_Dvi Dvi: An Dvi Display Widget
 * Provides a widget for displaying evas loadable dvis, and edjes.
 *
 * @{
 */

/**
 * @themekey /dvi/file
 * @themekey /dvi/group
 */

typedef struct Ewl_Dvi Ewl_Dvi;

#define EWL_DVI(dvi) ((Ewl_Dvi *) dvi)

/**
 * @struct Ewl_Dvi
 *
 */
struct Ewl_Dvi
{
	Ewl_Image             image;
	char                 *filename;
	int                   page;
	int                   page_length;

	Edvi_Device          *dvi_device;
	Edvi_Property        *dvi_property;
	Edvi_Document        *dvi_document;
	Edvi_Page            *dvi_page;
	Edvi_Page_Orientation orientation;
	double                hscale;
	double                vscale;

};

Ewl_Widget           *ewl_dvi_new(void);
int                   ewl_dvi_init(Ewl_Dvi *dvi);
void                  ewl_dvi_file_set(Ewl_Dvi *dvi, const char *filename);
void                  ewl_dvi_page_set(Ewl_Dvi *dvi, int page);
int                   ewl_dvi_page_get(Ewl_Dvi *dvi);
Edvi_Document        *ewl_dvi_dvi_document_get (Ewl_Dvi *dvi);
Edvi_Page            *ewl_dvi_dvi_page_get (Ewl_Dvi *dvi);
void                  ewl_dvi_size_get (Ewl_Dvi *dvi, int *width, int *height);

void                  ewl_dvi_orientation_set (Ewl_Dvi *dvi, Edvi_Page_Orientation o);
Edvi_Page_Orientation ewl_dvi_orientation_get (Ewl_Dvi *dvi);

void                  ewl_dvi_scale_set (Ewl_Dvi *dvi, double hscale, double vscale);
void                  ewl_dvi_scale_get (Ewl_Dvi *dvi, double *hscale, double *vscale);
void                  ewl_dvi_page_next (Ewl_Dvi *dvi);
void                  ewl_dvi_page_previous (Ewl_Dvi *dvi);
void                  ewl_dvi_page_page_length_set (Ewl_Dvi *dvi, int page_length);
int                   ewl_dvi_page_page_length_get (Ewl_Dvi *dvi);
void                  ewl_dvi_page_page_next (Ewl_Dvi *dvi);
void                  ewl_dvi_page_page_previous (Ewl_Dvi *dvi);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_dvi_reveal_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_dvi_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data );

/**
 * @}
 */

#endif				/* __EWL_DVI_H__ */
