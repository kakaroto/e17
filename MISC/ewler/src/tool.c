#include <Ewl.h>

#include "config.h"
#include "ewler.h"

static Ewl_Widget *tools_win;

EWL_CALLBACK_DEFN(tools_toggle)
{
	if( VISIBLE(tools_win) )
		ewl_widget_hide(tools_win);
	else
		ewl_widget_show(tools_win);
}

EWL_CALLBACK_DEFN(tools_click)
{
	Ewl_Widget *new_w;
	char *name = user_data;

	if( !form_active() )
		return;

	new_w = widget_new(name);
	if( !new_w )
		return;

	form_add(new_w);
}

int
tools_init( void )
{
	Ewl_Widget *vbox, *button, *image;

	if( tools_win )
		return -1;

	tools_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(tools_win), "Tools");
	ewl_object_size_request(EWL_OBJECT(tools_win), 200, 320);
	ewl_callback_append(tools_win, EWL_CALLBACK_DELETE_WINDOW,
											tools_toggle, NULL);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(tools_win), vbox);

	button = ewl_button_new(NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_object_insets_set(EWL_OBJECT(button), 0, 0, 0, 0);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, tools_click, "Ewl_Text");
	ewl_container_child_append(EWL_CONTAINER(vbox), button);

	image = ewl_image_new(EWLER_IMAGE_DIR"/text-small.png", "text");
	ewl_container_child_append(EWL_CONTAINER(button), image);
	ewl_widget_show(image);

	ewl_widget_show(button);

	button = ewl_button_new(NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_object_insets_set(EWL_OBJECT(button), 0, 0, 0, 0);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, tools_click, "Ewl_Entry");
	ewl_container_child_append(EWL_CONTAINER(vbox), button);

	image = ewl_image_new(EWLER_IMAGE_DIR"/entry-small.png", "entry");
	ewl_container_child_append(EWL_CONTAINER(button), image);
	ewl_widget_show(image);

	ewl_widget_show(button);

	button = ewl_button_new(NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_object_insets_set(EWL_OBJECT(button), 0, 0, 0, 0);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, tools_click, "Ewl_Border");
	ewl_container_child_append(EWL_CONTAINER(vbox), button);

	image = ewl_image_new(EWLER_IMAGE_DIR"/border-small.png", "border");
	ewl_container_child_append(EWL_CONTAINER(button), image);
	ewl_widget_show(image);

	ewl_widget_show(button);

	ewl_widget_show(vbox);
	
	ewl_widget_show(tools_win);

	return 0;
}
