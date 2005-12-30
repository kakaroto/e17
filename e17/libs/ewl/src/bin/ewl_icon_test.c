#include "ewl_test.h"

static Ewl_Widget *icon_button;

static void
cb_destroy_win(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(icon_button, EWL_CALLBACK_CLICKED,
				__create_icon_test_window, NULL);
}

void
__create_icon_test_window(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Widget *win, *box, *o, *o2;

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "Icon Test");
	ewl_window_name_set(EWL_WINDOW(win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(win), "EFL Test Application");

	if (w) 
	{
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
					__create_icon_test_window);
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
						cb_destroy_win, NULL);
	}
	else
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);

	ewl_widget_show(win);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), box);
	ewl_widget_show(box);

	o = ewl_icon_new();
	ewl_icon_image_set(EWL_ICON(o), 
				PACKAGE_DATA_DIR "/images/Draw.png", NULL);
	ewl_icon_label_set(EWL_ICON(o), "Draw (Editable)");
	ewl_icon_editable_set(EWL_ICON(o), TRUE);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o2 = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(o2), "This icon has\nextended data\n set "
					"on it.\n\n That data is just \n"
					"text, but could\nbe any widget.");
	ewl_widget_show(o2);

	o = ewl_icon_new();
	ewl_icon_image_set(EWL_ICON(o), 
				PACKAGE_DATA_DIR "/images/World.png", NULL);
	ewl_icon_extended_data_set(EWL_ICON(o), o2);
	ewl_icon_label_set(EWL_ICON(o), "World");
	ewl_icon_type_set(EWL_ICON(o), EWL_ICON_TYPE_LONG);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);
}

