/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_filepicker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int create_test(Ewl_Container *box);
static void ewl_filepicker_cb_value_changed(Ewl_Widget *w, void *ev,
                                                        void *data);
static int directory_test_set_get(char *buf, int len);
static int directory_test_path_split(char *buf, int len);

static Ewl_Unit_Test filepicker_unit_tests[] = {
                {"directory set/get", directory_test_set_get, NULL, -1, 1},
                {"directory path split", directory_test_path_split, NULL, -1, 1},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Filepicker";
        test->tip = "A simple file picking widget";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
        test->unit_tests = filepicker_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *w;
        Ewl_Filelist_Filter *f;

        w = ewl_filepicker_new();
        f = ewl_filepicker_filter_add(EWL_FILEPICKER(w), "C Files",
                        "*.c", NULL);
        ewl_filepicker_filter_add(EWL_FILEPICKER(w), "D Files", "*.d", NULL);
        ewl_filepicker_filter_set(EWL_FILEPICKER(w), f);
        ewl_container_child_append(box, w);
        ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED,
                        ewl_filepicker_cb_value_changed, NULL);
        ewl_widget_show(w);

        return 1;
}

static void
ewl_filepicker_cb_value_changed(Ewl_Widget *w, void *ev,
                                        void *data __UNUSED__)
{
        Ewl_Filepicker *fp;
        Ewl_Event_Action_Response *e;

        fp = EWL_FILEPICKER(w);
        e = ev;

        /* need to deal with multiselect in here */
        switch (e->response)
        {
                case EWL_STOCK_OPEN:
                {
                        char *file;

                        file = ewl_filepicker_selected_file_get(fp);

                        if (!file)
                                printf("No file selected.\n");
                        else
                        {
                                printf("%s\n", file);
                                free(file);
                        }
                        break;
                }
                default:
                case EWL_STOCK_CANCEL:
                        printf("Cancelled\n");
                        break;
        }
}

static int
directory_test_set_get(char *buf, int len)
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
directory_test_path_split(char *buf, int len)
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


