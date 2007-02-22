#include <math.h>

#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_dvi.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif


#define round(a) ( ((a)<0.0) ? (int)(floor((a) - 0.5)) : (int)(floor((a) + 0.5)) )

/**
 * @return Returns a pointer to a new dvi widget on success, NULL on failure.
 * @brief Load an dvi widget with specified dvi contents
 *
 * The @a k parameter is primarily used for loading edje groups or keyed data
 * in an dvi.
 */
Ewl_Widget *
ewl_dvi_new(void)
{
	Ewl_Dvi *dvi;

	DENTER_FUNCTION(DLEVEL_STABLE);

	dvi = NEW(Ewl_Dvi, 1);
	if (!dvi)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_dvi_init(dvi)) {
		ewl_widget_destroy(EWL_WIDGET(dvi));
		dvi = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(dvi), DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to initialize
 * @return Returns no value.
 * @brief Initialize an dvi widget to default values and callbacks
 *
 * Sets the fields and callbacks of @a dvi to their default
 * values. The property EDVI_PROPERTY_DELAYED_FONT_OPEN is set by
 * default.
 */
int
ewl_dvi_init(Ewl_Dvi *dvi)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("dvi", dvi, FALSE);

	w = EWL_WIDGET(dvi);

	if (!ewl_widget_init(w))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_DVI_TYPE);
	ewl_widget_inherit(w, EWL_DVI_TYPE);

	/*
	 * Append necessary callbacks.
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_dvi_configure_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_dvi_reveal_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_dvi_obscure_cb,
			    NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_dvi_destroy_cb,
			    NULL);

	dvi->filename = NULL;
	dvi->page = 0;
	dvi->page_length = 10;

	dvi->dvi_device = edvi_device_new (edvi_dpi_get(), edvi_dpi_get());
	dvi->dvi_property = edvi_property_new();
	edvi_property_property_set (dvi->dvi_property, EDVI_PROPERTY_DELAYED_FONT_OPEN);
	dvi->dvi_document = NULL;
	dvi->dvi_page = NULL;

	dvi->orientation = EDVI_PAGE_ORIENTATION_PORTRAIT;
	dvi->hscale = 1.0;
	dvi->vscale = 1.0;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the page of
 * @return Returns the currently set page
 * @brief get the page this dvi uses
 */
int
ewl_dvi_page_get(Ewl_Dvi *dvi)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("dvi", dvi, 0);
	DCHECK_TYPE_RET("dvi", dvi, EWL_DVI_TYPE, 0);

	DRETURN_INT(dvi->page, DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to change the displayed dvi
 * @param filename: the path to the new dvi to be displayed by @a dvi
 * @return Returns no value.
 * @brief Change the dvi file displayed by an dvi widget
 *
 * Set the dvi displayed by @a dvi to the one found at the path @a filename. If an
 * edje is used, a minimum size should be specified in the edje or the code.
 */
void
ewl_dvi_file_set(Ewl_Dvi *dvi, const char *filename)
{
	Ewl_Widget *w;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	w = EWL_WIDGET(dvi);
	emb = ewl_embed_widget_find(w);

	if (dvi->filename != filename) {
		IF_FREE(dvi->filename);
	}
	if (filename) {
		dvi->filename = strdup(filename);
	}

	if (dvi->dvi_document) {
		if (dvi->dvi_page)
			edvi_page_delete (dvi->dvi_page);
		edvi_document_delete (dvi->dvi_document);
	}

	dvi->dvi_document = edvi_document_new (filename, dvi->dvi_device, dvi->dvi_property);
	dvi->page = 0;

	/*
	 * Load the new dvi if widget has been realized
	 */
	if (REALIZED(w)) {
		ewl_widget_unrealize(w);
		ewl_widget_realize(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi to change constrain setting
 * @param size: the minimum constrain size
 * @return Returns no value.
 * @brief Set a size which, if the dvi is bigger than, scale proportionally
 *
 * Sets a size to scale to proportionally if the dvi exceeds this size
 */
void ewl_dvi_page_set(Ewl_Dvi *dvi, int page)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi->dvi_document ||
	    (page >= edvi_document_page_count_get (dvi->dvi_document)) ||
	    (page == dvi->page))
                DLEAVE_FUNCTION(DLEVEL_STABLE);

	dvi->page = page;
	ewl_callback_call (EWL_WIDGET (dvi), EWL_CALLBACK_REVEAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the document of
 * @return Returns the document of the dvi (NULL on failure)
 * @brief get the document of the dvi
 */
Edvi_Document *ewl_dvi_dvi_document_get (Ewl_Dvi *dvi)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("dvi", dvi, NULL);
	DCHECK_TYPE_RET("dvi", dvi, EWL_DVI_TYPE, NULL);

	DRETURN_PTR(dvi->dvi_document, DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the current page of
 * @return Returns the current page of the dvi (NULL on failure)
 * @brief get the current page of the dvi
 */
Edvi_Page *ewl_dvi_dvi_page_get (Ewl_Dvi *dvi)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("dvi", dvi, NULL);
	DCHECK_TYPE_RET("dvi", dvi, EWL_DVI_TYPE, NULL);

	DRETURN_PTR(dvi->dvi_page, DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the size of
 * @param width width of the current page
 * @param height height of the current page
 * @brief get the size of the dvi @p dvi. If @p dvi is NULL,
 * return a width equal to 0 and a height equal to 0
 */
void ewl_dvi_dvi_size_get (Ewl_Dvi *dvi, int *width, int *height)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi) {
		if (width) *width = 0;
		if (height) *height = 0;
	}
	else {
		if (width) *width = edvi_page_width_get (dvi->dvi_page);
		if (height) *height = edvi_page_height_get (dvi->dvi_page);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_orientation_set (Ewl_Dvi *dvi, Edvi_Page_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi || !dvi->dvi_page || (dvi->orientation == o))
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	dvi->orientation = o;
	ewl_callback_call (EWL_WIDGET (dvi), EWL_CALLBACK_REVEAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Edvi_Page_Orientation
ewl_dvi_orientation_get (Ewl_Dvi *dvi)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("dvi", dvi, EDVI_PAGE_ORIENTATION_PORTRAIT);
	DCHECK_TYPE_RET("dvi", dvi, EWL_DVI_TYPE, EDVI_PAGE_ORIENTATION_PORTRAIT);

	if (!dvi || !dvi->dvi_page)
		DRETURN_INT(EDVI_PAGE_ORIENTATION_PORTRAIT, DLEVEL_STABLE);

	DRETURN_INT(edvi_page_orientation_get (dvi->dvi_page), DLEVEL_STABLE);
}

void
ewl_dvi_scale_set (Ewl_Dvi *dvi, double hscale, double vscale)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	if (hscale != dvi->hscale) dvi->hscale = hscale;
	if (vscale != dvi->vscale) dvi->vscale = vscale;
	ewl_callback_call (EWL_WIDGET (dvi), EWL_CALLBACK_REVEAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_scale_get (Ewl_Dvi *dvi, double *hscale, double *vscale)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi) {
		if (hscale) *hscale = 1.0;
		if (vscale) *vscale = 1.0;
	}
	else {
		if (hscale) *hscale = dvi->hscale;
		if (vscale) *vscale = dvi->vscale;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_page_next (Ewl_Dvi *dvi)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = dvi->page;
	if (page < (edvi_document_page_count_get(dvi->dvi_document) - 1))
		page++;
	ewl_dvi_page_set (dvi, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_page_previous (Ewl_Dvi *dvi)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = dvi->page;
	if (page > 0)
		page--;
	ewl_dvi_page_set (dvi, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_page_page_length_set (Ewl_Dvi *dvi, int page_length)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi || (page_length <= 0) || (dvi->page_length == page_length))
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	dvi->page_length = page_length;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_dvi_page_page_length_get (Ewl_Dvi *dvi)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("dvi", dvi, 0);
	DCHECK_TYPE_RET("dvi", dvi, EWL_DVI_TYPE, 0);

	if (!dvi)
		DRETURN_INT(0, DLEVEL_STABLE);

	DRETURN_INT(dvi->page_length, DLEVEL_STABLE);
}

void
ewl_dvi_page_page_next (Ewl_Dvi *dvi)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = dvi->page + dvi->page_length;
	if (page >= edvi_document_page_count_get(dvi->dvi_document))
		page = edvi_document_page_count_get(dvi->dvi_document) - 1;
	ewl_dvi_page_set (dvi, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_page_page_previous (Ewl_Dvi *dvi)
{
	int page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dvi", dvi);
	DCHECK_TYPE("dvi", dvi, EWL_DVI_TYPE);

	if (!dvi)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	page = dvi->page - dvi->page_length;
	if (page < 0)
		page = 0;
	ewl_dvi_page_set (dvi, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_reveal_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		  void *user_data __UNUSED__)
{
	Ewl_Dvi   *dvi;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	dvi = EWL_DVI(w);
	emb = ewl_embed_widget_find(w);

	/*
	 * Load the dvi
	 */
	if (!dvi->image)
	  dvi->image = ewl_embed_object_request(emb, "dvi");
	if (!dvi->image)
	  dvi->image = evas_object_image_add(emb->canvas);
	if (!dvi->image)
	  DRETURN(DLEVEL_STABLE);

	if (dvi->dvi_document) {
		unsigned int *m;
		int w;
		int h;

		if (dvi->dvi_page)
			edvi_page_delete (dvi->dvi_page);
		dvi->dvi_page = edvi_page_new (dvi->dvi_document, dvi->page);

		w = edvi_page_width_get (dvi->dvi_page);
		h = edvi_page_height_get (dvi->dvi_page);
		evas_object_image_size_set (dvi->image, w, h);
		evas_object_image_fill_set (dvi->image, 0, 0, w, h);
		m = (unsigned int *)evas_object_image_data_get (dvi->image, 1);
                if (!m)
			DRETURN(DLEVEL_STABLE);

		memset(m, (255 << 24) | (255 << 16) | (255 << 8) | 255, w * h * 4);
		evas_object_image_data_update_add (dvi->image, 0, 0, w, h);
		evas_object_resize (dvi->image, w, h);

		edvi_page_render (dvi->dvi_page, dvi->dvi_device, dvi->image);
	}
	evas_object_image_size_get(dvi->image, &dvi->ow, &dvi->oh);

	evas_object_smart_member_add(dvi->image, w->smart_object);
	if (w->fx_clip_box)
		evas_object_stack_below(dvi->image, w->fx_clip_box);

	if (w->fx_clip_box)
		evas_object_clip_set(dvi->image, w->fx_clip_box);

	evas_object_pass_events_set(dvi->image, TRUE);
	evas_object_show(dvi->image);

	if (!dvi->ow)
		dvi->ow = 1;
	if (!dvi->oh)
		dvi->oh = 1;

	ewl_object_preferred_inner_w_set(EWL_OBJECT(w), dvi->ow);
	ewl_object_preferred_inner_h_set(EWL_OBJECT(w), dvi->oh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_obscure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Dvi *dvi;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);

	dvi = EWL_DVI(w);
	if (emb && dvi->image) {
		ewl_embed_object_cache(emb, dvi->image);
		dvi->image = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Dvi *dvi;
	Ewl_Embed *emb;
	int ww, hh;
	int dx = 0, dy = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	dvi = EWL_DVI(w);
	if (!dvi->image)
		DRETURN(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);

	ww = CURRENT_W(w);
	hh = CURRENT_H(w);
	if (ww > dvi->ow)
		ww = dvi->ow;
	if (hh > dvi->oh)
		hh = dvi->oh;

	dx = (CURRENT_W(w) - ww) / 2;
	dy = (CURRENT_H(w) - hh) / 2;

	evas_object_image_fill_set(dvi->image, 0, 0,
				ww, hh);

	evas_object_move(dvi->image, CURRENT_X(w) + dx, CURRENT_Y(w) + dy);
	evas_object_resize(dvi->image, ww, hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_dvi_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		   void *user_data __UNUSED__)
{
	Ewl_Dvi *dvi;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	dvi = EWL_DVI(w);

	if (dvi->dvi_document)
		edvi_document_delete (dvi->dvi_document);
	if (dvi->dvi_page)
		edvi_page_delete (dvi->dvi_page);
	if (dvi->dvi_property)
		edvi_property_delete (dvi->dvi_property);
	if (dvi->dvi_device)
		edvi_device_delete (dvi->dvi_device);
	IF_FREE(dvi->filename);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
