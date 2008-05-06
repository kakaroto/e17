/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_scrollpane.h"

#include <stdio.h>

static int create_test(Ewl_Container *box);

void
test_info(Ewl_Test *test)
{
        test->name = "Scrollpane";
        test->tip = "A scrollpane.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *scrollpane;
        Ewl_Widget *button;

        scrollpane = ewl_scrollpane_new();
        ewl_container_child_append(box, scrollpane);
        ewl_widget_show(scrollpane);

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Scroll!");
        ewl_object_minimum_size_set(EWL_OBJECT(button), 500, 500);
        ewl_object_padding_set(EWL_OBJECT(button), 20, 20, 20, 20);
        ewl_container_child_append(EWL_CONTAINER(scrollpane), button);
        ewl_widget_show(button);

        return 1;
}

