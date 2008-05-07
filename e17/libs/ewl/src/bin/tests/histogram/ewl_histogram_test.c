/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_overlay.h"
#include "ewl_histogram.h"
#include "ewl_image.h"

#include <stdio.h>

static int create_test(Ewl_Container *box);
static void ewl_histogram_test_cb_configure(Ewl_Widget *w, void *event, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Histogram";
        test->tip = "Defines an image histogram.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *overlay;
        Ewl_Widget *img;
        Ewl_Widget *hist;

        overlay = ewl_overlay_new();
        ewl_container_child_append(EWL_CONTAINER(box), overlay);
        ewl_object_fill_policy_set(EWL_OBJECT(overlay), EWL_FLAG_FILL_ALL);
         ewl_callback_append(EWL_WIDGET(overlay), EWL_CALLBACK_CONFIGURE,
                        ewl_histogram_test_cb_configure, NULL);
        ewl_widget_show(overlay);

        img = ewl_image_new();
        ewl_image_file_path_set(EWL_IMAGE(img),
                        PACKAGE_DATA_DIR "/ewl/images/entrance.png");
        ewl_image_proportional_set(EWL_IMAGE(img), TRUE);
        ewl_container_child_append(EWL_CONTAINER(overlay), img);
        ewl_object_fill_policy_set(EWL_OBJECT(img), EWL_FLAG_FILL_ALL);
        ewl_widget_show(img);

        hist = ewl_histogram_new();
        ewl_histogram_channel_set(EWL_HISTOGRAM(hist), EWL_HISTOGRAM_CHANNEL_Y);
        ewl_histogram_image_set(EWL_HISTOGRAM(hist), EWL_IMAGE(img));
        ewl_container_child_append(EWL_CONTAINER(overlay), hist);
        ewl_object_fill_policy_set(EWL_OBJECT(hist), EWL_FLAG_FILL_ALL);
        ewl_widget_show(hist);

        hist = ewl_histogram_new();
        ewl_histogram_channel_set(EWL_HISTOGRAM(hist), EWL_HISTOGRAM_CHANNEL_R);
        ewl_histogram_image_set(EWL_HISTOGRAM(hist), EWL_IMAGE(img));
        ewl_container_child_append(EWL_CONTAINER(overlay), hist);
        ewl_object_fill_policy_set(EWL_OBJECT(hist), EWL_FLAG_FILL_ALL);
        ewl_widget_show(hist);

        hist = ewl_histogram_new();
        ewl_histogram_channel_set(EWL_HISTOGRAM(hist), EWL_HISTOGRAM_CHANNEL_G);
        ewl_histogram_image_set(EWL_HISTOGRAM(hist), EWL_IMAGE(img));
        ewl_container_child_append(EWL_CONTAINER(overlay), hist);
        ewl_object_fill_policy_set(EWL_OBJECT(hist), EWL_FLAG_FILL_ALL);
        ewl_widget_show(hist);

        hist = ewl_histogram_new();
        ewl_histogram_channel_set(EWL_HISTOGRAM(hist), EWL_HISTOGRAM_CHANNEL_B);
        ewl_histogram_image_set(EWL_HISTOGRAM(hist), EWL_IMAGE(img));
        ewl_container_child_append(EWL_CONTAINER(overlay), hist);
        ewl_object_fill_policy_set(EWL_OBJECT(hist), EWL_FLAG_FILL_ALL);
        ewl_widget_show(hist);

        return 1;
}

static void
ewl_histogram_test_cb_configure(Ewl_Widget *w, void *event __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *child;
        Ewl_Container *c = EWL_CONTAINER(w);

        ewl_container_child_iterate_begin(c);
        while ((child = ewl_container_child_next(c))) {
                ewl_object_place(EWL_OBJECT(child), CURRENT_X(c), CURRENT_Y(c),
                                CURRENT_W(c), CURRENT_H(c));
        }
}

