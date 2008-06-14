#include "ephoto.h"

static void change_size(Ewl_Widget *w, void *event, void *data);
static void thumb_clicked(Ewl_Widget *w, void *event, void *data);

static void 
change_size(Ewl_Widget *w, void *event, void *data)
{
	ewl_mvc_dirty_set(EWL_MVC(em->fbox), TRUE);
	ewl_widget_configure(em->scroll);

	return;
}

static void
thumb_clicked(Ewl_Widget *w, void *event, void *data)
{
        const char *file;
	Ewl_Event_Mouse_Down *ev;

	ev = event;

	if (ev->clicks == 2)
	{
        	file = ewl_widget_name_get(w);
		show_single_view(NULL, NULL, NULL);
	}
}

static Ewl_Widget *
fbox_widget_fetch(void *data, unsigned int row, unsigned int column)
{
	char *imagef;
	Ewl_Widget *image;

	imagef = data;

	image = add_image(NULL, imagef, 1, thumb_clicked, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 
				ewl_range_value_get(EWL_RANGE(em->fthumb_size)));
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
	ewl_widget_name_set(image, imagef);

	return image;
}

Ewl_Widget *
add_normal_view(Ewl_Widget *c)
{
	Ewl_Widget *avbox, *box, *button, *hbox, *image, *vbox;
	Ewl_View *view;

        em->fbox_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox_vbox), 
					EWL_FLAG_FILL_ALL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->fbox_vbox, 
					"Normal");

	hbox = add_box(em->fbox_vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);

	avbox = add_box(hbox, EWL_ORIENTATION_VERTICAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(avbox), EWL_FLAG_FILL_VFILL);	
	ewl_object_minimum_w_set(EWL_OBJECT(avbox), 220);

	box = add_box(avbox, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);

	button = add_button(box, "Import", PACKAGE_DATA_DIR "/images/add.png", 
					ephoto_import_dialog, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

        button = add_button(box, "Export", 
			PACKAGE_DATA_DIR "/images/emblem-photos.png", NULL, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
	
	vbox = add_box(hbox, EWL_ORIENTATION_VERTICAL, 2);

        em->scroll = ewl_scrollpane_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), em->scroll);
	ewl_widget_show(em->scroll);

	view = ewl_view_new();
	ewl_view_widget_fetch_set(view, fbox_widget_fetch);

        em->fbox = ewl_hfreebox_mvc_new();
	ewl_mvc_model_set(EWL_MVC(em->fbox), ewl_model_ecore_list_instance());
	ewl_mvc_view_set(EWL_MVC(em->fbox), view);
	ewl_mvc_selection_mode_set(EWL_MVC(em->fbox), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(em->fbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(em->scroll), em->fbox);
        ewl_widget_show(em->fbox);

	show_albums(avbox);

	box = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 10);
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);

	image = add_image(box, PACKAGE_DATA_DIR "/images/image.png", 0, NULL, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 20);

	em->fthumb_size = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(em->fthumb_size), 16);
	ewl_range_maximum_value_set(EWL_RANGE(em->fthumb_size), 144);
	ewl_range_step_set(EWL_RANGE(em->fthumb_size), 16);
	ewl_range_value_set(EWL_RANGE(em->fthumb_size), 80);
	ewl_container_child_append(EWL_CONTAINER(box), em->fthumb_size);
	ewl_callback_append(em->fthumb_size, EWL_CALLBACK_VALUE_CHANGED, 
					change_size, NULL);
	ewl_object_maximum_size_set(EWL_OBJECT(em->fthumb_size), 160, 40);
	ewl_widget_show(em->fthumb_size);

	image = add_image(box, PACKAGE_DATA_DIR "/images/image.png", 0, NULL, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 32);

	return em->fbox_vbox;
}

void 
show_normal_view(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), 
					em->fbox_vbox);
}
