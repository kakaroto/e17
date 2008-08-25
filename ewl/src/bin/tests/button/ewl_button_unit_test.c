/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_checkbutton.h"
#include "ewl_radiobutton.h"
#include "ewl_separator.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

static int test_constructor(char *buf, int len);
static int test_label_set_get(char *buf, int len);
static int test_image_null_get(char *buf, int len);
static int test_image_null_set_get(char *buf, int len);
static int test_image_nonexist_relative_set_get(char *buf, int len);
static int test_image_size_null_height_set_get(char *buf, int len);
static int test_image_size_null_height_set_get(char *buf, int len);
static int test_image_size_null_width_set_get(char *buf, int len);
static int test_image_size_match_set_get(char *buf, int len);
static int test_image_size_differ_set_get(char *buf, int len);
static int test_image_size_max_int_set_get(char *buf, int len);
static int test_image_alignment_set_get(char *buf, int len);
static int test_image_fill_policy_set_get(char *buf, int len);

Ewl_Unit_Test button_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"label set/get", test_label_set_get, NULL, -1, 0},
                {"image null get", test_image_null_get, NULL, -1, 0},
                {"image null set/get", test_image_null_set_get, NULL, -1, 0},
                {"image nonexistent set/get", test_image_nonexist_relative_set_get, NULL, -1, 0},
                {"image size null height set/get", test_image_size_null_height_set_get, NULL, -1 , 0},
                {"image size null width set/get", test_image_size_null_width_set_get, NULL, -1, 0},
                {"image size match set/get", test_image_size_match_set_get, NULL, -1, 0},
                {"image size differ set/get", test_image_size_differ_set_get, NULL, -1, 0},
                {"image size max int set/get", test_image_size_max_int_set_get, NULL, -1, 0},
                {"image alignment set/get", test_image_alignment_set_get, NULL, -1, 0},
                {"image fill policy set/get", test_image_fill_policy_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *button;
        int ret = 0;

        button = ewl_button_new();

        if (!EWL_BUTTON_IS(button))
        {
                LOG_FAILURE(buf, len, "returned button is not of the type"
                                " button");
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(button), EWL_BUTTON_TYPE))
        {
                LOG_FAILURE(buf, len, "button has wrong appearance");
                goto DONE;
        }
        if (!ewl_stock_type_get(EWL_STOCK(button)) == EWL_STOCK_NONE)
        {
                LOG_FAILURE(buf, len, "button has a stock type set");
                goto DONE;
        }
        if (!ewl_widget_focusable_get(button))
        {
                LOG_FAILURE(buf, len, "button is not focusable");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(button);

        return ret;
}


static int
test_label_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int ret = 0;

        button = ewl_button_new();

        ewl_button_label_set(EWL_BUTTON(button), "my_label");
        if (!strcmp("my_label", ewl_button_label_get(EWL_BUTTON(button)))) {
                ewl_button_label_set(EWL_BUTTON(button), NULL);
                if (ewl_button_label_get(EWL_BUTTON(button)))
                        LOG_FAILURE(buf, len, "label_get not NULL");
                else
                        ret = 1;
        }
        else
                LOG_FAILURE(buf, len, "label_get doesn't match label_set");

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_null_get(char *buf, int len)
{
        Ewl_Widget *button;
        int ret = 0;

        button = ewl_button_new();

        if (ewl_button_image_get(EWL_BUTTON(button)))
                LOG_FAILURE(buf, len, "image_get not NULL");
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_null_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        const char *val;
        int ret = 0;

        button = ewl_button_new();

        ewl_button_image_set(EWL_BUTTON(button), NULL, NULL);
        val = ewl_button_image_get(EWL_BUTTON(button));
        if (val)
                LOG_FAILURE(buf, len, "image_get %s when set to NULL", val);
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_nonexist_relative_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int ret = 0;

        button = ewl_button_new();

        ewl_button_image_set(EWL_BUTTON(button), "my_image", NULL);
        if (strcmp("my_image", ewl_button_image_get(EWL_BUTTON(button))))
                LOG_FAILURE(buf, len, "image_get dosen't match image_set");
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_size_null_height_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int w = 0;
        int ret = 0;

        button = ewl_button_new();

        ewl_button_image_size_get(EWL_BUTTON(button), &w, NULL);
        if (w != 0)
                LOG_FAILURE(buf, len, "image_size_get width not 0");
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_size_null_width_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int h = 0;
        int ret = 0;

        button = ewl_button_new();

        ewl_button_image_size_get(EWL_BUTTON(button), NULL, &h);
        if (h != 0)
                LOG_FAILURE(buf, len, "image_size_get height not 0");
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_size_match_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int w = 0, h = 0;
        int ret = 0;

        button = ewl_button_new();

        ewl_button_image_size_set(EWL_BUTTON(button), 32, 32);
        ewl_button_image_size_get(EWL_BUTTON(button), &w, &h);
        if (w != 32 || h != 32)
                LOG_FAILURE(buf, len, "image_size_get width and height don't match");
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_size_differ_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int w = 0, h = 0;
        int ret = 0;

        button = ewl_button_new();
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 24);
        ewl_button_image_size_get(EWL_BUTTON(button), &w, &h);
        if (w != 30 || h != 24)
                LOG_FAILURE(buf, len, "image_size_get width and height don't differ");
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_size_max_int_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int w = 0, h = 0;
        int ret = 0;

        button = ewl_button_new();
        ewl_button_image_size_set(EWL_BUTTON(button), INT_MAX, INT_MAX);
        ewl_button_image_size_get(EWL_BUTTON(button), &w, &h);
        if (w != INT_MAX|| h != INT_MAX)
                LOG_FAILURE(buf, len, "image_size_get width and height not INT_MAX");
        else
                ret = 1;

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_alignment_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int align;
        int ret = 0;

        button = ewl_button_new();
        ewl_button_alignment_set(EWL_BUTTON(button), EWL_FLAG_ALIGN_RIGHT);
        align = ewl_button_alignment_get(EWL_BUTTON(button));

        if (align == EWL_FLAG_ALIGN_RIGHT) {
                ewl_button_alignment_set(EWL_BUTTON(button),
                                EWL_FLAG_ALIGN_TOP);
                align = ewl_button_alignment_get(EWL_BUTTON(button));
                if (align == EWL_FLAG_ALIGN_TOP)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "image alignment not top");
        }
        else
                LOG_FAILURE(buf, len, "image alignment not right");

        ewl_widget_destroy(button);

        return ret;
}

static int
test_image_fill_policy_set_get(char *buf, int len)
{
        Ewl_Widget *button;
        int align;
        int ret = 0;

        button = ewl_button_new();
        ewl_button_fill_policy_set(EWL_BUTTON(button), EWL_FLAG_FILL_NONE);
        align = ewl_button_fill_policy_get(EWL_BUTTON(button));

        if (align == EWL_FLAG_FILL_NONE) {
                ewl_button_fill_policy_set(EWL_BUTTON(button),
                                EWL_FLAG_FILL_FILL);
                align = ewl_button_fill_policy_get(EWL_BUTTON(button));
                if (align == EWL_FLAG_FILL_FILL)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "image fill policy not fill");
        }
        else
                LOG_FAILURE(buf, len, "image fill policy not none");

        ewl_widget_destroy(button);

        return ret;
}
