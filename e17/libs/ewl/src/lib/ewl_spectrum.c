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
Ewl_Widget *
ewl_spectrum_new(void)
{
	Ewl_Spectrum *sp = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sp = NEW(Ewl_Spectrum, 1);
	if (!sp)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_spectrum_init(sp))
	{
		ewl_widget_destroy(EWL_WIDGET(sp));
		sp = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(sp), DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to init
 * @return Returns no value.
 * @brief Initializes the given spectrum widget
 */
int
ewl_spectrum_init(Ewl_Spectrum *sp)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("sp", sp, FALSE);

	w = EWL_WIDGET(sp);

	ewl_image_init(EWL_IMAGE(w));
	ewl_widget_appearance_set(w, "spectrum");
	ewl_widget_inherit(w, "spectrum");

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_spectrum_configure_cb, NULL);

	sp->orientation = EWL_ORIENTATION_VERTICAL;
	sp->mode = EWL_COLOR_PICK_MODE_HSV_HUE;
	sp->dimensions = 2;
	sp->redraw = 1;

	sp->rgba.r = 255;
	sp->rgba.g = 255;
	sp->rgba.b = 255;
	sp->rgba.a = 255;

	sp->hsv.h = 0.0;
	sp->hsv.s = 0.0;
	sp->hsv.v = 1.0;

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, 
					ewl_spectrum_mouse_down_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, 
					ewl_spectrum_mouse_up_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, 
					ewl_spectrum_mouse_move_cb, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the orientation on 
 * @param o: The orientation to set
 * @return Returns no value.
 * @brief Sets the orientation on the spectrum to the given value either
 * EWL_ORIENTATION_HORIZONTAL or EWL_ORIENTATION_VERTICAL.
 */
void
ewl_spectrum_orientation_set(Ewl_Spectrum *sp, Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (sp->orientation != o) 
	{
		sp->orientation = o;
		if (sp->dimensions == 1) 
		{
			sp->redraw = 1;
			ewl_widget_configure(EWL_WIDGET(sp));
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to get the orientation from
 * @return Returns the orientation of the spectrum
 */
Ewl_Orientation
ewl_spectrum_orientation_get(Ewl_Spectrum *sp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("sp", sp, EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(sp->orientation, DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the mode on
 * @param mode: The mode to set
 * @return Returns no value.
 * @brief Sets to the mode of the spectrum to the given value.
 */
void
ewl_spectrum_mode_set(Ewl_Spectrum *sp, Ewl_Color_Pick_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (sp->mode != mode) 
	{
		sp->mode = mode;
		sp->redraw = 1;
		ewl_widget_configure(EWL_WIDGET(sp));
	}

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to get the pick mode from
 * @return Returns the Ewl_Color_Pick_Mode set on the spectrum
 */
Ewl_Color_Pick_Mode
ewl_spectrum_mode_get(Ewl_Spectrum *sp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("sp", sp, EWL_COLOR_PICK_MODE_RGB);

	DRETURN_INT(sp->mode, DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the dimension on
 * @param dimensions: The number of dimensions, 1 or 2
 * @return Returns no value.
 * @brief Sets the number of dimensions the spectrum has.
 */
void
ewl_spectrum_dimensions_set(Ewl_Spectrum *sp, unsigned int dimensions)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if ((dimensions == 1) || (dimensions == 2)) 
	{
		sp->dimensions = dimensions;
		sp->redraw = 1;
		ewl_widget_configure(EWL_WIDGET(sp));
	}

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to get the dimension from
 * @return Returns the dimension of the spectrum
 */
unsigned int
ewl_spectrum_dimensions_get(Ewl_Spectrum *sp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("sp", sp, 1);

	DRETURN_INT(sp->dimensions, DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to set the r, g, b values of
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set 
 * @return Returns no value.
 */ 
void
ewl_spectrum_rgba_set(Ewl_Spectrum *sp, int r, int g, int b, int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if ((r >= 0) && (r < 256)) sp->rgba.r = r;
	else sp->rgba.r = 0;

	if ((g >= 0) && (g < 256)) sp->rgba.g = g;
	else sp->rgba.g = 0;

	if ((b >= 0) && (b < 256)) sp->rgba.b = b;
	else sp->rgba.b = 0;

	if ((a >= 0) && (a < 256)) sp->rgba.a = a;
	else sp->rgba.a = 0;

	sp->redraw = 1;

	ewl_spectrum_rgb_to_hsv(r, g, b, &(sp->hsv.h), 
				&(sp->hsv.s), &(sp->hsv.v));

	ewl_widget_configure(EWL_WIDGET(sp));

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to get the values from
 * @param r: Where to store the red value
 * @param g: Where to store the green value
 * @param b: Where to store the blue value
 * @param a: Where to store the alpha value
 * @return Returns no value.
 */
void
ewl_spectrum_rgba_get(Ewl_Spectrum *sp, int *r, int *g, int *b, int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (r) *r = sp->rgba.r;
	if (g) *g = sp->rgba.g;
	if (b) *b = sp->rgba.b;
	if (a) *a = sp->rgba.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to work on
 * @param h: The hue value to set
 * @param s: The saturation value to set
 * @param v: The value value to set
 * @return Returns no value
 */
void
ewl_spectrum_hsv_set(Ewl_Spectrum *sp, float h, float s, float v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if ((h >= 0) && (h <= 360)) sp->hsv.h = h;
	else sp->hsv.h = 0;

	if ((s >= 0) && (s <= 1)) sp->hsv.s = s;
	else sp->hsv.s = 0;

	if ((v >= 0) && (v <= 1)) sp->hsv.v = v;
	else sp->hsv.v = 0;

	sp->redraw = 1;

	ewl_spectrum_hsv_to_rgb(h, s, v, &(sp->rgba.r), 
				&(sp->rgba.g), &(sp->rgba.b));

	ewl_widget_configure(EWL_WIDGET(sp));

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param sp: The spectrum to get the values from
 * @param h: Where to store the hue
 * @param s: Where to store the saturation
 * @param v: Where to store the value
 * @return Returns no value.
 */
void
ewl_spectrum_hsv_get(Ewl_Spectrum *sp, float *h, float *s, float *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (h) *h = sp->hsv.h;
	if (s) *s = sp->hsv.s;
	if (v) *v = sp->hsv.v;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spectrum_selected_rgba_get(Ewl_Spectrum *sp, int *r, int *g, int *b, int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (r) *r = sp->selected_rgba.r;
	if (g) *g = sp->selected_rgba.g;
	if (b) *b = sp->selected_rgba.b;
	if (a) *a = sp->selected_rgba.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

/**
 * @param hue: The hue to convert
 * @param Saturation: The saturation to convert
 * @param value: The value to convert
 * @param _r: Where to store the red value
 * @param _g: Where to store the green value
 * @param _b: Where to store the red value
 * @return Returns no value.
 */
void
ewl_spectrum_hsv_to_rgb(float hue, float saturation, float value,
			int *_r, int *_g, int *_b)
{
	int             i, p, q, t, h;
	float           vs, vsf;
	int             r, g, b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	i = (int) (value * 255.0);
	if (saturation == 0)
		r = g = b = i;
	else 
	{
		if (hue == 360) hue = 0;

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

	if (_r) *_r = r;
	if (_g) *_g = g;
	if (_b) *_b = b;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param r: The red value to convert
 * @param g: The green value to convert
 * @param b: The blue value to convert
 * @param h: Where to store the hue
 * @param s: Where to store the saturation
 * @param v: Where to store the value
 * @return Returns no value
 */
void
ewl_spectrum_rgb_to_hsv(int r, int g, int b,
			float *h, float *s, float *v)
{
        int min, max, delta;

	DENTER_FUNCTION(DLEVEL_STABLE);

        min = MIN(r,MIN(g,b));
        max = MAX(r,MAX(g,b));
        *v = (float)max / 255.0;                               // v

        delta = max - min;

        if (max != 0) *s = (float)delta / (float)max;               // s
        else 
	{
                *s = 0;
                *h = 0;
                return;
        }

        if (delta == 0) *h = 0;
        else if (r == max)
                *h = (float)(g - b) / (float)delta;         // between yellow & magenta
        else if (g == max)
                *h = 2.0 + (float)(b - r) / (float)delta;     // between cyan & yellow
        else
                *h = 4.0 + (float)(r - g) / (float)delta;     // between magenta & cyan

        *h *= 60;                               // degrees
        if (*h < 0) *h += 360;

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
	DCHECK_PARAM_PTR("w", w);

	if (!REALIZED(w))
	{
		DRETURN(DLEVEL_STABLE);
	}

	sp = EWL_SPECTRUM(w);
	if (!sp->redraw)
	{
		DRETURN(DLEVEL_STABLE);
	}

	o = EWL_IMAGE(sp)->image;
	if (!o)
	{
		DRETURN(DLEVEL_STABLE);
	}

	/* set/get the spectrum size and image data */
	evas_object_image_size_set(o, CURRENT_W(sp), CURRENT_H(sp));
	evas_object_image_size_get(o, &pw, &ph);
	data = evas_object_image_data_get(o, 1);
	if (!data)
	{
		DRETURN(DLEVEL_STABLE);
	}

	/* draw the spectrum */
	for (j = 0; j < ph; j++) 
	{
		for (i = 0; i < pw; i++) 
		{
			int r, g, b, a;
			ewl_spectrum_color_coord_map(sp, i, j, &r, &g, &b, &a);
			data[(j * pw) + i] = (a << 24) | (r << 16) | (g << 8) | b;
		}
	}

	evas_object_image_data_set(o, data);
	evas_object_image_data_update_add(o, 0, 0, pw, ph);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spectrum_mouse_move_cb(Ewl_Widget *w, void *ev_data, 
				void *user_data __UNUSED__)
{
	Ewl_Spectrum *sp;
	Ewl_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sp = EWL_SPECTRUM(w);
	ev = ev_data;
	if ((sp->drag) && (ev->x > CURRENT_X(w))
			&& (ev->x < CURRENT_X(w) + CURRENT_W(w))
			&& (ev->y > CURRENT_Y(w))
			&& (ev->y < CURRENT_Y(w) + CURRENT_H(w)))
        {
                int x, y;

		x = ev->x - CURRENT_X(w);
                y = ev->y - CURRENT_Y(w);

		ewl_spectrum_color_coord_map(EWL_SPECTRUM(w), x, y,
				     &(sp->selected_rgba.r), &(sp->selected_rgba.g),
				     &(sp->selected_rgba.b), &(sp->selected_rgba.a));

		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spectrum_mouse_down_cb(Ewl_Widget *w, void *ev_data, 
				void *user_data __UNUSED__)
{
	Ewl_Spectrum *sp;
	Ewl_Event_Mouse_Down *ev;
	int x, y;

	DENTER_FUNCTION(DLEVEL_STABLE);
	
	sp = EWL_SPECTRUM(w);
	ev = ev_data;
	sp->drag = 1;
        
        x = ev->x - CURRENT_X(w);
        y = ev->y - CURRENT_Y(w);

	ewl_spectrum_color_coord_map(EWL_SPECTRUM(w), x, y, 
				&(sp->selected_rgba.r), &(sp->selected_rgba.g), 
				&(sp->selected_rgba.b), &(sp->selected_rgba.a));

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spectrum_mouse_up_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
				void *user_data __UNUSED__)
{
	Ewl_Spectrum *sp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sp = EWL_SPECTRUM(w);
	sp->drag = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_spectrum_color_coord_map2d(Ewl_Spectrum *sp, int x, int y, 
				int *r, int *g, int *b, int *a)
{
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	width = CURRENT_W(sp);
	height = CURRENT_H(sp);

	if (a) *a = 255;

	if (sp->mode == EWL_COLOR_PICK_MODE_RGB) 
	{
		int red, green, blue;

		red = sp->rgba.r;
		green = sp->rgba.g;
		blue = sp->rgba.b;

		red = red + (y) * (255 - red) / height;
		red = red * x / width;
		green = green + (y) * (255 - green) / height;
		green = green * x / width;
		blue = blue + (y) * (255 - blue) / height;
		blue = blue * x / width;

		if (r) *r = red;
		if (g) *g = green;
		if (b) *b = blue;
	}
	else 
	{
		float h, s, v;

		h = sp->hsv.h;
		s = sp->hsv.s;
		v = sp->hsv.v;

		if (sp->mode == EWL_COLOR_PICK_MODE_HSV_HUE) 
		{
			s = 1 - (float) x / (float) width;
			v = 1 - (float) y / (float) height;
		}
		else if (sp->mode == EWL_COLOR_PICK_MODE_HSV_SATURATION) 
		{
			h = (float) x / (float) width *360;
			v = 1 - (float) y / (float) height;
		}
		else if (sp->mode == EWL_COLOR_PICK_MODE_HSV_VALUE) 
		{
			h = (float) x / (float) width *360;
			s = 1 - (float) y / (float) height;
		}

                ewl_spectrum_hsv_to_rgb(h, s, v, r, g, b);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_spectrum_color_coord_map1d(Ewl_Spectrum *sp, int x __UNUSED__, int y, 
				int *r, int *g, int *b, int *a)
{
	int height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	height = CURRENT_H(sp);

	if (a) *a = 255;

	if (sp->mode == EWL_COLOR_PICK_MODE_RGB) 
	{
		if (r) *r = sp->rgba.r * (1 - (float) y / (float) height);
		if (g) *g = sp->rgba.g * (1 - (float) y / (float) height);
		if (b) *b = sp->rgba.b * (1 - (float) y / (float) height);
	}
	else 
	{
		int h, s, v;

		h = sp->hsv.h;
		s = sp->hsv.s;
		v = sp->hsv.v;
		if (sp->mode == EWL_COLOR_PICK_MODE_HSV_HUE) 
		{
			h = 360 * (float) y / (float) height;
			s = 1.0;
			v = 1.0;
		}
		else if (sp->mode == EWL_COLOR_PICK_MODE_HSV_SATURATION) 
			s = 1 - (float) y / (float) height;

		else if (sp->mode == EWL_COLOR_PICK_MODE_HSV_VALUE)
			v = 1 - (float) y / (float) height;

		ewl_spectrum_hsv_to_rgb(h, s, v, r, g, b);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

