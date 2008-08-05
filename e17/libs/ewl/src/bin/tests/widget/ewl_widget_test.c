/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void ewl_widget_cb_toggle(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_first_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_second_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_toggle_fullscreen(Ewl_Widget *w, void *ev, void *data);

static int widget_is_test(char *buf, int len);
static int name_test_set_get(char *buf, int len);
static int name_test_nul_set_get(char *buf, int len);
static int name_find_test(char *buf, int len);
static int name_find_missing_test(char *buf, int len);
static int name_find_null_test(char *buf, int len);
static int widget_type_is_test(char *buf, int len);
static int widget_type_is_non_type_test(char *buf, int len);
static int widget_enable_test(char *buf, int len);
static int widget_disable_test(char *buf, int len);
static int widget_colour_test_set_get(char *buf, int len);
static int widget_colour_test_get_null(char *buf, int len);

static int appearance_test_set_get(char *buf, int len);
static int appearance_path_test_set_get(char *buf, int len);
static int inheritance_test_set_get(char *buf, int len);
static int internal_test_set_get(char *buf, int len);
static int unmanaged_test_set_get(char *buf, int len);
static int toplayered_test_set_get(char *buf, int len);
static int layer_priority_test_set_get(char *buf, int len);
static int clipped_test_set_get(char *buf, int len);
static int data_test_set_get(char *buf, int len);
static int data_test_set_remove(char *buf, int len);

static int widget_new(char *buf, int len);
static int init(char *buf, int len);
static int show(char *buf, int len);
static int realize(char *buf, int len);
static int realize_unrealize(char *buf, int len);
static int parent_set(char *buf, int len);
static int parent_set_show(char *buf, int len);
static int reparent_unrealized(char *buf, int len);
static int reparent_realized(char *buf, int len);
static int realize_reveal(char *buf, int len);
static int realize_reveal_obscure(char *buf, int len);
static int realize_reveal_unrealize(char *buf, int len);
static int show_realize_unrealize(char *buf, int len);

static int focusable_test_set_get(char *buf, int len);
static int focus_test_send_get(char *buf, int len);

static Ewl_Unit_Test widget_unit_tests[] = {
                {"EWL_WIDGET_IS", widget_is_test, NULL, -1, 0},
                {"Widget name set/get", name_test_set_get, NULL, -1, 0},
                {"Widget NULL name set/get", name_test_nul_set_get, NULL, -1, 0},
                {"Widget name find", name_find_test, NULL, -1, 0},
                {"Widget missing name find", name_find_missing_test, NULL, -1, 0},
                {"Widget find NULL", name_find_null_test, NULL, -1, 1},
                {"Widget type is test", widget_type_is_test, NULL, -1, 0},
                {"Widget type is without type test", widget_type_is_non_type_test, NULL, -1, 0},
                {"Widget enable", widget_enable_test, NULL, -1, 0},
                {"Widget disable", widget_disable_test, NULL, -1, 0},
                {"Widget colour set/get", widget_colour_test_set_get, NULL, -1, 0},
                {"Widget colour get NULL", widget_colour_test_get_null, NULL, -1, 0},
                {"widget appearance set/get", appearance_test_set_get, NULL, -1, 0},
                {"widget appearance path set/get", appearance_path_test_set_get, NULL, -1, 0},
                {"widget inheritance set/get", inheritance_test_set_get, NULL, -1, 0},
                {"widget internal set/get", internal_test_set_get, NULL, -1, 0},
                {"widget unmanaged set/get", unmanaged_test_set_get, NULL, -1, 0},
                {"widget layer top set/get", toplayered_test_set_get, NULL, -1, 0},
                {"widget layer priority set/get", layer_priority_test_set_get, NULL, -1, 0},
                {"widget clipped set/get", clipped_test_set_get, NULL, -1, 0},
                {"widget data set/get", data_test_set_get, NULL, -1, 0},
                {"widget data set/remove", data_test_set_remove, NULL, -1, 0},
                {"widget_new", widget_new, NULL, -1, 0},
                {"widget_init state", init, NULL, -1, 0},
                {"widget_show state", show, NULL, -1, 0},
                {"widget_realize state", realize, NULL, -1, 0},
                {"widget realize unrealize state", realize_unrealize, NULL, -1, 0},
                {"widget_parent_set state", parent_set, NULL, -1, 0},
                {"widget parent set while shown state", parent_set_show, NULL, -1, 0},
                {"widget Reparent unrealized state", reparent_unrealized, NULL, -1, 0},
                {"widget reparent realized state", reparent_realized, NULL, -1, 0},
                {"widget realize then reveal state", realize_reveal, NULL, -1, 0},
                {"widget realize reveal obscure state", realize_reveal_obscure, NULL, -1, 0},
                {"widget realize reveal unrealize state", realize_reveal_unrealize, NULL, -1, 0},
                {"widget show realize unrealize state", show_realize_unrealize, NULL, -1, 0},
                {"widget focusable set/get", focusable_test_set_get, NULL, -1, 0},
                {"widget focus send/get", focus_test_send_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Widget";
        test->tip = "The base widget type.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->func = create_test;
        test->unit_tests = widget_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *vbox, *b2, *o;

        vbox = ewl_vbox_new();
        ewl_container_child_append(box, vbox);
        ewl_widget_show(vbox);

        b2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), b2);
        ewl_widget_show(b2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "First button");
        ewl_widget_name_set(o, "first_widget");
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_first_click, NULL);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Second button");
        ewl_widget_name_set(o, "second_widget");
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_second_click, NULL);
        ewl_widget_disable(o);
        ewl_widget_show(o);

        b2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), b2);
        ewl_widget_show(b2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Toggle");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_toggle, NULL);
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_widget_show(o);

        b2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), b2);
        ewl_widget_show(b2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Change fullscreen setting");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_widget_cb_toggle_fullscreen, NULL);
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_widget_show(o);

        return 1;
}

static void
ewl_widget_cb_toggle(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *o, *o2;

        o = ewl_widget_name_find("first_widget");
        o2 = ewl_widget_name_find("second_widget");

        if (DISABLED(o))
        {
                ewl_widget_enable(o);
                ewl_widget_disable(o2);
        }
        else
        {
                ewl_widget_disable(o);
                ewl_widget_enable(o2);
        }
}

static void
ewl_widget_cb_first_click(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data __UNUSED__)
{
        printf("First clicked\n");
}

static void
ewl_widget_cb_second_click(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data __UNUSED__)
{
        printf("Second clicked\n");
}

static void
ewl_widget_cb_toggle_fullscreen(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Embed *win;

        win = ewl_embed_widget_find(w);
        ewl_window_fullscreen_set(EWL_WINDOW(win),
                !ewl_window_fullscreen_get(EWL_WINDOW(win)));
}

/*
 * Verify that appearance get returns the same value set.
 */
static int
appearance_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        ewl_widget_appearance_set(w, "my_appearance");
        if (strcmp("my_appearance", ewl_widget_appearance_get(w)))
                LOG_FAILURE(buf, len, "appearance_get doesn't match appearance_set");
        else
                ret = 1;

        return ret;
}

/*
 * Verify that appearance path get returns the correct full path.
 */
static int
appearance_path_test_set_get(char *buf, int len)
{
        Ewl_Widget *box;
        Ewl_Widget *w;
        int ret = 0;

        box = ewl_vbox_new();

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        ewl_container_child_append(EWL_CONTAINER(box), w);

        ewl_widget_appearance_set(w, "my_appearance");
        if (strcmp("/vbox/my_appearance", ewl_widget_appearance_path_get(w)))
                LOG_FAILURE(buf, len, "appearance_path_get doesn't match");
        else
                ret = 1;

        return ret;
}

/*
 * Verify that a widget inherits from a type after the inheritance has been set
 * and does not verify an inheritance that has not been set.
 */
static int
inheritance_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;
        const char *my_class = "myclass";
        const char *unknown_class = "unknownclass";

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        ewl_widget_inherit(w, my_class);
        if (!ewl_widget_type_is(w, my_class))
                LOG_FAILURE(buf, len, "inheritance doesn't contain correct type");
        else {
                if (ewl_widget_type_is(w, unknown_class))
                        LOG_FAILURE(buf, len,
                                        "inheritance contains incorrect type");
                else
                        ret = 1;
        }

        return ret;
}

/*
 * Verify that the internal flag on a widget is set properly after changing
 * between states.
 */
static int
internal_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        if (!ewl_widget_internal_is(w)) {
                ewl_widget_internal_set(w, TRUE);
                if (ewl_widget_internal_is(w)) {
                        ewl_widget_internal_set(w, FALSE);
                        if (ewl_widget_internal_is(w))
                                LOG_FAILURE(buf, len, "internal flag not FALSE");
                        else
                                ret = 1;
                }
                else
                        LOG_FAILURE(buf, len, "internal flag not TRUE");
        }
        else
                LOG_FAILURE(buf, len, "internal set after widget_init");

        return ret;
}

/*
 * Verify that the unmanaged flag on a widget is set properly after changing
 * between states.
 */
static int
unmanaged_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        if (!ewl_widget_unmanaged_is(w)) {
                ewl_widget_unmanaged_set(w, TRUE);
                if (ewl_widget_unmanaged_is(w)) {
                        ewl_widget_unmanaged_set(w, FALSE);
                        if (ewl_widget_unmanaged_is(w))
                                LOG_FAILURE(buf, len, "unmanaged flag not FALSE");
                        else
                                ret = 1;
                }
                else
                        LOG_FAILURE(buf, len, "unmanaged flag not TRUE");
        }
        else
                LOG_FAILURE(buf, len, "unmanaged set after widget_init");

        return ret;
}

/*
 * Verify that the toplayered flag on a widget is set properly after changing
 * between states.
 */
static int
toplayered_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        if (!ewl_widget_layer_top_get(w)) {
                ewl_widget_layer_top_set(w, TRUE);
                if (ewl_widget_layer_top_get(w)) {
                        ewl_widget_layer_top_set(w, FALSE);
                        if (ewl_widget_layer_top_get(w))
                                LOG_FAILURE(buf, len, "toplayered flag not FALSE");
                        else
                                ret = 1;
                }
                else
                        LOG_FAILURE(buf, len, "toplayered flag not TRUE");
        }
        else
                LOG_FAILURE(buf, len, "toplayered set after widget_init");

        return ret;
}

/*
 * Verify that the layer priortiy of a widget is set properly after changing
 * it.
 */
static int
layer_priority_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();

        if (ewl_widget_layer_priority_get(w) == 0)
        {
                ewl_widget_layer_priority_set(w, 10);
                if (ewl_widget_layer_priority_get(w) == 10)
                {
                        ewl_widget_layer_priority_set(w, -20);
                        if (ewl_widget_layer_priority_get(w) == -20)
                                ret = 1;
                        else
                                LOG_FAILURE(buf, len, "layer priority is not"
                                                " -20");
                }
                else
                        LOG_FAILURE(buf, len, "layer priority is not 10");
        }
        else
                LOG_FAILURE(buf, len, "layer priority is unequal zero "
                                        "after widget_init");

        return ret;
}


/*
 * Verify that the clipped flag on a widget is set properly after changing
 * between states.
 */
static int
clipped_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        if (ewl_widget_clipped_is(w)) {
                ewl_widget_clipped_set(w, FALSE);
                if (!ewl_widget_clipped_is(w)) {
                        ewl_widget_clipped_set(w, TRUE);
                        if (!ewl_widget_clipped_is(w))
                                LOG_FAILURE(buf, len, "clipped flag not TRUE");
                        else
                                ret = 1;
                }
                else
                        LOG_FAILURE(buf, len, "clipped flag not FALSE");
        }
        else
                LOG_FAILURE(buf, len, "clipped not set after widget_init");

        return ret;
}

/*
 * Verify that the data returned on a widget matches the data that was set on
 * it.
 */
static int
data_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;
        char *key, *value, *found;

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        key = strdup("Data key");
        value = strdup("Data value");

        ewl_widget_data_set(w, key, value);
        found = ewl_widget_data_get(w, key);

        if (!found)
                LOG_FAILURE(buf, len, "could not find set data");
        else if (found != value)
                LOG_FAILURE(buf, len, "found value does not match set data");
        else
                ret = 1;

        return ret;
}

/*
 * Verify that data removed from a widget is no longer present.
 */
static int
data_test_set_remove(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;
        char *key, *value, *found;

        w = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(w);

        key = strdup("Data key");
        value = strdup("Data value");

        ewl_widget_data_set(w, key, value);
        found = ewl_widget_data_del(w, key);

        if (!found)
                LOG_FAILURE(buf, len, "could not find set data");
        else if (found != value)
                LOG_FAILURE(buf, len, "removed value does not match set data");
        else if (ewl_widget_data_get(w, key))
                LOG_FAILURE(buf, len, "data value present after remove");
        else
                ret = 1;

        return ret;
}

/*
 * Test creation of an instance of a base widget.
 */
static int
widget_new(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        if (!w)
                LOG_FAILURE(buf, len, "Failed to create widget");
        else
        {
                ewl_widget_destroy(w);
                ret = 1;
        }

        return ret;
}

/*
 * Test initialization of a widget and the default visibility state of the
 * widget.
 */
static int
init(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        if (VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget VISIBLE after _init");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after _init");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after _init");
        else
                ret = 1;

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Verify the visibility states of a widget after show.
 */
static int
show(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_show(w);

        if (!VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget !VISIBLE after show");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after show");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after show");
        else
                ret = 1;

        ewl_widget_destroy(w);
        return ret;
}

/*
 * Verify the visibility states of a widget after realize.
 */
static int
realize(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_realize(w);
        if (VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget VISIBLE after realize");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED without window");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after realize");
        else
        {
                Ewl_Widget *win;

                /*
                 * Create a window and add the child to allow the realize to
                 * succeed. This will be using the buffer engine.
                 */
                win = ewl_window_new();
                ewl_embed_engine_name_set(EWL_EMBED(win), "evas_buffer");

                ewl_container_child_append(EWL_CONTAINER(win), w);
                ewl_widget_realize(w);

                if (VISIBLE(win))
                        LOG_FAILURE(buf, len, "Window VISIBLE after realize");
                else if (!REALIZED(win))
                        LOG_FAILURE(buf, len, "Window !REALIZED after realize");
                else if (!REALIZED(w))
                        LOG_FAILURE(buf, len, "Widget !REALIZED after realize");
                else if (REVEALED(w))
                        LOG_FAILURE(buf, len, "Widget REVEALED after realize");
                else if (REVEALED(win))
                        LOG_FAILURE(buf, len, "Window REVEALED after realize");
                else
                        ret = 1;

                ewl_widget_destroy(win);
        }
        ewl_widget_destroy(w);
        return ret;
}

/*
 * Verify the visibility states of a widget after realize and unrealize.
 */
static int
realize_unrealize(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_realize(w);
        ewl_widget_unrealize(w);

        if (VISIBLE(w))
                /* This is the currently expected behavior, but we're discussing
                 * if this is really the behavior we want */
                LOG_FAILURE(buf, len, "Widget VISIBLE after realize/unrealize");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after realize/unrealize");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after realize/unrealize");
        else 
                ret = 1;

        ewl_widget_destroy(w);
        return ret;
}

/*
 * Verify the visibility states of a widget after reparent.
 */
static int
parent_set(char *buf, int len)
{
        Ewl_Widget *w, *b;
        int ret = 0;

        b = ewl_box_new();
        ewl_widget_show(b);

        w = ewl_widget_new();
        ewl_widget_parent_set(w, b);
        if (!w->parent)
                LOG_FAILURE(buf, len, "Widget parent NULL after parent set");
        else if (ewl_widget_parent_get(w) != b)
                LOG_FAILURE(buf, len, "Widget parent wrong after parent set");
        else if (VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget VISIBLE after parent set");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after parent set");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after parent set");
        else
                ret = 1;

        ewl_widget_destroy(w);
        ewl_widget_destroy(b);
        return ret;
}

/*
 * Verify the visibility states of a widget after show and reparent.
 */
static int
parent_set_show(char *buf, int len)
{
        Ewl_Widget *w, *b;
        int ret = 0;

        b = ewl_box_new();
        ewl_widget_show(b);

        w = ewl_widget_new();
        ewl_widget_show(w);
        ewl_widget_parent_set(w, b);

        if (!w->parent)
                LOG_FAILURE(buf, len, "Parent NULL after parent_set");
        else if (!VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget !VISIBLE after parent_set");
        else if (REALIZED(w))
                /* The widget has not been realized yet as that happens in the
                 * idle loop, so this should test that it is still not realized
                 * after changing parents. */
                LOG_FAILURE(buf, len, "Widget REALIZED after parent_set");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after parent_set");
        else
                ret = 1;

        ewl_widget_destroy(w);
        ewl_widget_destroy(b);
        return ret;
}

/*
 * Verify the visibility states of a widget after reparent of an unrealized
 * widget.
 */
static int
reparent_unrealized(char *buf, int len)
{
        Ewl_Widget *w, *b1, *b2;
        int ret = 0;

        b1 = ewl_box_new();
        ewl_widget_show(b1);

        b2 = ewl_box_new();
        ewl_widget_show(b2);

        w = ewl_widget_new();
        ewl_widget_parent_set(w, b1);
        ewl_widget_parent_set(w, b2);

        if (!w->parent)
                LOG_FAILURE(buf, len, "Widget parent NULL after reparent");
        else if (w->parent != b2)
                LOG_FAILURE(buf, len, "Widget parent != b2 after reparent");
        else if (VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget VISIBLE after reparent");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after reparent");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after reparent");
        else 
                ret = 1;

        ewl_widget_destroy(w);
        ewl_widget_destroy(b1);
        ewl_widget_destroy(b2);
        return ret;
}

static int
reparent_realized(char *buf, int len)
{
        Ewl_Widget *w, *b1, *b2;
        int ret = 0;

        b1 = ewl_box_new();
        ewl_widget_show(b1);

        b2 = ewl_box_new();
        ewl_widget_show(b2);

        w = ewl_widget_new();
        ewl_widget_show(w);
        ewl_widget_parent_set(w, b1);
        ewl_widget_parent_set(w, b2);

        if (!w->parent)
                LOG_FAILURE(buf, len, "Widget parent NULL after reparent");
        else if (w->parent != b2)
                LOG_FAILURE(buf, len, "Widget parent != b2 after reparent");
        else if (!VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget !VISIBLE after reparent");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after reparent");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after reparent");
        else 
                ret = 1;

        ewl_widget_destroy(w);
        ewl_widget_destroy(b1);
        ewl_widget_destroy(b2);

        return ret;
}

static int
realize_reveal(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_realize(w);
        ewl_widget_reveal(w);

        if (VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget VISIBLE after realize/reveal");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after realize/reveal");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after realize/reveal");
        else 
        {
                Ewl_Widget *win;

                /*
                 * Create a window and add the child to allow the realize to
                 * succeed. This will be using the buffer engine.
                 */
                win = ewl_window_new();
                ewl_embed_engine_name_set(EWL_EMBED(win), "evas_buffer");

                ewl_container_child_append(EWL_CONTAINER(win), w);
                ewl_widget_realize(w);
                ewl_widget_reveal(w);

                if (VISIBLE(win))
                        LOG_FAILURE(buf, len,
                                        "Window VISIBLE after realize/reveal");
                else if (!REALIZED(win))
                        LOG_FAILURE(buf, len,
                                        "Window !REALIZED after realize/reveal");
                else if (!REALIZED(w))
                        LOG_FAILURE(buf, len,
                                        "Widget !REALIZED after realize/reveal");
                else if (!REVEALED(w))
                        LOG_FAILURE(buf, len,
                                        "Widget !REVEALED after realize/reveal");
                else if (REVEALED(win))
                        LOG_FAILURE(buf, len,
                                        "Window REVEALED after realize/reveal");
                else
                        ret = 1;

                ewl_widget_destroy(win);
        }

        ewl_widget_destroy(w);
        return ret;
}

static int
realize_reveal_obscure(char *buf, int len)
{
        Ewl_Widget *w;
        Ewl_Widget *win;
        int ret = 0;

        w = ewl_widget_new();

        /*
         * Create a window and add the child to allow the realize to
         * succeed. This will be using the buffer engine.
         */
        win = ewl_window_new();
        ewl_embed_engine_name_set(EWL_EMBED(win), "evas_buffer");

        ewl_container_child_append(EWL_CONTAINER(win), w);
        ewl_widget_realize(w);
        ewl_widget_reveal(w);
        ewl_widget_obscure(w);

        if (VISIBLE(win))
                LOG_FAILURE(buf, len,
                                "Window VISIBLE after realize/reveal/obscure");
        else if (!REALIZED(win))
                LOG_FAILURE(buf, len,
                                "Window !REALIZED after realize/reveal/obscure");
        else if (!REALIZED(w))
                LOG_FAILURE(buf, len,
                                "Widget !REALIZED after realize/reveal/obscure");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len,
                                "Widget REVEALED after realize/reveal/obscure");
        else if (REVEALED(win))
                LOG_FAILURE(buf, len,
                                "Window REVEALED after realize/reveal/obscure");
        else if (REVEALED(win))
                LOG_FAILURE(buf, len,
                                "Window REVEALED after realize/reveal/obscure");
        else
                ret = 1;

        ewl_widget_destroy(win);
        ewl_widget_destroy(w);
        return ret;
}

static int
realize_reveal_unrealize(char *buf, int len)
{
        Ewl_Widget *w;
        Ewl_Widget *win;
        int ret = 0;

        w = ewl_widget_new();

        /*
         * Create a window and add the child to allow the realize to
         * succeed. This will be using the buffer engine.
         */
        win = ewl_window_new();
        ewl_embed_engine_name_set(EWL_EMBED(win), "evas_buffer");

        ewl_container_child_append(EWL_CONTAINER(win), w);
        ewl_widget_realize(w);
        ewl_widget_reveal(w);
        ewl_widget_unrealize(w);

        if (VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget VISIBLE after realize/reveal/unrealize");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after realize/reveal/unrealize");
        else if (!REALIZED(win))
                LOG_FAILURE(buf, len, "Window !REALIZED after realize/reveal/unrealize");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after realize/reveal/unrealize");
        else if (REVEALED(win))
                LOG_FAILURE(buf, len, "Window REVEALED after realize/reveal/unrealize");
        else 
                ret = 1;

        ewl_widget_destroy(win);
        ewl_widget_destroy(w);
        return ret;
}

static int
show_realize_unrealize(char *buf, int len)
{
        Ewl_Widget *w;
        Ewl_Widget *win;
        int ret = 0;

        w = ewl_widget_new();

        /*
         * Create a window and add the child to allow the realize to
         * succeed. This will be using the buffer engine.
         */
        win = ewl_window_new();
        ewl_embed_engine_name_set(EWL_EMBED(win), "evas_buffer");

        ewl_container_child_append(EWL_CONTAINER(win), w);
        ewl_widget_show(w);
        ewl_widget_realize(w);
        ewl_widget_unrealize(w);

        if (!VISIBLE(w))
                LOG_FAILURE(buf, len, "Widget not VISIBLE after show/realize/unrealize");
        else if (REALIZED(w))
                LOG_FAILURE(buf, len, "Widget REALIZED after show/realize/unrealize");
        else if (!REALIZED(win))
                LOG_FAILURE(buf, len, "Window !REALIZED after show/realize/unrealize");
        else if (REVEALED(w))
                LOG_FAILURE(buf, len, "Widget REVEALED after show/realize/unrealize");
        else if (REVEALED(win))
                LOG_FAILURE(buf, len, "Window REVEALED after show/realize/unrealize");
        else 
                ret = 1;

        ewl_widget_destroy(win);
        ewl_widget_destroy(w);
        return ret;
}

/*
 * Test the EWL_WIDGET_IS macro (and the widget setting its type correctly)
 */
static int
widget_is_test(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        if (!EWL_WIDGET_IS(w))
                LOG_FAILURE(buf, len, "Widget is not Widget type");
        else
                ret = 1;
        
        ewl_widget_destroy(w);
        return ret;
}

/*
 * Test the name set/get functions
 */
static int
name_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;
        const char *name;

        w = ewl_widget_new();
        ewl_widget_name_set(w, "test widget");
        name = ewl_widget_name_get(w);
        if (strcmp("test widget", name))
                LOG_FAILURE(buf, len, "Returned name '%s' not 'test widget'", name);
        else
                ret = 1;
        
        ewl_widget_destroy(w);
        return ret;
}

/*
 * Set the widgets name to NULL
 */
static int
name_test_nul_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_name_set(w, "test name");
        ewl_widget_name_set(w, NULL);
        if (NULL != ewl_widget_name_get(w))
                LOG_FAILURE(buf, len, "Widget name not NULL");
        else
                ret = 1;

        ewl_widget_destroy(w);
        return ret;
}

/*
 * Test the name find function
 */
static int
name_find_test(char *buf, int len)
{
        Ewl_Widget *w1, *w2;
        int ret = 0;

        w1 = ewl_widget_new();
        ewl_widget_name_set(w1, "test widget");
        w2 = ewl_widget_name_find("test widget");
        if (w1 != w2)
                LOG_FAILURE(buf, len, "widget found not equal to widget set");
        else
                ret = 1;

        ewl_widget_destroy(w1);
        return ret;
}

/*
 * Search for a name that won't be in the hash
 */
static int
name_find_missing_test(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_name_find("Missing widget name");
        if (w != NULL)
                LOG_FAILURE(buf, len, "Found widget when we shouldn't have");
        else
                ret = 1;

        return ret;
}

/*
 * Find with a NULL name passed in
 */
static int
name_find_null_test(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_name_find(NULL);
        if (w != NULL)
                LOG_FAILURE(buf, len, "Found widget when searching for NULL");
        else
                ret = 1;
        return ret;
}

/*
 * test the ewl_widget_type_is function for a type on the widget
 */
static int
widget_type_is_test(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_inherit(w, "my type");
        if (!ewl_widget_type_is(w, "my type"))
                LOG_FAILURE(buf, len, "Failed to match 'my type' on widget");
        else
                ret = 1;

        ewl_widget_destroy(w);
        return ret;
}

/* 
 * test the ewl_widget_type_is for a non existant type on the widget
 */
static int
widget_type_is_non_type_test(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        if (ewl_widget_type_is(w, "my missing type"))
                LOG_FAILURE(buf, len, "Matchined 'my missing type' on widget without type set");
        else
                ret = 1;

        ewl_widget_destroy(w);
        return ret;
}

/*
 * Test the widget enable function
 */
static int
widget_enable_test(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_disable(w);
        ewl_widget_enable(w);

        if (DISABLED(w))
                LOG_FAILURE(buf, len, "Widget DISABLED after calling enable");
        else
                ret = 1;

        ewl_widget_destroy(w);
        return ret;
}

/*
 * Test the ewl_widget_disable function
 */
static int
widget_disable_test(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();

        if (!DISABLED(w))
        {
                ewl_widget_disable(w);
                if (!DISABLED(w))
                        LOG_FAILURE(buf, len, "Widget not disabled aftering "
                                        "calling disable");
                else
                        ret = 1;
        }
        else
                LOG_FAILURE(buf, len, "New created widget is disabled");

        ewl_widget_destroy(w);
        return ret;
}

/*
 * Test the colour set/get functions
 */
static int
widget_colour_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;
        unsigned int r = 0, g = 0, b = 0, a = 0;

        w = ewl_widget_new();
        ewl_widget_color_set(w, 248, 148, 48, 255);
        ewl_widget_color_get(w, &r, &g, &b, &a);
        if (r != 248)
                LOG_FAILURE(buf, len, "Red colour not retrieved correctly");
        else if (g != 148)
                LOG_FAILURE(buf, len, "Green colour not retrived correctly");
        else if (b != 48)
                LOG_FAILURE(buf, len, "Blue colour not retrived correctly");
        else if (a != 255)
                LOG_FAILURE(buf, len, "Alpha colour not retrieved correctly");
        else
                ret = 1;

        ewl_widget_destroy(w);
        return ret;
}

/*
 * Call color_get with the destination pointers as NULL. This will only
 * really fail if it crashes or throws warnings. Nothing to check.
 */
static int
widget_colour_test_get_null(char *buf __UNUSED__, int len __UNUSED__)
{
        Ewl_Widget *w;

        w = ewl_widget_new();
        ewl_widget_color_get(w, NULL, NULL, NULL, NULL);
        return 1;
}

/*
 * Call focusable_set and verify that the widget is now flagged to accept focus
 * events.
 */
static int
focusable_test_set_get(char *buf, int len)
{
        Ewl_Widget *w;
        unsigned int focusable;
        int ret = 0;

        w = ewl_widget_new();
        focusable = ewl_widget_focusable_get(w);
        if (focusable) {
                ewl_widget_focusable_set(w, FALSE);
                focusable = ewl_widget_focusable_get(w);
                if (!focusable) {
                        ewl_widget_focusable_set(w, TRUE);
                        focusable = ewl_widget_focusable_get(w);
                        if (focusable)
                                ret = 1;
                        else
                                LOG_FAILURE(buf, len, "focusable set to FALSE");
                }
                else
                        LOG_FAILURE(buf, len, "focusable set to TRUE");
        }
        else
                LOG_FAILURE(buf, len, "default focusable set to FALSE");

        return ret;
}

/*
 * Send focus to a specific widget and verify the widget has the current focus.
 */
static int
focus_test_send_get(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_widget_focus_send(w);

        if (ewl_widget_focused_get() == w)
                LOG_FAILURE(buf, len, "focused with no embed");
        else {
                Ewl_Widget *embed;

                embed = ewl_embed_new();
                ewl_container_child_append(EWL_CONTAINER(embed), w);
                ewl_widget_focus_send(w);

                if (ewl_widget_focused_get() == w)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "widget not focused");
        }

        return ret;
}
