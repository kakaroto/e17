/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_text_fmt.h"
#include "ewl_text_context.h"

#include <stdio.h>

static int tf_new(char *buf, int len);
static int tf_new_null_text(char *buf, int len);
static int tf_clear(char *buf, int len);

static int tf_count_empty(char *buf, int len);
static int tf_count_non_empty(char *buf, int len);

static int tf_prepend_empty(char *buf, int len);
static int tf_prepend_non_empty(char *buf, int len);
static int tf_append_empty(char *buf, int len);
static int tf_append_non_empty(char *buf, int len);
static int tf_insert_empty(char *buf, int len);
static int tf_insert_non_empty(char *buf, int len);

static int tf_delete_empty(char *buf, int len);
static int tf_delete_non_empty(char *buf, int len);
static int tf_delete_past_end(char *buf, int len);
static int tf_delete_all(char *buf, int len);
static int tf_delete_full_node(char *buf, int len);
static int tf_delete_partial_node(char *buf, int len);

static int tf_apply_empty_text(char *buf, int len);
static int tf_apply_full_text(char *buf, int len);
static int tf_apply_past_end_text(char *buf, int len);
static int tf_apply_full_node_text(char *buf, int len);
static int tf_apply_partial_node_text(char *buf, int len);

static Ewl_Unit_Test text_fmt_unit_tests[] = {
                {"New Format", tf_new, NULL, -1, 0},
                {"New NULL Text", tf_new_null_text, NULL, -1, 1},
                {"Clear Format", tf_clear, NULL, -1, 0},

                {"Count empty", tf_count_empty, NULL, -1, 0},
                {"Count non-empty", tf_count_non_empty, NULL, -1, 0},

                {"Prepend to empty", tf_prepend_empty, NULL, -1, 0},
                {"Prepend to non-empty", tf_prepend_non_empty, NULL, -1, 0},
                {"Append to empty", tf_append_empty, NULL, -1, 0},
                {"Append to non-empty", tf_append_non_empty, NULL, -1, 0},
                {"Insert to empty", tf_insert_empty, NULL, -1, 0},
                {"Insert to non-empty", tf_insert_non_empty, NULL, -1, 0},

                {"Delete from empty list", tf_delete_empty, NULL, -1, 0},
                {"Delete from non-empty list", tf_delete_non_empty, NULL, -1, 0},
                {"Delete past end text", tf_delete_past_end, NULL, -1, 0},
                {"Delete all", tf_delete_all, NULL, -1, 0},
                {"Delete full node", tf_delete_full_node, NULL, -1, 0},
                {"Delete partial node", tf_delete_partial_node, NULL, -1, 0},

                /* gets */
                /* goto's */

                /* apply */
                {"Apply empty text", tf_apply_empty_text, NULL, -1, 0},
                {"Apply full text", tf_apply_full_text, NULL, -1, 0},
                {"Apply past end text", tf_apply_past_end_text, NULL, -1, 0},
                {"Apply full node text", tf_apply_full_node_text, NULL, -1, 0},
                {"Apply partial node text", tf_apply_partial_node_text, NULL, -1, 0},

                /* char_to_byte */
                /* byte_to_char */

                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Text Format";
        test->tip = "Defines unit tests for the text formatting nodes.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_UNIT;
        test->unit_tests = text_fmt_unit_tests;
}

static int
tf_new(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_Text_Fmt *fmt;

        t = ewl_text_new();
        fmt = ewl_text_fmt_new(EWL_TEXT(t));
        if (!fmt)
        {
                LOG_FAILURE(buf, len, "_new returned NULL.");
                return 0;
        }
        ewl_widget_destroy(t);
        ewl_text_fmt_destroy(fmt);
        return 1;
}

static int
tf_new_null_text(char *buf, int len)
{
        Ewl_Text_Fmt *fmt;

        fmt = ewl_text_fmt_new(NULL);
        if (fmt)
        {
                LOG_FAILURE(buf, len, "_new returned fmt when Ewl_Text was NULL.");
                ewl_text_fmt_destroy(fmt);
                return 0;
        }
        return 1;
}

static int
tf_clear(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_Text_Fmt *fmt;
        Ewl_Text_Context *tx;

        tx = ewl_text_context_new();

        t = ewl_text_new();
        fmt = ewl_text_fmt_new(EWL_TEXT(t));
        ewl_text_fmt_node_prepend(fmt, tx, 20, 20);
        ewl_text_fmt_node_prepend(fmt, tx, 30, 30);

        ewl_text_fmt_clear(fmt);
        if (ecore_dlist_count(fmt->nodes) != 0)
        {
                LOG_FAILURE(buf, len, "_clear left items in -> nodes.");
                return 0;
        }
        if ((fmt->current_node.char_idx != 0)
                        || (fmt->current_node.byte_idx != 0))
        {
                LOG_FAILURE(buf, len, "_clear didn't reset current_node values.");
                return 0;
        }
        if ((fmt->length.char_len != 0)
                        || (fmt->length.byte_len != 0))
        {
                LOG_FAILURE(buf, len, "_clear didn't reset length values.");
                return 0;
        }

        ewl_text_fmt_destroy(fmt);
        ewl_widget_destroy(t);

        return 1;
}

static int
tf_count_empty(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_Text_Fmt *fmt;

        t = ewl_text_new();
        fmt = ewl_text_fmt_new(EWL_TEXT(t));

        if (ewl_text_fmt_node_count_get(fmt) != 0)
        {
                LOG_FAILURE(buf, len, "_count returned non-zero on blank list.");
                return 0;
        }
        return 1;
}

static int
tf_count_non_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_prepend_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_prepend_non_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_append_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_append_non_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_insert_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_insert_non_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_delete_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_delete_non_empty(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_delete_past_end(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_delete_all(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_delete_full_node(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_delete_partial_node(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_apply_empty_text(char *buf __UNUSED__, int len __UNUSED__)
{
        Ewl_Widget *t;

        t = ewl_text_new();
        ewl_text_color_apply(EWL_TEXT(t), 255, 0, 0, 255, 0);

        return 1;
}

static int
tf_apply_full_text(char *buf __UNUSED__, int len __UNUSED__)
{
        Ewl_Widget *t;

        t = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(t), "four");
        ewl_text_color_apply(EWL_TEXT(t), 255, 0, 0, 255, 4);

        return 1;
}

static int
tf_apply_past_end_text(char *buf __UNUSED__, int len __UNUSED__)
{
        Ewl_Widget *t;

        t = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(t), "four");
        ewl_text_color_apply(EWL_TEXT(t), 255, 0, 0, 255, 5);

        return 1;
}

static int
tf_apply_full_node_text(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}

static int
tf_apply_partial_node_text(char *buf __UNUSED__, int len __UNUSED__)
{
        return 1;
}
