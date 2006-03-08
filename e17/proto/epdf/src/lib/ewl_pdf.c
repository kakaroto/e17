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
 * Sets the fields and callbacks of @a i to their default values.
 */
int
ewl_pdf_init(Ewl_Pdf *pdf)
{
	Ewl_Widget *w;
	Ewl_Image  *i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, FALSE);

	w = EWL_WIDGET(pdf);
	i = EWL_IMAGE(pdf);

	if (!ewl_image_init(i))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, "pdf");
	ewl_widget_inherit(w, "pdf");

	/*
	 * Append necessary callbacks.
	 */
	ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_pdf_reveal_cb,
			    NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_pdf_destroy_cb,
			    NULL);

	i->type = EWL_IMAGE_TYPE_NORMAL;

	i->path = NULL;
	pdf->page = 0;

	pdf->pdf_document = NULL;
	pdf->pdf_page = NULL;
	pdf->pdf_index = NULL;

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
	DCHECK_TYPE_RET("pdf", pdf, "pdf", 0);

	DRETURN_INT(pdf->page, DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to change the displayed pdf
 * @param filename: the path to the new pdf to be displayed by @a pdf
 * @return Returns no value.
 * @brief Change the pdf file displayed by an pdf widget
 *
 * Set the pdf displayed by @a pdf to the one found at the path @a im. If an
 * edje is used, a minimum size should be specified in the edje or the code.
 */
void
ewl_pdf_file_set(Ewl_Pdf *pdf, const char *filename)
{
	Ewl_Widget *w;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, "pdf");

	w = EWL_WIDGET(pdf);
	emb = ewl_embed_widget_find(w);

	ewl_image_file_set(EWL_IMAGE(pdf), filename, NULL);

	if (pdf->pdf_document) {
		if (pdf->pdf_page)
			evas_poppler_page_delete (pdf->pdf_page);
		if (pdf->pdf_index)
			evas_poppler_index_delete (pdf->pdf_index);
		evas_poppler_document_delete (pdf->pdf_document);
	}

	pdf->pdf_document = evas_poppler_document_new (filename);
	pdf->pdf_index = evas_poppler_index_new (pdf->pdf_document);
	pdf->page = 0;

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
	DCHECK_TYPE("pdf", pdf, "pdf");

	if (!pdf->pdf_document ||
	    (page >= evas_poppler_document_page_count_get (pdf->pdf_document)) ||
	    (page == pdf->page))
                DLEAVE_FUNCTION(DLEVEL_STABLE);

	pdf->page = page;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the poppler document of
 * @return Returns the poppler document of the pdf (NULL on failure)
 * @brief get the poppler document of the pdf
 */
Evas_Poppler_Document *ewl_pdf_pdf_document_get (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, 0);
	DCHECK_TYPE_RET("pdf", pdf, "pdf", 0);

	DRETURN_PTR(pdf->pdf_document, DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the current poppler page of
 * @return Returns the current poppler page of the pdf (NULL on failure)
 * @brief get the current poppler page of the pdf
 */
Evas_Poppler_Page *ewl_pdf_pdf_page_get (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, 0);
	DCHECK_TYPE_RET("pdf", pdf, "pdf", 0);

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
	DCHECK_PARAM_PTR_RET("pdf", pdf, 0);
	DCHECK_TYPE_RET("pdf", pdf, "pdf", 0);

	DRETURN_PTR(pdf->pdf_index, DLEVEL_STABLE);
}

void
ewl_pdf_search_text_set (Ewl_Pdf *pdf, const char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, "pdf");

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
	DCHECK_TYPE("pdf", pdf, "pdf");

        if (page != pdf->search.page)
                pdf->search.page = page;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_pdf_search_is_case_sensitive (Ewl_Pdf *pdf, int is_case_sensitive)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pdf", pdf);
	DCHECK_TYPE("pdf", pdf, "pdf");

	if (is_case_sensitive != pdf->search.is_case_sensitive)
                pdf->search.is_case_sensitive = is_case_sensitive;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_pdf_search_next (Ewl_Pdf *pdf)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("pdf", pdf, FALSE);
	DCHECK_TYPE_RET("pdf", pdf, "pdf", 0);

	if (!pdf->search.text)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (!pdf->search.o) {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(EWL_WIDGET (pdf));
                pdf->search.o = evas_object_rectangle_add(emb->evas);
                if (!pdf->search.o)
                        DRETURN_INT(FALSE, DLEVEL_STABLE);
                evas_object_color_set(pdf->search.o, 0, 128, 0, 128);
                evas_object_hide (pdf->search.o);
        }

 next_page:
        /* no list, we search one */
        while (!pdf->search.list &&
               pdf->search.page < evas_poppler_document_page_count_get (pdf->pdf_document)) {
                Evas_Poppler_Page *page;

                pdf->search.page++;
                printf ("page : %d\n", pdf->search.page);
                page = evas_poppler_document_page_get (pdf->pdf_document, pdf->search.page);
                pdf->search.list = evas_poppler_page_text_find (page,
                                                                pdf->search.text,
                                                                pdf->search.is_case_sensitive);
                if (pdf->search.list)
                        ecore_list_goto_first (pdf->search.list);
                evas_poppler_page_delete (page);
        }
        
        /* an already existing list or a newly one */
        if (pdf->search.list) {
                Rectangle *rect;

                if ((rect = (Rectangle *)ecore_list_next (pdf->search.list))) {
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
ewl_pdf_reveal_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		  void *user_data __UNUSED__)
{
	Ewl_Pdf   *pdf;
	Ewl_Image *i;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	pdf = EWL_PDF(w);
	i = EWL_IMAGE(w);
	emb = ewl_embed_widget_find(w);

	/*
	 * Load the pdf
	 */
	if (!i->image)
	  i->image = ewl_embed_object_request(emb, "pdf");
	if (!i->image)
	  i->image = evas_object_image_add(emb->evas);
	if (!i->image)
	  DRETURN(DLEVEL_STABLE);

	if (pdf->pdf_document) {
		if (pdf->pdf_page)
			evas_poppler_page_delete (pdf->pdf_page);
		pdf->pdf_page = evas_poppler_document_page_get (pdf->pdf_document, pdf->page);
		evas_poppler_page_render (pdf->pdf_page, i->image, 0, 0, 0, 0, 72.0, 72.0);
	}
	evas_object_image_size_get(i->image, &i->ow, &i->oh);

	evas_object_layer_set(i->image, ewl_widget_layer_sum_get(w));
	if (w->fx_clip_box)
		evas_object_clip_set(i->image, w->fx_clip_box);

	evas_object_pass_events_set(i->image, TRUE);
	evas_object_show(i->image);

	if (!i->ow)
		i->ow = 1;
	if (!i->oh)
		i->oh = 1;

	if (i->aw || i->ah) {
		ewl_image_scale_to(i, i->aw, i->ah);
	}
	else {
		ewl_object_preferred_inner_w_set(EWL_OBJECT(i), i->ow);
		ewl_object_preferred_inner_h_set(EWL_OBJECT(i), i->oh);
		ewl_image_scale(i, i->sw, i->sh);
	}

	/*Constrain settings*/
	if (i->cs && (i->ow > i->cs || i->oh > i->cs)) {
		double cp = 0;
		if (i->ow > i->oh)
			cp = i->cs / (double)i->ow;
		else
			cp = i->cs / (double)i->oh;

		ewl_image_scale(i, cp, cp);
		ewl_image_tile_set(i, 0, 0, cp*i->ow, cp*i->oh);

	}

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

	if (pdf->pdf_document)
	  evas_poppler_document_delete (pdf->pdf_document);
	if (pdf->pdf_page)
	  evas_poppler_page_delete (pdf->pdf_page);
	if (pdf->pdf_index)
	  evas_poppler_index_delete (pdf->pdf_index);
        if (pdf->search.text) free (pdf->search.text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
