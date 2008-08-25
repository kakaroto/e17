/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_colorpicker.h"

#include <stdio.h>

extern Ewl_Unit_Test colorpicker_unit_tests[];

static int create_test(Ewl_Container *box);
static void color_value_changed(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Colorpicker";
        test->tip = "Defines a colour picker.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = colorpicker_unit_tests;
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
        unsigned int r, g, b;

        ewl_colorpicker_current_rgb_get(EWL_COLORPICKER(w), &r, &g, &b);
        printf("value changed to (%u, %u, %u)\n", r, g, b);
}
