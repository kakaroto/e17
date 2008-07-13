#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

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
 * @addtogroup Ewl_Pdf
 * @{
 */

/**
 * @return Returns a pointer to a new pdf widget on success, NULL on failure.
 * @brief Load an pdf widget with specified pdf contents
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
 * @param pdf the pdf widget to initialize
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
        DCHECK_PARAM_PTR_RET(pdf, FALSE);

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

        pdf->pdf_document = NULL;
        pdf->pdf_page = NULL;
        pdf->pdf_index = NULL;

        pdf->dirty = 1;

        pdf->search.o = NULL;
        pdf->search.text = NULL;
        pdf->search.list = NULL;
        pdf->search.page = -1;
        pdf->search.is_case_sensitive = FALSE;
        pdf->search.is_circular = FALSE;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param pdf the pdf widget to change the displayed pdf
 * @param filename: the path to the new pdf to be displayed by @a pdf
 * @return 0 on failure, 1 otherwise.
 * @brief Change the pdf file displayed by an pdf widget
 *
 * Set the pdf displayed by @a pdf to the one found at the filename @a filename. If an
 * edje is used, a minimum size should be specified in the edje or the code.
 */
int
ewl_pdf_file_set(Ewl_Pdf *pdf, const char *filename)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, FALSE);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        w = EWL_WIDGET(pdf);

        if (!filename || (filename[0] == '\0'))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (pdf->filename != filename) {
                IF_FREE(pdf->filename);
        }

        if (pdf->pdf_index) {
                epdf_index_delete(pdf->pdf_index);
                pdf->pdf_index = NULL;
        }

        if (pdf->pdf_page) {
                epdf_page_delete(pdf->pdf_page);
                pdf->pdf_page = NULL;
        }

        if (pdf->pdf_document) {
                epdf_document_delete(pdf->pdf_document);
                pdf->pdf_document = NULL;
        }

        pdf->filename = strdup(filename);

        pdf->search.o = NULL;
        pdf->search.text = NULL;
        pdf->search.list = NULL;
        pdf->search.page = -1;
        pdf->search.is_case_sensitive = FALSE;
        pdf->search.is_circular = FALSE;

        /*
         * Load the new PDF document
         */

        pdf->pdf_document = epdf_document_new(pdf->filename);
        if (!pdf->pdf_document)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        pdf->pdf_page = epdf_page_new(pdf->pdf_document);
        if (!pdf->pdf_page) {
                epdf_document_delete(pdf->pdf_document);
                pdf->pdf_document = NULL;
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        pdf->pdf_index = epdf_index_new(pdf->pdf_document);
        if (!pdf->pdf_index) {
                epdf_page_delete(pdf->pdf_page);
                pdf->pdf_page = NULL;
                epdf_document_delete(pdf->pdf_document);
                pdf->pdf_document = NULL;
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        if (REALIZED(w)) {
                ewl_widget_obscure(w);
                ewl_widget_reveal(w);
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param pdf the pdf widget to get the file of
 * @return Returns the currently set file name
 * @brief get the file name this pdf uses
 */
const char *
ewl_pdf_file_get(Ewl_Pdf *pdf)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, NULL);
        DCHECK_TYPE_RET(pdf, EWL_PDF_TYPE, NULL);

        DRETURN_PTR(pdf->filename, DLEVEL_STABLE);
}

/**
 * @param pdf the pdf to set the page of
 * @param page:  the page number
 * @return Returns no value.
 * @brief Set the page of the document
 *
 * Sets the page of the document @p pdf to @p page
 */
void
ewl_pdf_page_set(Ewl_Pdf *pdf, int page)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (!pdf->pdf_document ||
            !pdf->pdf_page ||
            (page < 0) ||
            (page >= epdf_document_page_count_get(pdf->pdf_document)) ||
            (page == epdf_page_page_get(pdf->pdf_page)))
                DRETURN(DLEVEL_STABLE);

        pdf->dirty = 1;
        epdf_page_page_set(pdf->pdf_page, page);
        ewl_widget_configure(EWL_WIDGET(pdf));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf the pdf widget to get the page of
 * @return Returns the currently set page
 * @brief get the page this pdf uses
 */
int
ewl_pdf_page_get(Ewl_Pdf *pdf)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, 0);
        DCHECK_TYPE_RET(pdf, EWL_PDF_TYPE, 0);

        if (!pdf->pdf_page)
                DRETURN_INT(0, DLEVEL_STABLE);

        DRETURN_INT(epdf_page_page_get(pdf->pdf_page), DLEVEL_STABLE);
}

/**
 * @param pdf the pdf widget to get the size of
 * @param width width of the current page
 * @param height height of the current page
 * @brief get the size of the pdf @p pdf. If @p pdf is NULL,
 * return a width equal to 0 and a height equal to 0
 */
void
ewl_pdf_pdf_size_get(Ewl_Pdf *pdf, int *width, int *height)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (!pdf->pdf_page) {
                if (width) *width = 0;
                if (height) *height = 0;
                DRETURN(DLEVEL_STABLE);
        }

        epdf_page_size_get(pdf->pdf_page, width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf to change the orientation
 * @param o: the orientation
 * @return Returns no value.
 * @brief Set an orientation of the document
 *
 * Sets an orientation @p o of the document
 */
void
ewl_pdf_orientation_set(Ewl_Pdf *pdf, Epdf_Page_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (!pdf->pdf_page || (epdf_page_orientation_get(pdf->pdf_page) == o))
                DRETURN(DLEVEL_STABLE);

        pdf->dirty = 1;
        epdf_page_orientation_set(pdf->pdf_page, o);
        ewl_widget_configure(EWL_WIDGET(pdf));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the orientation of
 * @return Returns the orientation of the document.
 * @brief get the orientation of the document @p pdf. If @p pdf
 * is NULL, return EPDF_PAGE_ORIENTATION_PORTRAIT
 */
Epdf_Page_Orientation
ewl_pdf_orientation_get(Ewl_Pdf *pdf)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, EPDF_PAGE_ORIENTATION_PORTRAIT);
        DCHECK_TYPE_RET(pdf, EWL_PDF_TYPE, EPDF_PAGE_ORIENTATION_PORTRAIT);

        if (!pdf->pdf_page)
                DRETURN_INT(EPDF_PAGE_ORIENTATION_PORTRAIT, DLEVEL_STABLE);

        DRETURN_INT(epdf_page_orientation_get(pdf->pdf_page), DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf to change the scale
 * @param hscale: the horizontal scale
 * @param vscale: the vertical scale
 * @return Returns no value.
 * @brief Set the scale of the document
 *
 * Sets the horizontal scale @p hscale and the vertical scale @p vscale
 * of the document @p pdf
 */
void
ewl_pdf_scale_set(Ewl_Pdf *pdf, double hscale, double vscale)
{
        double hs;
        double vs;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (!pdf->pdf_page)
                DRETURN(DLEVEL_STABLE);

        epdf_page_scale_get(pdf->pdf_page, &hs, &vs);
        if ((hs == hscale) && (vs == vscale))
                DRETURN(DLEVEL_STABLE);

        pdf->dirty = 1;
        epdf_page_scale_set(pdf->pdf_page, hscale, vscale);
        ewl_widget_configure(EWL_WIDGET(pdf));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget to get the orientation of
 * @param hscale: horizontal scale of the current page
 * @param vscale: vertical scale of the current page
 * @return Returns  no value.
 * @brief get the horizontal scale @p hscale and the vertical scale
 * @p vscale of the document @p pdf. If @p pdf is NULL, their values are 1.0
 */
void
ewl_pdf_scale_get(Ewl_Pdf *pdf, double *hscale, double *vscale)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (!pdf->pdf_page)
        {
                if (hscale) *hscale = 1.0;
                if (vscale) *vscale = 1.0;
                DRETURN(DLEVEL_STABLE);
        }

        epdf_page_scale_get(pdf->pdf_page, hscale, vscale);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget
 * @return Returns  no value.
 * @brief go to the next page and render it
 */
void
ewl_pdf_page_next(Ewl_Pdf *pdf)
{
        int page;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (!pdf->pdf_page)
                DRETURN(DLEVEL_STABLE);

        page = epdf_page_page_get(pdf->pdf_page);
        if (page < (epdf_document_page_count_get(pdf->pdf_document) - 1))
                page++;
        ewl_pdf_page_set(pdf, page);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf widget
 * @return Returns  no value.
 * @brief go to the previous page and render it
 */
void
ewl_pdf_page_previous(Ewl_Pdf *pdf)
{
        int page;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (!pdf->pdf_page)
                DRETURN(DLEVEL_STABLE);

        page = epdf_page_page_get(pdf->pdf_page);
        if (page > 0)
                page--;
        ewl_pdf_page_set(pdf, page);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf
 * @param text: the text to search
 * @return Returns no value.
 * @brief Set the text to search in the document
 *
 * Sets the text to search to the value @p text.
 */
void
ewl_pdf_search_text_set(Ewl_Pdf *pdf, const char *text)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if ((!text) ||
            (pdf->search.text &&
             strcmp(text, pdf->search.text) == 0))
                DRETURN(DLEVEL_STABLE);

        if (pdf->search.text) free(pdf->search.text);
        pdf->search.text = strdup(text);
        pdf->search.page = -1;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf
 * @param page: the page from which to begin the search
 * @return Returns no value.
 * @brief Set the page from which to begin the search
 *
 * Sets the page from which the search of text will begin
 * from to the value @p page
 */
void
ewl_pdf_search_first_page_set(Ewl_Pdf *pdf, int page)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (page != pdf->search.page)
                pdf->search.page = page;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf to change the scale
 * @param is_case_sensitive: 0 for case insensitive search, otherwise case sensitive search
 * @return Returns no value.
 * @brief Set the case of the search
 *
 * Sets the case of the search. If @p is_case_sensitive is 0,
 * then the search is case insensitive. Otherwise, the search
 * is case sensitive.
 */
void
ewl_pdf_search_is_case_sensitive(Ewl_Pdf *pdf, int is_case_sensitive)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pdf);
        DCHECK_TYPE(pdf, EWL_PDF_TYPE);

        if (is_case_sensitive != pdf->search.is_case_sensitive)
                pdf->search.is_case_sensitive = is_case_sensitive;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pdf: the pdf to change the scale
 * @return 1 if the text is found, 0 otherwise.
 * @brief Set the case of the search
 *
 * Start the search of text in the @p pdf document. The
 * text to search should have been set by ewl_pdf_search_text_set().
 * It is @c NULL by default. The first page from which the search
 * starts is set by ewl_pdf_search_first_page_set(). It is 0 by default.
 * The search can be case sensitive or not. It is set by
 * ewl_pdf_search_is_case_sensitive() and it is case insensitive by
 * default.
 *
 * Several call of that function can be done to search several occurences
 * of the text in the document. When a text is found,
 * epdf_page_page_set() is called.
 */
int
ewl_pdf_search_next(Ewl_Pdf *pdf)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, FALSE);
        DCHECK_TYPE_RET(pdf, EWL_PDF_TYPE, FALSE);

        if (!pdf->search.text)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (!pdf->search.o) {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(EWL_WIDGET(pdf));
                pdf->search.o = evas_object_rectangle_add(emb->canvas);
                if (!pdf->search.o)
                        DRETURN_INT(FALSE, DLEVEL_STABLE);
                evas_object_color_set(pdf->search.o, 0, 128, 0, 128);
                evas_object_hide(pdf->search.o);
        }

 next_page:
        /* no list, we search one */
        while (!pdf->search.list &&
               pdf->search.page < epdf_document_page_count_get(pdf->pdf_document)) {
                Epdf_Page *page;

                pdf->search.page++;
                printf("page : %d\n", pdf->search.page);
                epdf_page_page_set(pdf->pdf_page, pdf->search.page);
                pdf->search.list = epdf_page_text_find(page,
                                                       pdf->search.text,
                                                       pdf->search.is_case_sensitive);
                if (pdf->search.list)
                        ecore_list_first_goto(pdf->search.list);
                epdf_page_delete(page);
        }

        /* an already existing list or a newly one */
        if (pdf->search.list) {
                Epdf_Rectangle *rect;

                if ((rect = (Epdf_Rectangle *)ecore_list_next(pdf->search.list))) {
                  if (pdf->search.page != epdf_page_page_get(pdf->pdf_page)) {
                          ewl_pdf_page_set(pdf, pdf->search.page);
                          ewl_callback_call(EWL_WIDGET(pdf), EWL_CALLBACK_REVEAL);
                  }
                        evas_object_move(pdf->search.o,
                                         CURRENT_X(EWL_WIDGET(pdf)) + round(rect->x1 - 1),
                                         CURRENT_Y(EWL_WIDGET(pdf)) + round(rect->y1 - 1));
                        evas_object_resize(pdf->search.o,
                                           round(rect->x2 - rect->x1 + 1),
                                           round(rect->y2 - rect->y1));
                        if (!evas_object_visible_get(pdf->search.o))
                                evas_object_show(pdf->search.o);
                        /* we leave... */
                        DRETURN_INT(TRUE, DLEVEL_STABLE);
                }
                else { /* no more word to find. We destroy the list */
                        ecore_list_destroy(pdf->search.list);
                        pdf->search.list = NULL;
                        /* we search a new one */
                printf("page0 : %d\n", pdf->search.page);
                        goto next_page;
                }
        }
        evas_object_hide(pdf->search.o);

        if (pdf->search.is_circular) {
                pdf->search.page = -1;
                DRETURN_INT(TRUE, DLEVEL_STABLE);
        }
        else
                DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param pdf the pdf widget to get the document of
 * @return Returns the document of the pdf (NULL on failure)
 * @brief get the document of the pdf
 */
Epdf_Document *
ewl_pdf_pdf_document_get(Ewl_Pdf *pdf)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, NULL);
        DCHECK_TYPE_RET(pdf, EWL_PDF_TYPE, NULL);

        DRETURN_PTR(pdf->pdf_document, DLEVEL_STABLE);
}

/**
 * @param pdf the pdf widget to get the current page of
 * @return Returns the current page of the pdf (NULL on failure)
 * @brief get the current page of the pdf
 */
Epdf_Page *
ewl_pdf_pdf_page_get(Ewl_Pdf *pdf)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, NULL);
        DCHECK_TYPE_RET(pdf, EWL_PDF_TYPE, NULL);

        DRETURN_PTR(pdf->pdf_page, DLEVEL_STABLE);
}

/**
 * @param pdf the pdf widget to get the index of
 * @return Returns the poppler index of the pdf (NULL on failure)
 * @brief get the poppler index of the pdf
 */
Ecore_List *
ewl_pdf_pdf_index_get(Ewl_Pdf *pdf)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pdf, NULL);
        DCHECK_TYPE_RET(pdf, EWL_PDF_TYPE, NULL);

        DRETURN_PTR(pdf->pdf_index, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_pdf_configure_cb(Ewl_Widget *w,
                     void       *ev_data __UNUSED__,
                     void       *user_data __UNUSED__)
{
        Ewl_Pdf   *pdf;
        Ewl_Embed *emb;
        double     hscale;
        double     vscale;
        int        ww;
        int        hh;
        int        ow;
        int        oh;
        int dx = 0, dy = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        pdf = EWL_PDF(w);
        if (!pdf->image || !pdf->pdf_page)
                DRETURN(DLEVEL_STABLE);

        emb = ewl_embed_widget_find(w);

        epdf_page_size_get(pdf->pdf_page, &ow, &oh);
        epdf_page_scale_get(pdf->pdf_page, &hscale, &vscale);
        ow = (int)(ow * hscale);
        oh = (int)(oh * vscale);

        ww = CURRENT_W(w);
        hh = CURRENT_H(w);
        if (ww > ow)
                ww = ow;
        if (hh > oh)
                hh = oh;

        dx = (CURRENT_W(w) - ww) / 2;
        dy = (CURRENT_H(w) - hh) / 2;

        evas_object_image_fill_set(pdf->image, 0, 0, ow, oh);

        evas_object_move(pdf->image, CURRENT_X(w), CURRENT_Y(w));
        evas_object_resize(pdf->image, ow, oh);
        if (pdf->dirty) {
                epdf_page_render(pdf->pdf_page, pdf->image);
                pdf->dirty = 0;
        }

        ewl_object_preferred_inner_w_set(EWL_OBJECT(w), ow);
        ewl_object_preferred_inner_h_set(EWL_OBJECT(w), oh);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The reveal callback
 */
void
ewl_pdf_reveal_cb(Ewl_Widget *w,
                  void       *ev_data __UNUSED__,
                  void       *user_data __UNUSED__)
{
        Ewl_Pdf   *pdf;
        Ewl_Embed *emb;
        double     hscale;
        double     vscale;
        int        ow;
        int        oh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        pdf = EWL_PDF(w);
        emb = ewl_embed_widget_find(w);
        if (!emb)
                DRETURN(DLEVEL_STABLE);

        if (!pdf->image)
          pdf->image = ewl_embed_object_request(emb, "pdf");
        if (!pdf->image)
          pdf->image = evas_object_image_add(emb->canvas);
        if (!pdf->image)
          DRETURN(DLEVEL_STABLE);

        evas_object_smart_member_add(pdf->image, w->smart_object);
        if (w->fx_clip_box)
                evas_object_stack_below(pdf->image, w->fx_clip_box);

        if (w->fx_clip_box)
                evas_object_clip_set(pdf->image, w->fx_clip_box);

        evas_object_pass_events_set(pdf->image, TRUE);
        evas_object_show(pdf->image);

        epdf_page_size_get(pdf->pdf_page, &ow, &oh);
        epdf_page_scale_get(pdf->pdf_page, &hscale, &vscale);
        ow = (int)(ow * hscale);
        oh = (int)(oh * vscale);

        ewl_object_preferred_inner_w_set(EWL_OBJECT(pdf), ow);
        ewl_object_preferred_inner_h_set(EWL_OBJECT(pdf), oh);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The obscure callback
 */
void
ewl_pdf_obscure_cb(Ewl_Widget *w,
                   void       *ev_data __UNUSED__,
                   void       *user_data __UNUSED__)
{
        Ewl_Pdf *pdf;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);

        pdf = EWL_PDF(w);
        if (emb && pdf->image) {
                ewl_embed_object_cache(emb, pdf->image);
                pdf->image = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_pdf_destroy_cb(Ewl_Widget *w,
                   void       *ev_data __UNUSED__,
                   void       *user_data __UNUSED__)
{
        Ewl_Pdf *pdf;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, "widget");

        pdf = EWL_PDF(w);

        IF_FREE(pdf->filename);
        if (pdf->pdf_document)
          epdf_document_delete(pdf->pdf_document);
        if (pdf->pdf_page)
          epdf_page_delete(pdf->pdf_page);
        if (pdf->pdf_index)
          epdf_index_delete(pdf->pdf_index);
        if (pdf->search.text) free(pdf->search.text);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @}
 */
