#include <Ewl.h>

static void  ewl_spectrum_hsv_to_rgb(float hue, float saturation, float value,
				     int *r, int *g, int *b);

/**
 *
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


void ewl_spectrum_init(Ewl_Spectrum * sp)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	w = EWL_WIDGET(sp);

	ewl_image_init(EWL_IMAGE(w), NULL, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_spectrum_configure_cb, NULL);

	sp->mode = PICK_MODE_HSV_HUE;
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

void ewl_spectrum_mode_set(Ewl_Spectrum * sp, int mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	sp->mode = mode;
	sp->redraw = 1;
	ewl_widget_configure(EWL_WIDGET(sp));

	DRETURN(DLEVEL_STABLE);
}

void
ewl_spectrum_dimensions_set(Ewl_Spectrum * sp,
					int dimensions)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);

	if (dimensions == 1 || dimensions == 2)
		sp->dimensions = dimensions;

	sp->redraw = 1;
	ewl_widget_configure(EWL_WIDGET(sp));

	DRETURN(DLEVEL_STABLE);
}

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

	ewl_widget_configure(EWL_WIDGET(sp));

	DRETURN(DLEVEL_STABLE);
}

/*
 * Callback for drawing the spectrum to the image data.
 */
void
ewl_spectrum_configure_cb(Ewl_Widget * w, void *ev_data,
			  void *user_data)
{
	Evas_Object    *o;
	int             red, green, blue;
	float           hue, sat, val;
	int             pick_mode;
	float           h, s, v;
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
	red = sp->r;
	green = sp->g;
	blue = sp->b;
	hue = sp->h;
	sat = sp->s;
	val = sp->v;
	pick_mode = sp->mode;

	if (!o)
		return;

	if (sp->dimensions == 2) {
		/* set/get the spectrum size and image data */
		evas_object_image_size_set(o, 256, 256);
		evas_object_image_size_get(o, &pw, &ph);
		data = evas_object_image_data_get(o, 1);
		if (!data)
			return;

		/* draw the spectrum */
		for (i = 0; i < pw; i++) {
			for (j = 0; j < ph; j++) {
				int             r, g, b, a;

				a = 255;

				if (pick_mode == PICK_MODE_RGB_RED) {
					r = red;
					g = 255 * (float) i / (float) pw;
					b = 255 * (float) j / (float) ph;
				} else if (pick_mode == PICK_MODE_RGB_GREEN) {
					r = 255 * (float) i / (float) pw;
					g = green;
					b = 255 * (float) j / (float) ph;
				} else if (pick_mode == PICK_MODE_RGB_BLUE) {
					r = 255 * (float) i / (float) pw;
					g = 255 * (float) j / (float) ph;
					b = blue;
				} else if (pick_mode == PICK_MODE_HSV_HUE) {
					h = hue;
					s = 1 - (float) i / (float) pw;
					v = 1 - (float) j / (float) ph;
					ewl_spectrum_hsv_to_rgb(h, s, v,
								&r, &g, &b);
				} else if (pick_mode ==
					   PICK_MODE_HSV_SATURATION) {
					h = (float) i / (float) pw *360;

					s = sat;
					v = 1 - (float) j / (float) ph;
					ewl_spectrum_hsv_to_rgb(h, s, v,
								&r, &g, &b);
				} else if (pick_mode == PICK_MODE_HSV_VALUE) {
					h = (float) i / (float) pw *360;

					s = 1 - (float) j / (float) ph;
					v = val;
					ewl_spectrum_hsv_to_rgb(h, s, v,
								&r, &g, &b);
				}
				//printf("r: %d\n", r);

				data[(j * pw) + i] =
				    (a << 24) | (r << 16) | (g << 8) | b;
			}
		}
		evas_object_image_data_set(o, data);
		evas_object_image_data_update_add(o, 0, 0, pw, ph);
	}

	if (sp->dimensions == 1) {
		evas_object_image_size_set(o, 20, 256);
		/* get the vertical bar size and image data */
		evas_object_image_size_get(o, &pw, &ph);
		data = NULL;
		data = evas_object_image_data_get(o, 1);
		if (!data)
			return;

		/* draw the vertical bar */

		/* draw the spectrum */
		for (j = 0; j < ph; j++) {
			int             r, g, b, a;

			a = 255;

			if (pick_mode == PICK_MODE_RGB_RED) {
				r = 255 * (1 - (float) j / (float) ph);
				g = 0;
				b = 0;
			} else if (pick_mode == PICK_MODE_RGB_GREEN) {
				r = 0;
				g = 255 * (1 - (float) j / (float) ph);
				b = 0;
			} else if (pick_mode == PICK_MODE_RGB_BLUE) {
				r = 0;
				g = 0;
				b = 255 * (1 - (float) j / (float) ph);
			} else if (pick_mode == PICK_MODE_HSV_HUE) {
				h = 360 * (float) j / (float) ph;
				s = 1.0;
				v = 1.0;
				ewl_spectrum_hsv_to_rgb(h, s, v, &r, &g, &b);
			} else if (pick_mode == PICK_MODE_HSV_SATURATION) {
				h = hue;
				s = 1 - (float) j / (float) ph;
				v = val;
				ewl_spectrum_hsv_to_rgb(h, s, v, &r, &g, &b);
			} else if (pick_mode == PICK_MODE_HSV_VALUE) {
				h = hue;
				s = sat;
				v = 1 - (float) j / (float) ph;
				ewl_spectrum_hsv_to_rgb(h, s, v, &r, &g, &b);
			}

			for (i = 0; i < pw; i++) {
				data[(j * pw) + i] =
				    (a << 24) | (r << 16) | (g << 8) | b;
			}
		}

		evas_object_image_data_set(o, data);
		evas_object_image_data_update_add(o, 0, 0, pw, ph);
	}

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
