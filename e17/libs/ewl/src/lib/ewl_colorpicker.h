/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_COLORPICKER_H
#define EWL_COLORPICKER_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Colorpicker Ewl_Colorpicker: The colour picking widget
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Colorpicker_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_COLORPICKER_TYPE
 * The type name for the Ewl_Colorpicker widget
 */
#define EWL_COLORPICKER_TYPE "colorpicker"

/**
 * @def EWL_COLORPICKER_IS(w)
 * Returns TRUE if the widget is an Ewl_Colorpicker, FALSE otherwise
 */
#define EWL_COLORPICKER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_COLORPICKER_TYPE))

/**
 * @def EWL_COLORPICKER(cp)
 * Typecast a pointer to an Ewl_Colorpicker pointer.
 */
#define EWL_COLORPICKER(cp) ((Ewl_Colorpicker *) cp)

/**
 * The Ewl_Colorpicker
 */
typedef struct Ewl_Colorpicker Ewl_Colorpicker;

/**
 * @brief Inherits from Ewl_Box to produce a color picker widget
 */
struct Ewl_Colorpicker
{
        Ewl_Box box;                        /**< Inherit from Ewl_Box */
        struct
        {
                Ewl_Widget *square;        /**< The square picker box */
                Ewl_Widget *vertical;        /**< The vertical picker box */
        } picker;                        /**< This holds the two picker boxes */

        struct
        {
                Ewl_Widget *current;        /**< The current colour preview */
                Ewl_Widget *previous;        /**< The previous colour preview */
        } preview;                        /**< The preview boxes */

        struct
        {
                struct
                {
                        Ewl_Widget *r;        /**< The red value */
                         Ewl_Widget *g;        /**< The green value */
                        Ewl_Widget *b;        /**< The blue value */
                } rgb;                        /**< The pickers RGB values */

                struct
                {
                        Ewl_Widget *h;        /**< The current hue */
                        Ewl_Widget *s;        /**< The current saturation */
                        Ewl_Widget *v;        /**< The current value */
                } hsv;                        /**< The pickers HSV values */

                Ewl_Widget *alpha;        /**< The alpha value */
        } spinners;                        /**< The spinners to hold the different values */

        Ewl_Widget *alpha_box;                /**< Box holding alpha information */

        Ewl_Color_Set previous;                /**< The previous color */
        Ewl_Color_Mode mode;                /**< The current mode */

        unsigned int has_alpha:1;        /**< Does the picker deal with alpha values */
        unsigned int updating:1;        /**< Are we currently updating the widget */
};

Ewl_Widget              *ewl_colorpicker_new(void);
int                      ewl_colorpicker_init(Ewl_Colorpicker *cp);

void                     ewl_colorpicker_has_alpha_set(Ewl_Colorpicker *cp,
                                        unsigned int alpha);
unsigned int             ewl_colorpicker_has_alpha_get(Ewl_Colorpicker *cp);

void                     ewl_colorpicker_alpha_set(Ewl_Colorpicker *cp,
                                        unsigned int alpha);
unsigned int             ewl_colorpicker_alpha_get(Ewl_Colorpicker *cp);

void                     ewl_colorpicker_current_rgb_set(Ewl_Colorpicker *cp,
                                        unsigned int r, unsigned int g,
                                        unsigned int b);
void                     ewl_colorpicker_current_rgb_get(Ewl_Colorpicker *cp,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b);

void                     ewl_colorpicker_previous_rgba_set(Ewl_Colorpicker *cp,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void                     ewl_colorpicker_previous_rgba_get(Ewl_Colorpicker *cp,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a);

void                     ewl_colorpicker_color_mode_set(Ewl_Colorpicker *cp,
                                        Ewl_Color_Mode type);
Ewl_Color_Mode           ewl_colorpicker_color_mode_get(Ewl_Colorpicker *cp);

 /*
  * * internally used callbacks, override at your own risk.
  *   */
void ewl_colorpicker_cb_square_change(Ewl_Widget *w, void *ev, void *data);
void ewl_colorpicker_cb_vertical_change(Ewl_Widget *w, void *ev, void *data);

void ewl_colorpicker_cb_spinner_change(Ewl_Widget *w, void *ev, void *data);
void ewl_colorpicker_cb_alpha_change(Ewl_Widget *w, void *ev, void *data);
void ewl_colorpicker_cb_radio_change(Ewl_Widget *w, void *ev, void *data);

void ewl_colorpicker_cb_previous_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_colorpicker_cb_dnd_data(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

