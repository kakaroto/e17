#ifndef _EWL_SPECTRUM_H
#define _EWL_SPECTRUM_H

enum {
	PICK_MODE_RGB_RED,
	PICK_MODE_RGB_GREEN,
	PICK_MODE_RGB_BLUE,
	PICK_MODE_HSV_HUE,
	PICK_MODE_HSV_SATURATION,
	PICK_MODE_HSV_VALUE
};

typedef struct _ewl_spectrum Ewl_Spectrum;

#define EWL_SPECTRUM(cp) ((Ewl_Spectrum *)cp)

struct _ewl_spectrum {
	Ewl_Image       widget;

	int             mode;
	int             dimensions;

	int             r, g, b;
	float           h, s, v;

	int             redraw;
};

Ewl_Widget     *ewl_spectrum_new();
void            ewl_spectrum_init(Ewl_Spectrum * cp);

void            ewl_spectrum_mode_set(Ewl_Spectrum * sp,
						  int mode);
		/* 1 or 2 */
void            ewl_spectrum_dimensions_set(Ewl_Spectrum * sp,
							int dimensions);

void            ewl_spectrum_rgb_set(Ewl_Spectrum * sp,
						 int r, int g, int b);
void            ewl_spectrum_hsv_set(Ewl_Spectrum * sp,
						 float h, float s, float v);

/*
 * Internally used callbacks, override at your own risk.
 */
void         ewl_spectrum_configure_cb(Ewl_Widget * w, void *ev_data,
				       void *user_data);

#endif
