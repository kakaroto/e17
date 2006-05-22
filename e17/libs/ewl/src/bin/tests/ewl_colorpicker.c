#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>

/**
 * @addtogroup Ewl_Colorpicker
 * @section image_tut Tutorial
 *
 * The Ewl_Colorpicker widget provides a way to let the user select a color
 * either by manually entering its values, or graphically via the built-in
 * Ewl_Spectrum widget, which also provides a preview of the color.
 * The color can be selected in RGB or HSV format (but can be retrieved only
 * in RGB format for now).
 *
 * @code
 * Ewl_Widget *cp;
 * cp = ewl_colorpicker_new();
 * ewl_widget_show(cp);
 * @endcode
 *
 * Once you've setup the colour picker, you can read its current and previous
 * value and set them. For example the following swaps the two.
 *
 * @code
 * unsigned int r,g,b;
 * ewl_colorpicker_current_rgb_get(EWL_COLORPICKER(cp), &r, &g, &b);
 * ewl_colorpicker_previous_rgb_set(EWL_COLORPICKER(cp), r, g, b);
 * @endcode
 *
 * You can also change the display mode of the spectrum programmatically
 *
 * @code
 * void         ewl_colorpicker_color_mode_set(Ewl_Colorpicker *cp, Ewl_Color_Mode type);
 * Ewl_Color_Mode       ewl_colorpicker_color_mode_get(Ewl_Colorpicker *cp);
 * @endcode
 *
 */

static int create_test(Ewl_Container *box);
static void color_value_changed(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Colorpicker";
	test->tip = "Defines a colour picker.";
	test->filename = "ewl_colorpicker.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_MISC;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *colorpicker;

	colorpicker = ewl_colorpicker_new();
	ewl_container_child_append(EWL_CONTAINER(box), colorpicker);
	ewl_callback_append(colorpicker, EWL_CALLBACK_VALUE_CHANGED,
					    color_value_changed, NULL);
	ewl_widget_show(colorpicker);

	return 1;
}

static void
color_value_changed(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	int r, g, b;

	ewl_colorpicker_current_rgb_get(EWL_COLORPICKER(w), &r, &g, &b);
	printf("value changed to (%i, %i, %i)\n", r, g, b);
}


