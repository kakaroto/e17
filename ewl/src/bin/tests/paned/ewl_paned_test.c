/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_paned.h"

#include <stdio.h>

static int create_test(Ewl_Container *box);
static void ewl_paned_test_cb_clicked_destroy(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_paned_test_cb_clicked(Ewl_Widget *w, void *ev, void *data);
static void ewl_paned_test_cb_add(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test paned_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Paned";
        test->tip = "Defines the Ewl_Paned to hold resizable panes.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = paned_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *pane1, *pane2, *pane3, *o, *pbox;

        pane1 = ewl_vpaned_new();
        ewl_container_child_append(EWL_CONTAINER(box), pane1);
        ewl_widget_show(pane1);

        pane2 = ewl_hpaned_new();
        ewl_container_child_append(EWL_CONTAINER(pane1), pane2);
        ewl_widget_show(pane2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Fill");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_FILL);
        ewl_container_child_append(EWL_CONTAINER(pane2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "Fill");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "HFill");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(pane2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "HFill");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "VFill");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_VFILL);
        ewl_container_child_append(EWL_CONTAINER(pane2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "VFill");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "shrinkable");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINKABLE);
        ewl_container_child_append(EWL_CONTAINER(pane2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "Shrinkable");
        ewl_widget_show(o);

        pane3 = ewl_hpaned_new();
        ewl_container_child_append(EWL_CONTAINER(pane1), pane3);
        ewl_widget_show(pane3);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Left Top");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(o),
                                EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);
        ewl_container_child_append(EWL_CONTAINER(pane3), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "Left Top");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Center");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(pane3), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "Center");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Right Bottom");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(o),
                        EWL_FLAG_ALIGN_RIGHT | EWL_FLAG_ALIGN_BOTTOM);
        ewl_container_child_append(EWL_CONTAINER(pane3), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "Right Bottom");
        ewl_widget_show(o);

        pbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(pane1), pbox);
        ewl_object_fill_policy_set(EWL_OBJECT(pbox),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_NONE);
        ewl_widget_show(pbox);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Add to top pane");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_paned_test_cb_add, pane2);
        ewl_container_child_append(EWL_CONTAINER(pbox), o);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Add to bottom pane");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_paned_test_cb_add, pane3);
        ewl_container_child_append(EWL_CONTAINER(pbox), o);
        ewl_widget_show(o);

        return 1;
}

static void
ewl_paned_test_cb_clicked(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data)
{
        printf("Clicked %s\n", (char *)data);
        ewl_widget_hide(w);
}

static void
ewl_paned_test_cb_clicked_destroy(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data)
{
        printf("Destroy Clicked %s\n", (char *)data);
        ewl_widget_destroy(w);
}

static void
ewl_paned_test_cb_add(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                                void *data)
{
        Ewl_Widget *o;
        Ewl_Paned *p;

        p = data;

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "New Button");
        ewl_container_child_append(EWL_CONTAINER(p), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked_destroy, "New Button");
        ewl_widget_show(o);
}
