/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_menubar.h"
#include "ewl_separator.h"
#include "ewl_spacer.h"

#include <stdio.h>

static int create_test(Ewl_Container *win);

extern Ewl_Unit_Test menubar_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Menubar";
        test->tip = "Defines a menubar that can be used to hold menus.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = menubar_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *h_menubar = NULL, *v_menubar = NULL;

        h_menubar = ewl_hmenubar_new();
        {
                int i;
                Ewl_Widget *item;
                Ewl_Widget *foo;
                char *m_items[] = {"File",
                                   "Test",
                                   "About",
                                   NULL};
                for(i = 0; m_items[i] != NULL; i++) {

                        if (i != 0) {
                                Ewl_Widget *sep;

                                sep = ewl_vseparator_new();
                                ewl_container_child_append(EWL_CONTAINER(h_menubar), sep);
                                ewl_widget_show(sep);
                        }

                        item = ewl_menu_new();
                        ewl_button_label_set(EWL_BUTTON(item), m_items[i]);
                        ewl_container_child_append(EWL_CONTAINER(h_menubar), item);
                        ewl_widget_show(item);

                        foo = ewl_menu_item_new();
                        ewl_button_label_set(EWL_BUTTON(foo), "foo");
                        ewl_container_child_append(EWL_CONTAINER(item), foo);
                        ewl_widget_show(foo);

                        foo = ewl_menu_item_new();
                        ewl_button_label_set(EWL_BUTTON(foo), "foobar");
                        ewl_container_child_append(EWL_CONTAINER(item), foo);
                        ewl_widget_show(foo);

                }

                item = ewl_spacer_new();
                ewl_object_fill_policy_set(EWL_OBJECT(item), EWL_FLAG_FILL_ALL);
                ewl_container_child_append(EWL_CONTAINER(h_menubar), item);
                ewl_widget_show(item);

                item = ewl_menu_new();
                ewl_button_label_set(EWL_BUTTON(item), "Help");
                ewl_container_child_append(EWL_CONTAINER(h_menubar), item);
                ewl_widget_show(item);

                foo = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(foo), "foo");
                ewl_container_child_append(EWL_CONTAINER(item), foo);
                ewl_widget_show(foo);

                foo = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(foo), "foobar");
                ewl_container_child_append(EWL_CONTAINER(item), foo);
                ewl_widget_show(foo);
        }
        ewl_container_child_append(EWL_CONTAINER(box), h_menubar);
        ewl_widget_show(h_menubar);

        v_menubar = ewl_vmenubar_new();
        {
                int i;
                Ewl_Widget *item;
                char *m_items[] = {"About",
                                   "Left",
                                   "Right",
                                   "foo",
                                   NULL};
                for(i = 0; m_items[i] != NULL; i++) {
                        Ewl_Widget *foo;

                        if (i != 0 && (i & 1))
                        {
                                Ewl_Widget *sep;

                                sep = ewl_hseparator_new();
                                ewl_container_child_append(EWL_CONTAINER(v_menubar), sep);
                                ewl_widget_show(sep);
                        }

                        item = ewl_menu_new();
                        ewl_button_label_set(EWL_BUTTON(item), m_items[i]);
                        ewl_container_child_append(EWL_CONTAINER(v_menubar), item);
                        ewl_widget_show(item);

                        foo = ewl_menu_item_new();
                        ewl_button_label_set(EWL_BUTTON(foo), "foo");
                        ewl_container_child_append(EWL_CONTAINER(item), foo);
                        ewl_widget_show(foo);

                        foo = ewl_menu_item_new();
                        ewl_button_label_set(EWL_BUTTON(foo), "foobar");
                        ewl_container_child_append(EWL_CONTAINER(item), foo);
                        ewl_widget_show(foo);

                }
        }
        ewl_container_child_append(EWL_CONTAINER(box), v_menubar);
        ewl_object_alignment_set(EWL_OBJECT(v_menubar), EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(v_menubar);

        return 1;
}

