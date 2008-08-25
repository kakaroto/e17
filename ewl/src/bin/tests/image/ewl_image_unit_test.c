/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"
#include "ewl_filedialog.h"
#include "ewl_image.h"
#include "ewl_scrollpane.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ecore_File.h>

static int test_path_set_get(char *buf, int len);
static int test_scale_set_get(char *buf, int len);
static int test_size_set_get(char *buf, int len);
static int test_constrain_set_get(char *buf, int len);
static int test_proportional_set_get(char *buf, int len);

Ewl_Unit_Test image_unit_tests[] = {
                {"image path set/get", test_path_set_get, NULL, -1, 0},
                {"image scale set/get", test_scale_set_get, NULL, -1, 0},
                {"image size set/get", test_size_set_get, NULL, -1, 0},
                {"image constrain set/get", test_constrain_set_get, NULL, -1, 0},
                {"image proportional set/get", test_proportional_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_path_set_get(char *buf, int len)
{
        Ewl_Widget *o;
        const char *t;
        int ret = 0;

        o = ewl_image_new();
        ewl_image_file_path_set(EWL_IMAGE(o), "/invalid/path");
        t = ewl_image_file_path_get(EWL_IMAGE(o));

        if (strcmp(t, "/invalid/path"))
                LOG_FAILURE(buf, len, "path_get did not match path_set.");
        else
                ret = 1;

        return ret;
}

static int
test_scale_set_get(char *buf, int len)
{
        Ewl_Widget *o;
        int ret = 0;
        double sw, sh;

        o = ewl_image_new();
        ewl_image_scale_set(EWL_IMAGE(o), 2.0, 2.0);
        ewl_image_scale_get(EWL_IMAGE(o), &sw, &sh);

        if (sw != 2.0 || sh != 2.0)
                LOG_FAILURE(buf, len, "scale_get did not match scale_set.");
        else
                ret = 1;

        return ret;
}

static int
test_size_set_get(char *buf, int len)
{
        Ewl_Widget *o;
        int ret = 0;
        int sw, sh;

        o = ewl_image_new();
        ewl_image_size_set(EWL_IMAGE(o), 2, 2);
        ewl_image_size_get(EWL_IMAGE(o), &sw, &sh);

        if (sw != 2 || sh != 2)
                LOG_FAILURE(buf, len, "size_get did not match size_set.");
        else
                ret = 1;

        return ret;
}

static int
test_constrain_set_get(char *buf, int len)
{
        Ewl_Widget *o;
        int ret = 0;
        int sw;

        o = ewl_image_new();
        ewl_image_constrain_set(EWL_IMAGE(o), 2);
        sw = ewl_image_constrain_get(EWL_IMAGE(o));

        if (sw != 2)
                LOG_FAILURE(buf, len, "scale_get did not match scale_set.");
        else
                ret = 1;

        return ret;
}

static int
test_proportional_set_get(char *buf, int len)
{
        Ewl_Widget *o;
        unsigned int p;
        int ret = 0;

        o = ewl_image_new();
        ewl_image_proportional_set(EWL_IMAGE(o), TRUE);
        p = ewl_image_proportional_get(EWL_IMAGE(o));

        if (p != TRUE)
                LOG_FAILURE(buf, len, "proportional_get did not match set.");
        else
                ret = 1;

        return ret;
}
