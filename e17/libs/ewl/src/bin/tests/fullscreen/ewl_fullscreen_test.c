/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void ewl_widget_cb_click(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Fullscreen";
        test->tip = "Testing fullscreen.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->func = create_test;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *vbox, *o;
        Ewl_Embed *emb;

        vbox = ewl_vbox_new();
        ewl_container_child_append(box, vbox);
        ewl_widget_show(vbox);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "button");
        ewl_container_child_append(EWL_CONTAINER(vbox), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_click, NULL);
        ewl_widget_show(o);

        emb = ewl_embed_widget_find(EWL_WIDGET(box));
        ewl_window_fullscreen_set(EWL_WINDOW(emb), TRUE);

        return 1;
}

static void
ewl_widget_cb_click(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Embed *emb;

        emb = ewl_embed_widget_find(w);
        if (ewl_window_fullscreen_get(EWL_WINDOW(emb)))
                ewl_window_fullscreen_set(EWL_WINDOW(emb), FALSE);
        else
                ewl_window_fullscreen_set(EWL_WINDOW(emb), TRUE);
}

