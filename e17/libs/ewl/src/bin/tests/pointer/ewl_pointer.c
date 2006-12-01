#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>

static int create_test(Ewl_Container *win);
static void cb_destroy(Ewl_Widget *w, void *ev, void *data);
static int timer_cb_in(void *data);
static int timer_cb_out(void *data);

static Ecore_Timer *cur_timer;

void 
test_info(Ewl_Test *test)
{
	test->name = "Pointer";
	test->tip = "Creates a custom cursor from a buffer engine window.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *pointer_win;
	Ewl_Widget *o;

	pointer_win = ewl_cursor_new();
	ewl_widget_show(pointer_win);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(pointer_win), o);
	ewl_widget_color_set(EWL_WIDGET(o), 255, 255, 255, 200);
	ewl_widget_show(o);

	cur_timer = ecore_timer_add(2.0, timer_cb_in, o);

	o = ewl_entry_new();
	ewl_attach_mouse_argb_cursor_set(o, pointer_win);
	ewl_text_text_set(EWL_TEXT(o), "Expect to see a button cursor\n"
		       "over this entry and a normal cursor\n"
		       "over other parts of the window");
	ewl_entry_editable_set(EWL_ENTRY(o), FALSE);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_callback_append(pointer_win, EWL_CALLBACK_DESTROY,
			cb_destroy, o);
	ewl_widget_show(o);

	return 1;
}

static int
timer_cb_in(void *data)
{
	Ewl_Widget *button = EWL_WIDGET(data);

	ewl_callback_call(button, EWL_CALLBACK_MOUSE_IN);

	cur_timer = ecore_timer_add(2.0, timer_cb_out, data);

	return 0;
}

static int
timer_cb_out(void *data)
{
	Ewl_Widget *button = EWL_WIDGET(data);

	ewl_callback_call(button, EWL_CALLBACK_MOUSE_OUT);

	cur_timer = ecore_timer_add(2.0, timer_cb_in, data);

	return 0;
}

static void
cb_destroy(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	if (cur_timer)
		ecore_timer_del(cur_timer);
	cur_timer = NULL;
}
