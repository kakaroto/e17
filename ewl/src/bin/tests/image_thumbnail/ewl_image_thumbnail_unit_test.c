/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test image_thumbnail_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_image_thumbnail_new();

        if (!EWL_IMAGE_THUMBNAIL_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_IMAGE_THUMBNAIL_TYPE);
        else if (ewl_image_thumbnail_size_get(EWL_IMAGE_THUMBNAIL(s))
                        != EWL_THUMBNAIL_SIZE_NORMAL)
                LOG_FAILURE(buf, len, "thumbnail is set to large thumbs");
        else
                ret = 1;

        ewl_widget_destroy(s);

        return ret;
}

