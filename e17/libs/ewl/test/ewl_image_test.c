#include <Ewl.h>
#include <ewl-config.h>

static Ewl_Widget *image_button;
Ewl_Widget *image_win;
Ewl_Widget *image;
Ewd_DList *images;
Ewl_Widget *entry_path;


void __create_image_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);


void
__destroy_image_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char *str;

	ewd_dlist_goto_first(images);

	while ((str = ewd_dlist_next(images)) != NULL)
		FREE(str);

	ewl_widget_destroy(w);

	ewl_callback_append(image_button, EWL_CALLBACK_CLICKED,
			    __create_image_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

int
__image_exists(char *i)
{
	struct stat st;

	if (!i || !strlen(i))
		return -1;

	if (((stat(i, &st)) == -1) || !S_ISREG(st.st_mode))
		return -1;

	return 1;
}

void
__image_update_window_size(void)
{
	int ww, hh;

	ewl_object_get_current_size(EWL_OBJECT(image), &ww, &hh);

	if (ww < 390)
		ww = 390;
	if (hh < 297)
		hh = 297;
	else
		hh += 100;

	ewl_window_resize(image_win, ww, hh);
	ewl_window_set_min_size(image_win, ww, hh);
}

void
__image_goto_prev_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char *img = NULL;

	ewd_dlist_previous(images);
	img = ewd_dlist_current(images);

	if (!img)
		img = ewd_dlist_goto_last(images);

	ewl_entry_set_text(entry_path, img);
	ewl_image_set_file(image, img);

	ewl_widget_configure(image_win);

	__image_update_window_size();

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__image_load_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char *img = NULL;

	img = ewl_entry_get_text(entry_path);

	if (img && __image_exists(img))
	  {
		  ewd_dlist_append(images, img);
		  ewd_dlist_goto_last(images);
		  ewl_image_set_file(image, img);
	  }
	else
		printf("ERROR: %s does not exist\n", img);


	ewl_widget_configure(image_win);

	__image_update_window_size();

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__image_goto_next_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char *img = NULL;

	ewd_dlist_next(images);
	img = ewd_dlist_current(images);

	if (!img)
		img = ewd_dlist_goto_first(images);

	ewl_entry_set_text(entry_path, img);
	ewl_image_set_file(image, img);

	ewl_widget_configure(image_win);

	__image_update_window_size();

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_image_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *main_vbox;
	Ewl_Widget *button_hbox;
	Ewl_Widget *button_prev, *button_load, *button_next;
	char *image_file = NULL;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_image_test_window);

	image_button = w;

	images = ewd_dlist_new();

	image_win = ewl_window_new();
	ewl_window_resize(image_win, 390, 297);
	ewl_window_set_min_size(image_win, 390, 297);
	ewl_callback_append(image_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_image_test_window, NULL);
	ewl_widget_show(image_win);

	main_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(image_win), main_vbox);
	ewl_box_set_spacing(main_vbox, 10);
	ewl_widget_show(main_vbox);

	if ((__image_exists(PACKAGE_DATA_DIR "/images/e-logo.png")) != -1)
		image_file = strdup(PACKAGE_DATA_DIR "/images/e-logo.png");
	else if ((__image_exists("./data/images/e-logo.png")) != -1)
		image_file = strdup("./data/images/e-logo.png");
	else if ((__image_exists("../data/images/e-logo.png")) != -1)
		image_file = strdup("../data/images/e-logo.png");

	image = ewl_image_load(image_file);
	ewl_object_set_padding(EWL_OBJECT(image), 0, 0, 5, 0);
	ewl_object_set_alignment(EWL_OBJECT(image), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), image);
	ewl_widget_show(image);

	if (image_file)
		ewd_dlist_append(images, image_file);

	entry_path = ewl_entry_new();
	if (image_file)
		ewl_entry_set_text(entry_path, image_file);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), entry_path);
	ewl_widget_show(entry_path);

	button_hbox = ewl_hbox_new();
	ewl_box_set_spacing(button_hbox, 5);
	ewl_object_set_alignment(EWL_OBJECT(button_hbox),
				 EWL_ALIGNMENT_CENTER);
	ewl_object_set_custom_size(EWL_OBJECT(button_hbox), 112, 38);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), button_hbox);
	ewl_widget_show(button_hbox);

	button_prev = ewl_button_new(NULL);
	button_load = ewl_button_new(NULL);
	button_next = ewl_button_new(NULL);

	ewl_object_set_custom_size(EWL_OBJECT(button_prev), 32, 32);
	ewl_object_set_custom_size(EWL_OBJECT(button_load), 32, 32);
	ewl_object_set_custom_size(EWL_OBJECT(button_next), 32, 32);

	ewl_container_append_child(EWL_CONTAINER(button_hbox), button_prev);
	ewl_container_append_child(EWL_CONTAINER(button_hbox), button_load);
	ewl_container_append_child(EWL_CONTAINER(button_hbox), button_next);

	image_file = NULL;

	if ((__image_exists(PACKAGE_DATA_DIR "/images/button_prev.bits.db"))
	    != -1)
		image_file =
			strdup(PACKAGE_DATA_DIR
			       "/images/button_prev.bits.db");
	else if ((__image_exists("./data/images/button_prev.bits.db")) != -1)
		image_file = strdup("./data/images/button_prev.bits.db");
	else if ((__image_exists("../data/images/button_prev.bits.db")) != -1)
		image_file = strdup("../data/images/button_prev.bits.db");

	ewl_theme_data_set(button_prev, "/appearance/button/default/base",
			   image_file);

	IF_FREE(image_file);

	if ((__image_exists(PACKAGE_DATA_DIR "/images/button_load.bits.db"))
	    != -1)
		image_file =
			strdup(PACKAGE_DATA_DIR
			       "/images/button_load.bits.db");
	else if ((__image_exists("./data/images/button_load.bits.db")) != -1)
		image_file = strdup("./data/images/button_load.bits.db");
	else if ((__image_exists("../data/images/button_load.bits.db")) != -1)
		image_file = strdup("../data/images/button_load.bits.db");

	ewl_theme_data_set(button_load, "/appearance/button/default/base",
			   image_file);

	IF_FREE(image_file);

	if ((__image_exists(PACKAGE_DATA_DIR "/images/button_next.bits.db"))
	    != -1)
		image_file =
			strdup(PACKAGE_DATA_DIR
			       "/images/button_next.bits.db");
	else if ((__image_exists("./data/images/button_next.bits.db")) != -1)
		image_file = strdup("./data/images/button_next.bits.db");
	else if ((__image_exists("../data/images/button_next.bits.db")) != -1)
		image_file = strdup("../data/images/button_next.bits.db");

	ewl_theme_data_set(button_next, "/appearance/button/default/base",
			   image_file);

	IF_FREE(image_file);

	ewl_callback_append(button_prev, EWL_CALLBACK_CLICKED,
			    __image_goto_prev_cb, NULL);
	ewl_callback_append(button_load, EWL_CALLBACK_CLICKED,
			    __image_load_cb, NULL);
	ewl_callback_append(button_next, EWL_CALLBACK_CLICKED,
			    __image_goto_next_cb, NULL);

	ewl_widget_show(button_prev);
	ewl_widget_show(button_load);
	ewl_widget_show(button_next);

	image_file = NULL;

	if ((__image_exists(PACKAGE_DATA_DIR "/images/e17-border.bits.db")) !=
	    -1)
		image_file =
			strdup(PACKAGE_DATA_DIR "/images/e17-border.bits.db");
	else if ((__image_exists("./data/images/e17-border.bits.db")) != -1)
		image_file = strdup("./data/images/e17-border.bits.db");
	else if ((__image_exists("../data/images/e17-border.bits.db")) != -1)
		image_file = strdup("../data/images/e17-border.bits.db");

	if (image_file)
		ewd_dlist_append(images, image_file);

	return;
	ev_data = NULL;
	user_data = NULL;
}
