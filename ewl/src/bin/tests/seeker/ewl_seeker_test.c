/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_seeker.h"

#include <stdio.h>

static int create_test(Ewl_Container *win);
static void cb_print_value(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test seeker_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Seeker";
        test->tip = "A seeker widget.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = seeker_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *hseeker, *vseeker;

        hseeker = ewl_hseeker_new();
        ewl_object_position_request(EWL_OBJECT(hseeker), 30, 0);
        ewl_callback_append(hseeker, EWL_CALLBACK_VALUE_CHANGED,
                                                cb_print_value, NULL);
        ewl_container_child_append(box, hseeker);
        ewl_widget_show(hseeker);

        vseeker = ewl_vseeker_new();
        ewl_object_position_request(EWL_OBJECT(vseeker), 0, 30);
        ewl_callback_append(vseeker, EWL_CALLBACK_VALUE_CHANGED,
                                                cb_print_value, NULL);
        ewl_container_child_append(box, vseeker);
        ewl_widget_show(vseeker);

        return 1;
}

static void
cb_print_value(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Range *r;

        r = EWL_RANGE(w);
        printf("Seeker set to %g\n", ewl_range_value_get(r));
}


