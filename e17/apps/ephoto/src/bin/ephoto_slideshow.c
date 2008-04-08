#include "ephoto.h"

static int change_picture(void *data);
static void destroy(Ewl_Widget *w, void *event, void *data);
static void show_first_image(Ewl_Widget *w, void *event, void *data);
static Ecore_Timer *change;
static Ewl_Widget *win;

static int 
change_picture(void *data)
{
	char *image_path;
	Ewl_Widget *image;

	image = data;	
	
	ecore_dlist_next(em->images);
	image_path = ecore_dlist_current(em->images);

	if(!image_path)
	{
		ecore_timer_del(change);
		ewl_widget_destroy(win);
		return 1;
	}
	
	ewl_image_file_set(EWL_IMAGE(image), image_path, NULL);
	
	return 1;
}

static void 
destroy(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(win);
	ecore_timer_del(change);
}

static void 
show_first_image(Ewl_Widget *w, void *event, void *data)
{
	char *image_path;
	
	ecore_dlist_first_goto(em->images);
	image_path = ecore_dlist_current(em->images);

	ewl_image_file_set(EWL_IMAGE(w), image_path, NULL);

	change = ecore_timer_add(5, change_picture, w);
	
	return;
}

void 
start_slideshow(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *cell, *image;

	win = add_window("Ephoto Slideshow!", 0, 0, destroy, NULL);
	ewl_window_fullscreen_set(EWL_WINDOW(win), 1);
	ewl_callback_append(win, EWL_CALLBACK_CLICKED, destroy, NULL);

	cell = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(cell), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(win), cell);
	ewl_theme_data_str_set(cell, "/cell/file", 
				PACKAGE_DATA_DIR "/images/single_vbox.edj");
        ewl_theme_data_str_set(cell, "/cell/group", "bg");
	ewl_widget_show(cell);

	image = add_image(cell, NULL, 0, NULL, NULL);
	ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_SHRINK);
	ewl_callback_append(image, EWL_CALLBACK_SHOW, show_first_image, NULL);
	ewl_widget_show(image);

	return;
}
