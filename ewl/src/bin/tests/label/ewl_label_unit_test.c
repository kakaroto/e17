/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_paned.h"
#include "ewl_label.h"

#include <stdio.h>
#include <string.h>

static int test_null_get(char *buf, int len);
static int test_null_set_get(char *buf, int len);
static int test_set_get(char *buf, int len);

Ewl_Unit_Test label_unit_tests[] = {
                {"label null get", test_null_get, NULL, -1, 0},
                {"label null set/get", test_null_set_get, NULL, -1, 0},
                {"label set/get", test_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_null_get(char *buf, int len)
{
        Ewl_Widget *label;
        int ret = 0;

        label = ewl_label_new();

        if (ewl_label_text_get(EWL_LABEL(label)))
                LOG_FAILURE(buf, len, "text_get not NULL");
        else
                ret = 1;

        ewl_widget_destroy(label);

        return ret;
}

static int
test_null_set_get(char *buf, int len)
{
        Ewl_Widget *label;
        const char *val;
        int ret = 0;

        label = ewl_label_new();

        ewl_label_text_set(EWL_LABEL(label), "some text");
        ewl_label_text_set(EWL_LABEL(label), NULL);

        val = ewl_label_text_get(EWL_LABEL(label));
        if (val)
                LOG_FAILURE(buf, len, "text_set_get %s instead of NULL", val);
        else
                ret = 1;

        ewl_widget_destroy(label);

        return ret;
}

static int
test_set_get(char *buf, int len)
{
        Ewl_Widget *label;
        const char *val;
        int ret = 0;

        label = ewl_label_new();

        ewl_label_text_set(EWL_LABEL(label), "some text");

        val = ewl_label_text_get(EWL_LABEL(label));
        if (strcmp(val, "some text"))
                LOG_FAILURE(buf, len, "%s is not 'some text'", val);
        else
                ret = 1;

        ewl_widget_destroy(label);

        return ret;
}
