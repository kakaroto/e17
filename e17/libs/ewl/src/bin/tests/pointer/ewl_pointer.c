#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>

static int create_test(Ewl_Container *win);
static void cb_render(Ewl_Widget *w, void *ev, void *data);
static void cb_destroy(Ewl_Widget *w, void *ev, void *data);
static int timer_cb_in(void *data);
static int timer_cb_out(void *data);

#define WIN_NAME "buffer_engine_window"

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

	o = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(o), "Expect to see a button cursor\n"
		       "over this entry and a normal cursor\n"
		       "over other parts of the window");
	ewl_entry_editable_set(EWL_ENTRY(o), FALSE);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	pointer_win = ewl_window_new();
	ewl_widget_name_set(pointer_win, WIN_NAME);
	ewl_object_fill_policy_set(EWL_OBJECT(pointer_win), EWL_FLAG_FILL_ALL);
	ewl_object_size_request(EWL_OBJECT(pointer_win), 64, 64);
	ewl_embed_engine_name_set(EWL_EMBED(pointer_win), "evas_buffer");
	ewl_callback_append(pointer_win, EWL_CALLBACK_VALUE_CHANGED,
			cb_render, o);
	ewl_callback_append(pointer_win, EWL_CALLBACK_DESTROY,
			cb_destroy, o);
	ewl_widget_appearance_set(pointer_win, "none");
	ewl_widget_show(pointer_win);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(pointer_win), o);
	ewl_widget_color_set(EWL_WIDGET(o), 255, 255, 255, 200);
	ewl_widget_show(o);

	cur_timer = ecore_timer_add(2.0, timer_cb_in, o);

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
cb_render(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	int old, handle;
	int width, height;
	Ewl_Embed *parent;
	Ewl_Embed *emb = EWL_EMBED(w);
	Ewl_Widget *entry = EWL_WIDGET(data);

	parent = ewl_embed_widget_find(entry);

	width = ewl_object_current_w_get(EWL_OBJECT(emb));
	height = ewl_object_current_h_get(EWL_OBJECT(emb));

	old = (int)ewl_attach_get(entry, EWL_ATTACH_TYPE_MOUSE_CURSOR);
	if (old >= EWL_MOUSE_CURSOR_MAX)
		ewl_engine_pointer_free(parent, old);

	handle = ewl_engine_pointer_data_new(parent, emb->evas_window, width,
			height);

	ewl_attach_mouse_cursor_set(entry, handle);
	if (parent->cursor == old)
		ewl_embed_mouse_cursor_set(entry);
}

static void
cb_destroy(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	if (cur_timer)
		ecore_timer_del(cur_timer);
	cur_timer = NULL;
}
