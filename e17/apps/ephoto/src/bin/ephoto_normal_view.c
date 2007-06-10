#include "ephoto.h"

static void change_size(Ewl_Widget *w, void *event, void *data);
static void iterate(char *point2);

/*Change the thumb size*/
static void change_size(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *child;

	ewl_container_child_iterate_begin(EWL_CONTAINER(em->fbox));

	while ((child = ewl_container_child_next(EWL_CONTAINER(em->fbox))))
	{
		ewl_image_constrain_set(EWL_IMAGE(child), 
				ewl_range_value_get(EWL_RANGE(em->fthumb_size)));
		ewl_widget_reparent(child);
	}
	ewl_widget_configure(em->fbox_vbox);

	return;
}

/*Add the normal view*/
Ewl_Widget *add_normal_view(Ewl_Widget *c)
{
	Ewl_Widget *sp;

        em->fbox_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox_vbox), EWL_FLAG_FILL_ALL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->fbox_vbox, "Normal");

        sp = ewl_scrollpane_new();
        ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(em->fbox_vbox), sp);
        ewl_widget_show(sp);

        em->fbox = ewl_hfreebox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(sp), em->fbox);
        ewl_widget_show(em->fbox);

	em->fthumb_size = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(em->fthumb_size), 8);
	ewl_range_maximum_value_set(EWL_RANGE(em->fthumb_size), 128);
	ewl_range_step_set(EWL_RANGE(em->fthumb_size), 16);
	ewl_range_value_set(EWL_RANGE(em->fthumb_size), 64);
	ewl_container_child_append(EWL_CONTAINER(em->fbox_vbox), em->fthumb_size);
	ewl_callback_append(em->fthumb_size, EWL_CALLBACK_VALUE_CHANGED, change_size, NULL);
	ewl_object_alignment_set(EWL_OBJECT(em->fthumb_size), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_maximum_size_set(EWL_OBJECT(em->fthumb_size), 100, 25);
	ewl_widget_show(em->fthumb_size);

	return em->fbox_vbox;
}

/*Show the normal view*/
void show_normal_view(Ewl_Widget *w, void *event, void *data)
{
	show_main_view(NULL, NULL, NULL);
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), em->fbox_vbox);
}

/*Find our spot in the list*/
static void iterate(char *point2)
{
	char *point1;
	
	ecore_dlist_goto_first(em->images);
	while(ecore_dlist_current(em->images))
	{
		point1 = ecore_dlist_current(em->images);
		if (!strcmp(point1, point2)) return;
		ecore_dlist_next(em->images);
	}
}

/*Action when an image is clicked*/
void freebox_image_clicked(Ewl_Widget *w, void *event, void *data)
{
	const char *path;
	

	path = ewl_widget_name_get(w);
	iterate(strdup(path));
	show_single_view(NULL, NULL, NULL);

	return;
}

