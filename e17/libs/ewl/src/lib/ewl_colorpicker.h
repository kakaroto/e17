#ifndef EWL_COLORPICKER_H
#define EWL_COLORPICKER_H

/**
 * @file ewl_colorpicker.h
 * @defgroup Ewl_Colorpicker Colorpicker: The colour picking widget
 *
 * @{
 */

/**
 * @def EWL_COLORPICKER(cp)
 * Typecast a pointer to an Ewl_Colorpicker pointer.
 */
#define EWL_COLORPICKER(cp) ((Ewl_Colorpicker *) cp)

typedef struct Ewl_Colorpicker Ewl_Colorpicker;

struct Ewl_Colorpicker
{
	Ewl_Box box;
	struct
	{
		Ewl_Widget *square;
		Ewl_Widget *vertical;
	} picker;

	struct
	{
		Ewl_Widget *current;
		Ewl_Widget *previous;
	} preview;

	struct
	{
		struct
		{
			Ewl_Widget *r;
		 	Ewl_Widget *g;
			Ewl_Widget *b;
		} rgb;

		struct
		{
			Ewl_Widget *h;
			Ewl_Widget *s;
			Ewl_Widget *v;
		} hsv;

		Ewl_Widget *alpha;
	} spinners;

	Ewl_Color_Set previous;
	Ewl_Color_Mode mode;

	unsigned int has_alpha:1;
};

Ewl_Widget 	*ewl_colorpicker_new(void);
int		 ewl_colorpicker_init(Ewl_Colorpicker *cp);

void		 ewl_colorpicker_has_alpha_set(Ewl_Colorpicker *cp, unsigned int alpha);
unsigned int	 ewl_colorpicker_has_alpha_get(Ewl_Colorpicker *cp);

void		 ewl_colorpicker_alpha_set(Ewl_Colorpicker *cp, unsigned int alpha);
unsigned int	 ewl_colorpicker_alpha_get(Ewl_Colorpicker *cp);

void		 ewl_colorpicker_current_rgb_set(Ewl_Colorpicker *cp, unsigned int r,
							unsigned int g, unsigned int b);
void		 ewl_colorpicker_current_rgb_get(Ewl_Colorpicker *cp, unsigned int *r,
							unsigned int *g, unsigned int *b);

void		 ewl_colorpicker_previous_rgb_set(Ewl_Colorpicker *cp, unsigned int r,
							unsigned int g, unsigned int b);
void		 ewl_colorpicker_previous_rgb_get(Ewl_Colorpicker *cp, unsigned int *r,
							unsigned int *g, unsigned int *b);

void		 ewl_colorpicker_color_mode_set(Ewl_Colorpicker *cp, Ewl_Color_Mode type);
Ewl_Color_Mode	 ewl_colorpicker_color_mode_get(Ewl_Colorpicker *cp);
 
 /*
  * * internally used callbacks, override at your own risk.
  *   */
void ewl_colorpicker_cb_square_change(Ewl_Widget *w, void *ev, void *data);
void ewl_colorpicker_cb_vertical_change(Ewl_Widget *w, void *ev, void *data);
void ewl_colorpicker_cb_spinner_change(Ewl_Widget *w, void *ev, void *data);
void ewl_colorpicker_cb_radio_change(Ewl_Widget *w, void *ev, void *data); 

/**
 * @}
 */

#endif

