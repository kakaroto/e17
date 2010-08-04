#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_image.h"

#include <Ecore_File.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static int create_test(Ewl_Container *box);

extern Ewl_Unit_Test image_thumbnail_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Image Thumbnail";
        test->tip = "Provides a widget for displaying\n"
                                "thumbnails of images.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = image_thumbnail_unit_tests;
}

int
create_test(Ewl_Container *box)
{
        Ewl_Widget *image;
        Ewl_Widget *thumb;

        image = ewl_image_new();
        ewl_image_file_set(EWL_IMAGE(image), ewl_test_image_get("e-logo.png"),
                        NULL);
        ewl_object_padding_type_top_set(EWL_OBJECT(image), EWL_PADDING_MEDIUM);
        ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(box, image);
        ewl_widget_show(image);

        thumb = ewl_image_thumbnail_get(EWL_IMAGE(image));
        ewl_image_constrain_set(EWL_IMAGE(thumb), 32);
        ewl_image_file_path_set(EWL_IMAGE(thumb),
                                ewl_test_image_get("Draw.png"));
        ewl_container_child_append(box, thumb);
        ewl_widget_show(thumb);

        return 1;
}
