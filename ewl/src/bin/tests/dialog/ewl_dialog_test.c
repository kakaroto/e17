/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_border.h"
#include "ewl_radiobutton.h"
#include "ewl_dialog.h"
#include "ewl_image.h"
#include "ewl_icon_theme.h"
#include "ewl_label.h"
#include "ewl_text.h"
#include <stdio.h>
#include <string.h>

static int create_test(Ewl_Container *box);
static void dialog_response_cb(Ewl_Widget *w, void *ev, void *data);
static void run_dialog(Ewl_Widget *w, void *ev, void *data);
static void dialog_delete_cb(Ewl_Widget *w, void *ev, void *data);

/* unit tests */
static int constructor_test(char *buf, int len);

static Ewl_Unit_Test dialog_unit_tests[] = {
                {"constructor", constructor_test, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Dialog";
        test->tip = "Defines a dialog window.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
        test->unit_tests = dialog_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o;
        Ewl_Widget *border;
        Ewl_Widget *chain;

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "");
        ewl_container_child_append(box, o);
        ewl_widget_name_set(o, "dialog_label");
        ewl_widget_show(o);

        border = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(border), "Button Position");
        ewl_container_child_append(EWL_CONTAINER(box), border);
        ewl_widget_show(border);
        
        chain = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(chain), "Top");
        ewl_radiobutton_value_set(EWL_RADIOBUTTON(chain), 
                                                (void *)EWL_POSITION_TOP);
        ewl_container_child_append(EWL_CONTAINER(border), chain);
        ewl_widget_show(chain);

        o = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(o), "Right");
        ewl_radiobutton_value_set(EWL_RADIOBUTTON(o), 
                                                (void *)EWL_POSITION_RIGHT);
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(chain));
        ewl_container_child_append(EWL_CONTAINER(border), o);
        ewl_widget_show(o);

        o = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(o), "Left");
        ewl_radiobutton_value_set(EWL_RADIOBUTTON(o), 
                                                (void *)EWL_POSITION_LEFT);
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(chain));
        ewl_container_child_append(EWL_CONTAINER(border), o);
        ewl_widget_show(o);

        o = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(o), "Bottom");
        ewl_radiobutton_value_set(EWL_RADIOBUTTON(o), 
                                                (void *)EWL_POSITION_BOTTOM);
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), EWL_RADIOBUTTON(chain));
        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(o), TRUE);
        ewl_container_child_append(EWL_CONTAINER(border), o);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Launch Dialog");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, run_dialog, chain);
        ewl_container_child_append(box, o);
        ewl_widget_show(o);

        return 1;
}

static void
run_dialog(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Widget *o, *dialog, *hbox;
        Ewl_Radiobutton *radio;
        Ewl_Window *win;
        Ewl_Position pos;

        radio = EWL_RADIOBUTTON(data);
        pos = (Ewl_Position)ewl_radiobutton_value_get(
                        ewl_radiobutton_chain_selected_get(radio));
        /* this is a bad cast, but the ewl_test will ever be a standalone app */
        win = EWL_WINDOW(ewl_embed_widget_find(w));

        dialog = ewl_dialog_new();
        ewl_window_title_set(EWL_WINDOW(dialog), "Dialog Test");
        ewl_window_name_set(EWL_WINDOW(dialog), "EWL Test Application");
        ewl_window_class_set(EWL_WINDOW(dialog), "EFL Test Application");
        ewl_window_transient_for(EWL_WINDOW(dialog), win);
        ewl_callback_append(dialog, EWL_CALLBACK_DELETE_WINDOW,
                                                dialog_delete_cb, NULL);
        ewl_dialog_action_position_set(EWL_DIALOG(dialog), pos);
        ewl_widget_show(dialog);
        if (pos == EWL_POSITION_TOP)
                ewl_dialog_active_area_set(EWL_DIALOG(dialog), 
                                EWL_POSITION_BOTTOM);
        else if (pos == EWL_POSITION_BOTTOM)
                ewl_dialog_active_area_set(EWL_DIALOG(dialog), 
                                EWL_POSITION_TOP);
        else if (pos == EWL_POSITION_LEFT)
                ewl_dialog_active_area_set(EWL_DIALOG(dialog), 
                                EWL_POSITION_RIGHT);
        else if (pos == EWL_POSITION_RIGHT)
                ewl_dialog_active_area_set(EWL_DIALOG(dialog), 
                                EWL_POSITION_LEFT);

        hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(dialog), hbox);
        ewl_object_padding_set(EWL_OBJECT(hbox), 20, 20, 20, 20);
        ewl_widget_show(hbox);

        o = ewl_image_new();
        ewl_image_file_set(EWL_IMAGE(o),
                ewl_icon_theme_icon_path_get(EWL_ICON_DIALOG_INFORMATION, 46),
                EWL_ICON_DIALOG_INFORMATION);
        ewl_container_child_append(EWL_CONTAINER(hbox), o);
        ewl_object_padding_set(EWL_OBJECT(o), 0, 20, 0, 0);
        ewl_widget_show(o);

        o = ewl_text_new();
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(hbox), o);
        ewl_text_text_set(EWL_TEXT(o), "This is a dialog window");
        ewl_widget_show(o);

        ewl_dialog_active_area_set(EWL_DIALOG(dialog), pos);

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
        ewl_container_child_append(EWL_CONTAINER(dialog), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        dialog_response_cb, dialog);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_CANCEL);
        ewl_container_child_append(EWL_CONTAINER(dialog), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        dialog_response_cb, dialog);
        ewl_widget_show(o);

        ewl_dialog_active_area_set(EWL_DIALOG(dialog), EWL_POSITION_TOP);
        ewl_widget_show(dialog);
}

static void
dialog_response_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Stock_Type response;
        Ewl_Widget *o;

        o = ewl_widget_name_find("dialog_label");
        response = ewl_stock_type_get(EWL_STOCK(w));
        if (response == EWL_STOCK_OK)
                ewl_label_text_set(EWL_LABEL(o), "OK button clicked.");
        else
                ewl_label_text_set(EWL_LABEL(o), "Cancel button clicked.");

        /* get rid of the dialog box */
        ewl_widget_destroy(EWL_WIDGET(data));
}

static void
dialog_delete_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        ewl_widget_destroy(w);
}

static int
constructor_test(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_dialog_new();

        if (!EWL_DIALOG_IS(c))
        {
                LOG_FAILURE(buf, len, "returned dialog is not of the type"
                                " " EWL_DIALOG_TYPE);
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(c), EWL_DIALOG_TYPE))
        {
                LOG_FAILURE(buf, len, "the dialog's appearance differs to "
                                EWL_DIALOG_TYPE);
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

