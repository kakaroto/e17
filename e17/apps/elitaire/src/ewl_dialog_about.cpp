/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "Eli_App.h"
#include <Ewl.h>
#include "ewl_dialogs.h"
#include <Ecore_X.h>
#include <stdio.h>

static void _about_destroy_cb(Ewl_Widget * w, void * event, void * data);

void ewl_frontend_dialog_about_open(Eli_App * eap)
{
    Ewl_Widget * win;
    Ewl_Widget * o;
    char text[2000];
    
    /* Setup and show the about window */
    win = ewl_icondialog_new();
    ewl_dialog_action_position_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);
    ewl_window_title_set(EWL_WINDOW(win), _("About"));
    ewl_window_name_set(EWL_WINDOW(win), "Elitaire About");
    ewl_window_class_set(EWL_WINDOW(win), "Elitaire About");
    ewl_window_transient_for_foreign(EWL_WINDOW(win), 
		    		EWL_EMBED_WINDOW(eap->main_win));
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, _about_destroy_cb,
                        win);
    ewl_dialog_has_separator_set(EWL_DIALOG(win), 1);
    ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_NONE);
    ewl_widget_show(win);

    /* Setup and show the stock icons */
    ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);
    o = ewl_button_new();
    ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
    ewl_container_child_append(EWL_CONTAINER(win), o);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, _about_destroy_cb, win);
    ewl_widget_show(o);

    snprintf(text, 2000, _("%s version %s\n\nAuthor:\n  %s\n"
                           "  %s\n  %s\n\nThanks to:\n  %s\n  %s"),
             PACKAGE_NAME,
             PACKAGE_VERSION,
             "Peter Wehrfritz",
             "peter.wehrfritz@web.de",
             "www.mowem.de/elitaire",
             "The Enlightenment Project",
             "www.enlightenment.org");

    /* Textbox */
    ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_TOP);
    o = ewl_text_new();
    ewl_text_text_set(EWL_TEXT(o), text);
    ewl_container_child_append(EWL_CONTAINER(win), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);

    ewl_widget_show(o);

}

static void _about_destroy_cb(Ewl_Widget * w, void *event, void * data)
{
    Ewl_Widget * win;

    win = (Ewl_Widget *) data;
    ewl_widget_destroy(win);
}

