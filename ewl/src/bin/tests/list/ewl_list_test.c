/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_border.h"
#include "ewl_button.h"
#include "ewl_label.h"
#include "ewl_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct List_Test_Row_Data List_Test_Row_Data;
struct List_Test_Row_Data
{
        char *image;
        char *text;
};

typedef struct List_Test_Data List_Test_Data;
struct List_Test_Data
{
        unsigned int count;
        List_Test_Row_Data **rows;
};

static int create_test(Ewl_Container *win);

static void *list_test_data_setup(void);
static void list_cb_value_changed(Ewl_Widget *w, void *ev, void *data);
static void list_cb_multi_value_changed(Ewl_Widget *w, void *ev, void *data);

static Ewl_Widget *list_test_cb_widget_fetch(void *data, unsigned int row,
                                                unsigned int col,
                                                void *pr_data);
static void *list_test_data_fetch(void *data, unsigned int row,
                                                unsigned int column);
static unsigned int list_test_data_count_get(void *data);
static void list_cb_select_none(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "List";
        test->tip = "Defines a widget for laying out other\n"
                        "widgets in a list like manner.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
        Ecore_List *str_data;
        Ewl_Widget *list, *o;
        Ewl_Model *model;
        Ewl_View *view;
        void *data;

        model = ewl_model_ecore_list_instance();
        view = ewl_label_view_get();

        o = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(o), "Label List (single select)");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        str_data = ecore_list_new();
        ecore_list_append(str_data, "first");
        ecore_list_append(str_data, "second");
        ecore_list_append(str_data, "third");
        ecore_list_append(str_data, "fourth");

        list = ewl_list_new();
        ewl_container_child_append(EWL_CONTAINER(o), list);
        ewl_box_orientation_set(EWL_BOX(list), EWL_ORIENTATION_HORIZONTAL);
        ewl_mvc_model_set(EWL_MVC(list), model);
        ewl_mvc_view_set(EWL_MVC(list), view);
        ewl_mvc_data_set(EWL_MVC(list), str_data);
        ewl_callback_append(list, EWL_CALLBACK_VALUE_CHANGED,
                                        list_cb_value_changed, NULL);
        ewl_widget_show(list);

        /* create a list using an ecore_list of strings of labels */
        o = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(o), "Label List (multi select)");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        list = ewl_list_new();
        ewl_container_child_append(EWL_CONTAINER(o), list);
        ewl_box_orientation_set(EWL_BOX(list), EWL_ORIENTATION_HORIZONTAL);
        ewl_mvc_model_set(EWL_MVC(list), model);
        ewl_mvc_view_set(EWL_MVC(list), view);
        ewl_mvc_data_set(EWL_MVC(list), str_data);
        ewl_mvc_selection_mode_set(EWL_MVC(list), EWL_SELECTION_MODE_MULTI);
        ewl_callback_append(list, EWL_CALLBACK_VALUE_CHANGED,
                                        list_cb_multi_value_changed, NULL);
        ewl_widget_show(list);

        /* Create a list from a custom array with a custom assign on the
         * view */
        o = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(o), "Custom List");
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        model = ewl_model_new();
        ewl_model_data_fetch_set(model, list_test_data_fetch);
        ewl_model_data_count_set(model, list_test_data_count_get);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, list_test_cb_widget_fetch);
        ewl_view_header_fetch_set(view, NULL);

        data = list_test_data_setup();

        list = ewl_list_new();
        ewl_container_child_append(EWL_CONTAINER(o), list);
        ewl_mvc_model_set(EWL_MVC(list), model);
        ewl_mvc_view_set(EWL_MVC(list), view);
        ewl_mvc_data_set(EWL_MVC(list), data);
        ewl_mvc_selection_mode_set(EWL_MVC(list), EWL_SELECTION_MODE_NONE);
        ewl_callback_append(list, EWL_CALLBACK_VALUE_CHANGED,
                                        list_cb_select_none, NULL);
        ewl_widget_show(list);


        return 1;
}

static void *
list_test_data_setup(void)
{
        List_Test_Data *data;

        data = calloc(1, sizeof(List_Test_Data));
        data->rows = calloc(3, sizeof(List_Test_Row_Data *));

        data->rows[0] = calloc(1, sizeof(List_Test_Row_Data));
        data->rows[0]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        data->rows[0]->text = strdup("The E logo");

        data->rows[1] = calloc(1, sizeof(List_Test_Row_Data));
        data->rows[1]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/entice.png");
        data->rows[1]->text = strdup("The Entice image");

        data->rows[2] = calloc(1, sizeof(List_Test_Row_Data));
        data->rows[2]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/entrance.png");
        data->rows[2]->text = strdup("The Entrance image");

        data->count = 3;

        return data;
}

static Ewl_Widget *
list_test_cb_widget_fetch(void *data, unsigned int row __UNUSED__,
                                        unsigned int col __UNUSED__,
                                        void *pr_data __UNUSED__)
{
        Ewl_Widget *w;
        List_Test_Row_Data *d;

        d = data;

        w = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(w), d->text);
        ewl_button_image_set(EWL_BUTTON(w), d->image, NULL);
        ewl_button_image_size_set(EWL_BUTTON(w), 24, 24);
        ewl_widget_show(w);

        return w;
}

static void *
list_test_data_fetch(void *data, unsigned int row,
                                        unsigned int column __UNUSED__)
{
        List_Test_Data *d;

        d = data;

        /* NOTE: this is just for testing purposes, should not be needed in a
         * normal app */
        if (row >= d->count)
        {
                printf("Asking for too many rows\n");
                return NULL;
        }

        return d->rows[row];
}

static unsigned int
list_test_data_count_get(void *data)
{
        List_Test_Data *d;

        d = data;

        return d->count;
}

static void
list_cb_value_changed(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_List *list;
        Ecore_List *el;
        Ewl_Selection_Idx *idx;

        list = EWL_LIST(w);
        el = ewl_mvc_data_get(EWL_MVC(list));
        idx = ewl_mvc_selected_get(EWL_MVC(list));

        ecore_list_index_goto(el, idx->row);
        printf("Selected (%d) (%s)\n", idx->row,
                        (char *)ecore_list_current(el));
}

static void
list_cb_multi_value_changed(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ecore_List *selected, *el;
        Ewl_Selection *sel;

        printf("Selected:\n");

        el = ewl_mvc_data_get(EWL_MVC(w));
        selected = ewl_mvc_selected_list_get(EWL_MVC(w));

        ecore_list_first_goto(selected);
        while ((sel = ecore_list_next(selected)))
        {
                if (sel->type == EWL_SELECTION_TYPE_INDEX)
                {
                        Ewl_Selection_Idx *idx;

                        idx = EWL_SELECTION_IDX(sel);
                        ecore_list_index_goto(el, idx->row);
                        printf("    %d (%s)\n", idx->row,
                                        (char *)ecore_list_current(el));
                }
                else
                {
                        Ewl_Selection_Range *idx;
                        unsigned int i;

                        idx = EWL_SELECTION_RANGE(sel);
                        for (i = idx->start.row; i <= idx->end.row; i++)
                        {
                                ecore_list_index_goto(el, i);
                                printf("    %d (%s)\n", i,
                                        (char *)ecore_list_current(el));
                        }
                }
        }
}

static void
list_cb_select_none(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        printf("ERROR, shouldn't get selection changed callbacks.\n");
}


