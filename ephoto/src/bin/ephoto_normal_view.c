#include "ephoto.h"

static void change_size(Ewl_Widget *w, void *event, void *data);
static void thumb_clicked(Ewl_Widget *w, void *event, void *data);
static void update_info(Ewl_Widget *w, void *event, void *data);
static Ewl_Widget *fbox_widget_fetch(void *data, unsigned int row, unsigned int column);
static void populate(Ewl_Widget *w, void *event, void *data);

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

static void
update_info(Ewl_Widget *w, void *event, void *data)
{
	char info[PATH_MAX];

	if (ewl_container_child_count_get(EWL_CONTAINER(em->fbox)) == 0)
		return;
	
	snprintf(info, PATH_MAX, "%d images; %d selected;",
                                ewl_container_child_count_get(EWL_CONTAINER(em->fbox)),
                                ewl_mvc_selected_count_get(EWL_MVC(em->fbox)));
	ewl_label_text_set(EWL_LABEL(em->info), info);
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

static void
populate(Ewl_Widget *w, void *event, void *data)
{
	if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(w)) == em->atree)
		populate_albums(NULL, NULL, NULL);
	else
		populate_fsystem(NULL, NULL, NULL);
}

Ewl_Widget *
add_normal_view(Ewl_Widget *c)
{
	Ewl_Widget *nb, *box, *button, *hbox, *image, *footer;
	Ewl_View *view;

        em->fbox_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox_vbox), 
					EWL_FLAG_FILL_ALL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->fbox_vbox, 
					"Normal");
	hbox = add_box(em->fbox_vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);

	nb = ewl_notebook_new();
	ewl_notebook_tabbar_homogeneous_set(EWL_NOTEBOOK(nb), TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(nb), EWL_FLAG_FILL_VFILL);	
	ewl_object_minimum_w_set(EWL_OBJECT(nb), 220);
	ewl_container_child_append(EWL_CONTAINER(hbox), nb);
	ewl_callback_append(nb, EWL_CALLBACK_CLICKED, populate, NULL);
	ewl_widget_show(nb);

        em->scroll = ewl_scrollpane_new();
        ewl_container_child_append(EWL_CONTAINER(hbox), em->scroll);
	ewl_widget_show(em->scroll);

	view = ewl_view_new();
	ewl_view_widget_fetch_set(view, fbox_widget_fetch);

        em->fbox = ewl_hfreebox_mvc_new();
	ewl_mvc_model_set(EWL_MVC(em->fbox), ewl_model_ecore_list_instance());
	ewl_mvc_view_set(EWL_MVC(em->fbox), view);
	ewl_mvc_selection_mode_set(EWL_MVC(em->fbox), EWL_SELECTION_MODE_MULTI);
	ewl_object_fill_policy_set(EWL_OBJECT(em->fbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(em->scroll), em->fbox);
	ewl_callback_append(em->fbox, EWL_CALLBACK_VALUE_CHANGED, update_info, NULL);
        ewl_widget_show(em->fbox);

	show_albums(nb);
        show_fsystem(nb);

	footer = ewl_grid_new();
	ewl_grid_dimensions_set(EWL_GRID(footer), 3, 1);
	ewl_grid_row_fixed_h_set(EWL_GRID(footer), 0, 40);
	ewl_container_child_append(EWL_CONTAINER(em->fbox_vbox), footer);
	ewl_object_fill_policy_set(EWL_OBJECT(footer), EWL_FLAG_FILL_HFILL);
	ewl_widget_show(footer);

	box = add_box(footer, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_grid_child_position_set(EWL_GRID(footer), box, 0,1,0,0);
        ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(box), EWL_FLAG_ALIGN_LEFT);

        button = add_button(box, "Import", PACKAGE_DATA_DIR "/images/add.png",
                                        ephoto_import_dialog, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

        button = add_button(box, "Export",
                        PACKAGE_DATA_DIR "/images/emblem-photos.png", NULL, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

	em->info = add_label(footer, "Images");
	ewl_grid_child_position_set(EWL_GRID(footer), em->info, 1,2,0,0);
	ewl_object_alignment_set(EWL_OBJECT(em->info), EWL_FLAG_ALIGN_CENTER);

	box = add_box(footer, EWL_ORIENTATION_HORIZONTAL, 2);
        ewl_grid_child_position_set(EWL_GRID(footer), box, 2,3,0,0);
        ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);
        ewl_object_alignment_set(EWL_OBJECT(box), EWL_FLAG_ALIGN_RIGHT);

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

	populate_albums(NULL, NULL, NULL);

	return em->fbox_vbox;
}

void 
show_normal_view(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), 
					em->fbox_vbox);
}
