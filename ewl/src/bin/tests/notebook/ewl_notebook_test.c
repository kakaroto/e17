/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_border.h"
#include "ewl_label.h"
#include "ewl_notebook.h"
#include "ewl_radiobutton.h"

#include <stdio.h>
#include <string.h>

static int create_test(Ewl_Container *box);
static Ewl_Widget *create_page(const char *name);
static Ewl_Widget *create_main_page(void);
static void notebook_cb_toggle_clicked(Ewl_Widget *w, void *ev, void *data);
static void notebook_change_align(Ewl_Widget *w, void *ev, void *data);
static void notebook_change_position(Ewl_Widget *w, void *ev, void *data);
static void notebook_append_page(Ewl_Widget *w, void *ev, void *data);
static void notebook_prepend_page(Ewl_Widget *w, void *ev, void *data);
static void notebook_delete_page(Ewl_Widget *w, void *ev, void *data);

static int count = 1;

extern Ewl_Unit_Test notebook_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Notebook";
        test->tip = "Provides a container whose children\n"
                        "are pages that can be switched\n"
                        "between using tab labels along one\nedge";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = notebook_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *n, *o, *o2;
        char buf[10];

        n = ewl_notebook_new();
        ewl_container_child_append(EWL_CONTAINER(box), n);
        ewl_widget_name_set(n, "notebook");
        ewl_widget_show(n);

        /* append 3 pages */
        for (count = 1; count < 4; count++)
        {
                o2 = ewl_label_new();
                snprintf(buf, sizeof(buf), "Page %d", count);
                ewl_label_text_set(EWL_LABEL(o2), buf);
                ewl_widget_show(o2);

                o = create_page(buf);
                ewl_container_child_append(EWL_CONTAINER(n), o);
                ewl_notebook_page_tab_widget_set(EWL_NOTEBOOK(n), o, o2);
                ewl_widget_show(o);
        }

        /* insert a page after the first */
        o2 = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o2), "Page 1.5");
        ewl_widget_show(o2);

        o = create_page("Page 1.5");
        ewl_container_child_insert(EWL_CONTAINER(n), o, 1);
        ewl_notebook_page_tab_widget_set(EWL_NOTEBOOK(n), o, o2);
        ewl_widget_show(o);

        /* prepend the main page */
        o = create_main_page();
        ewl_container_child_prepend(EWL_CONTAINER(n), o);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(n), o, "Main");
        ewl_widget_show(o);

        ewl_notebook_visible_page_set(EWL_NOTEBOOK(n), o);

        return 1;
}

static void
notebook_cb_toggle_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data __UNUSED__)
{
        Ewl_Widget *n;

        n = ewl_widget_name_find("notebook");
        ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(n),
                        !ewl_notebook_tabbar_visible_get(EWL_NOTEBOOK(n)));
}

static void
notebook_cb_homogeneous_toggle_clicked(Ewl_Widget *w __UNUSED__,
                                void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Widget *n;

        n = ewl_widget_name_find("notebook");
        ewl_notebook_tabbar_homogeneous_set(EWL_NOTEBOOK(n),
                        !ewl_notebook_tabbar_homogeneous_get(EWL_NOTEBOOK(n)));
}

static void
notebook_change_align(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data)
{
        Ewl_Widget *n;
        unsigned int align = EWL_FLAG_ALIGN_CENTER;
        char *pos;

        pos = data;
        if (!strcmp(pos, "Top"))
                align = EWL_FLAG_ALIGN_TOP;
        else if (!strcmp(pos, "Bottom"))
                align = EWL_FLAG_ALIGN_BOTTOM;
        else if (!strcmp(pos, "Left"))
                align = EWL_FLAG_ALIGN_LEFT;
        else if (!strcmp(pos, "Right"))
                align = EWL_FLAG_ALIGN_RIGHT;
        else if (!strcmp(pos, "Center"))
                align = EWL_FLAG_ALIGN_CENTER;

        n = ewl_widget_name_find("notebook");
        ewl_notebook_tabbar_alignment_set(EWL_NOTEBOOK(n), align);
}

static void
notebook_change_position(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data)
{
        Ewl_Widget *n;
        unsigned int align = EWL_POSITION_TOP;
        char *pos;

        pos = data;
        if (!strcmp(pos, "Top"))
                align = EWL_POSITION_TOP;
        else if (!strcmp(pos, "Bottom"))
                align = EWL_POSITION_BOTTOM;
        else if (!strcmp(pos, "Left"))
                align = EWL_POSITION_LEFT;
        else if (!strcmp(pos, "Right"))
                align = EWL_POSITION_RIGHT;

        n = ewl_widget_name_find("notebook");
        ewl_notebook_tabbar_position_set(EWL_NOTEBOOK(n), align);
}

static void
notebook_append_page(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *o2, *o, *vis, *n;
        char buf[10];
        int idx;

        n = ewl_widget_name_find("notebook");

        vis = ewl_notebook_visible_page_get(EWL_NOTEBOOK(n));
        idx = ewl_container_child_index_get(EWL_CONTAINER(n), vis);

        o2 = ewl_label_new();
        snprintf(buf, sizeof(buf), "Page %d", count++);
        ewl_label_text_set(EWL_LABEL(o2), buf);
        ewl_widget_show(o2);

        o = create_page(buf);
        ewl_container_child_insert(EWL_CONTAINER(n), o, idx + 1);
        ewl_notebook_page_tab_widget_set(EWL_NOTEBOOK(n), o, o2);
        ewl_widget_show(o);
}

static void
notebook_prepend_page(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *n, *o2, *o, *vis;
        char buf[10];
        int idx;

        n = ewl_widget_name_find("notebook");

        vis = ewl_notebook_visible_page_get(EWL_NOTEBOOK(n));
        idx = ewl_container_child_index_get(EWL_CONTAINER(n), vis);

        o2 = ewl_label_new();
        snprintf(buf, sizeof(buf), "Page %d", count++);
        ewl_label_text_set(EWL_LABEL(o2), buf);
        ewl_widget_show(o2);

        o = create_page(buf);
        ewl_container_child_insert(EWL_CONTAINER(n), o, idx);
        ewl_notebook_page_tab_widget_set(EWL_NOTEBOOK(n), o, o2);
        ewl_widget_show(o);
}

static void
notebook_delete_page(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data)
{
        Ewl_Widget *n;

        n = ewl_widget_name_find("notebook");
        ewl_container_child_remove(EWL_CONTAINER(n), EWL_WIDGET(data));
}

static Ewl_Widget *
create_main_page(void)
{
        Ewl_Widget *box2, *o, *o2 = NULL, *body, *border;
        unsigned int i;
        char *alignment[] = {"Top", "Left", "Center", "Right", "Bottom"};
        char *position[] = {"Top", "Left", "Right", "Bottom"};

        /* box to hold everything */
        body = ewl_vbox_new();

        /* title label */
        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "Main");
        ewl_container_child_append(EWL_CONTAINER(body), o);
        ewl_widget_show(o);

        /* box to hold alignment and position lists */
        box2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(body), box2);
        ewl_widget_show(box2);

        /* box to hold alignment list */
        border = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(border), "Tabbar Alignment");
        ewl_container_child_append(EWL_CONTAINER(box2), border);
        ewl_widget_show(border);

        for (i = 0; i < (sizeof(alignment) / sizeof(char *)); i++)
        {
                o = ewl_radiobutton_new();
                ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
                ewl_button_label_set(EWL_BUTTON(o), alignment[i]);
                ewl_container_child_append(EWL_CONTAINER(border), o);
                ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                        notebook_change_align, alignment[i]);

                if (o2) ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o),
                                                        EWL_RADIOBUTTON(o2));

                if (!strcmp(alignment[i], "Center"))
                        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(o), TRUE);

                ewl_widget_show(o);

                o2 = o;
        }

        /* box to hold position list */
        border = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(border), "Tabbar Position");
        ewl_container_child_append(EWL_CONTAINER(box2), border);
        ewl_widget_show(border);

        o2 = NULL;
        for (i = 0; i < (sizeof(position) / sizeof(char *)); i++)
        {
                o = ewl_radiobutton_new();
                ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
                ewl_button_label_set(EWL_BUTTON(o), position[i]);
                ewl_container_child_append(EWL_CONTAINER(border), o);
                ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                        notebook_change_position, position[i]);

                if (o2) ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o),
                                                        EWL_RADIOBUTTON(o2));

                if (!strcmp(alignment[i], "Top"))
                        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(o), TRUE);

                ewl_widget_show(o);

                o2 = o;
        }

        o = ewl_checkbutton_new();
        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(o), TRUE);
        ewl_button_label_set(EWL_BUTTON(o), "Show tabbar");
        ewl_container_child_append(EWL_CONTAINER(body), o);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                notebook_cb_toggle_clicked, NULL);
        ewl_widget_show(o);

        o = ewl_checkbutton_new();
        ewl_button_label_set(EWL_BUTTON(o), "Homogeneous tabbar");
        ewl_container_child_append(EWL_CONTAINER(body), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                notebook_cb_homogeneous_toggle_clicked, NULL);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(o);

        return body;
}

static Ewl_Widget *
create_page(const char *name)
{
        Ewl_Widget *box, *box2, *o;

        box = ewl_vbox_new();
        ewl_box_spacing_set(EWL_BOX(box), 10);
        ewl_widget_show(box);

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), name);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        box2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), box2);
        ewl_object_alignment_set(EWL_OBJECT(box2), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(box2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Prepend Page");
        ewl_container_child_append(EWL_CONTAINER(box2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                notebook_prepend_page, box);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Append Page");
        ewl_container_child_append(EWL_CONTAINER(box2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                notebook_append_page, box);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Remove This Page");
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                notebook_delete_page, box);
        ewl_widget_show(o);

        return box;
}

