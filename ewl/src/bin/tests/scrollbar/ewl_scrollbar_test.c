/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_scrollbar.h"

#include <stdio.h>

static int create_test(Ewl_Container *win);

extern Ewl_Unit_Test scrollbar_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Scrollbar";
        test->tip = "A scrollbar.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = scrollbar_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *hscrollbar, *vscrollbar;

        hscrollbar = ewl_hscrollbar_new();
        ewl_object_padding_set(EWL_OBJECT(hscrollbar), 10, 10, 10, 0);
        ewl_container_child_append(box, hscrollbar);
        ewl_widget_show(hscrollbar);

        vscrollbar = ewl_vscrollbar_new();
        ewl_object_padding_set(EWL_OBJECT(vscrollbar), 10, 10, 10, 10);
        ewl_container_child_append(box, vscrollbar);
        ewl_widget_show(vscrollbar);

        return 1;
}

