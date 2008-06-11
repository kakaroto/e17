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

/* unit tests */
static int initial_size_get(char *buf, int len);
static int initial_size_unset_get(char *buf, int len);
static int initial_size_after_remove_get(char *buf, int len);
static int initial_size_many_get(char *buf, int len);
static int fixed_size_get(char *buf, int len);
static int fixed_size_unset_get(char *buf, int len);

static Ewl_Unit_Test paned_unit_tests[] = {
                {"get initial size", initial_size_get, NULL, -1, 0},
                {"get unset initial size", initial_size_unset_get, NULL, -1, 0},
                {"get initial size after remove", initial_size_after_remove_get, NULL, -1, 0},
                {"get initial size for many widgets", initial_size_many_get, NULL, -1, 0},
                {"get the fixed size flag", fixed_size_get, NULL, -1, 0},
                {"get the unset fixed size flag", fixed_size_unset_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

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

static int 
initial_size_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        ewl_paned_initial_size_set(EWL_PANED(paned), child, 240);
        val = ewl_paned_initial_size_get(EWL_PANED(paned), child);

        if (val != 240) {
                LOG_FAILURE(buf, len, "get value is different from the set one");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
initial_size_unset_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        val = ewl_paned_initial_size_get(EWL_PANED(paned), child);

        if (val != 0) {
                LOG_FAILURE(buf, len, "get value is not 0");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
initial_size_after_remove_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        int val;

        /* use a hpaned here to cover this up */
        paned = ewl_hpaned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        ewl_paned_initial_size_set(EWL_PANED(paned), child, 240);
        ewl_container_child_remove(EWL_CONTAINER(paned), child);
        val = ewl_paned_initial_size_get(EWL_PANED(paned), child);

        if (val != 0) {
                LOG_FAILURE(buf, len, "get value is not zero");
                ret = 0;
        }

        ewl_widget_destroy(child);
        ewl_widget_destroy(paned);

        return ret;
}

static int 
initial_size_many_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *w[4];
        int ret = 1, i;

        /* use a vpaned here to cover this up */
        paned = ewl_vpaned_new();

        /* build the children and set the initial size for them */
        for (i = 0; i < 4; i++) {
                w[i] = ewl_cell_new();
                ewl_container_child_append(EWL_CONTAINER(paned), w[i]);
                ewl_paned_initial_size_set(EWL_PANED(paned), w[i], 240 + i);
        }

        /* now check the set values */
        for (i = 0; i < 4; i++) {
                int val = ewl_paned_initial_size_get(EWL_PANED(paned), w[i]);

                if (val != 240 + i) {
                        LOG_FAILURE(buf, len, "get value is not zero");
                        ret = 0;
                        break;
                }
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
fixed_size_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        unsigned int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        ewl_paned_fixed_size_set(EWL_PANED(paned), child, TRUE);
        val = ewl_paned_fixed_size_get(EWL_PANED(paned), child);

        if (!val) {
                LOG_FAILURE(buf, len, "get value is different from the set one");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
fixed_size_unset_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        unsigned int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        val = ewl_paned_fixed_size_get(EWL_PANED(paned), child);

        if (val) {
                LOG_FAILURE(buf, len, "get value is not FALSE");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

