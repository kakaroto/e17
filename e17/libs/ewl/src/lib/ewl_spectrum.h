#ifndef EWL_SPECTRUM_H
#define EWL_SPECTRUM_H

/**
 * @file ewl_spectrum.h
 * @defgroup Ewl_Spectrum Spectrum The colour spectrum widget
 *
 * @{
 */

#define EWL_SPECTRUM_TYPE "spectrum"

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
 * Inherits from Ewl_Overlay and extends to privide a colour spectrum 
 */
struct Ewl_Spectrum 
{
	Ewl_Overlay overlay;
	Ewl_Widget *canvas;
	
	struct 
	{
		Ewl_Widget *vertical;
		Ewl_Widget *horizontal;
	} cross_hairs;

	Ewl_Color_Set rgb;
	struct {
		double h;
		double s;
		double v;
	} hsv;

	Ewl_Color_Mode mode;
	Ewl_Spectrum_Type type;
};

Ewl_Widget	*ewl_spectrum_new(void);
int		 ewl_spectrum_init(Ewl_Spectrum *sp);

void		  ewl_spectrum_type_set(Ewl_Spectrum *sp, Ewl_Spectrum_Type type);
Ewl_Spectrum_Type ewl_spectrum_type_get(Ewl_Spectrum *sp);

void		 ewl_spectrum_mode_set(Ewl_Spectrum *sp, Ewl_Color_Mode mode);
Ewl_Color_Mode	 ewl_spectrum_mode_get(Ewl_Spectrum *sp);

void		 ewl_spectrum_rgb_set(Ewl_Spectrum *sp, unsigned int r, 
					unsigned int g, unsigned int b);
void		 ewl_spectrum_rgb_get(Ewl_Spectrum *sp, unsigned int *r,
					unsigned int *g, unsigned int *b);

void		 ewl_spectrum_hsv_set(Ewl_Spectrum *sp, double h,
						double s, double v);
void		 ewl_spectrum_hsv_get(Ewl_Spectrum *sp, double *h,
						double *s, double *v);
 
/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_spectrum_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_spectrum_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_spectrum_cb_mouse_move(Ewl_Widget *w, void *ev, void *data);
void ewl_spectrum_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

