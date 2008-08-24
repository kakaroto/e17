/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_colorpicker.h"
#include "ewl_grid.h"
#include "ewl_label.h"
#include "ewl_radiobutton.h"
#include "ewl_spectrum.h"
#include "ewl_spinner.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_colorpicker_display_update(Ewl_Colorpicker *cp, unsigned int r,
                                unsigned int g, unsigned int b, double h,
                                double s, double v);

#define EWL_COLORPICKER_SPINNER_TYPE "colorpicker_spinner"
#define EWL_COLORPICKER_SPINNER(spinner) ((Ewl_Colorpicker_Spinner *)spinner)

/**
 * Ewl_Colorpicker_Spinner
 */
typedef struct Ewl_Colorpicker_Spinner Ewl_Colorpicker_Spinner;

/**
 * @brief Inerits from Ewl_Spinner and extneds to provide colorpicker
 * information
 */
struct Ewl_Colorpicker_Spinner
{
        Ewl_Spinner spinner;
        Ewl_Color_Mode mode;
};

#define EWL_COLORPICKER_RADIOBUTTON_TYPE "colorpicker_radiobutton"
#define EWL_COLORPICKER_RADIOBUTTON(button) ((Ewl_Colorpicker_Radiobutton *)button)

/**
 * Ewl_Colorpicker_Radiobutton
 */
typedef struct Ewl_Colorpicker_Radiobutton Ewl_Colorpicker_Radiobutton;

/**
 * @brief Inherits from Ewl_Radiobutton and extends to provide colorpicker
 * information
 */
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
        Ewl_Widget *vbox, *hbox, *o, *grid, *prev = NULL;
        unsigned int r, g, b;
        double h, s, v;
        int i;
        struct
        {
                char *name;
                Ewl_Color_Mode mode;
                int max;
        } modes[] = {
                {"H", EWL_COLOR_MODE_HSV_HUE, 360},
                {"S", EWL_COLOR_MODE_HSV_SATURATION, 100},
                {"V", EWL_COLOR_MODE_HSV_VALUE, 100},
                {"R", EWL_COLOR_MODE_RGB_RED, 255},
                {"G", EWL_COLOR_MODE_RGB_GREEN, 255},
                {"B", EWL_COLOR_MODE_RGB_BLUE, 255},
                {NULL, EWL_COLOR_MODE_RGB_RED, 0}
        };
        const char *types[] = { "application/x-color", NULL };

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cp, FALSE);

        if (!ewl_box_init(EWL_BOX(cp)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(cp), EWL_ORIENTATION_HORIZONTAL);
        ewl_widget_appearance_set(EWL_WIDGET(cp), EWL_COLORPICKER_TYPE);
        ewl_widget_inherit(EWL_WIDGET(cp), EWL_COLORPICKER_TYPE);
        ewl_dnd_accepted_types_set(EWL_WIDGET(cp), types);

        ewl_callback_append(EWL_WIDGET(cp), EWL_CALLBACK_DND_DATA_RECEIVED,
                                ewl_colorpicker_cb_dnd_data, NULL);

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
        /* XXX this shouldn't be hard coded */
        ewl_object_minimum_size_set(EWL_OBJECT(o), 15, 150);
        ewl_object_maximum_size_set(EWL_OBJECT(o), 15, EWL_OBJECT_MAX_SIZE);
        ewl_object_padding_set(EWL_OBJECT(o), 2, 2, 2, 2);
        ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_colorpicker_cb_vertical_change, cp);
        cp->picker.vertical = o;
        ewl_widget_show(o);

        ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h, &s, &v);

        /* grid to hold the colour previews */
        hbox = ewl_grid_new();
        ewl_widget_internal_set(hbox, TRUE);
        ewl_grid_dimensions_set(EWL_GRID(hbox), 2, 2);
        ewl_grid_vhomogeneous_set(EWL_GRID(hbox), TRUE);
        ewl_grid_column_preferred_w_use(EWL_GRID(hbox), 0);
        ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox),
                                EWL_FLAG_FILL_NONE | EWL_FLAG_FILL_HFILL);
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
        ewl_object_fill_policy_set(EWL_OBJECT(o),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINKABLE);
        cp->preview.current = o;
        ewl_widget_show(o);

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
        ewl_object_fill_policy_set(EWL_OBJECT(o),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINKABLE);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_colorpicker_cb_previous_clicked, cp);
        cp->preview.previous = o;
        ewl_widget_show(o);

        /* do the spinner side */
        vbox = ewl_vbox_new();
        ewl_widget_internal_set(vbox, TRUE);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(vbox), EWL_FLAG_ALIGN_TOP);
        ewl_container_child_append(EWL_CONTAINER(cp), vbox);
        ewl_widget_show(vbox);

        grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(grid), 2, 6);
        ewl_container_child_append(EWL_CONTAINER(vbox), grid);
        ewl_widget_show(grid);

        /* setup the spinners and radiobuttons */
        for (i = 0; modes[i].name != NULL; i++)
        {
                o = ewl_colorpicker_radiobutton_new();
                ewl_widget_internal_set(o, TRUE);
                ewl_button_label_set(EWL_BUTTON(o), modes[i].name);
                if (!prev)
                        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(o), TRUE);
                else
                        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(o), FALSE);

                ewl_container_child_append(EWL_CONTAINER(grid), o);
                ewl_colorpicker_radiobutton_mode_set(EWL_COLORPICKER_RADIOBUTTON(o),
                                                                        modes[i].mode);
                if (prev)
                        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(prev));
                ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_colorpicker_cb_radio_change, cp);
                prev = o;
                ewl_widget_show(o);

                o = ewl_colorpicker_spinner_new();
                ewl_widget_internal_set(o, TRUE);
                ewl_container_child_append(EWL_CONTAINER(grid), o);
                ewl_range_minimum_value_set(EWL_RANGE(o), 0);
                ewl_range_maximum_value_set(EWL_RANGE(o), modes[i].max);

                switch(modes[i].mode)
                {
                        case EWL_COLOR_MODE_HSV_HUE:
                                ewl_range_value_set(EWL_RANGE(o), h);
                                cp->spinners.hsv.h = o;
                                break;
                        case EWL_COLOR_MODE_HSV_SATURATION:
                                ewl_range_value_set(EWL_RANGE(o), s * 100);
                                cp->spinners.hsv.s = o;
                                break;
                        case EWL_COLOR_MODE_HSV_VALUE:
                                ewl_range_value_set(EWL_RANGE(o), v * 100);
                                cp->spinners.hsv.v = o;
                                break;
                        case EWL_COLOR_MODE_RGB_RED:
                                ewl_range_value_set(EWL_RANGE(o), r);
                                cp->spinners.rgb.r = o;
                                break;
                        case EWL_COLOR_MODE_RGB_GREEN:
                                ewl_range_value_set(EWL_RANGE(o), g);
                                cp->spinners.rgb.g = o;
                                break;
                        case EWL_COLOR_MODE_RGB_BLUE:
                                ewl_range_value_set(EWL_RANGE(o), b);
                                cp->spinners.rgb.b = o;
                                break;
                }

                ewl_range_step_set(EWL_RANGE(o), 1);
                ewl_spinner_digits_set(EWL_SPINNER(o), 0);
                ewl_colorpicker_spinner_mode_set(EWL_COLORPICKER_SPINNER(o),
                                                                modes[i].mode);
                ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_colorpicker_cb_spinner_change, cp);
                ewl_widget_show(o);
        }

        o = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), o);
        cp->alpha_box = o;

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "A");
        ewl_container_child_append(EWL_CONTAINER(cp->alpha_box), o);
        ewl_widget_show(o);

        /* alpha spinner */
        o = ewl_spinner_new();
        ewl_widget_internal_set(o, TRUE);
        ewl_container_child_append(EWL_CONTAINER(cp->alpha_box), o);
        ewl_range_minimum_value_set(EWL_RANGE(o), 0);
        ewl_range_maximum_value_set(EWL_RANGE(o), 255);
        ewl_range_value_set(EWL_RANGE(o), 255);
        ewl_range_step_set(EWL_RANGE(o), 1);
        ewl_spinner_digits_set(EWL_SPINNER(o), 0);
        ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_colorpicker_cb_alpha_change, cp);
        ewl_widget_show(o);
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
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

        if (cp->has_alpha == !!alpha)
                DRETURN(DLEVEL_STABLE);

        cp->has_alpha = !!alpha;

        if (cp->has_alpha) ewl_widget_show(cp->alpha_box);
        else ewl_widget_hide(cp->alpha_box);

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
        DCHECK_PARAM_PTR_RET(cp, FALSE);
        DCHECK_TYPE_RET(cp, EWL_COLORPICKER_TYPE, FALSE);

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
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

        if (alpha > 255) alpha = 255;
        ewl_range_value_set(EWL_RANGE(cp->spinners.alpha), alpha);

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
        DCHECK_PARAM_PTR_RET(cp, 255);
        DCHECK_TYPE_RET(cp, EWL_COLORPICKER_TYPE, 255);

        alpha = ewl_range_value_get(EWL_RANGE(cp->spinners.alpha));

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
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;

        ewl_widget_color_set(cp->preview.current, r, g, b, 
                                ewl_range_value_get(EWL_RANGE(cp->spinners.alpha)));
        ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->picker.square), r, g, b);
        ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->picker.vertical), r, g, b);

        ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h, &s, &v);
        ewl_colorpicker_display_update(cp, r, g, b, h, s, v);

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
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

        ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), r, g, b);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colorpicker to set the previous rgb value into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @param a: The alpha value to set
 * @return Returns no value.
 * @brief Set the previous RBGA values into the color picker
 */
void
ewl_colorpicker_previous_rgba_set(Ewl_Colorpicker *cp, unsigned int r,
                                unsigned int g, unsigned int b, 
                                unsigned int a)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

        cp->previous.r = r;
        cp->previous.g = g;
        cp->previous.b = b;
        cp->previous.a = a;

        ewl_widget_color_set(cp->preview.previous, r, g, b, a);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cp: The colorpicker to get the previous rgb value from.
 * @param r: Where to store the red value
 * @param g: Where to store the green value
 * @param b: Where to store the blue value
 * @param a: Where to store the alpha value
 * @return Returns no value.
 * @brief Retrieve the previous RGBA values from the color picker
 */
void
ewl_colorpicker_previous_rgba_get(Ewl_Colorpicker *cp, unsigned int *r,
                                unsigned int *g, unsigned int *b, 
                                unsigned int *a)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

        if (r) *r = cp->previous.r;
        if (g) *g = cp->previous.g;
        if (b) *b = cp->previous.b;
        if (a) *a = cp->previous.a;

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
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

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
        DCHECK_PARAM_PTR_RET(cp, FALSE);
        DCHECK_TYPE_RET(cp, EWL_COLORPICKER_TYPE, FALSE);

        DRETURN_INT(cp->mode, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: The colorpicker
 * @return Returns no value
 * @brief Callback for when the square spectrum changes values
 */
void
ewl_colorpicker_cb_square_change(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                                void *data)
{
        Ewl_Colorpicker *cp;
        unsigned int r, g, b;
        double h, s, v;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_COLORPICKER_TYPE);

        cp = data;
        ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), &r, &g, &b);
        ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.square), &h, &s, &v);

        ewl_colorpicker_display_update(cp, r, g, b, h, s, v);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: The colorpicker
 * @return Returns no value
 * @brief Callback for a vertical spectrum value change
 */
void
ewl_colorpicker_cb_vertical_change(Ewl_Widget *w __UNUSED__,
                                        void *ev __UNUSED__, void *data)
{
        Ewl_Colorpicker *cp;
        unsigned int r, g, b;
        unsigned int r2, g2, b2;
        double h, s, v;
        double h2, s2, v2;
        unsigned int set_hsv = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_COLORPICKER_TYPE);

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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: The colorpicker
 * @return Returns no value
 * @brief The callback for a spinner change
 */
void
ewl_colorpicker_cb_spinner_change(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Colorpicker *cp;
        unsigned int r, g, b;
        double h, s, v;
        Ewl_Color_Mode mode;
        int set_hsv = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_COLORPICKER_SPINNER_TYPE);
        DCHECK_TYPE(data, EWL_COLORPICKER_TYPE);

        cp = data;
        ewl_spectrum_rgb_get(EWL_SPECTRUM(cp->picker.square), &r, &g, &b);
        ewl_spectrum_hsv_get(EWL_SPECTRUM(cp->picker.vertical), &h, &s, &v);

        mode = ewl_colorpicker_spinner_mode_get(EWL_COLORPICKER_SPINNER(w));
        switch ((int)mode)
        {
                case EWL_COLOR_MODE_RGB_RED:
                        r = ewl_range_value_get(EWL_RANGE(cp->spinners.rgb.r));
                        break;

                case EWL_COLOR_MODE_RGB_GREEN:
                        g = ewl_range_value_get(EWL_RANGE(cp->spinners.rgb.g));
                        break;

                case EWL_COLOR_MODE_RGB_BLUE:
                        b = ewl_range_value_get(EWL_RANGE(cp->spinners.rgb.b));
                        break;

                case EWL_COLOR_MODE_HSV_HUE:
                        h = ewl_range_value_get(EWL_RANGE(cp->spinners.hsv.h));
                        set_hsv = TRUE;
                        break;

                case EWL_COLOR_MODE_HSV_SATURATION:
                        s = ewl_range_value_get(EWL_RANGE(cp->spinners.hsv.s)) / 100.0;
                        set_hsv = TRUE;
                        break;

                case EWL_COLOR_MODE_HSV_VALUE:
                        v = ewl_range_value_get(EWL_RANGE(cp->spinners.hsv.v)) / 100.0;
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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: The colorpicker
 * @return Returns no value
 * @brief The callback for a spinner change
 */
void
ewl_colorpicker_cb_alpha_change(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Colorpicker *cp;
        unsigned int r, g, b, a;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_SPINNER_TYPE);
        DCHECK_TYPE(data, EWL_COLORPICKER_TYPE);

        cp = data;
        r = ewl_range_value_get(EWL_RANGE(cp->spinners.rgb.r));
        g = ewl_range_value_get(EWL_RANGE(cp->spinners.rgb.g));
        b = ewl_range_value_get(EWL_RANGE(cp->spinners.rgb.b));
        a = ewl_range_value_get(EWL_RANGE(w));

        ewl_widget_color_set(cp->preview.current, r, g, b, a);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: The colorpicker widget
 * @return Returns no value
 * @brief Callback for a radio button change
 */
void
ewl_colorpicker_cb_radio_change(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Colorpicker *cp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_COLORPICKER_RADIOBUTTON_TYPE);
        DCHECK_TYPE(data, EWL_COLORPICKER_TYPE);

        cp = data;
        cp->mode = ewl_colorpicker_radiobutton_mode_get(EWL_COLORPICKER_RADIOBUTTON(w));

        ewl_spectrum_mode_set(EWL_SPECTRUM(cp->picker.square), cp->mode);
        ewl_spectrum_mode_set(EWL_SPECTRUM(cp->picker.vertical), cp->mode);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param data: The colorpicker
 * @return Returns no value
 * @brief Callback for when a user clicks on the previous colour
 */
void
ewl_colorpicker_cb_previous_clicked(Ewl_Widget *w __UNUSED__,
                                        void *ev __UNUSED__, void *data)
{
        Ewl_Colorpicker *cp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_COLORPICKER_TYPE);

        cp = data;
        ewl_colorpicker_current_rgb_set(cp, cp->previous.r, cp->previous.g,
                                                        cp->previous.b);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param data: The colorpicker
 * @return Returns no value
 * @brief Callback for when a user clicks on the previous colour
 */
void
ewl_colorpicker_cb_dnd_data(Ewl_Widget *w, void *ev,
                                void *data __UNUSED__)
{
        unsigned int i;
        unsigned int curcolors[4];
        Ewl_Colorpicker *cp = EWL_COLORPICKER(w);
        Ewl_Event_Dnd_Data_Received *event = ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);
        DCHECK_TYPE(w, EWL_COLORPICKER_TYPE);

        ewl_colorpicker_current_rgb_get(cp, &curcolors[0], &curcolors[1],
                                                        &curcolors[2]);
        curcolors[3] = ewl_colorpicker_alpha_get(cp);

        if (event->format == 32) {
                unsigned long *colors;
                colors = event->data;
                for (i = 0; i < event->len && i < 4; i++)
                        curcolors[i] = colors[i] >> ((sizeof(long) * 8) - 8);
        }
        else if (event->format == 16) {
                unsigned short *colors;
                colors = event->data;
                for (i = 0; i < event->len && i < 4; i++)
                        curcolors[i] = colors[i] >> 8;
        }
        else {
                unsigned char *colors;
                colors = event->data;
                for (i = 0; i < event->len && i < 4; i++)
                        curcolors[i] = colors[i];
        }

        ewl_colorpicker_current_rgb_set(cp, curcolors[0], curcolors[1],
                        curcolors[2]);
        ewl_colorpicker_alpha_set(cp, curcolors[3]);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_colorpicker_display_update(Ewl_Colorpicker *cp, unsigned int r, unsigned int g,
                                unsigned int b, double h, double s, double v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cp);
        DCHECK_TYPE(cp, EWL_COLORPICKER_TYPE);

        if (cp->updating)
                DRETURN(DLEVEL_STABLE);

        cp->updating = TRUE;

        ewl_range_value_set(EWL_RANGE(cp->spinners.rgb.r), r);
        ewl_range_value_set(EWL_RANGE(cp->spinners.rgb.g), g);
        ewl_range_value_set(EWL_RANGE(cp->spinners.rgb.b), b);

        ewl_range_value_set(EWL_RANGE(cp->spinners.hsv.h), h);
        ewl_range_value_set(EWL_RANGE(cp->spinners.hsv.s), s * 100);
        ewl_range_value_set(EWL_RANGE(cp->spinners.hsv.v), v * 100);

        ewl_widget_color_set(cp->preview.current, r, g, b,
                                ewl_range_value_get(EWL_RANGE(cp->spinners.alpha)));
        ewl_callback_call(EWL_WIDGET(cp), EWL_CALLBACK_VALUE_CHANGED);

        cp->updating = FALSE;

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
        DCHECK_PARAM_PTR(sp);
        DCHECK_TYPE(sp, EWL_COLORPICKER_SPINNER_TYPE);

        sp->mode = mode;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Color_Mode
ewl_colorpicker_spinner_mode_get(Ewl_Colorpicker_Spinner *sp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(sp, EWL_COLOR_MODE_RGB_RED);
        DCHECK_TYPE_RET(sp, EWL_COLORPICKER_SPINNER_TYPE,
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
        DCHECK_PARAM_PTR(rb);
        DCHECK_TYPE(rb, EWL_COLORPICKER_RADIOBUTTON_TYPE);

        rb->mode = mode;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Color_Mode
ewl_colorpicker_radiobutton_mode_get(Ewl_Colorpicker_Radiobutton *rb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(rb, EWL_COLOR_MODE_RGB_RED);
        DCHECK_TYPE_RET(rb, EWL_COLORPICKER_RADIOBUTTON_TYPE,
                                        EWL_COLOR_MODE_RGB_RED);

        DRETURN_INT(rb->mode, DLEVEL_STABLE);
}
