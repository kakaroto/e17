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
static void debug_callbacks_append(Ewl_Widget *w);

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
run_window(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data __UNUSED__)
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

        debug_callbacks_append(box);

        return box;
}

static void
reparent_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Embed *emb;
        Ewl_Widget *o;
        Ewl_Container *d2 = data;

        printf("START TOGGEL\n");

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

        printf("END TOGGEL\n");

        ewl_widget_enable(o);
}

static void
del_window_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        ewl_widget_destroy(w);
        ewl_widget_destroy(EWL_WIDGET(data));
}

/*
 * debug code
 */

typedef struct {
        Ewl_Child_Add child_add;
        Ewl_Child_Remove child_remove;
        Ewl_Child_Resize child_resize;
        Ewl_Child_Show child_show; 
        Ewl_Child_Hide child_hide;
} Container_Cb;

static const char *container_cb = "blah";

static void
debug_widget_print(Ewl_Widget *w)
{
        if (w->parent)
        {
                debug_widget_print(w->parent);
                printf("|%s", w->appearance);
        }
        else
                printf("%s(%p)", w->appearance, w);
}

static void
debug_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        const char *cb_name = data;
        printf("%s\t", cb_name);
        debug_widget_print(w);
        printf("\n");
}

static void
debug_destroy_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        void *d = ewl_widget_data_del(w, &container_cb);
        free(d);
        printf("DESTROY  \t");
        debug_widget_print(w);
        printf("\n");
}

static void
debug_child_add(Ewl_Container *c, Ewl_Widget *w)
{
        Container_Cb *cc = ewl_widget_data_get(EWL_WIDGET(c), &container_cb);
        printf("CHILD ADD\t");
        debug_widget_print(EWL_WIDGET(c));
        printf(" += ");
        debug_widget_print(w);
        printf("\n");

        if (cc->child_add)
                cc->child_add(c, w);
}

static void
debug_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx)
{
        Container_Cb *cc = ewl_widget_data_get(EWL_WIDGET(c), &container_cb);
        printf("CHILD REMOVE\t");
        debug_widget_print(EWL_WIDGET(c));
        printf(" -= ");
        debug_widget_print(w);
        printf("\n");
        
        if (cc->child_remove)
                cc->child_remove(c, w, idx);
}

static void
debug_child_resize(Ewl_Container *c, Ewl_Widget *w, int size, Ewl_Orientation o)
{
        Container_Cb *cc = ewl_widget_data_get(EWL_WIDGET(c), &container_cb);
        printf("CHILD RESIZE\t");
        debug_widget_print(EWL_WIDGET(c));
        printf(" %s + %d ", (o == EWL_ORIENTATION_VERTICAL) ? "vert" : "hor",
                        size);
        debug_widget_print(w);
        printf("\n");
        
        if (cc->child_resize)
                cc->child_resize(c, w, size, o);
}

static void
debug_child_show(Ewl_Container *c, Ewl_Widget *w)
{
        Container_Cb *cc = ewl_widget_data_get(EWL_WIDGET(c), &container_cb);
        printf("CHILD SHOW\t");
        debug_widget_print(EWL_WIDGET(c));
        printf(" : ");
        debug_widget_print(w);
        printf("\n");
        
        if (cc->child_show)
                cc->child_show(c, w);
}

static void
debug_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
        Container_Cb *cc = ewl_widget_data_get(EWL_WIDGET(c), &container_cb);
        printf("CHILD HIDE\t");
        debug_widget_print(EWL_WIDGET(c));
        printf(" : ");
        debug_widget_print(w);
        printf("\n");
        
        if (cc->child_hide)
                cc->child_hide(c, w);
}

static void
debug_callbacks_append(Ewl_Widget *w)
{
        ewl_callback_append(w, EWL_CALLBACK_REALIZE, debug_cb,   "REALIZE  ");
        ewl_callback_append(w, EWL_CALLBACK_UNREALIZE, debug_cb, "UNREALIZE");
        ewl_callback_append(w, EWL_CALLBACK_SHOW, debug_cb,      "SHOW     ");
        ewl_callback_append(w, EWL_CALLBACK_HIDE, debug_cb,      "HIDE     ");
        ewl_callback_append(w, EWL_CALLBACK_OBSCURE, debug_cb,   "OBSCURE  ");
        ewl_callback_append(w, EWL_CALLBACK_REVEAL, debug_cb,    "REVEAL   ");
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, debug_cb, "CONFIGURE");
        ewl_callback_append(w, EWL_CALLBACK_REPARENT, debug_cb,  "REPARENT ");

        if (EWL_CONTAINER_IS(w))
        {
                Ewl_Container *c = EWL_CONTAINER(w);
                Ewl_Widget *child;
                Container_Cb *cc = malloc(sizeof(Container_Cb));

                /* append the callbacks although for the children */
                ecore_dlist_first_goto(c->children);
                while ((child = ecore_dlist_next(c->children)))
                        debug_callbacks_append(child);

                cc->child_add = c->child_add;
                cc->child_show = c->child_show;
                cc->child_remove = c->child_remove;
                cc->child_resize = c->child_resize;
                cc->child_hide = c->child_hide;

                ewl_widget_data_set(w, &container_cb, cc);
                ewl_container_add_notify_set(c, debug_child_add);
                ewl_container_show_notify_set(c, debug_child_show);
                ewl_container_remove_notify_set(c, debug_child_remove);
                ewl_container_resize_notify_set(c, debug_child_resize);
                ewl_container_hide_notify_set(c, debug_child_hide);

                ewl_callback_append(w, EWL_CALLBACK_DESTROY, debug_destroy_cb,
                                NULL);
        }
        else
                ewl_callback_append(w, EWL_CALLBACK_DESTROY, debug_cb, 
                                "DESTROY  ");
}

