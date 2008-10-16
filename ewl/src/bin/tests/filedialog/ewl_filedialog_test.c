/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_filedialog.h"
#include "ewl_label.h"
#include <stdio.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void dialog_cb_single_clicked(Ewl_Widget *w, void *ev, void *data);
static void dialog_cb_multi_clicked(Ewl_Widget *w, void *ev, void *data);
static void fd_cb_delete(Ewl_Widget *w, void *ev, void *data);
static void fd_cb_value_changed(Ewl_Widget *w, void *ev, void *data);
static void fd_append(void *value, void *data);

extern Ewl_Unit_Test filedialog_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Filedialog";
        test->tip = "The filedialog is intended to be used for a\n"
                        "single or multi-file chooser.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
        test->unit_tests = filedialog_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o, *o2;

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "Selected Files");
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(box, o);
        ewl_widget_show(o);

        o = ewl_vbox_new();
        ewl_widget_name_set(o, "file_list");
        ewl_container_child_append(box, o);
        ewl_widget_show(o);

        o = ewl_hbox_new();
        ewl_container_child_append(box, o);
        ewl_widget_show(o);

        o2 = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o2), "Single select");
        ewl_container_child_append(EWL_CONTAINER(o), o2);
        ewl_callback_append(o2, EWL_CALLBACK_CLICKED,
                                dialog_cb_single_clicked, NULL);
        ewl_widget_show(o2);

        o2 = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o2), "Multi select");
        ewl_container_child_append(EWL_CONTAINER(o), o2);
        ewl_callback_append(o2, EWL_CALLBACK_CLICKED,
                                dialog_cb_multi_clicked, NULL);
        ewl_widget_show(o2);

        return 1;
}

static void
dialog_cb_single_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *fd;
        Ecore_List *list;

        list = ecore_list_new();
        ecore_list_append(list, "text/directory");

        fd = ewl_filedialog_new();
        ewl_filedialog_filter_add(EWL_FILEDIALOG(fd), "Directories", NULL, list);
        ewl_filedialog_filter_add(EWL_FILEDIALOG(fd), "C Files", "*.c", NULL);
        ewl_filedialog_filter_add(EWL_FILEDIALOG(fd), "D Files", "*.d", NULL);
        ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW,
                                        fd_cb_delete, NULL);
        ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED,
                                        fd_cb_value_changed, NULL);
        ewl_widget_show(fd);
}

static void
dialog_cb_multi_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *fd;

        fd = ewl_filedialog_multiselect_new();
        ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW,
                                        fd_cb_delete, NULL);
        ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED,
                                        fd_cb_value_changed, NULL);
        ewl_widget_show(fd);
}

static void
fd_cb_delete(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        ewl_widget_destroy(w);
}

static void
fd_cb_value_changed(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_Action_Response *e;
        Ewl_Widget *list;
        Ewl_Filedialog *fd;

        e = ev;

        list = ewl_widget_name_find("file_list");
        ewl_container_reset(EWL_CONTAINER(list));

        /* nothing to do on a cancel */
        if (e->response == EWL_STOCK_CANCEL)
        {
                ewl_widget_destroy(w);
                return;
        }

        fd = EWL_FILEDIALOG(w);
        Ecore_List *l;
        l = ewl_filedialog_selected_files_get(fd);
        ecore_list_free_cb_set(l, ECORE_FREE_CB(free));
        ecore_list_for_each(l, fd_append, list);

        ecore_list_destroy(l);
        ewl_widget_destroy(w);
}

static void
fd_append(void *value, void *data)
{
        Ewl_Widget *list, *o;
        const char *file;

        file = value;
        list = data;

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), file);
        ewl_container_child_append(EWL_CONTAINER(list), o);
        ewl_widget_show(o);
        printf("%s\n", file);
}

