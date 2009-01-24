/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_image.h"
#include "ewl_list2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int
create_test(Ewl_Container *box)
{
        Ecore_List *long_data;
        Ewl_Widget *list, *o;
        Ewl_Model *model;
        const Ewl_View *view;
        int i;

        model = ewl_model_ecore_list_instance();
        view = ewl_image_view_get();

        /* Create a very long list to test performance */
        o = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        long_data = ecore_list_new();
        for (i = 0; i < 10000; i++)
                ecore_list_append(long_data, PACKAGE_DATA_DIR
                                                "/ewl/images/e-logo.png");

        list = ewl_list2_new();
        ewl_container_child_append(EWL_CONTAINER(o), list);
        ewl_list2_fixed_size_set(EWL_LIST2(list), 1);
        ewl_mvc_model_set(EWL_MVC(list), model);
        ewl_mvc_view_set(EWL_MVC(list), view);
        ewl_mvc_data_set(EWL_MVC(list), long_data);
        ewl_widget_show(list);

        return 1;
}


