#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_histogram_draw(Ewl_Histogram *hist);

Ewl_Widget *
ewl_histogram_new(void)
{
	Ewl_Histogram *hist;

	DENTER_FUNCTION(DLEVEL_STABLE);

	hist = calloc(1, sizeof(Ewl_Histogram));
	if (!ewl_histogram_init(hist)) {
		free(hist);
		hist = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(hist), DLEVEL_STABLE);
}

int
ewl_histogram_init(Ewl_Histogram *hist)
{
	int result;
	Ewl_Widget *w = EWL_WIDGET(hist);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("hist", hist, FALSE);

	result = ewl_image_init(EWL_IMAGE(hist));
	if (result) {
		ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				    ewl_histogram_cb_configure, NULL);
		ewl_object_preferred_inner_size_set(EWL_OBJECT(hist), 256, 256);
	}

	DRETURN_INT(result, DLEVEL_STABLE);
}

void
ewl_histogram_color_set(Ewl_Histogram *hist, char r, char g, char b, char a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("hist", hist);

	hist->r = r;
	hist->g = g;
	hist->b = b;
	hist->a = a;
	ewl_widget_configure(EWL_WIDGET(hist));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* FIXME: Handle different histogram types, currently only outputs the Y
 * component of a YIQ conversion, need to handle separate RGB, HSV, CMYK, etc.
void
ewl_histogram_type_set(Ewl_Histogram *hist, int type)
{
}
 */

/* a = ((*data) >> 24) & 0xFF; */
#define A_CALC(color) ((color) >> 24)
#define R_CALC(color) ((color << 8) >> 24)
#define G_CALC(color) ((color << 16) >> 24)
#define B_CALC(color) ((color << 24) >> 24)
/* r = ((color) >> 16) & 0xFF; */
/* g = ((color) >> 8) & 0xFF; */
/* b = (color) & 0xFF; */
/* brightness = (unsigned char)(((r * 299) + (g * 587) + (b * 114)) / 1000); */
#define Y_CALC(color) (((R_CALC(color) * 299) + (G_CALC(color) * 587) + (B_CALC(color) * 114)) / 1000)

void
ewl_histogram_data_set(Ewl_Histogram *hist, unsigned int *data, int width, int height)
{
	int x, y;
	int maxv = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("hist", hist);

	hist->data = data;
	hist->width = width;
	hist->height = height;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			unsigned int color;
			unsigned char brightness;

			color = *data;
			brightness = Y_CALC(color);
			hist->graph[brightness]++;
			data++;
		}
	}

	/*
	 * Find the maximum value in the range to scale the graph. This is done
	 * outside the data loop since that may have a much higher cost
	 * and this lookup is over a fixed data size.
	 */
	for (x = 0; x < 256; x++) {
		if (hist->graph[x] > maxv)
			maxv = hist->graph[x];
	}

	hist->maxv = maxv;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_histogram_cb_configure(Ewl_Widget *w, void *event, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_histogram_draw(EWL_HISTOGRAM(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_histogram_draw(Ewl_Histogram *hist)
{
	int x, y;
	unsigned int color;
	unsigned int *data, *dst;
	Evas_Coord img_w, img_h;
	Evas_Object *img;

	DENTER_FUNCTION(DLEVEL_STABLE);

	img = EWL_IMAGE(hist)->image;

	evas_object_image_size_set(img, CURRENT_W(hist), CURRENT_H(hist));
	evas_object_image_alpha_set(img, 1);
	evas_object_image_size_get(img, &img_w, &img_h);

	dst = data = evas_object_image_data_get(img, 1);
	if (!data)
		return;

	if (!hist->a)
		color = (unsigned int)(255 << 24);
	else
		color = (unsigned int)(hist->a << 24 | hist->r << 16 | hist->g << 8 | hist->b);

	for (y = 0; y < img_h; y++) {
		for (x = 0; x < img_w; x++) {
			int index;
			int cutoff;
			int x_scale;
			int w1, w2;

			/* Determine the base index for this x position */
			x_scale = x << 8;
			index = x_scale / img_w;

			/*
			 * Determine distance between index points.
			 * Used to determine rounding error and distance from
			 * next point.
			 * This is at x * 256 scale.
			 */
			w1 = index * img_w;
			w2 = w1 + img_w;

			cutoff = hist->graph[index];

			/* Determine if this index should be weighted */
			if (x_scale != w1 && index < 255 && (cutoff || hist->graph[index + 1])) {
				cutoff = (cutoff * (w2 - x_scale));
				cutoff += (hist->graph[index + 1] * (x_scale - w1));
				cutoff = (cutoff / (w2 - w1));
			}

			cutoff = (img_h * cutoff) / hist->maxv;

			*dst = ((img_h - y) < cutoff ? color : 0x0);
			dst++;
		}
	}

	evas_object_image_data_set(img, data);
	evas_object_image_data_update_add(img, 0, 0, img_w, img_h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
