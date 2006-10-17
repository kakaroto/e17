#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>

/**
 * @addtogroup Ewl_Paned
 * @section pane_infi Information
 * The Ewl_Paned widget is designed to hold multiple panes if needed. If you
 * want to have three panes or four panges just add the panes and it will
 * add the grabbers as needed. The paned widget is an Ewl_Container so to
 * add panes you just use the container functions to append, prepend and
 * insert as needed.
 * 
 * @section init_layout Initial Layout
 * If you need to set the initial sizes of the items in the grabber then you
 * will need to set their preferred size. If, after the paned is already
 * displayed, you need to change the size it will use the current size of
 * the pane.
 */

static int create_test(Ewl_Container *box);
static void ewl_paned_test_cb_clicked_destroy(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_paned_test_cb_clicked(Ewl_Widget *w, void *ev, void *data);
static void ewl_paned_test_cb_add(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Paned";
	test->tip = "Defines the Ewl_Paned to hold two resizable panes.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
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
        ewl_button_label_set(EWL_BUTTON(o), "HFill | VShrink");
        ewl_object_fill_policy_set(EWL_OBJECT(o),
                        EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
        ewl_container_child_append(EWL_CONTAINER(pane2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "HFill | VShrink");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "HShrink | VFill");
        ewl_object_fill_policy_set(EWL_OBJECT(o),
                        EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
        ewl_container_child_append(EWL_CONTAINER(pane2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "HShrink | VFill");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "shrink");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
        ewl_container_child_append(EWL_CONTAINER(pane2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked, "Shrink");
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
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
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
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
        ewl_container_child_append(EWL_CONTAINER(p), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                        ewl_paned_test_cb_clicked_destroy, "New Button");
        ewl_widget_show(o);
}

