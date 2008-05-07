/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_image.h"
#include "ewl_shadow.h"

#include <stdio.h>

static int create_test(Ewl_Container *box);

void test_info(Ewl_Test *test)
{
        test->name = "Shadow";
        test->tip = "Defines the Ewl_Shadow class for displaying\n"
                        "a container that will shadow other widgets.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *shadow;
        Ewl_Widget *image;

        shadow = ewl_shadow_new();
        ewl_object_minimum_size_set(EWL_OBJECT(shadow), 280, 250);
        ewl_object_alignment_set(EWL_OBJECT(shadow), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(box), shadow);
        ewl_widget_show(shadow);

        image = ewl_image_new();
        ewl_image_file_set(EWL_IMAGE(image),
                        PACKAGE_DATA_DIR "/ewl/images/elicit.png", NULL);
        ewl_image_size_set(EWL_IMAGE(image), 220, 210);
        ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(shadow), image);
        ewl_widget_show(image);

        return 1;
}

