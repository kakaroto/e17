#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_colorpicker_display_update(Ewl_Colorpicker *cp, unsigned int r,
				unsigned int g, unsigned int b, double h, 
				double s, double v);

#define EWL_COLORPICKER_SPINNER_TYPE "colorpicker_spinner"
#define EWL_COLORPICKER_SPINNER(spinner) ((Ewl_Colorpicker_Spinner *)spinner)

typedef struct Ewl_Colorpicker_Spinner Ewl_Colorpicker_Spinner;
struct Ewl_Colorpicker_Spinner
{
	Ewl_Spinner spinner;
	Ewl_Color_Mode mode;
};

#define EWL_COLORPICKER_RADIOBUTTON_TYPE "colorpicker_radiobutton"
#define EWL_COLORPICKER_RADIOBUTTON(button) ((Ewl_Colorpicker_Radiobutton *)button)

typedef struct Ewl_Colorpicker_Radiobutton Ewl_Colorpicker_Radiobutton;
struct Ewl_Colorpicker_Radiobutton
{
	Ewl_Radiobutton button;
	Ewl_Color_Mode mode;
};

static Ewl_Widget *ewl_colorpicker_spinner_new(void);
static int ewl_colorpicker_spinner_init(Ewl_Colorpicker_Spinner *sp);
static void ewl_colorpicker_spinner_mode_set(Ewl_Colorpicker_Spinner *sp, 
							Ewl_Color_Mode mode);
static Ewl_Color_Mode ewl_colorpicker_spinner_mode_get(
						Ewl_Colorpicker_Spinner *sp);

static Ewl_Widget *ewl_colorpicker_radiobutton_new(void);
static int ewl_colorpicker_radiobutton_init(Ewl_Colorpicker_Radiobutton *sp);
static void ewl_colorpicker_radiobutton_mode_set(
						Ewl_Colorpicker_Radiobutton *sp, 
						Ewl_Color_Mode mode);

static Ewl_Color_Mode ewl_colorpicker_radiobutton_mode_get(
					Ewl_Colorpicker_Radiobutton *sp);

/**
 * @return Returns a new Ewl_Colorpicker widget or NULL on failure
 * @brief Creates a new Ewl_Colorpicker widget
 */
Ewl_Widget *
ewl_colorpicker_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Colorpicker, 1);
	if (!w)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_colorpicker_init(EWL_COLORPICKER(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param cp: The Ewl_Colorpicker to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes the @p cp color picker widget
 */
int
ewl_colorpicker_init(Ewl_Colorpicker *cp)
{
	Ewl_Widget *vbox, *hbox, *o, *prev;
	unsigned int r, g, b;
	double h, s, v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cp", cp, FALSE);

	if (!ewl_box_init(EWL_BOX(cp)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(cp), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_appearance_set(EWL_WIDGET(cp), EWL_COLORPICKER_TYPE);
	ewl_widget_inherit(EWL_WIDGET(cp), EWL_COLORPICKER_TYPE);

	r = g = b = 0;

	vbox = ewl_vbox_new();
	ewl_widget_internal_set(vbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(cp), vbox);
	ewl_widget_show(vbox);

	/* hbox to hold the spectrums */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	o = ewl_spectrum_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spectrum_type_set(EWL_SPECTRUM(o), EWL_SPECTRUM_TYPE_SQUARE);
	ewl_spectrum_rgb_set(EWL_SPECTRUM(o), r, g, b);
	ewl_object_minimum_size_set(EWL_OBJECT(o), 150, 150);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_FILL);
	ewl_object_padding_set(EWL_OBJECT(o), 2, 2, 2, 2);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, 
				ewl_colorpicker_cb_square_change, cp);
	cp->picker.square = o;
	ewl_widget_show(o);

	o = ewl_spectrum_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spectrum_type_set(EWL_SPECTRUM(o), EWL_SPECTRUM_TYPE_VERTICAL);
	ewl_spectrum_rgb_set(EWL_SPECTRUM(o), r, g, b);
	ewl_object_minimum_size_set(EWL_OBJECT(o), 15, 150);
	ewl_object_maximum_size_set(EWL_OBJECT(o), 15, EWL_OBJECT_MAX_SIZE);
	ewl_object_padding_set(EWL_OBJECT(o), 2, 2, 2, 2);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, 
				ewl_colorpicker_cb_vertical_change, cp);
	cp->picker.vertical = o;
	ewl_widget_show(o);

	ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h, &s, &v);

	/* hbox to hold the colour previews */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
	ewl_widget_show(hbox);

	o = ewl_label_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_label_text_set(EWL_LABEL(o), "Current:");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);	
	ewl_widget_show(o);

	o = NEW(Ewl_Widget, 1);
	ewl_widget_init(o);
	ewl_widget_internal_set(o, TRUE);
	ewl_widget_appearance_set(o, "coloured_rect");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_widget_color_set(o, r, g, b, 255);
	ewl_object_padding_set(EWL_OBJECT(o), 2, 2, 2, 2);
	ewl_object_minimum_h_set(EWL_OBJECT(o), 20);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_ALL);
	cp->preview.current = o;
	ewl_widget_show(o);

	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
	ewl_widget_show(hbox);

	o = ewl_label_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_label_text_set(EWL_LABEL(o), "Previous:");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);	
	ewl_widget_show(o);

	o = NEW(Ewl_Widget, 1);
	ewl_widget_init(o);
	ewl_widget_internal_set(o, TRUE);
	ewl_widget_appearance_set(o, "coloured_rect");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_widget_color_set(o, r, g, b, 255);
	ewl_object_padding_set(EWL_OBJECT(o), 2, 2, 2, 2);
	ewl_object_minimum_h_set(EWL_OBJECT(o), 20);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_ALL);
	cp->preview.previous = o;
	ewl_widget_show(o);

	/* do the spinner side */
	vbox = ewl_vbox_new();
	ewl_widget_internal_set(vbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(cp), vbox);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(vbox);

	/* h button/spinner */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	o = ewl_colorpicker_radiobutton_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_button_label_set(EWL_BUTTON(o), "h");
	ewl_radiobutton_checked_set(EWL_RADIOBUTTON(o), TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_colorpicker_radiobutton_mode_set(EWL_COLORPICKER_RADIOBUTTON(o),
							EWL_COLOR_MODE_HSV_HUE);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
				ewl_colorpicker_cb_radio_change, cp);
	prev = o;
	ewl_widget_show(o);

	o = ewl_colorpicker_spinner_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spinner_min_val_set(EWL_SPINNER(o), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(o), 360);
	ewl_spinner_value_set(EWL_SPINNER(o), h);
	ewl_spinner_step_set(EWL_SPINNER(o), 1);
	ewl_spinner_digits_set(EWL_SPINNER(o), 0);
	ewl_colorpicker_spinner_mode_set(EWL_COLORPICKER_SPINNER(o),
						EWL_COLOR_MODE_HSV_HUE);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
				ewl_colorpicker_cb_spinner_change, cp);
	cp->spinners.hsv.h = o;
	ewl_widget_show(o);

	/* s button/spinner */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	o = ewl_colorpicker_radiobutton_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_button_label_set(EWL_BUTTON(o), "s");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(prev));
	ewl_colorpicker_radiobutton_mode_set(EWL_COLORPICKER_RADIOBUTTON(o),
						EWL_COLOR_MODE_HSV_SATURATION);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
				ewl_colorpicker_cb_radio_change, cp);
	prev = o;
	ewl_widget_show(o);

	o = ewl_colorpicker_spinner_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spinner_min_val_set(EWL_SPINNER(o), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(o), 100);
	ewl_spinner_value_set(EWL_SPINNER(o), s * 100);
	ewl_spinner_step_set(EWL_SPINNER(o), 1);
	ewl_spinner_digits_set(EWL_SPINNER(o), 0);
	ewl_colorpicker_spinner_mode_set(EWL_COLORPICKER_SPINNER(o),
					EWL_COLOR_MODE_HSV_SATURATION);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
				ewl_colorpicker_cb_spinner_change, cp);
	cp->spinners.hsv.s = o;
	ewl_widget_show(o);

	/* v button/spinner */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	o = ewl_colorpicker_radiobutton_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_button_label_set(EWL_BUTTON(o), "v");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(prev));
	ewl_colorpicker_radiobutton_mode_set(EWL_COLORPICKER_RADIOBUTTON(o),
						EWL_COLOR_MODE_HSV_VALUE);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
				ewl_colorpicker_cb_radio_change, cp);
	prev = o;
	ewl_widget_show(o);

	o = ewl_colorpicker_spinner_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spinner_min_val_set(EWL_SPINNER(o), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(o), 100);
	ewl_spinner_value_set(EWL_SPINNER(o), v * 100);
	ewl_spinner_step_set(EWL_SPINNER(o), 1);
	ewl_spinner_digits_set(EWL_SPINNER(o), 0);
	ewl_colorpicker_spinner_mode_set(EWL_COLORPICKER_SPINNER(o),
						EWL_COLOR_MODE_HSV_VALUE);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
				ewl_colorpicker_cb_spinner_change, cp);
	cp->spinners.hsv.v = o;
	ewl_widget_show(o);

	/* r button/spinner */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	o = ewl_colorpicker_radiobutton_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_button_label_set(EWL_BUTTON(o), "r");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(prev));
	ewl_colorpicker_radiobutton_mode_set(EWL_COLORPICKER_RADIOBUTTON(o),
						EWL_COLOR_MODE_RGB_RED);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
			ewl_colorpicker_cb_radio_change, cp);
	prev = o;
	ewl_widget_show(o);

	o = ewl_colorpicker_spinner_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spinner_min_val_set(EWL_SPINNER(o), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(o), 255);
	ewl_spinner_value_set(EWL_SPINNER(o), r);
	ewl_spinner_step_set(EWL_SPINNER(o), 1);
	ewl_spinner_digits_set(EWL_SPINNER(o), 0);
	ewl_colorpicker_spinner_mode_set(EWL_COLORPICKER_SPINNER(o),
						EWL_COLOR_MODE_RGB_RED);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
			ewl_colorpicker_cb_spinner_change, cp);
	cp->spinners.rgb.r = o;
	ewl_widget_show(o);

	/* g button/spinner */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	o = ewl_colorpicker_radiobutton_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_button_label_set(EWL_BUTTON(o), "g");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(prev));
	ewl_colorpicker_radiobutton_mode_set(EWL_COLORPICKER_RADIOBUTTON(o),
						EWL_COLOR_MODE_RGB_GREEN);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
			ewl_colorpicker_cb_radio_change, cp);
	prev = o;
	ewl_widget_show(o);

	o = ewl_colorpicker_spinner_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spinner_min_val_set(EWL_SPINNER(o), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(o), 255);
	ewl_spinner_value_set(EWL_SPINNER(o), g);
	ewl_spinner_step_set(EWL_SPINNER(o), 1);
	ewl_spinner_digits_set(EWL_SPINNER(o), 0);
	ewl_colorpicker_spinner_mode_set(EWL_COLORPICKER_SPINNER(o),
						EWL_COLOR_MODE_RGB_GREEN);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
			ewl_colorpicker_cb_spinner_change, cp);
	cp->spinners.rgb.g = o;
	ewl_widget_show(o);

	/* b button/spinner */
	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	o = ewl_colorpicker_radiobutton_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_button_label_set(EWL_BUTTON(o), "b");
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(prev));
	ewl_colorpicker_radiobutton_mode_set(EWL_COLORPICKER_RADIOBUTTON(o),
						EWL_COLOR_MODE_RGB_BLUE);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
			ewl_colorpicker_cb_radio_change, cp);
	prev = o;
	ewl_widget_show(o);

	o = ewl_colorpicker_spinner_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_spinner_min_val_set(EWL_SPINNER(o), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(o), 255);
	ewl_spinner_value_set(EWL_SPINNER(o), b);
	ewl_spinner_step_set(EWL_SPINNER(o), 1);
	ewl_spinner_digits_set(EWL_SPINNER(o), 0);
	ewl_colorpicker_spinner_mode_set(EWL_COLORPICKER_SPINNER(o),
						EWL_COLOR_MODE_RGB_BLUE);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
			ewl_colorpicker_cb_spinner_change, cp);
	cp->spinners.rgb.b = o;
	ewl_widget_show(o);

	/* alpha spinner */
	o = ewl_spinner_new();
	ewl_widget_internal_set(o, TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), o);
	ewl_spinner_min_val_set(EWL_SPINNER(o), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(o), 255);
	ewl_spinner_value_set(EWL_SPINNER(o), 0);
	ewl_spinner_digits_set(EWL_SPINNER(o), 0);
	cp->spinners.alpha = o;

	ewl_colorpicker_color_mode_set(cp, EWL_COLOR_MODE_HSV_HUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cp: The colour picker to set the has_alpha flag on.
 * @param alpha: Value to set the has_alpha param too. (TRUE or FALSE)
 * @return Returns no value.
 * @brief Set if the color picker displays alpha information
 */
void
ewl_colorpicker_has_alpha_set(Ewl_Colorpicker *cp, unsigned int alpha)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	cp->has_alpha = alpha;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colour picker to get the has_alpha flag from
 * @return Returns the value of the has_alpha flag (TRUE or FALSE)
 * @brief Retrieves if the color picker displays alpha information
 */
unsigned int
ewl_colorpicker_has_alpha_get(Ewl_Colorpicker *cp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cp", cp, FALSE);
	DCHECK_TYPE_RET("cp", cp, EWL_COLORPICKER_TYPE, FALSE);

	DRETURN_INT(cp->has_alpha, DLEVEL_STABLE);
}

/**
 * @param cp: the colourpicker to set the alpha value too
 * @param alpha: The alpha value to set.
 * @brief Set the alpha value into the color picker
 */
void
ewl_colorpicker_alpha_set(Ewl_Colorpicker *cp, unsigned int alpha)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	if (alpha > 255) alpha = 255;
	ewl_spinner_value_set(EWL_SPINNER(cp->spinners.alpha), alpha);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colourpicker to get the alpha value from
 * @return Returns the alpha value of the colorpicker (0-255)
 * @brief Retrive the alpha value from the color picker
 */
unsigned int
ewl_colorpicker_alpha_get(Ewl_Colorpicker *cp)
{
	unsigned int alpha;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cp", cp, 255);
	DCHECK_TYPE_RET("cp", cp, EWL_COLORPICKER_TYPE, 255);

	alpha = ewl_spinner_value_get(EWL_SPINNER(cp->spinners.alpha));

	DRETURN_INT(alpha, DLEVEL_STABLE);
}

/**
 * @param cp: The colourpicker to set the current rgb value into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @return Returns no value.
 * @brief Set the current RGB values into the color picker
 */
void
ewl_colorpicker_current_rgb_set(Ewl_Colorpicker *cp, unsigned int r,
				unsigned int g, unsigned int b)
{
	double h, s, v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	ewl_widget_color_set(cp->preview.current, r, g, b, 255);
	ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->picker.square), r, g, b);
	ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->picker.vertical), r, g, b);

	ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h, &s, &v);
	ewl_colorpicker_display_update(cp, r, g, b, h, s * 100, v * 100);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colorpicker to get the current rgb values from
 * @param r: Where to store the red value
 * @param g: Where to store the green value
 * @param b: Where to store the blue value
 * @return Returns no value
 * @brief Retrieve the current RGB values from the color picker
 */
void
ewl_colorpicker_current_rgb_get(Ewl_Colorpicker *cp, unsigned int *r,
				unsigned int *g, unsigned int *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), r, g, b);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colorpicker to set the previous rgb value into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @return Returns no value.
 * @brief Set the previous RBG values into the color picker
 */
void
ewl_colorpicker_previous_rgb_set(Ewl_Colorpicker *cp, unsigned int r,
				unsigned int g, unsigned int b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	cp->previous.r = r;
	cp->previous.g = g;
	cp->previous.b = b;

	ewl_widget_color_set(cp->preview.previous, r, g, b, 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colorpicker to get the previous rgb value from.
 * @param r: Where to store the red value
 * @param g: Where to store the green value
 * @param b: Where to store the blue value
 * @return Returns no value.
 * @brief Retrieve the previous RGB values from the color picker
 */
void
ewl_colorpicker_previous_rgb_get(Ewl_Colorpicker *cp, unsigned int *r,
				unsigned int *g, unsigned int *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	if (r) *r = cp->previous.r;
	if (g) *g = cp->previous.g;
	if (b) *b = cp->previous.b;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colorpicker to set the mode on
 * @param mode: The mode to set on the colorpicker
 * @return Returns no value.
 * @brief Set the color mode of the color picker
 */
void
ewl_colorpicker_color_mode_set(Ewl_Colorpicker *cp, Ewl_Color_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	cp->mode = mode;
	ewl_spectrum_mode_set(EWL_SPECTRUM(cp->picker.square), mode);
	ewl_spectrum_mode_set(EWL_SPECTRUM(cp->picker.vertical), mode);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colorpicker to get the mode from
 * @return Returns the Ewl_Color_Mode of the colorpicker
 * @brief Get the color mode of the color picker
 */
Ewl_Color_Mode
ewl_colorpicker_color_mode_get(Ewl_Colorpicker *cp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cp", cp, FALSE);
	DCHECK_TYPE_RET("cp", cp, EWL_COLORPICKER_TYPE, FALSE);

	DRETURN_INT(cp->mode, DLEVEL_STABLE);
}

void
ewl_colorpicker_cb_square_change(Ewl_Widget *w, void *ev __UNUSED__,
								void *data)
{
	Ewl_Colorpicker *cp;
	unsigned int r, g, b;
	double h, s, v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cp = data;
	ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), &r, &g, &b);
	ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h, &s, &v);

	ewl_colorpicker_display_update(cp, r, g, b, h, s, v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colorpicker_cb_vertical_change(Ewl_Widget *w, void *ev __UNUSED__,
								void *data)
{
	Ewl_Colorpicker *cp;
	unsigned int r, g, b;
	unsigned int r2, g2, b2;
	double h, s, v;
	double h2, s2, v2;
	unsigned int set_hsv = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cp = data;
	ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.vertical), &r, &g, &b);
	ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.vertical), &h, &s, &v);

	ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), &r2, &g2, &b2);
	ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h2, &s2, &v2);

	switch (cp->mode)
	{
		case EWL_COLOR_MODE_RGB_RED:
			r2 = r;
			break;
		case EWL_COLOR_MODE_RGB_GREEN:
			g2 = g;
			break;
		case EWL_COLOR_MODE_RGB_BLUE:
			b2 = b;
			break;
		case EWL_COLOR_MODE_HSV_HUE:
			h2 = h;
			set_hsv = TRUE;
			break;
		case EWL_COLOR_MODE_HSV_SATURATION:
			s2 = s;
			set_hsv = TRUE;
			break;
		case EWL_COLOR_MODE_HSV_VALUE:
			v2 = v;
			set_hsv = TRUE;
			break;
		default:
			break;
	}

	if (set_hsv == FALSE)
	{
		ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->picker.square), r2, g2, b2);
		ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h2, &s2, &v2);
	}
	else
	{
		ewl_spectrum_hsv_set(EWL_SPECTRUM(cp->picker.square), h2, s2, v2);
		ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), &r2, &g2, &b2);
	}
	ewl_colorpicker_display_update(cp, r2, g2, b2, h2, s2, v2);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colorpicker_cb_spinner_change(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Colorpicker *cp;
	unsigned int r, g, b;
	double h, s, v;
	Ewl_Color_Mode mode;
	int set_hsv = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cp = data;
	ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), &r, &g, &b);
	ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.vertical), &h, &s, &v);

	mode = ewl_colorpicker_spinner_mode_get(EWL_COLORPICKER_SPINNER(w));
	switch ((int)mode)
	{
		case EWL_COLOR_MODE_RGB_RED:
			r = ewl_spinner_value_get(EWL_SPINNER(cp->spinners.rgb.r));
			break;

		case EWL_COLOR_MODE_RGB_GREEN:
			g = ewl_spinner_value_get(EWL_SPINNER(cp->spinners.rgb.g));
			break;

		case EWL_COLOR_MODE_RGB_BLUE:
			b = ewl_spinner_value_get(EWL_SPINNER(cp->spinners.rgb.b));
			break;

		case EWL_COLOR_MODE_HSV_HUE:
			h = ewl_spinner_value_get(EWL_SPINNER(cp->spinners.hsv.h));
			set_hsv = TRUE;
			break;

		case EWL_COLOR_MODE_HSV_SATURATION:
			s = ewl_spinner_value_get(EWL_SPINNER(cp->spinners.hsv.s)) / 100.0;
			set_hsv = TRUE;
			break;

		case EWL_COLOR_MODE_HSV_VALUE:
			v = ewl_spinner_value_get(EWL_SPINNER(cp->spinners.hsv.v)) / 100.0;
			set_hsv = TRUE;
			break;

		default:
			break;
	}

	if (set_hsv)
	{
		ewl_spectrum_hsv_set(EWL_SPECTRUM(cp->picker.square), h, s, v);
		ewl_spectrum_hsv_set(EWL_SPECTRUM(cp->picker.vertical), h, s, v);

		ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), &r, &g, &b);
	}
	else
	{
		ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->picker.square), r, g, b);
		ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->picker.vertical), r, g, b);

		ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h, &s, &v);
	}
	ewl_colorpicker_display_update(cp, r, g, b, h, s, v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colorpicker_cb_radio_change(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Colorpicker *cp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cp = data;
	cp->mode = ewl_colorpicker_radiobutton_mode_get(EWL_COLORPICKER_RADIOBUTTON(w));

	ewl_spectrum_mode_set(EWL_SPECTRUM(cp->picker.square), cp->mode);
	ewl_spectrum_mode_set(EWL_SPECTRUM(cp->picker.vertical), cp->mode);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_colorpicker_display_update(Ewl_Colorpicker *cp, unsigned int r, unsigned int g,
				unsigned int b, double h, double s, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cp", cp);
	DCHECK_TYPE("cp", cp, EWL_COLORPICKER_TYPE);

	ewl_spinner_value_set(EWL_SPINNER(cp->spinners.rgb.r), r);
	ewl_spinner_value_set(EWL_SPINNER(cp->spinners.rgb.g), g);
	ewl_spinner_value_set(EWL_SPINNER(cp->spinners.rgb.b), b);

	ewl_spinner_value_set(EWL_SPINNER(cp->spinners.hsv.h), h);
	ewl_spinner_value_set(EWL_SPINNER(cp->spinners.hsv.s), s * 100);
	ewl_spinner_value_set(EWL_SPINNER(cp->spinners.hsv.v), v * 100);

	ewl_widget_color_set(cp->preview.current, r, g, b, 255);

	ewl_callback_call(EWL_WIDGET(cp), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_colorpicker_spinner_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Colorpicker_Spinner, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_colorpicker_spinner_init(EWL_COLORPICKER_SPINNER(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

static int
ewl_colorpicker_spinner_init(Ewl_Colorpicker_Spinner *sp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_spinner_init(EWL_SPINNER(sp)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(sp), EWL_COLORPICKER_SPINNER_TYPE);
	sp->mode = EWL_COLOR_MODE_RGB_RED;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ewl_colorpicker_spinner_mode_set(Ewl_Colorpicker_Spinner *sp,
					Ewl_Color_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sp", sp);
	DCHECK_TYPE("sp", sp, EWL_COLORPICKER_SPINNER_TYPE);

	sp->mode = mode;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Color_Mode
ewl_colorpicker_spinner_mode_get(Ewl_Colorpicker_Spinner *sp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("sp", sp, EWL_COLOR_MODE_RGB_RED);
	DCHECK_TYPE_RET("sp", sp, EWL_COLORPICKER_SPINNER_TYPE,	 
					EWL_COLOR_MODE_RGB_RED);

	DRETURN_INT(sp->mode, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_colorpicker_radiobutton_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Colorpicker_Radiobutton, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_colorpicker_radiobutton_init(EWL_COLORPICKER_RADIOBUTTON(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

static int
ewl_colorpicker_radiobutton_init(Ewl_Colorpicker_Radiobutton *rb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_radiobutton_init(EWL_RADIOBUTTON(rb)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(rb), EWL_COLORPICKER_RADIOBUTTON_TYPE);
	rb->mode = EWL_COLOR_MODE_RGB_RED;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ewl_colorpicker_radiobutton_mode_set(Ewl_Colorpicker_Radiobutton *rb, 
						Ewl_Color_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("rb", rb);
	DCHECK_TYPE("rb", rb, EWL_COLORPICKER_RADIOBUTTON_TYPE);

	rb->mode = mode;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Color_Mode
ewl_colorpicker_radiobutton_mode_get(Ewl_Colorpicker_Radiobutton *rb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("rb", rb, EWL_COLOR_MODE_RGB_RED);
	DCHECK_TYPE_RET("rb", rb, EWL_COLORPICKER_RADIOBUTTON_TYPE,	 
					EWL_COLOR_MODE_RGB_RED);

	DRETURN_INT(rb->mode, DLEVEL_STABLE);
}


