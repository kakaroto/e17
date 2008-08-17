/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_colorpicker.h"

#include <stdio.h>

static int create_test(Ewl_Container *box);
static void color_value_changed(Ewl_Widget *w, void *ev, void *data);

/* unit tests */
static int constructor_test(char *buf, int len);

static Ewl_Unit_Test colorpicker_unit_tests[] = {
                {"constructor", constructor_test, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

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

static int
constructor_test(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;
        unsigned int r, g, b;

        c = ewl_colorpicker_new();

        if (!EWL_COLORPICKER_IS(c))
        {
                LOG_FAILURE(buf, len, "returned color picker is not of the type"
                                " " EWL_COLORPICKER_TYPE);
                goto DONE;
        }
        if (!ewl_colorpicker_color_mode_get(EWL_COLORPICKER(c)) 
                        == EWL_COLOR_MODE_HSV_HUE)
        {
                LOG_FAILURE(buf, len, "colorpicker's color mode differs");
                goto DONE;
        }
        ewl_colorpicker_current_rgb_get(EWL_COLORPICKER(c), &r, &g, &b);
        if (r != 0 || g != 0 || b != 0)
        {
                LOG_FAILURE(buf, len, "colorpicker's color is not black");
                goto DONE;
        }


        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

