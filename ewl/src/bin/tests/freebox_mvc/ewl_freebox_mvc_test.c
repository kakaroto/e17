/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_freebox.h"
#include "ewl_freebox_mvc.h"
#include "ewl_icon.h"
#include "ewl_scrollpane.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Freebox_MVC_Test_Row_Data Freebox_MVC_Test_Row_Data;
struct Freebox_MVC_Test_Row_Data
{
        char *image;
        char *text;
};

static int create_test(Ewl_Container *box);
static void ewl_freebox_mvc_test_cb_add(Ewl_Widget *w, void *ev, void *data);
static void ewl_freebox_mvc_test_cb_clear(Ewl_Widget *w, void *ev, void *data);
static void ewl_freebox_mvc_test_data_append(Ecore_List *d);

static void                 *ewl_freebox_mvc_test_data_setup();
static Ewl_Widget           *ewl_freebox_mvc_test_widget_fetch(void *data, 
                                                               unsigned int row,
                                                               unsigned int col,
							       void *pr_data
							       );
static void                 *ewl_freebox_mvc_test_data_fetch(void *data, 
                                                             unsigned int row, 
                                                             unsigned int column);
static unsigned int          ewl_freebox_mvc_test_data_count_get(void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Freebox MVC";
        test->tip = "Defines a widget for laying out other\n"
                    "widgets in a free like manner using MVC.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *fb_mvc, *hbox, *pane, *o, *vbox;
        Ewl_Model *model;
        Ewl_View *view;
        
        void *data;

        ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_ALL);

        vbox = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(box), vbox);
        ewl_widget_show(vbox);

        pane = ewl_scrollpane_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), pane);
        ewl_widget_show(pane);

        model = ewl_model_new();
        ewl_model_data_fetch_set(model, ewl_freebox_mvc_test_data_fetch);
        ewl_model_data_count_set(model, ewl_freebox_mvc_test_data_count_get);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, ewl_freebox_mvc_test_widget_fetch);
        ewl_view_header_fetch_set(view, NULL);

        data = ewl_freebox_mvc_test_data_setup();

        fb_mvc = ewl_freebox_mvc_new();
        ewl_mvc_model_set(EWL_MVC(fb_mvc), model);
        ewl_mvc_view_set(EWL_MVC(fb_mvc), view);
        ewl_mvc_data_set(EWL_MVC(fb_mvc), data);
        ewl_mvc_selection_mode_set(EWL_MVC(fb_mvc), EWL_SELECTION_MODE_MULTI);
        ewl_container_child_append(EWL_CONTAINER(pane), fb_mvc);
        ewl_widget_show(fb_mvc);

        hbox = ewl_hbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
        ewl_widget_show(hbox);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Add items");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINKABLE);
        ewl_container_child_append(EWL_CONTAINER(hbox), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
                               ewl_freebox_mvc_test_cb_add, fb_mvc);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Clear items");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINKABLE);
        ewl_container_child_append(EWL_CONTAINER(hbox), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
                               ewl_freebox_mvc_test_cb_clear, fb_mvc);
        ewl_widget_show(o);

        return 1;
}

static void *
ewl_freebox_mvc_test_data_setup(void)
{
        Ecore_List *d;

        d = ecore_list_new();

        ewl_freebox_mvc_test_data_append(d);
        
        return d;
}

static Ewl_Widget *
ewl_freebox_mvc_test_widget_fetch(void *data, unsigned int row __UNUSED__,
                                              unsigned int col __UNUSED__,
					      void *pr_data __UNUSED__)
{
        Ewl_Widget *w;
        Freebox_MVC_Test_Row_Data *d;

        d = data;

        w = ewl_icon_simple_new();
        ewl_icon_image_set(EWL_ICON(w), d->image, NULL);
        ewl_icon_label_set(EWL_ICON(w), d->text);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);
        ewl_object_minimum_size_set(EWL_OBJECT(w), 32, 32);
        ewl_widget_show(w);

        return w;
};

static void *
ewl_freebox_mvc_test_data_fetch(void *data, unsigned int row, 
                                            unsigned int column __UNUSED__)
{
        Ecore_List *d;

        d = data;

        if ((int) row >= ecore_list_count(d))
        {
                printf("Asking for too many items\n");
                return NULL;
        }

        return ecore_list_index_goto(d, row);
}

static unsigned int
ewl_freebox_mvc_test_data_count_get(void *data)
{
        Ecore_List *d;

        d = data;

        return ecore_list_count(d);
}

static void 
ewl_freebox_mvc_test_cb_add(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
				void *data)
{
        Ecore_List *d;

        d = ewl_mvc_data_get(EWL_MVC(data));
        if (!d)
                d = ecore_list_new();
        ewl_freebox_mvc_test_data_append(d);
        ewl_mvc_data_set(EWL_MVC(data), d);
        ewl_mvc_dirty_set(EWL_MVC(data), TRUE);
}

static void
ewl_freebox_mvc_test_cb_clear(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
				void *data)
{
        Ecore_List *d;

        d = ewl_mvc_data_get(EWL_MVC(data));
        d = NULL;
        ewl_mvc_data_set(EWL_MVC(data), d);
        ewl_mvc_dirty_set(EWL_MVC(data), TRUE);
        ewl_container_reset(EWL_CONTAINER(data));
}

static void 
ewl_freebox_mvc_test_data_append(Ecore_List *d)
{
        Freebox_MVC_Test_Row_Data *data;

        data = calloc(1, sizeof(Freebox_MVC_Test_Row_Data));
        data->image = strdup(PACKAGE_DATA_DIR "/ewl/images/Draw.png");
        data->text = strdup("Draw");
        ecore_list_append(d, data);

        data = calloc(1, sizeof(Freebox_MVC_Test_Row_Data));
        data->image = strdup(PACKAGE_DATA_DIR "/ewl/images/End.png");
        data->text = strdup("End");
        ecore_list_append(d, data);

        data = calloc(1, sizeof(Freebox_MVC_Test_Row_Data));
        data->image = strdup(PACKAGE_DATA_DIR "/ewl/images/NewBCard.png");
        data->text = strdup("Card");
        ecore_list_append(d, data);

        data = calloc(1, sizeof(Freebox_MVC_Test_Row_Data));
        data->image = strdup(PACKAGE_DATA_DIR "/ewl/images/Open.png");
        data->text = strdup("Open");
        ecore_list_append(d, data);

        data = calloc(1, sizeof(Freebox_MVC_Test_Row_Data));
        data->image = strdup(PACKAGE_DATA_DIR "/ewl/images/Package.png");
        data->text = strdup("Package");
        ecore_list_append(d, data);

        data = calloc(1, sizeof(Freebox_MVC_Test_Row_Data));
        data->image = strdup(PACKAGE_DATA_DIR "/ewl/images/World.png");
        data->text = strdup("World");
        ecore_list_append(d, data);
}

