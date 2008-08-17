/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_dialog.h"
#include "ewl_seeker.h"
#include "ewl_checkbutton.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void run_window(Ewl_Widget *w, void *ev, void *data);
static Ewl_Widget *fill_window(Ewl_Dialog *d, Ewl_Dialog *d2, Ewl_Widget *box);
static Ewl_Widget *setup_window(void);
static void reparent_cb(Ewl_Widget *w, void *ev, void *data);
static void del_window_cb(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Reparent";
        test->tip = "Test to reparent a widget.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = NULL;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o;

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Launch the windows");
        ewl_container_child_append(box, o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, run_window, NULL);
        ewl_widget_show(o);

        return 1;
}

static void
run_window(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Widget *win1, *win2, *box;

        /* create the to windows */
        win1 = setup_window();
        win2 = setup_window();

        box = fill_window(EWL_DIALOG(win1), EWL_DIALOG(win2), NULL);
        fill_window(EWL_DIALOG(win2), EWL_DIALOG(win1), box);
}

static Ewl_Widget *
setup_window(void)
{
        Ewl_Widget *win;
        
        win = ewl_dialog_new();
        ewl_window_title_set(EWL_WINDOW(win), "Reparent Dialog");
        ewl_widget_show(win);

        return win;
}

static Ewl_Widget *
fill_window(Ewl_Dialog *d, Ewl_Dialog *d2, Ewl_Widget *box)
{
        Ewl_Widget *o;

        /* first append the delete callback for the dialog */
        ewl_callback_append(EWL_WIDGET(d), EWL_CALLBACK_DELETE_WINDOW,
                        del_window_cb, d2);

        /* add the buttons */
        ewl_dialog_active_area_set(d, EWL_POSITION_BOTTOM);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Fetch the content");
        ewl_container_child_append(EWL_CONTAINER(d), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, reparent_cb, d2);
        ewl_widget_show(o);

        /* and now put the content in */
        ewl_dialog_active_area_set(d, EWL_POSITION_TOP);

        if (box)
                return NULL;

        box = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(d), box);
        ewl_widget_show(box);

        ewl_widget_disable(EWL_WIDGET(o));

        o = ewl_seeker_new();
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);
        
        o = ewl_checkbutton_new();
        ewl_button_label_set(EWL_BUTTON(o), "Click me");
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        return box;
}

static void
reparent_cb(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Embed *emb;
        Ewl_Widget *o;
        Ewl_Container *d2 = data;

        /* get the box to reparent from the other dialog */
        ewl_dialog_active_area_set(EWL_DIALOG(d2), EWL_POSITION_TOP);
        ewl_container_child_iterate_begin(d2);
        o = ewl_container_child_next(d2);
        
        /* now let us append the box to the dialog of the pressed button */
        emb = ewl_embed_widget_find(w);
        ewl_dialog_active_area_set(EWL_DIALOG(emb), EWL_POSITION_TOP);
        ewl_container_child_append(EWL_CONTAINER(emb), o);

        /* work is done for this button, disable it */
        ewl_widget_disable(w);

        /* we still need to enable the other button */
        ewl_dialog_active_area_set(EWL_DIALOG(d2), EWL_POSITION_BOTTOM);
        ewl_container_child_iterate_begin(d2);
        o = ewl_container_child_next(d2);

        ewl_widget_enable(o);
}

static void
del_window_cb(Ewl_Widget *w, void *ev, void *data)
{
        ewl_widget_destroy(w);
        ewl_widget_destroy(EWL_WIDGET(data));
}

