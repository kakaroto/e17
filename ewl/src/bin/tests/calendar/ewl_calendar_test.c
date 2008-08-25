/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Ewl_Unit_Test calendar_unit_tests[];

static int create_test(Ewl_Container *box);
static void ewl_calendar_test(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Calendar";
        test->tip = "Defines a calendar widget.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = calendar_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *cal;

        cal = ewl_calendar_new();
        ewl_container_child_append(EWL_CONTAINER(box), cal);
        ewl_callback_append(EWL_WIDGET(cal),
                        EWL_CALLBACK_VALUE_CHANGED, ewl_calendar_test,
                        cal);
        ewl_widget_show(cal);

        return 1;
}

static void
ewl_calendar_test(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
        char *date;

        date = ewl_calendar_ascii_time_get(EWL_CALENDAR(data));
        printf ("Selected: %s\n", date);
        free(date);
}
