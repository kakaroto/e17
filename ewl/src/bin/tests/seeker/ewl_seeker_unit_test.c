/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_seeker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);
static int test_hconstructor(char *buf, int len);
static int test_vconstructor(char *buf, int len);

Ewl_Unit_Test seeker_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"constructor horizontal", test_hconstructor, NULL, -1, 0},
                {"constructor vertical", test_vconstructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_seeker_new();

        if (!EWL_SEEKER_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SEEKER_TYPE);
        else if (ewl_seeker_autohide_get(EWL_SEEKER(s)))
                LOG_FAILURE(buf, len, "seeker auto-hides");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(s)) 
                        != (EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINKABLE))
                LOG_FAILURE(buf, len, "fill policy is HFILL | HSHRINKABLE");
        else if (ewl_seeker_orientation_get(EWL_SEEKER(s)) 
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "seeker is not horizontal");
        else
                ret = 1;

        return ret;
}

static int
test_hconstructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_hseeker_new();

        if (!EWL_SEEKER_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SEEKER_TYPE);
        else if (ewl_seeker_autohide_get(EWL_SEEKER(s)))
                LOG_FAILURE(buf, len, "seeker auto-hides");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(s)) 
                        != (EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINKABLE))
                LOG_FAILURE(buf, len, "fill policy is HFILL | HSHRINKABLE");
        else if (ewl_seeker_orientation_get(EWL_SEEKER(s)) 
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "seeker is not horizontal");
        else
                ret = 1;

        return ret;
}

static int
test_vconstructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_vseeker_new();

        if (!EWL_SEEKER_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SEEKER_TYPE);
        else if (ewl_seeker_autohide_get(EWL_SEEKER(s)))
                LOG_FAILURE(buf, len, "seeker auto-hides");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(s)) 
                        != (EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_VSHRINKABLE))
                LOG_FAILURE(buf, len, "fill policy is VFILL | VSHRINKABLE");
        else if (ewl_seeker_orientation_get(EWL_SEEKER(s)) 
                        != EWL_ORIENTATION_VERTICAL)
                LOG_FAILURE(buf, len, "seeker is not vertical");
        else
                ret = 1;

        return ret;
}
