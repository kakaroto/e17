/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_colorpicker.h"

#include <stdio.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test colorpicker_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
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

