/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_filepicker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_directory_set_get(char *buf, int len);
static int test_directory_path_split(char *buf, int len);

Ewl_Unit_Test filepicker_unit_tests[] = {
                {"directory set/get", test_directory_set_get, NULL, -1, 1},
                {"directory path split", test_directory_path_split, NULL, -1, 1},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_directory_set_get(char *buf, int len)
{
        Ewl_Widget *fp;
        const char *t;
        int ret = 0;

        fp = ewl_filepicker_new();
        ewl_filepicker_directory_set(EWL_FILEPICKER(fp),
                                        "/tmp");
        t = ewl_filepicker_directory_get(EWL_FILEPICKER(fp));

        if (strcmp(t, "/tmp"))
                LOG_FAILURE(buf, len, "directory_get did not match "
                                                "directory_set %s", t);
        else
                ret = 1;

        return ret;
}

static int
test_directory_path_split(char *buf, int len)
{
        Ewl_Widget *fp;
        char *t;
        int i = 0, ret = 1;
        char *rez[] = {"/usr/local/bin", "/usr/local", "/usr", "/"};

        fp = ewl_filepicker_new();
        ewl_filepicker_directory_set(EWL_FILEPICKER(fp),
                                        "/usr/local/bin/");

        ecore_list_first_goto(EWL_FILEPICKER(fp)->path);
        while ((t = ecore_list_next(EWL_FILEPICKER(fp)->path)))
        {
                if (strcmp(rez[i], t))
                {
                        LOG_FAILURE(buf, len, "path list not setup correctly. "
                                        "[%s didn't match %s]", t, rez[i]);
                        ret = 0;
                        break;
                }
                i++;
        }

        return ret;
}
