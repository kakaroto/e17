/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_border.h"
#include "ewl_label.h"
#include "ewl_radiobutton.h"
#include "ewl_separator.h"

#include <stdio.h>
#include <string.h>

static int test_constructor(char *buf, int len);
static int test_label_set_get(char *buf, int len);
static int test_label_position_set_get(char *buf, int len);
static int test_label_alignment_set_get(char *buf, int len);

Ewl_Unit_Test border_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"Border label set/get", test_label_set_get, NULL, -1, 0},
                {"Border label position set/get", test_label_position_set_get, NULL, -1, 0},
                {"Border label alignment set/get", test_label_alignment_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };


static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *b;
        int ret = 0;

        b = ewl_border_new();

        if (!EWL_BORDER_IS(b))
        {
                LOG_FAILURE(buf, len, "returned border widget  is not of the"
                               " type border");
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(b), EWL_BORDER_TYPE))
        {
                LOG_FAILURE(buf, len, "border has wrong appearance");
                goto DONE;
        }
        if (!ewl_border_label_position_get(EWL_BORDER(b)) == EWL_POSITION_TOP)
        {
                LOG_FAILURE(buf, len, "the label position of the border "
                                "is wrong");
                goto DONE;
        }
        if (ewl_widget_focusable_get(b))
        {
                LOG_FAILURE(buf, len, "border widget is focusable");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(b);

        return ret;
}

static int
test_label_set_get(char *buf, int len)
{
        Ewl_Widget *border;
        int ret = 0;

        border = ewl_border_new();
        if (ewl_border_label_get(EWL_BORDER(border)))
                LOG_FAILURE(buf, len, "default border label set");
        else {
                const char *label;

                ewl_border_label_set(EWL_BORDER(border), "label value");

                label = ewl_border_label_get(EWL_BORDER(border));
                if (label && !strcmp(label, "label value"))
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "border label doesn't match");
        }

        ewl_widget_destroy(border);

        return ret;
}

static int
test_label_position_set_get(char *buf, int len)
{
        Ewl_Widget *border;
        unsigned int pos;
        int ret = 0;

        border = ewl_border_new();
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_TOP) {
                LOG_FAILURE(buf, len, "default border label position wrong");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_LEFT);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_LEFT) {
                LOG_FAILURE(buf, len, "border label position not left");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_RIGHT);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_RIGHT) {
                LOG_FAILURE(buf, len, "border label position not right");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_BOTTOM);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_BOTTOM) {
                LOG_FAILURE(buf, len, "border label position not bottom");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_TOP);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_TOP) {
                LOG_FAILURE(buf, len, "border label position not top");
                goto POSITION_ERROR;
        }

        ret = 1;

POSITION_ERROR:
        ewl_widget_destroy(border);

        return ret;
}

static int
test_label_alignment_set_get(char *buf, int len)
{
        Ewl_Widget *border;
        int align;
        int req_align;
        int ret = 0;

        border = ewl_border_new();
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_LEFT) {
                LOG_FAILURE(buf, len, "default border label alignment wrong");
                goto POSITION_ERROR;
        }

        req_align = (EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_RIGHT |
                                EWL_FLAG_ALIGN_BOTTOM | EWL_FLAG_ALIGN_TOP |
                                EWL_FLAG_ALIGN_CENTER);

        while (req_align >= 0) {

                ewl_border_label_alignment_set(EWL_BORDER(border), req_align);
                align = ewl_border_label_alignment_get(EWL_BORDER(border));
                if (align != req_align) {
                        LOG_FAILURE(buf, len,
                                        "border label alignment %x does not "
                                        "match requested %x", align, req_align);
                        goto POSITION_ERROR;
                }

                --req_align;
        }

        ewl_border_label_alignment_set(EWL_BORDER(border),
                        EWL_FLAG_ALIGN_RIGHT);
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_RIGHT) {
                LOG_FAILURE(buf, len, "border label alignment not right");
                goto POSITION_ERROR;
        }

        ewl_border_label_alignment_set(EWL_BORDER(border),
                        EWL_FLAG_ALIGN_BOTTOM);
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_BOTTOM) {
                LOG_FAILURE(buf, len, "border label alignment not bottom");
                goto POSITION_ERROR;
        }

        ewl_border_label_alignment_set(EWL_BORDER(border),
                        EWL_FLAG_ALIGN_TOP);
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_TOP) {
                LOG_FAILURE(buf, len, "border label alignment not top");
                goto POSITION_ERROR;
        }

        ret = 1;

POSITION_ERROR:
        ewl_widget_destroy(border);

        return ret;
}
