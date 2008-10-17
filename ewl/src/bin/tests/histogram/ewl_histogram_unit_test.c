/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_histogram.h"

#include <stdio.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test histogram_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *his;
        int ret = 0;
        unsigned int r, g, b, a;

        his = ewl_histogram_new();
        ewl_histogram_color_get(EWL_HISTOGRAM(his), &r, &g, &b, &a);

        if (!EWL_HISTOGRAM_IS(his))
                LOG_FAILURE(buf, len, "widget is not of the type "
                                EWL_HISTOGRAM_TYPE);
        else if (r != 0 && g != 0 && b != 0 && a != 0)
                LOG_FAILURE(buf, len, "color is not 0x00000000");
        else if (ewl_histogram_channel_get(EWL_HISTOGRAM(his))
                       !=  EWL_HISTOGRAM_CHANNEL_R)
                LOG_FAILURE(buf, len, "histogram doesn't use the red channel");
        else if (ewl_histogram_image_get(EWL_HISTOGRAM(his)))
                LOG_FAILURE(buf, len, "histogram has already an image set");
        else
                ret = 1;

        ewl_widget_destroy(his);

        return ret;
}

