/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_combo.h"
#include "ewl_entry.h"
#include "ewl_grid.h"
#include "ewl_image.h"
#include "ewl_icon_theme.h"
#include "ewl_label.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
        unsigned int count;
        char ** data;
} Ewl_Combo_Test_Data;

static void *combo_test_data_setup(void);
static int create_test(Ewl_Container *win);
static void combo_value_changed(Ewl_Widget *w, void *ev, void *data);
static Ewl_Widget *combo_test_data_header_fetch(void *data, unsigned int col,
                                                void *pr_data);
static void *combo_test_data_fetch(void *data, unsigned int row,
                                                unsigned int col);
static unsigned int combo_test_data_count_get(void *data);
static void combo_cb_add(Ewl_Widget *w, void *ev, void *data);
static void combo_cb_clear(Ewl_Widget *w, void *ev, void *data);
static void combo_cb_entry_changed(Ewl_Widget *w, void *ev, void *data);

static Ewl_Widget *combo_test_editable_cb_header_fetch(void *data,
                                                        unsigned int col,
                                                        void *pr_data);
static Ewl_Widget *combo_test_editable_cb_widget_fetch(void *data,
                                                        unsigned int row,
                                                        unsigned int col,
                                                        void *pr_data);

extern Ewl_Unit_Test combo_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Combo";
        test->tip = "Defines a combo box used internally.\n"
                "The contents of the box are not drawn\n"
                "outside of the evas.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = combo_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *combo, *hbox, *o, *grid;
        Ewl_Model *model;
        Ewl_View *view;
        void *data;

        hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), hbox);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
        ewl_widget_show(hbox);

        data = combo_test_data_setup();

        /* create the model */
        model = ewl_model_new();
        ewl_model_data_fetch_set(model, combo_test_data_fetch);
        ewl_model_data_count_set(model, combo_test_data_count_get);

        /* create the view for ewl_label widgets */
        view = ewl_view_clone(ewl_label_view_get());
        ewl_view_header_fetch_set(view, combo_test_data_header_fetch);

        combo = ewl_combo_new();
        ewl_widget_name_set(combo, "combo_label");
        ewl_container_child_append(EWL_CONTAINER(hbox), combo);
        ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
                                        combo_value_changed, NULL);
        ewl_mvc_model_set(EWL_MVC(combo), model);
        ewl_mvc_view_set(EWL_MVC(combo), view);
        ewl_mvc_data_set(EWL_MVC(combo), data);
        ewl_widget_show(combo);

        /* create the view for ewl_image widgets */
        view = ewl_view_clone(ewl_image_view_get());
        ewl_view_header_fetch_set(view, combo_test_data_header_fetch);

        grid = ewl_grid_new();
        ewl_grid_homogeneous_set(EWL_GRID(grid), TRUE);
        ewl_grid_dimensions_set(EWL_GRID(grid), 4, 1);

        combo = ewl_combo_new();
        ewl_widget_name_set(combo, "combo_image");
        ewl_object_fill_policy_set(EWL_OBJECT(combo), EWL_FLAG_FILL_NONE);
        ewl_combo_popup_container_set(EWL_COMBO(combo), EWL_CONTAINER(grid));
        ewl_container_child_append(EWL_CONTAINER(hbox), combo);
        ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
                                        combo_value_changed, NULL);
        ewl_mvc_model_set(EWL_MVC(combo), model);
        ewl_mvc_view_set(EWL_MVC(combo), view);
        ewl_mvc_data_set(EWL_MVC(combo), data);
        ewl_mvc_selected_set(EWL_MVC(combo), model, data, 0, 0);
        ewl_widget_show(combo);

        /* create the editable model/view */
        model = ewl_model_new();
        ewl_model_data_count_set(model, combo_test_data_count_get);
        ewl_model_data_fetch_set(model, combo_test_data_fetch);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, combo_test_editable_cb_widget_fetch);
        ewl_view_header_fetch_set(view,
                        combo_test_editable_cb_header_fetch);

        combo = ewl_combo_new();
        ewl_widget_name_set(combo, "combo_custom");
        ewl_container_child_append(EWL_CONTAINER(hbox), combo);
        ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
                                        combo_value_changed, NULL);
        ewl_mvc_model_set(EWL_MVC(combo), model);
        ewl_mvc_view_set(EWL_MVC(combo), view);
        ewl_mvc_data_set(EWL_MVC(combo), data);
        ewl_combo_editable_set(EWL_COMBO(combo), TRUE);
        ewl_widget_show(combo);

        hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), hbox);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_VFILL);
        ewl_widget_show(hbox);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Add items");
        ewl_container_child_append(EWL_CONTAINER(hbox), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, combo_cb_add, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Clear items");
        ewl_container_child_append(EWL_CONTAINER(hbox), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, combo_cb_clear, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_widget_show(o);

        return 1;
}

static void *
combo_test_data_setup(void)
{
        Ewl_Combo_Test_Data *data;
        unsigned int i;

        const char *icons[] = {
                EWL_ICON_EDIT_COPY,
                EWL_ICON_EDIT_CUT,
                EWL_ICON_EDIT_DELETE,
                EWL_ICON_EDIT_FIND,
                EWL_ICON_EDIT_FIND_REPLACE,
                EWL_ICON_EDIT_PASTE,
                EWL_ICON_EDIT_REDO,
                EWL_ICON_EDIT_SELECT_ALL,
                EWL_ICON_EDIT_UNDO,
                EWL_ICON_FORMAT_INDENT_LESS,
                EWL_ICON_FORMAT_INDENT_MORE,
                EWL_ICON_FORMAT_JUSTIFY_CENTER,
                EWL_ICON_FORMAT_JUSTIFY_FILL,
                EWL_ICON_FORMAT_JUSTIFY_LEFT,
                EWL_ICON_FORMAT_JUSTIFY_RIGHT,
        };

        data = calloc(1, sizeof(Ewl_Combo_Test_Data));
        data->count = sizeof(icons) / sizeof(const char *);

        data->data = calloc(data->count, sizeof(char *));

        for (i = 0; i < data->count; i++)
        {
                const char *icon;

                icon = ewl_icon_theme_icon_path_get(icons[i], EWL_ICON_SIZE_MEDIUM);
                data->data[i] = strdup((icon ? icon : icons[i]));
        }

        return data;
}

static Ewl_Widget *
combo_test_data_header_fetch(void *data, unsigned int col __UNUSED__,
                                void *pr_data __UNUSED__)
{
        Ewl_Widget *header;
        const char *path = data;

        if (!data)
        {
                header = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(header), "Select Image");
        }
        else
        {
                header = ewl_image_new();
                ewl_image_file_path_set(EWL_IMAGE(header), path);
        }

        return header;
}

static void *
combo_test_data_fetch(void *data, unsigned int row,
                                unsigned int col __UNUSED__)
{
        Ewl_Combo_Test_Data *d;

        d = data;
        if (row < d->count)
                return d->data[row];
        else
                return NULL;
}

static unsigned int
combo_test_data_count_get(void *data)
{
        Ewl_Combo_Test_Data *d;

        d = data;
        return d->count;
}

static void
combo_value_changed(Ewl_Widget *w, void *ev __UNUSED__,
                                void *data __UNUSED__)
{
        Ewl_Combo_Test_Data *d;
        Ewl_Selection_Idx *idx;

        d = ewl_mvc_data_get(EWL_MVC(w));
        idx = ewl_mvc_selected_get(EWL_MVC(w));

        /*
        printf("value changed to %d %p)\n",
                idx->row, d->data[idx->row]);
        */
}

static void
combo_cb_add(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *c;
        Ewl_Combo_Test_Data *d;
        unsigned int s;

        c = ewl_widget_name_find("combo_label");
        d = ewl_mvc_data_get(EWL_MVC(c));

        s = d->count;
        d->count += 2;
        d->data = realloc(d->data, sizeof(char *) * d->count);

        d->data[s] = strdup(PACKAGE_DATA_DIR "/images/Package.png");
        d->data[s + 1] = strdup(PACKAGE_DATA_DIR "/images/Open.png");

        ewl_mvc_dirty_set(EWL_MVC(c), 1);

        c = ewl_widget_name_find("combo_image");
        ewl_mvc_dirty_set(EWL_MVC(c), 1);
}

static void
combo_cb_clear(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data __UNUSED__)
{
        Ewl_Widget *c;
        Ewl_Combo_Test_Data *d;

        c = ewl_widget_name_find("combo_label");
        d = ewl_mvc_data_get(EWL_MVC(c));

        d->count = 0;
        free(d->data);
        d->data = NULL;

        ewl_mvc_dirty_set(EWL_MVC(c), 1);

        c = ewl_widget_name_find("combo_image");
        ewl_mvc_dirty_set(EWL_MVC(c), 1);
}

static Ewl_Widget *
combo_test_editable_cb_header_fetch(void *data, unsigned int col __UNUSED__,
                                        void *pr_data __UNUSED__)
{
        const char *path;
        Ewl_Widget *w, *o, *o2;

        path = data;

        w = ewl_hbox_new();
        ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_LEFT);
        
        o = ewl_entry_new();
        if (path)
        {
                const char *val;
                val = strrchr(path, '/');
                ewl_text_text_set(EWL_TEXT(o), (val ? val + 1 : path));
                
                o2 = ewl_image_new();
                ewl_image_file_path_set(EWL_IMAGE(o2), path);
                ewl_container_child_append(EWL_CONTAINER(w), o2);
                ewl_widget_show(o2);
        }
        else
                ewl_text_text_set(EWL_TEXT(o), "Please select an option.");

        ewl_container_child_append(EWL_CONTAINER(w), o);
        ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                combo_cb_entry_changed, NULL);
        ewl_widget_show(o);

        return w;
}

static Ewl_Widget *
combo_test_editable_cb_widget_fetch(void *data, unsigned int row __UNUSED__,
                                                unsigned int col __UNUSED__,
                                                void *pr_data __UNUSED__)
{
        Ewl_Widget *w;
        Ewl_Widget *o;
        char *str;

        w = ewl_hbox_new();
        ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_LEFT);

        o = ewl_image_new();
        ewl_image_file_path_set(EWL_IMAGE(o), (const char *)data);
        ewl_container_child_append(EWL_CONTAINER(w), o);
        ewl_widget_show(o);

        o = ewl_label_new();
        str = strrchr((const char *)data, '/');
        ewl_label_text_set(EWL_LABEL(o), (str ? str + 1 : data));
        ewl_container_child_append(EWL_CONTAINER(w), o);
        ewl_widget_show(o);

        return w;
}

static void
combo_cb_entry_changed(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        printf("value changed to (%s)\n", ewl_text_text_get(EWL_TEXT(w)));
}

