/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_icon.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);
static int test_constructor_simple(char *buf, int len);

Ewl_Unit_Test icon_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"simple constructor", test_constructor_simple, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *i;
        int ret = 0;

        i = ewl_icon_new();
        
        if (!EWL_ICON_IS(i))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_ICON_TYPE);
        else if (ewl_icon_type_get(EWL_ICON(i)) != EWL_ICON_TYPE_SHORT)
                LOG_FAILURE(buf, len, "icon is not of the type short");
        else if (ewl_icon_image_file_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon image file is  not NULL");
        else if (!ewl_icon_thumbnailing_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is not set to thumbnail");
        else if (ewl_icon_constrain_get(EWL_ICON(i)) != 16)
                LOG_FAILURE(buf, len, "icon has differnet constrain");
        else if (ewl_icon_alt_text_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon has alt text");
        else if (ewl_icon_label_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon has a label text");
        else if (ewl_icon_editable_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is editable");
        else if (ewl_icon_label_compressed_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is compressed");
        else if (!ewl_icon_label_complex_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is not complex");
        else
                ret = 1;

        ewl_widget_destroy(i);

        return ret;
}

static int
test_constructor_simple(char *buf, int len)
{
        Ewl_Widget *i;
        int ret = 0;

        i = ewl_icon_simple_new();
        
        if (!EWL_ICON_IS(i))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_ICON_TYPE);
        else if (ewl_icon_type_get(EWL_ICON(i)) != EWL_ICON_TYPE_SHORT)
                LOG_FAILURE(buf, len, "icon is not of the type short");
        else if (ewl_icon_image_file_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon image file is  not NULL");
        else if (ewl_icon_thumbnailing_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is set to thumbnail");
        else if (ewl_icon_constrain_get(EWL_ICON(i)) != 16)
                LOG_FAILURE(buf, len, "icon has different constrain");
        else if (ewl_icon_alt_text_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon has alt text");
        else if (ewl_icon_label_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon has a label text");
        else if (ewl_icon_editable_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is editable");
        else if (ewl_icon_label_compressed_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is compressed");
        else if (ewl_icon_label_complex_get(EWL_ICON(i)))
                LOG_FAILURE(buf, len, "icon is complex");
        else
                ret = 1;

        ewl_widget_destroy(i);

        return ret;
}

