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

void
__create_text_test_window(Ewl_Widget *w, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Widget *win, *o;

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

	ewl_text_text_insert(EWL_TEXT(o), "The first bunch of text\n", 0);

	ewl_text_cursor_position_set(EWL_TEXT(o), 10);
	ewl_text_color_apply(EWL_TEXT(o), 0, 0, 255, 255, 5);

	ewl_text_text_append(EWL_TEXT(o), "The second bunch of text\n");
	
	ewl_text_font_size_set(EWL_TEXT(o), 20);
	ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_DOUBLE_UNDERLINE 
						| EWL_TEXT_STYLE_OUTLINE 
						| EWL_TEXT_STYLE_SOFT_SHADOW);
	ewl_text_double_underline_color_set(EWL_TEXT(o), 255, 0, 0, 255);
	ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);
	ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);
	ewl_text_text_append(EWL_TEXT(o), "The third bunch of text\n");
	ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_NONE);

	ewl_text_text_insert(EWL_TEXT(o), "The fourth bunch of text\n", 31);

	ewl_text_text_insert(EWL_TEXT(o), "The fifth bunch of text\n", 0);

	ewl_text_cursor_position_set(EWL_TEXT(o), 0);
	ewl_text_color_apply(EWL_TEXT(o), 255, 0, 0, 255, 24);

	ewl_text_text_insert(EWL_TEXT(o), "The sixth bunch of text\n", 24);

	ewl_text_cursor_position_set(EWL_TEXT(o), 43);
	ewl_text_color_apply(EWL_TEXT(o), 0, 255, 0, 255, 14);

	ewl_text_color_set(EWL_TEXT(o), 255, 0, 0, 255);
	ewl_text_text_append(EWL_TEXT(o), "And in red\n");
	ewl_widget_show(o);

#if 0
	printf("DUMP\n");
	ewl_text_btree_dump((EWL_TEXT(o))->formatting, "");
	printf("DUMP DONE\n");
#endif
}

