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

/**
 * @def EWL_SPECTRUM(sp)
 * Typecasts a pointer to an Ewl_Spectrum pointer.
 */
#define EWL_SPECTRUM(sp) ((Ewl_Spectrum *)sp)

/**
 * Provides a colour spectrum.
 */
typedef struct Ewl_Spectrum Ewl_Spectrum;

/**
 * Inherits from Ewl_Image and extends to privide a colour spectrum 
 */
struct Ewl_Spectrum 
{
	Ewl_Image       	widget;

	Ewl_Orientation 	orientation;
	Ewl_Color_Pick_Mode 	mode;
	unsigned int		dimensions;
	int             	redraw;

	Ewl_Color_Set		rgba;
	struct
	{
				float h, s, v;
	} hsv;
};

Ewl_Widget     *ewl_spectrum_new(void);
int             ewl_spectrum_init(Ewl_Spectrum *sp);

void		ewl_spectrum_orientation_set(Ewl_Spectrum *sp, Ewl_Orientation o);
Ewl_Orientation ewl_spectrum_orientation_get(Ewl_Spectrum *sp);

void            ewl_spectrum_mode_set(Ewl_Spectrum *sp, Ewl_Color_Pick_Mode mode);
Ewl_Color_Pick_Mode ewl_spectrum_mode_get(Ewl_Spectrum *sp);

void            ewl_spectrum_dimensions_set(Ewl_Spectrum *sp, unsigned int dimensions);
unsigned int	ewl_spectrum_dimensions_get(Ewl_Spectrum *sp);

void            ewl_spectrum_rgba_set(Ewl_Spectrum *sp, int r, int g, int b, int a);
void            ewl_spectrum_rgba_get(Ewl_Spectrum *sp, int *r, int *g, int *b, int *a);

void            ewl_spectrum_hsv_set(Ewl_Spectrum *sp, float h, float s, float v);
void            ewl_spectrum_hsv_get(Ewl_Spectrum *sp, float *h, float *s, float *v);

void            ewl_spectrum_color_coord_map(Ewl_Spectrum *sp, int x, int y, 
					     int *r, int *g, int *b, int *a);

void  		ewl_spectrum_hsv_to_rgb(float hue, float saturation, float value,
					     int *r, int *g, int *b);
void  		ewl_spectrum_rgb_to_hsv(int r, int g, int b,
					     float *h, float *s, float *v);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_spectrum_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * @}
 */
#endif
