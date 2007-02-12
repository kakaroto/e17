#include <math.h>

#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_pdf.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif


#define round(a) ( ((a)<0.0) ? (int)(floor((a) - 0.5)) : (int)(floor((a) + 0.5)) )

/**
 * @return Returns a pointer to a new pdf widget on success, NULL on failure.
 * @brief Load an pdf widget with specified pdf contents
 *
 * The @a k parameter is primarily used for loading edje groups or keyed data
 * in an pdf.
 */
Ewl_Widget *
ewl_pdf_new(void)
{
	Ewl_Pdf *pdf;

	DENTER_FUNCTION(DLEVEL_STABLE);

	pdf = NEW(Ewl_Pdf, 1);
	if (!pdf)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_pdf_init(pdf)) {
		ewl_widget_destroy(EWL_WIDGET(pdf));
		pdf = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(pdf), DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to initialize
 * @return Returns no value.
 * @brief Initialize an pdf widget to default values and callbacks
 *
 * Sets the fields and callbacks of @a pdf to their default values.
 */
int
ewl_pdf_init(Ewl_Pdf *pdf)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, FALSE);

	w = EWL_WIDGET(pdf);

	if (!ewl_widget_init(w))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_PDF_TYPE);
	ewl_widget_inherit(w, EWL_PDF_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);

	/*
	 * Append necessary callbacks.
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_pdf_configure_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_pdf_reveal_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_pdf_obscure_cb,
			    NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_pdf_destroy_cb,
			    NULL);

	pdf->filename = NULL;
	pdf->page = 0;
	pdf->page_length = 10;

	pdf->pdf_document = NULL;
	pdf->pdf_page = NULL;
	pdf->pdf_index = NULL;

	pdf->orientation = EPDF_PAGE_ORIENTATION_PORTRAIT;
	pdf->hscale = 1.0;
	pdf->vscale = 1.0;

        pdf->search.o = NULL;
        pdf->search.text = NULL;
        pdf->search.list = NULL;
        pdf->search.page = -1;
        pdf->search.is_case_sensitive = FALSE;
        pdf->search.is_circular = FALSE;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the file of
 * @return Returns the currently set page
 * @brief get the page this pdf uses
 */
int
ewl_pdf_page_get(Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, 0);
	DCHECK_TYPE_RET("pdf", pdf, EWL_PDF_TYPE, 0);

	DRETURN_INT(pdf->page, DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to change the displayed pdf
 * @param filename: the filename to the new pdf to be displayed by @a pdf
 * @return Returns no value.
 * @brief Change the pdf file displayed by an pdf widget
 *
 * Set the pdf displayed by @a pdf to the one found at the filename @a filename. If an
 * edje is used, a minimum size should be specified in the edje or the code.
 */
void
ewl_pdf_file_set(Ewl_Pdf *pdf, const char *filename)
{
	Ewl_Widget *w;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	w = EWL_WIDGET(pdf);
	emb = ewl_embed_widget_find(w);

	if (pdf->filename != filename) {
		IF_FREE(pdf->filename);
	}
	if (filename) {
		pdf->filename = strdup(filename);
	}

	if (pdf->pdf_document) {
		if (pdf->pdf_page)
			epdf_page_delete (pdf->pdf_page);
		if (pdf->pdf_index)
			epdf_index_delete (pdf->pdf_index);
		epdf_document_delete (pdf->pdf_document);
	}

	pdf->pdf_document = epdf_document_new (filename);
	pdf->pdf_index = epdf_index_new (pdf->pdf_document);
	pdf->page = 0;

        pdf->search.o = NULL;
        pdf->search.text = NULL;
        pdf->search.list = NULL;
        pdf->search.page = -1;
        pdf->search.is_case_sensitive = FALSE;
        pdf->search.is_circular = FALSE;

	/*
	 * Load the new pdf if widget has been realized
	 */
	if (REALIZED(w)) {
		ewl_widget_unrealize(w);
		ewl_widget_realize(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf to change constrain setting
 * @param size: the minimum constrain size
 * @return Returns no value.
 * @brief Set a size which, if the pdf is bigger than, scale proportionally
 *
 * Sets a size to scale to proportionally if the pdf exceeds this size
 */
void ewl_pdf_page_set(Ewl_Pdf *pdf, int page)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf->pdf_document ||
	    (page >= epdf_document_page_count_get (pdf->pdf_document)) ||
	    (page == pdf->page))
                DLEAVE_FUNCTION(DLEVEL_STABLE);

	pdf->page = page;
	ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the poppler document of
 * @return Returns the poppler document of the pdf (NULL on failure)
 * @brief get the poppler document of the pdf
 */
Epdf_Document *ewl_pdf_pdf_document_get (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, NULL);
	DCHECK_TYPE_RET("pdf", pdf, EWL_PDF_TYPE, NULL);

	DRETURN_PTR(pdf->pdf_document, DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the current poppler page of
 * @return Returns the current poppler page of the pdf (NULL on failure)
 * @brief get the current poppler page of the pdf
 */
Epdf_Page *ewl_pdf_pdf_page_get (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, NULL);
	DCHECK_TYPE_RET("pdf", pdf, EWL_PDF_TYPE, NULL);

	DRETURN_PTR(pdf->pdf_page, DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the index of
 * @return Returns the poppler index of the pdf (NULL on failure)
 * @brief get the poppler index of the pdf
 */
Ecore_List *ewl_pdf_pdf_index_get (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, NULL);
	DCHECK_TYPE_RET("pdf", pdf, EWL_PDF_TYPE, NULL);

	DRETURN_PTR(pdf->pdf_index, DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the size of
 * @param width width of the current page
 * @param height height of the current page
 * @brief get the poppler size of the pdf @p pdf. If @p pdf is NULL,
 * return a width equal to 0 and a height equal to 0
 */
void ewl_pdf_pdf_size_get (Ewl_Pdf *pdf, int *width, int *height)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf) {
		if (width) *width = 0;
		if (height) *height = 0;
	}
	else {
		if (width) *width = epdf_page_width_get (pdf->pdf_page);
		if (height) *height = epdf_page_height_get (pdf->pdf_page);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_search_text_set (Ewl_Pdf *pdf, const char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if ((!text) ||
            (pdf->search.text &&
             strcmp (text, pdf->search.text) == 0))
                DRETURN(DLEVEL_STABLE);

        if (pdf->search.text) free (pdf->search.text);
	pdf->search.text = strdup (text);
	pdf->search.page = -1;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_search_first_page_set (Ewl_Pdf *pdf, int page)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

        if (page != pdf->search.page)
                pdf->search.page = page;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_search_is_case_sensitive (Ewl_Pdf *pdf, int is_case_sensitive)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (is_case_sensitive != pdf->search.is_case_sensitive)
                pdf->search.is_case_sensitive = is_case_sensitive;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_pdf_search_next (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, FALSE);
	DCHECK_TYPE_RET("pdf", pdf, EWL_PDF_TYPE, FALSE);

	if (!pdf->search.text)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (!pdf->search.o) {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(EWL_WIDGET (pdf));
                pdf->search.o = evas_object_rectangle_add(emb->canvas);
                if (!pdf->search.o)
                        DRETURN_INT(FALSE, DLEVEL_STABLE);
                evas_object_color_set(pdf->search.o, 0, 128, 0, 128);
                evas_object_hide (pdf->search.o);
        }

 next_page:
        /* no list, we search one */
        while (!pdf->search.list &&
               pdf->search.page < epdf_document_page_count_get (pdf->pdf_document)) {
                Epdf_Page *page;

                pdf->search.page++;
                printf ("page : %d\n", pdf->search.page);
                page = epdf_document_page_get (pdf->pdf_document, pdf->search.page);
                pdf->search.list = epdf_page_text_find (page,
                                                        pdf->search.text,
                                                        pdf->search.is_case_sensitive);
                if (pdf->search.list)
                        ecore_list_goto_first (pdf->search.list);
                epdf_page_delete (page);
        }

        /* an already existing list or a newly one */
        if (pdf->search.list) {
                Epdf_Rectangle *rect;

                if ((rect = (Epdf_Rectangle *)ecore_list_next (pdf->search.list))) {
                  if (pdf->search.page != pdf->page) {
                          ewl_pdf_page_set (pdf, pdf->search.page);
                          ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL);
                  }
                        evas_object_move (pdf->search.o,
                                          CURRENT_X(EWL_WIDGET (pdf)) + round (rect->x1 - 1),
                                          CURRENT_Y(EWL_WIDGET (pdf)) + round (rect->y1 - 1));
                        evas_object_resize (pdf->search.o,
                                            round (rect->x2 - rect->x1 + 1),
                                            round (rect->y2 - rect->y1));
                        if (!evas_object_visible_get (pdf->search.o))
                                evas_object_show (pdf->search.o);
                        /* we leave... */
                        DRETURN_INT(TRUE, DLEVEL_STABLE);
                }
                else { /* no more word to find. We destroy the list */
                        ecore_list_destroy (pdf->search.list);
                        pdf->search.list = NULL;
                        /* we search a new one */
                printf ("page0 : %d\n", pdf->search.page);
                        goto next_page;
                }
        }
        evas_object_hide (pdf->search.o);

        if (pdf->search.is_circular) {
                pdf->search.page = -1;
                DRETURN_INT(TRUE, DLEVEL_STABLE);
        }
        else
                DRETURN_INT(FALSE, DLEVEL_STABLE);
}

void
ewl_pdf_orientation_set (Ewl_Pdf *pdf, Epdf_Page_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf || !pdf->pdf_page || (pdf->orientation == o))
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	pdf->orientation = o;
	ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Epdf_Page_Orientation
ewl_pdf_orientation_get (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, EPDF_PAGE_ORIENTATION_PORTRAIT);
	DCHECK_TYPE_RET("pdf", pdf, EWL_PDF_TYPE, EPDF_PAGE_ORIENTATION_PORTRAIT);

	if (!pdf || !pdf->pdf_page)
		DRETURN_INT(EPDF_PAGE_ORIENTATION_PORTRAIT, DLEVEL_STABLE);

	DRETURN_INT(epdf_page_orientation_get (pdf->pdf_page), DLEVEL_STABLE);
}

void
ewl_pdf_scale_set (Ewl_Pdf *pdf, double hscale, double vscale)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	if (hscale != pdf->hscale) pdf->hscale = hscale;
	if (vscale != pdf->vscale) pdf->vscale = vscale;
	ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_scale_get (Ewl_Pdf *pdf, double *hscale, double *vscale)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf) {
		if (hscale) *hscale = 1.0;
		if (vscale) *vscale = 1.0;
	}
	else {
		if (hscale) *hscale = pdf->hscale;
		if (vscale) *vscale = pdf->vscale;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_page_next (Ewl_Pdf *pdf)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = pdf->page;
	if (page < (epdf_document_page_count_get(pdf->pdf_document) - 1))
		page++;
	ewl_pdf_page_set (pdf, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_page_previous (Ewl_Pdf *pdf)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = pdf->page;
	if (page > 0)
		page--;
	ewl_pdf_page_set (pdf, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_page_page_length_set (Ewl_Pdf *pdf, int page_length)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf || (page_length <= 0) || (pdf->page_length == page_length))
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	pdf->page_length = page_length;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_pdf_page_page_length_get (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, 0);
	DCHECK_TYPE_RET("pdf", pdf, EWL_PDF_TYPE, 0);

	if (!pdf)
		DRETURN_INT(0, DLEVEL_STABLE);

	DRETURN_INT(pdf->page_length, DLEVEL_STABLE);
}

void
ewl_pdf_page_page_next (Ewl_Pdf *pdf)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = pdf->page + pdf->page_length;
	if (page >= epdf_document_page_count_get(pdf->pdf_document))
		page = epdf_document_page_count_get(pdf->pdf_document) - 1;
	ewl_pdf_page_set (pdf, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_page_page_previous (Ewl_Pdf *pdf)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, EWL_PDF_TYPE);

	if (!pdf)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = pdf->page - pdf->page_length;
	if (page < 0)
		page = 0;
	ewl_pdf_page_set (pdf, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_reveal_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		  void *user_data __UNUSED__)
{
	Ewl_Pdf   *pdf;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	pdf = EWL_PDF(w);
	emb = ewl_embed_widget_find(w);

	/*
	 * Load the pdf
	 */
	if (!pdf->image)
	  pdf->image = ewl_embed_object_request(emb, "pdf");
	if (!pdf->image)
	  pdf->image = evas_object_image_add(emb->canvas);
	if (!pdf->image)
	  DRETURN(DLEVEL_STABLE);

	if (pdf->pdf_document) {
		if (pdf->pdf_page)
			epdf_page_delete (pdf->pdf_page);
		pdf->pdf_page = epdf_document_page_get (pdf->pdf_document, pdf->page);
		epdf_page_render (pdf->pdf_page, pdf->image,
                                  pdf->orientation,
                                  0, 0, -1, -1,
                                  pdf->hscale, pdf->vscale);
	}
	evas_object_image_size_get(pdf->image, &pdf->ow, &pdf->oh);

	evas_object_smart_member_add(pdf->image, w->smart_object);
	if (w->fx_clip_box)
		evas_object_stack_below(pdf->image, w->fx_clip_box);

	if (w->fx_clip_box)
		evas_object_clip_set(pdf->image, w->fx_clip_box);

	evas_object_pass_events_set(pdf->image, TRUE);
	evas_object_show(pdf->image);

	if (!pdf->ow)
		pdf->ow = 1;
	if (!pdf->oh)
		pdf->oh = 1;

	ewl_object_preferred_inner_w_set(EWL_OBJECT(w), pdf->ow);
	ewl_object_preferred_inner_h_set(EWL_OBJECT(w), pdf->oh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_obscure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Pdf *pdf;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);

	pdf = EWL_PDF(w);
	if (emb && pdf->image) {
		ewl_embed_object_cache(emb, pdf->image);
		pdf->image = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Pdf *pdf;
	Ewl_Embed *emb;
	int ww, hh;
	int dx = 0, dy = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	pdf = EWL_PDF(w);
	if (!pdf->image)
		DRETURN(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);

	ww = CURRENT_W(w);
	hh = CURRENT_H(w);
	if (ww > pdf->ow)
		ww = pdf->ow;
	if (hh > pdf->oh)
		hh = pdf->oh;

	dx = (CURRENT_W(w) - ww) / 2;
	dy = (CURRENT_H(w) - hh) / 2;

	evas_object_image_fill_set(pdf->image, 0, 0,
				ww, hh);

	evas_object_move(pdf->image, CURRENT_X(w) + dx, CURRENT_Y(w) + dy);
	evas_object_resize(pdf->image, ww, hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		   void *user_data __UNUSED__)
{
	Ewl_Pdf *pdf;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	pdf = EWL_PDF(w);

	IF_FREE(pdf->filename);
	if (pdf->pdf_document)
	  epdf_document_delete (pdf->pdf_document);
	if (pdf->pdf_page)
	  epdf_page_delete (pdf->pdf_page);
	if (pdf->pdf_index)
	  epdf_index_delete (pdf->pdf_index);
        if (pdf->search.text) free (pdf->search.text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
