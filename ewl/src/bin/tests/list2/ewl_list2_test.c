/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_image.h"
#include "ewl_list2.h"
#include "ewl_label.h"
#include "ewl_tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore_File.h>

static int create_test(Ewl_Container *win);

void
test_info(Ewl_Test *test)
{
        test->name = "List2";
        test->tip = "Defines a widget for laying out other\n"
                        "widgets in a list like manner.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
}

static void *
data_fetch(void *data __UNUSED__, unsigned int row,
                               unsigned int column __UNUSED__)
{
        static char buffer[128];

        snprintf(buffer, sizeof(buffer), "Row Item %u", row + 1);
        return buffer;
}

static unsigned int
data_count(void *data)
{
        return (unsigned int)data;
}

static void
list2_destroy_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
        if (data)
                ewl_model_destroy(data);
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *list;
        Ewl_Model *model;
        const Ewl_View *view;

        model = ewl_model_new();
        ewl_model_data_count_set(model, data_count);
        ewl_model_data_fetch_set(model, data_fetch);
        view = ewl_label_view_get();

        /* Create a very long list to test performance */
        list = ewl_list2_new();
        ewl_container_child_append(EWL_CONTAINER(box), list);
        ewl_list2_size_acquisition_set(EWL_LIST2(list),
                        EWL_SIZE_ACQUISITION_GROW);
        ewl_mvc_model_set(EWL_MVC(list), model);
        ewl_mvc_view_set(EWL_MVC(list), view);
        ewl_mvc_data_set(EWL_MVC(list), (void *)10000);
        ewl_callback_append(list, EWL_CALLBACK_DESTROY, list2_destroy_cb,
                        model);
        ewl_widget_show(list);

        return 1;
}


