/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void ewl_widget_cb_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_close_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_go_modal(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Modal";
        test->tip = "Testing modal windows.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->func = create_test;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *vbox, *o;

        vbox = ewl_vbox_new();
        ewl_container_child_append(box, vbox);
        ewl_widget_show(vbox);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Launch modal window");
        ewl_container_child_append(EWL_CONTAINER(vbox), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_click, NULL);
        ewl_widget_show(o);

        return 1;
}

static void
ewl_widget_cb_click(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Embed *emb;
        Ewl_Widget *win, *vbox, *o;

        emb = ewl_embed_widget_find(w);

        win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(win), "MODAL");
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                                        ewl_widget_cb_close_click, NULL);
        ewl_widget_show(win);

        vbox = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(win), vbox);
        ewl_widget_show(vbox);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Go modal");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_widget_cb_go_modal, emb);
        ewl_container_child_append(EWL_CONTAINER(vbox), o);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Close");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_widget_cb_close_click, NULL);
        ewl_container_child_append(EWL_CONTAINER(vbox), o);
        ewl_widget_show(o);
}

static void
ewl_widget_cb_close_click(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Embed *emb;

        emb = ewl_embed_widget_find(w);
        ewl_widget_destroy(EWL_WIDGET(emb));
}

static void
ewl_widget_cb_go_modal(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Embed *emb;

        emb = ewl_embed_widget_find(w);
        ewl_window_transient_for(EWL_WINDOW(emb), EWL_WINDOW(data));
        ewl_window_modal_set(EWL_WINDOW(emb), TRUE);
}
