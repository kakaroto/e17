#include <Ewl.h>

void button_down(Ewl_Widget *w, void *ev_data, void *user_data)
{
	printf("%s\n", ewl_button_get_label(EWL_BUTTON(w)));
}

int main(int argc, char **argv)
{
	Ewl_Widget *win;
	Ewl_Widget *button;

	ewl_init(&argc, argv);

	win = ewl_window_new();
	ewl_widget_show(win);

	button = ewl_button_new("Hello World!");
	ewl_container_append_child(EWL_CONTAINER(win), button);
	ewl_object_set_fill_policy(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
	ewl_object_set_alignment(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
	ewl_callback_append(button, EWL_CALLBACK_MOUSE_DOWN, button_down, NULL);
	ewl_widget_show(button);

	ewl_main();

	return 0;
}
