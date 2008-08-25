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
extern Ewl_Unit_Test filepicker_unit_tests[];

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
