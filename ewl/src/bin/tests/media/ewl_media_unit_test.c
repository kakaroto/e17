/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_media.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test media_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *media;
        int ret = 0;

        media = ewl_media_new();

        if (!EWL_MEDIA_IS(media))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_MEDIA_TYPE);
        else if (ewl_media_media_get(EWL_MEDIA(media)))
                LOG_FAILURE(buf, len, "media has media set");
        else
                ret = 1;

        ewl_widget_destroy(media);

        return ret;
}

