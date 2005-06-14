#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/**
 * ewl_coloredrect_new - create a new color picker widget
 *
 * Returns a newly allocated color picker widget on success, NULL on failure.
 */
Ewl_Widget     *ewl_coloredrect_new(int r, int g, int b, int a)
{
	Ewl_ColoredRect *cr = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	cr = NEW(Ewl_ColoredRect, 1);
	if (!cr)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_coloredrect_init(cr, r, g, b, a);

	DRETURN_PTR(EWL_WIDGET(cr), DLEVEL_STABLE);
}

/**
 * @param cr: The coloredrect to init
 * @return Returns no value.
 * @brief Initializes the given coloredrect widget
 */
void ewl_coloredrect_init(Ewl_ColoredRect * cr, int r, int g, int b, int a)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cr", cr);

	w = EWL_WIDGET(cr);

        ewl_coloredrect_color_set(EWL_COLOREDRECT(w), r, g, b, a);

	ewl_image_init(EWL_IMAGE(w), NULL, NULL);
	ewl_widget_appearance_set(w, "coloredrect");
	ewl_widget_inherit(w, "coloredrect");

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_coloredrect_configure_cb, NULL);

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param cr: The coloredrect to set the r, g, b values of
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set 
 * @return Returns no value.
 */ 
void
ewl_coloredrect_color_set(Ewl_ColoredRect * cr, int r, int g, int b, int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cr", cr);

	if (r >= 0 && r < 256)
		cr->r = r;
	if (g >= 0 && g < 256)
		cr->g = g;
	if (b >= 0 && b < 256)
		cr->b = b;

	cr->redraw = 1;
	ewl_widget_configure(EWL_WIDGET(cr));

	DRETURN(DLEVEL_STABLE);
}

/*
 * Callback for drawing the coloredrect to the image data.
 */
void
ewl_coloredrect_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			  void *user_data __UNUSED__)
{
	Evas_Object    *o;
	int             pw, ph;
	int             i, j;
	int            *data = NULL;
	Ewl_ColoredRect   *cr;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	cr = EWL_COLOREDRECT(w);
	if (!cr->redraw)
		DRETURN(DLEVEL_STABLE);

	o = EWL_IMAGE(cr)->image;
	if (!o)
		return;

	/* set/get the coloredrect size and image data */
	evas_object_image_size_set(o, CURRENT_W(cr), CURRENT_H(cr));
	evas_object_image_size_get(o, &pw, &ph);
	data = evas_object_image_data_get(o, 1);
	if (!data)
		return;

	/* draw the coloredrect */
	for (j = 0; j < ph; j++) {
		for (i = 0; i < pw; i++) {
			data[(j * pw) + i] =
			    (cr->a << 24) | (cr->r << 16) | (cr->g << 8) | cr->b;
		}
	}

	evas_object_image_data_set(o, data);
	evas_object_image_data_update_add(o, 0, 0, pw, ph);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

