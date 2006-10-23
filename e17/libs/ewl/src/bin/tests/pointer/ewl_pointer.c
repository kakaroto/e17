#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>

static int create_test(Ewl_Container *win);
static void cb_configure(Ewl_Widget *w, void *ev, void *data);

#define WIN_NAME "buffer_engine_window"

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
			cb_configure, o);
	ewl_widget_show(pointer_win);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(pointer_win), o);
	ewl_widget_show(o);

	return 1;
}

static void
cb_configure(Ewl_Widget *w, void *ev, void *data)
{
	int handle;
	int width, height;
	Ewl_Embed *parent;
	Ewl_Embed *emb = EWL_EMBED(w);
	Ewl_Widget *entry = EWL_WIDGET(data);

	width = ewl_object_current_w_get(EWL_OBJECT(emb));
	height = ewl_object_current_h_get(EWL_OBJECT(emb));

	parent = ewl_embed_widget_find(entry);

	handle = (int)ewl_attach_get(entry, EWL_ATTACH_TYPE_MOUSE_CURSOR);
	if (handle >= EWL_MOUSE_CURSOR_MAX)
		ewl_engine_pointer_free(parent, handle);

	handle = ewl_engine_pointer_data_new(parent, emb->evas_window, width,
			height);

	printf("Setting cursor %d\n", handle);
	ewl_attach_mouse_cursor_set(entry, handle);
}
