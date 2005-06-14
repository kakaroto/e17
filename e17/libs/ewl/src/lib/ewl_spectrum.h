#ifndef _EWL_SPECTRUM_H
#define _EWL_SPECTRUM_H

/**
 * @file ewl_spectrum.h
 * @defgroup Ewl_Spectrum Spectrum The spectrum widget
 *
 * @{
 */

/**
 * @themekey /spectrum/file
 * @themekey /spectrum/group
 */

typedef struct _ewl_spectrum Ewl_Spectrum;

#define EWL_SPECTRUM(cp) ((Ewl_Spectrum *)cp)

struct _ewl_spectrum {
	Ewl_Image       widget;

	int             orientation;
	Ewl_Pick_Mode   mode;
	int             dimensions;

	int             r, g, b;
	float           h, s, v;

	int             redraw;
};

Ewl_Widget     *ewl_spectrum_new(void);
void            ewl_spectrum_init(Ewl_Spectrum * cp);

void            ewl_spectrum_mode_set(Ewl_Spectrum * sp, Ewl_Pick_Mode mode);
void            ewl_spectrum_dimensions_set(Ewl_Spectrum * sp, int dimensions);

void            ewl_spectrum_rgb_set(Ewl_Spectrum * sp, int r, int g, int b);
void            ewl_spectrum_hsv_set(Ewl_Spectrum * sp,
						 float h, float s, float v);
void            ewl_spectrum_color_coord_map(Ewl_Spectrum *sp, int x, int y, 
					     int *r, int *g, int *b, int *a);
void  ewl_spectrum_hsv_to_rgb(float hue, float saturation, float value,
				     int *r, int *g, int *b);
void  ewl_spectrum_rgb_to_hsv(int r, int g, int b,
				     float *h, float *s, float *v);

/*
 * Internally used callbacks, override at your own risk.
 */
void         ewl_spectrum_configure_cb(Ewl_Widget * w, void *ev_data,
				       void *user_data);

/**
 * @}
 */
#endif
