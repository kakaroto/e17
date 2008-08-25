/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_border.h"
#include "ewl_radiobutton.h"
#include "ewl_dialog.h"
#include "ewl_image.h"
#include "ewl_icon_theme.h"
#include "ewl_label.h"
#include "ewl_text.h"
#include <stdio.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test dialog_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_dialog_new();

        if (!EWL_DIALOG_IS(c))
        {
                LOG_FAILURE(buf, len, "returned dialog is not of the type"
                                " " EWL_DIALOG_TYPE);
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(c), EWL_DIALOG_TYPE))
        {
                LOG_FAILURE(buf, len, "the dialog's appearance differs to "
                                EWL_DIALOG_TYPE);
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

