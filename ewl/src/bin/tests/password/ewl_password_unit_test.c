/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"
#include "ewl_label.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test password_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *e;
        int ret = 0;
        const char *obscure;

        e = ewl_password_new();

        if (!EWL_ENTRY_IS(e))
        {
                LOG_FAILURE(buf, len, "returned widget is not of the type"
                                " " EWL_ENTRY_TYPE);
                goto DONE;
        }
        if (ewl_object_fill_policy_get(EWL_OBJECT(e)) != (EWL_FLAG_FILL_HSHRINK
                                | EWL_FLAG_FILL_HFILL))
        {
                LOG_FAILURE(buf, len, "default fill policy is wrong");
                goto DONE;
        }
        if (!ewl_widget_focusable_get(e))
        {
                LOG_FAILURE(buf, len, "entry is not focusable");
                goto DONE;
        }
        
        obscure = ewl_text_obscure_get(EWL_TEXT(e));
        if (!obscure)
        {
                LOG_FAILURE(buf, len, "password doesn't obscure");
                goto DONE;
        }
        if (strcmp(obscure, "*"))
        {
                LOG_FAILURE(buf, len, "password has not * as obscure character");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(e);

        return ret;
}

