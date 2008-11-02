/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_icon.h"
#include "ewl_seeker.h"
#include "ewl_separator.h"
#include "ewl_toolbar.h"

#include <stdio.h>

static int create_test(Ewl_Container *win);
static void cb_click(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test toolbar_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Toolbar";
        test->tip = "Defines a widget for displaying a toolbar";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = toolbar_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *body, *o2, *o;

        body = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), body);
        ewl_widget_show(body);

        o2 = ewl_toolbar_new();
        ewl_container_child_append(EWL_CONTAINER(body), o2);
        ewl_object_alignment_set(EWL_OBJECT(o2), EWL_FLAG_ALIGN_CENTER);
        ewl_toolbar_icon_part_hide(EWL_TOOLBAR(o2), EWL_ICON_PART_LABEL);
        ewl_widget_show(o2);

        o = ewl_icon_simple_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_ARROW_LEFT);
        ewl_callback_append(EWL_WIDGET(o), EWL_CALLBACK_CLICKED, cb_click, o2);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_icon_simple_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_ARROW_RIGHT);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_icon_simple_new();
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_MEDIA_STOP);
        ewl_widget_show(o);

        o = ewl_icon_simple_new();
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_HOME);
        ewl_widget_show(o);

        o = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_hseeker_new();
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        return 1;
}

void
cb_click(Ewl_Widget *w __UNUSED__, void *e __UNUSED__, void *data)
{
        ewl_toolbar_icon_part_hide(EWL_TOOLBAR(data), EWL_ICON_PART_NONE);
}


