/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_macros.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* unit tests */
static int constructor_test(char *buf, int len);
static int switch_parents(char *buf, int len);

static Ewl_Unit_Test container_unit_tests[] = {
                {"constructor", constructor_test, NULL, -1, 0},
                {"switch parents", switch_parents, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Container";
        test->tip = "The base container type.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_UNIT;
        test->unit_tests = container_unit_tests;
}

static int
constructor_test(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = NEW(Ewl_Container, 1);
        ewl_container_init(EWL_CONTAINER(c));

        if (!EWL_CONTAINER_IS(c))
        {
                LOG_FAILURE(buf, len, "returned widget is not of the type"
                                " " EWL_CONTAINER_TYPE);
                goto DONE;
        }
        if (!ewl_widget_recursive_get(c))
        {
                LOG_FAILURE(buf, len, "container is not recrusive");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

static int
switch_parents(char *buf, int len)
{
        Ewl_Widget *p1, *p2, *w;

        p1 = ewl_vbox_new();
        ewl_widget_show(p1);

        w = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(p1), w);
        ewl_widget_show(w);

        if (!(w->parent == p1))
        {
                snprintf(buf, len, "initial parent incorrect (%p vs %p)", w->parent, p1);
                return FALSE;
        }

        p2 = ewl_vbox_new();
        ewl_widget_show(p2);

        ewl_container_child_append(EWL_CONTAINER(p2), w);
        if (!(w->parent == p2))
        {
                snprintf(buf, len, "reparent incorrect (%p vs %p)", w->parent, p2);
                return FALSE;
        }

        return TRUE;
}


