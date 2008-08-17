/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int create_test(Ewl_Container *box);
static void ewl_calendar_test(Ewl_Widget *w, void *ev, void *data);

/* unit tests */
static int constructor_test(char *buf, int len);

static Ewl_Unit_Test calendar_unit_tests[] = {
                {"constructor", constructor_test, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };


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

static int
constructor_test(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_calendar_new();

        if (!EWL_CALENDAR_IS(c))
        {
                LOG_FAILURE(buf, len, "returned calendar is not of the type"
                                " calendar");
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(c), EWL_CALENDAR_TYPE))
        {
                LOG_FAILURE(buf, len, "calendar has wrong appearance");
                goto DONE;
        }
        if (!ewl_object_fill_policy_get(EWL_OBJECT(c)) == EWL_FLAG_FILL_FILL)
        {
                LOG_FAILURE(buf, len, "calendar's fill policy is not set to fill");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

