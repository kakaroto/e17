#include "ephoto.h"

static void change_size(Ewl_Widget *w, void *event, void *data);

static void 
change_size(Ewl_Widget *w, void *event, void *data)
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

Ewl_Widget *
add_normal_view(Ewl_Widget *c)
{
	Ewl_Widget *box, *button, *hbox, *sp, *vbox;

        em->fbox_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox_vbox), 
					EWL_FLAG_FILL_ALL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->fbox_vbox, 
					"Normal");

	hbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->fbox_vbox), hbox);
	ewl_widget_show(hbox);

	vbox = add_box(hbox, EWL_ORIENTATION_VERTICAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_VFILL);	

	show_albums(vbox);

	box = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);

	button = add_button(box, "Import", PACKAGE_DATA_DIR "/images/add.png", 
					ephoto_import_dialog, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);

        button = add_button(box, "Export", PACKAGE_DATA_DIR "/images/emblem-photos.png", NULL, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	
	vbox = add_box(hbox, EWL_ORIENTATION_VERTICAL, 2);

        sp = ewl_scrollpane_new();
        ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(vbox), sp);
	ewl_widget_show(sp);

        em->fbox = ewl_hfreebox_new();
        ewl_freebox_layout_type_set(EWL_FREEBOX(em->fbox), 
					EWL_FREEBOX_LAYOUT_AUTO);
	ewl_object_fill_policy_set(EWL_OBJECT(em->fbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(sp), em->fbox);
        ewl_widget_show(em->fbox);

	em->fthumb_size = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(em->fthumb_size), 16);
	ewl_range_maximum_value_set(EWL_RANGE(em->fthumb_size), 144);
	ewl_range_step_set(EWL_RANGE(em->fthumb_size), 16);
	ewl_range_value_set(EWL_RANGE(em->fthumb_size), 80);
	ewl_container_child_append(EWL_CONTAINER(vbox), em->fthumb_size);
	ewl_callback_append(em->fthumb_size, EWL_CALLBACK_VALUE_CHANGED, 
					change_size, NULL);
	ewl_object_maximum_size_set(EWL_OBJECT(em->fthumb_size), 160, 40);
	ewl_widget_show(em->fthumb_size);

	return em->fbox_vbox;
}

void 
show_normal_view(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), 
					em->fbox_vbox);
}
