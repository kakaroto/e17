/* vim: set sw=8 ts=8 sts=8 expandtab: */
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

void
test_info(Ewl_Test *test)
{
        test->name = "Image Thumbnail";
        test->tip = "Provides a widget for displaying\n"
                                "thumbnails of images.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
}

int
create_test(Ewl_Container *box)
{
        Ewl_Widget *image;
        Ewl_Widget *thumb;
        Ewl_Widget *image_box;
        char *image_file = NULL;
        char *icon_file = NULL;

        image_box = EWL_WIDGET(box);

        if ((ecore_file_exists(PACKAGE_DATA_DIR "/ewl/images/e-logo.png")) != -1)
                image_file = strdup(PACKAGE_DATA_DIR "/ewl/images/e-logo.png");
        else if ((ecore_file_exists(PACKAGE_SOURCE_DIR "/data/images/e-logo.png")) != -1)
                image_file = strdup(PACKAGE_SOURCE_DIR "/data/images/e-logo.png");
        else if ((ecore_file_exists("./data/images/e-logo.png")) != -1)
                image_file = strdup("./data/images/e-logo.png");
        else if ((ecore_file_exists("../data/images/e-logo.png")) != -1)
                image_file = strdup("../data/images/e-logo.png");

        image = ewl_image_new();
        ewl_image_file_set(EWL_IMAGE(image), image_file, NULL);
        ewl_object_padding_set(EWL_OBJECT(image), 0, 0, 5, 0);
        ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(image_box), image);
        ewl_widget_show(image);

        if ((ecore_file_exists(PACKAGE_DATA_DIR "/ewl/images/Draw.png")) != -1)
                icon_file = strdup(PACKAGE_DATA_DIR "/ewl/images/Draw.png");
        else if ((ecore_file_exists(PACKAGE_SOURCE_DIR "/data/images/Draw.png")) != -1)
                icon_file = strdup(PACKAGE_SOURCE_DIR "/data/images/Draw.png");
        else if ((ecore_file_exists("./data/images/Draw.png")) != -1)
                icon_file = strdup("./data/images/Draw.png");
        else if ((ecore_file_exists("../data/images/Draw.png")) != -1)
                icon_file = strdup("../data/images/Draw.png");


        thumb = ewl_image_thumbnail_get(EWL_IMAGE(image));
        ewl_image_constrain_set(EWL_IMAGE(thumb), 32);
        if (icon_file)
                ewl_image_file_path_set(EWL_IMAGE(thumb), icon_file);
        ewl_container_child_append(EWL_CONTAINER(image_box), thumb);
        ewl_widget_show(thumb);

        return 1;
}
