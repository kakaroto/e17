#include <Ewl.h>

Ewl_Widget *win;
Ewl_Widget *hbox, *vbox, *vbox2;
Ewl_Widget *button[3];
Ewl_Widget *h_seeker, *v_seeker;

static void
cb_window_close(Ewl_Widget * w, void *event_data, void *user_data)
{
	ewl_widget_destroy(h_seeker);
	ewl_widget_destroy(v_seeker);
	ewl_main_quit();

	return;
	w = NULL;
	event_data = NULL;
	user_data = NULL;
}

static void
cb_set_seeker_value(Ewl_Widget * w, void *event_data, void *user_data)
{
	ewl_seeker_set_value(h_seeker, rand() % 100);
	ewl_seeker_set_value(v_seeker, rand() % 100);

	return;
	w = NULL;
	event_data = NULL;
	user_data = NULL;
}

static void
cb_get_seeker_value(Ewl_Widget * w, void *event_data, void *user_data)
{
	printf("h_seeker value: %f\n", ewl_seeker_get_value(h_seeker));
	printf("v_seeker value: %f\n", ewl_seeker_get_value(v_seeker));

	return;
	w = NULL;
	event_data = NULL;
	user_data = NULL;
}

int
main(int argc, char **argv)
{
	ewl_init(argc, argv);

/*
	ewl_theme_data_set_default("/appearance/box/horizontal/base",
			"/appearance/seeker/horizontal/base.bits.db");
*/

/*	ewl_theme_data_gen_default_theme_db("./theme.db");

	exit(-1);*/

	win = ewl_window_new();
	ewl_window_set_title(win, "Ewl Demo");
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cb_window_close,
			    win);
	ewl_window_resize(win, 300, 400);

	ewl_widget_show(win);

	vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(win), vbox);
	ewl_widget_show(vbox);

	hbox = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	vbox2 = ewl_vbox_new();

/*
	ewl_theme_data_set(vbox2, "/appearance/box/vertical/base/visible",
			   "no");
*/
	ewl_container_append_child(EWL_CONTAINER(hbox), vbox2);
	ewl_widget_show(vbox2);

	button[0] = ewl_button_new("Set Value");
	ewl_object_set_custom_size(EWL_OBJECT(button[0]), 100, 30);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    cb_set_seeker_value, NULL);
	ewl_container_append_child(EWL_CONTAINER(vbox2), button[0]);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Get Value");
	ewl_object_set_custom_size(EWL_OBJECT(button[1]), 100, 30);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    cb_get_seeker_value, NULL);
	ewl_container_append_child(EWL_CONTAINER(vbox2), button[1]);
	ewl_widget_show(button[1]);

	button[2] = ewl_checkbutton_new("Test");

/*	ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
			    cb_window_close, NULL);
*/
	ewl_container_append_child(EWL_CONTAINER(vbox2), button[2]);
	ewl_widget_show(button[2]);

	v_seeker = ewl_vseeker_new();
	ewl_container_append_child(EWL_CONTAINER(hbox), v_seeker);

/*	ewl_theme_data_set(hbox, "/appearance/box/horisontal/base",
				 "/appearance/box/horisontal/base.bits.db");
*/
	ewl_widget_show(v_seeker);

	h_seeker = ewl_hseeker_new();
	ewl_container_append_child(EWL_CONTAINER(vbox), h_seeker);
	ewl_widget_show(h_seeker);

	ewl_main();

	exit(-1);
}
