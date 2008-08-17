/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_paned.h"
#include "ewl_label.h"

#include <stdio.h>
#include <string.h>

static int counter = 0;
static int create_test(Ewl_Container *win);
static void cb_click(Ewl_Widget *w, void *ev, void *data);

static int label_null_test_get(char *buf, int len);
static int label_null_test_set_get(char *buf, int len);
static int label_test_set_get(char *buf, int len);

static Ewl_Unit_Test label_unit_tests[] = {
                {"label null get", label_null_test_get, NULL, -1, 0},
                {"label null set/get", label_null_test_set_get, NULL, -1, 0},
                {"label set/get", label_test_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Label";
        test->tip = "Defines a widget for displaying a label";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = label_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *body, *o2, *o, *p;

        body = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), body);
        ewl_widget_show(body);

        p = ewl_hpaned_new();
        ewl_container_child_append(EWL_CONTAINER(body), p);
        ewl_widget_show(p);

        o = ewl_label_new();
        ewl_container_child_append(EWL_CONTAINER(p), o);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_label_text_set(EWL_LABEL(o), "A non-shrinkable, centered  label");
        ewl_widget_show(o);

        o = ewl_label_new();
        ewl_container_child_prepend(EWL_CONTAINER(p), o);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HSHRINKABLE);
        ewl_label_text_set(EWL_LABEL(o), "Shrinkable label");
        ewl_widget_show(o);

        o2 = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o2), "Change Label");
        ewl_container_child_append(EWL_CONTAINER(body), o2);
        ewl_callback_append(o2, EWL_CALLBACK_CLICKED, cb_click, o);
        ewl_widget_show(o2);

        return 1;
}

void
cb_click(Ewl_Widget *w __UNUSED__, void *e __UNUSED__, void *data)
{
        if ((counter % 2) == 0)
                ewl_label_text_set(EWL_LABEL(data), "Still Shrinkable label");
        else
                ewl_label_text_set(EWL_LABEL(data), "Shrinkable label");

        counter ++;
}

static int
label_null_test_get(char *buf, int len)
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
label_null_test_set_get(char *buf, int len)
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
label_test_set_get(char *buf, int len)
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
