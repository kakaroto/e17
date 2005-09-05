#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_spectrum_color_coord_map2d(Ewl_Spectrum *sp, int x, int y, 
					   int *r, int *g, int *b, int *a);
static void ewl_spectrum_color_coord_map1d(Ewl_Spectrum *sp, int x, int y, 
					   int *r, int *g, int *b, int *a);

/**
 * ewl_spectrum_new - create a new color picker widget
 *
 * Returns a newly allocated color picker widget on success, NULL on failure.
 */
Ewl_Widget     *ewl_spectrum_new()
{
	Ewl_Spectrum *sp = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	sp = NEW(Ewl_Spectrum, 1);
	if (!sp)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_spectrum_init(sp);

	DRETURN_PTR(EWL_WIDGET(sp), DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to init
 * @return Returns no value.
 * @brief Initializes the given spectrum widget
 */
void ewl_spectrum_init(Ewl_Spectrum * sp)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	w = EWL_WIDGET(sp);

	ewl_image_init(EWL_IMAGE(w), NULL, NULL);
	ewl_widget_appearance_set(w, "spectrum");
	ewl_widget_inherit(w, "spectrum");

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_spectrum_configure_cb, NULL);

	sp->orientation = EWL_ORIENTATION_VERTICAL;
	sp->mode = EWL_PICK_MODE_HSV_HUE;
	sp->dimensions = 2;
	sp->redraw = 1;
	sp->r = 255;
	sp->g = 255;
	sp->b = 255;
	sp->h = 0.0;
	sp->s = 0.0;
	sp->v = 1.0;

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the orientation on 
 * @param o: The orientation to set
 * @return Returns no value.
 * @brief Sets the orientation on the spectrum to the given value either
 * EWL_ORIENTATION_HORIZONTAL or EWL_ORIENTATION_VERTICAL.
 */
void ewl_spectrum_orientation_set(Ewl_Spectrum *sp, int o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (sp->orientation != o) {
		sp->orientation = o;
		if (sp->dimensions == 1) {
			sp->redraw = 1;
			ewl_widget_configure(EWL_WIDGET(sp));
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the mode on
 * @param mode: The mode to set
 * @return Returns no value.
 * @brief Sets to the mode of the spectrum to the given value.
 */
void ewl_spectrum_mode_set(Ewl_Spectrum * sp, Ewl_Pick_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (sp->mode != mode) {
		sp->mode = mode;
		sp->redraw = 1;
		ewl_widget_configure(EWL_WIDGET(sp));
	}

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the dimension on
 * @param dimensions: The number of dimensions, 1 or 2
 * @return Returns no value.
 * @brief Sets the number of dimensions the spectrum has.
 */
void
ewl_spectrum_dimensions_set(Ewl_Spectrum * sp,
					int dimensions)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (dimensions == 1 || dimensions == 2) {
		sp->dimensions = dimensions;
		sp->redraw = 1;
		ewl_widget_configure(EWL_WIDGET(sp));
	}

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the r, g, b values of
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set 
 * @return Returns no value.
 */ 
void
ewl_spectrum_rgb_set(Ewl_Spectrum * sp, int r, int g,
				 int b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (r >= 0 && r < 256)
		sp->r = r;
	if (g >= 0 && g < 256)
		sp->g = g;
	if (b >= 0 && b < 256)
		sp->b = b;

	sp->redraw = 1;
	ewl_widget_configure(EWL_WIDGET(sp));

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to work on
 * @param h: The hue value to set
 * @param s: The saturation value to set
 * @param v: The ??? value to set
 * @return Returns no value
 */
void
ewl_spectrum_hsv_set(Ewl_Spectrum * sp, float h,
				 float s, float v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (h >= 0 && h <= 360)
		sp->h = h;
	if (s >= 0 && s <= 1)
		sp->s = s;
	if (v >= 0 && v <= 1)
		sp->v = v;

	sp->redraw = 1;

	ewl_widget_configure(EWL_WIDGET(sp));

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to get the colour from
 * @param x: The x coord to get the color from
 * @param y: The y coord to get the color from
 * @param r: Where to store the red value
 * @param g: Where to store the green value
 * @param b: Where to store the blue value
 * @param a: Where to store the alpha value
 * @return The returned parametesr are done through the r, g, b, a
 * parameters
 */
void
ewl_spectrum_color_coord_map(Ewl_Spectrum *sp, int x, int y, int *r, int *g, int *b, int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (sp->dimensions == 2)
		ewl_spectrum_color_coord_map2d(sp, x, y, r, g, b, a);
	else
		ewl_spectrum_color_coord_map1d(sp, x, y, r, g, b, a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Callback for drawing the spectrum to the image data.
 */
void
ewl_spectrum_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			  void *user_data __UNUSED__)
{
	Evas_Object    *o;
	int             pw, ph;
	int             i, j;
	int            *data = NULL;
	Ewl_Spectrum   *sp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	sp = EWL_SPECTRUM(w);
	if (!sp->redraw)
		DRETURN(DLEVEL_STABLE);

	o = EWL_IMAGE(sp)->image;
	if (!o)
		return;

	/* set/get the spectrum size and image data */
	evas_object_image_size_set(o, CURRENT_W(sp), CURRENT_H(sp));
	evas_object_image_size_get(o, &pw, &ph);
	data = evas_object_image_data_get(o, 1);
	if (!data)
		return;

	/* draw the spectrum */
	for (j = 0; j < ph; j++) {
		for (i = 0; i < pw; i++) {
			int r, g, b, a;
			ewl_spectrum_color_coord_map(sp, i, j, &r, &g, &b, &a);
			data[(j * pw) + i] =
			    (a << 24) | (r << 16) | (g << 8) | b;
		}
	}

	evas_object_image_data_set(o, data);
	evas_object_image_data_update_add(o, 0, 0, pw, ph);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spectrum_hsv_to_rgb(float hue, float saturation, float value,
			int *_r, int *_g, int *_b)
{
	int             i, p, q, t, h;
	float           vs, vsf;
	int             r, g, b;

	i = (int) (value * 255.0);
	if (saturation == 0)
		r = g = b = i;
	else {
		if (hue == 360)
			hue = 0;
		hue = hue / 60.0;
		h = (int) hue;
		vs = value * saturation;
		vsf = vs * (hue - h);
		p = (int) (255.0 * (value - vs));
		q = (int) (255.0 * (value - vsf));
		t = (int) (255.0 * (value - vs + vsf));
		switch (h) {
		case 0:
			r = i;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = i;
			b = p;
			break;
		case 2:
			r = p;
			g = i;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = i;
			break;
		case 4:
			r = t;
			g = p;
			b = i;
			break;
		case 5:
		default:
			r = i;
			g = p;
			b = q;
			break;
		}
	}

	if (_r)
		*_r = r;
	if (_g)
		*_g = g;
	if (_b)
		*_b = b;
}

void
ewl_spectrum_rgb_to_hsv(int r, int g, int b,
			float *h, float *s, float *v)
{
        int min, max, delta;

        min = MIN(r,MIN(g,b));
        max = MAX(r,MAX(g,b));
        *v = (float)max / 255.0;                               // v

        delta = max - min;

        if( max != 0 )
                *s = (float)delta / (float)max;               // s
        else {
                *s = 0;
                *h = 0;
                return;
        }

        if(delta == 0)
                *h = 0;
        else if( r == max )
                *h = (float)( g - b ) / (float)delta;         // between yellow & magenta
        else if( g == max )
                *h = 2.0 + (float)( b - r ) / (float)delta;     // between cyan & yellow
        else
                *h = 4.0 + (float)( r - g ) / (float)delta;     // between magenta & cyan

        *h *= 60;                               // degrees
        if( *h < 0 )
                *h += 360;
}

static void
ewl_spectrum_color_coord_map2d(Ewl_Spectrum *sp, int x, int y, 
				int *r, int *g, int *b, int *a)
{
	int width, height;

	width = CURRENT_W(sp);
	height = CURRENT_H(sp);

	if (a)
		*a = 255;

	if (sp->mode == EWL_PICK_MODE_RGB) {
		int red, green, blue;

		red = sp->r;
		green = sp->g;
		blue = sp->b;

		red = red + (y) * (255 - red) / height;
		red = red * x / width;
		green = green + (y) * (255 - green) / height;
		green = green * x / width;
		blue = blue + (y) * (255 - blue) / height;
		blue = blue * x / width;

		if (r)
			*r = red;
		if (g)
			*g = green;
		if (b)
			*b = blue;
	} else {
		float h, s, v;

		h = sp->h;
		s = sp->s;
		v = sp->v;

		if (sp->mode == EWL_PICK_MODE_HSV_HUE) {
			s = 1 - (float) x / (float) width;
			v = 1 - (float) y / (float) height;
		} else if (sp->mode == EWL_PICK_MODE_HSV_SATURATION) {
			h = (float) x / (float) width *360;
			v = 1 - (float) y / (float) height;
		} else if (sp->mode == EWL_PICK_MODE_HSV_VALUE) {

			h = (float) x / (float) width *360;
			s = 1 - (float) y / (float) height;
		}

                ewl_spectrum_hsv_to_rgb(h, s, v, r, g, b);
	}
}

static void
ewl_spectrum_color_coord_map1d(Ewl_Spectrum *sp, int x __UNUSED__, int y, 
				int *r, int *g, int *b, int *a)
{
	int height;

	height = CURRENT_H(sp);

	/* draw the spectrum */
	if (a)
		*a = 255;

	if (sp->mode == EWL_PICK_MODE_RGB) {
		if (r)
			*r = sp->r * (1 - (float) y / (float) height);
		if (g)
			*g = sp->g * (1 - (float) y / (float) height);
		if (b)
			*b = sp->b * (1 - (float) y / (float) height);
	} else {
		int h, s, v;

		h = sp->h;
		s = sp->s;
		v = sp->v;
		if (sp->mode == EWL_PICK_MODE_HSV_HUE) {
			h = 360 * (float) y / (float) height;
			s = 1.0;
			v = 1.0;
		} else if (sp->mode == EWL_PICK_MODE_HSV_SATURATION) {
			s = 1 - (float) y / (float) height;
		} else if (sp->mode == EWL_PICK_MODE_HSV_VALUE) {
			v = 1 - (float) y / (float) height;
		}

		ewl_spectrum_hsv_to_rgb(h, s, v, r, g, b);
	}
}
