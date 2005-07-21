#include "ewl_test.h"

static Ewl_Widget *text_button;

static void
__destroy_text_test_window(Ewl_Widget *w, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(text_button, EWL_CALLBACK_CLICKED,
				__create_text_test_window, NULL);
}

static void
__trigger_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
	printf("%s\n", (char *)data);
}

static void
__trigger_cb_mouse_in(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Text_Trigger *t;

	t = EWL_TEXT_TRIGGER(w);

	ewl_text_cursor_position_set(EWL_TEXT(t->parent), t->pos);
	ewl_text_color_apply(EWL_TEXT(t->parent), 255, 0, 0, 255, t->len);

#if 0
	printf("DUMP\n");
	ewl_text_btree_dump((EWL_TEXT(t->parent))->formatting, "");
	printf("DUMP DONE\n");
#endif
}

static void
__trigger_cb_mouse_out(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Text_Trigger *t;

	t = EWL_TEXT_TRIGGER(w);

	ewl_text_cursor_position_set(EWL_TEXT(t->parent), t->pos);
	ewl_text_color_apply(EWL_TEXT(t->parent), 0, 0, 0, 255, t->len);
}

void
__create_text_test_window(Ewl_Widget *w, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Widget *win, *o;
	Ewl_Text_Trigger *trigger;
	int len;

	text_button = w;

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "Text Test");
	ewl_window_name_set(EWL_WINDOW(win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(win), "EFL Test Application");
	ewl_object_size_request(EWL_OBJECT(win), 200, 400);

	if (w)
	{
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
				 	__create_text_test_window);
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
					__destroy_text_test_window, NULL);
	}
	else
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(win);

	o = ewl_text_new(NULL);
	ewl_container_child_append(EWL_CONTAINER(win), o);
	ewl_text_bg_color_set(EWL_TEXT(o), 50, 50, 50, 255);

	ewl_text_text_insert(EWL_TEXT(o), "The first bunch of text\n", 0); /* 24 */

	ewl_text_cursor_position_set(EWL_TEXT(o), 10);
	ewl_text_color_apply(EWL_TEXT(o), 0, 0, 255, 255, 5);

	ewl_text_text_append(EWL_TEXT(o), "The second bunch of text\n"); /* 25 */
	
	ewl_text_font_size_set(EWL_TEXT(o), 20);
	ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_DOUBLE_UNDERLINE 
						| EWL_TEXT_STYLE_OUTLINE 
						| EWL_TEXT_STYLE_SOFT_SHADOW);
	ewl_text_double_underline_color_set(EWL_TEXT(o), 255, 0, 0, 255);
	ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);
	ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);
	ewl_text_text_append(EWL_TEXT(o), "The third bunch of text\n"); /* 24 */

	ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_NONE);
	ewl_text_text_insert(EWL_TEXT(o), "The fourth bunch of text\n", 31); /* 25 */

        trigger = ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_TRIGGER);
	ewl_text_trigger_start_pos_set(trigger, ewl_text_length_get(EWL_TEXT(o)));
	ewl_text_text_append(EWL_TEXT(o), "This is the link."); /* 17 */

	len = ewl_text_cursor_position_get(EWL_TEXT(o)) - 
			ewl_text_trigger_start_pos_get(trigger) + 1;
	ewl_text_trigger_length_set(trigger, len);

	ewl_container_child_append(EWL_CONTAINER(o), EWL_WIDGET(trigger));
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_MOUSE_UP, 
			__trigger_cb, "You clicked the trigger, have a cookie.");
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_FOCUS_IN,
			__trigger_cb_mouse_in, NULL);
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_FOCUS_OUT,
			__trigger_cb_mouse_out, NULL);

	ewl_text_text_insert(EWL_TEXT(o), "The fifth bunch of text\n", 0); /* 24 */

	ewl_text_cursor_position_set(EWL_TEXT(o), 0);
	ewl_text_color_apply(EWL_TEXT(o), 255, 0, 0, 255, 24);

	ewl_text_text_insert(EWL_TEXT(o), "The sixth bunch of text\n", 24); /* 24 */

	ewl_text_cursor_position_set(EWL_TEXT(o), 43);
	ewl_text_color_apply(EWL_TEXT(o), 0, 255, 0, 255, 14);

	ewl_text_color_set(EWL_TEXT(o), 255, 0, 0, 255);
	ewl_text_text_append(EWL_TEXT(o), "And in red\n"); /* 11 */
	ewl_widget_show(o);

#if 0
	printf("-- DUMP --\n");
	ewl_text_btree_dump((EWL_TEXT(o))->formatting, "");
	printf("-- DUMP DONE --\n");
#endif
}

