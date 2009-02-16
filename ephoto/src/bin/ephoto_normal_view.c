#include "ephoto.h"
#include "ephoto_fsystem.h"
#include "ephoto_utils.h"

static void change_size(Ewl_Widget *w, void *event, void *data);
static void thumb_clicked(Ewl_Widget *w, void *event, void *data);
static Ewl_Widget *fbox_widget_constructor(unsigned int column, void *pr_data);
static void fbox_widget_assign(Ewl_Widget *w, void *data, unsigned int row,
					unsigned int column, void *pr_data);

void add_normal_view(Ewl_Widget *c) {
	Ewl_Widget *nbox, *fbox, *box, *button, *abox, *sbox;
        Ewl_Widget *hpaned, *image, *footer, *scroll, *seeker;
	Ewl_View *view;

	nbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(nbox), 5);
	ewl_object_fill_policy_set(EWL_OBJECT(nbox), 
						EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), nbox);
	ewl_widget_show(nbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), 
						nbox, "Normal");
	ephoto_set_normal_vbox(nbox);

	hpaned = ewl_hpaned_new();
	ewl_object_fill_policy_set(EWL_OBJECT(hpaned), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(nbox), hpaned);
	ewl_widget_show(hpaned);

	scroll = ewl_scrollpane_new();	
	ewl_container_child_append(EWL_CONTAINER(hpaned), scroll);
	ewl_widget_show(scroll);

	view = ewl_view_new();
	ewl_view_widget_constructor_set(view, fbox_widget_constructor);
	ewl_view_widget_assign_set(view, fbox_widget_assign);

	fbox = ewl_hfreebox_mvc_new();
	ewl_mvc_model_set(EWL_MVC(fbox), ewl_model_ecore_list_instance());
	ewl_mvc_view_set(EWL_MVC(fbox), view);
	ewl_mvc_selection_mode_set(EWL_MVC(fbox), 
					EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(fbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(scroll), fbox);
	ewl_widget_show(fbox);
	ephoto_set_fbox(fbox);

	footer = ewl_hbox_new();
	ewl_object_minimum_h_set(EWL_OBJECT(footer), 30);
	ewl_object_fill_policy_set(EWL_OBJECT(footer), EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(nbox), footer);
	ewl_widget_show(footer);

	box = ewl_hbox_new();
	ewl_box_homogeneous_set(EWL_BOX(box), TRUE);
        ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(footer), box);
        ewl_widget_show(box);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Browse Tags");
	ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/camera-photo.png", NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 16, 16);
	ewl_container_child_append(EWL_CONTAINER(box), button);
	ewl_widget_show(button);	

	button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Import");
        ewl_button_image_set(EWL_BUTTON(button),
                                PACKAGE_DATA_DIR "/images/add.png", NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 16, 16);
        ewl_container_child_append(EWL_CONTAINER(box), button);
        ewl_widget_show(button);

	button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Export");
        ewl_button_image_set(EWL_BUTTON(button),
                                PACKAGE_DATA_DIR "/images/emblem-photos.png", NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 16, 16);
        ewl_container_child_append(EWL_CONTAINER(box), button);
        ewl_widget_show(button);

	abox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(abox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(footer), abox);
	ewl_widget_show(abox);

	sbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sbox), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(footer), sbox);
	ewl_widget_show(sbox);
	
	image = ewl_image_new();
	ewl_image_file_path_set(EWL_IMAGE(image), 
				PACKAGE_DATA_DIR "/images/image.png");
	ewl_image_constrain_set(EWL_IMAGE(image), 20);
	ewl_container_child_append(EWL_CONTAINER(sbox), image);
	ewl_widget_show(image);

	seeker = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(seeker), 16);
	ewl_range_maximum_value_set(EWL_RANGE(seeker), 144);
	ewl_range_step_set(EWL_RANGE(seeker), 16);
	ewl_range_value_set(EWL_RANGE(seeker), 80);
	ephoto_set_thumb_size(80);
	ewl_object_maximum_size_set(EWL_OBJECT(seeker), 160, 40);
	ewl_container_child_append(EWL_CONTAINER(sbox), seeker);
	ewl_callback_append(seeker, EWL_CALLBACK_VALUE_CHANGED,
				change_size, NULL);
	ewl_widget_show(seeker);

	image = ewl_image_new();
	ewl_image_file_path_set(EWL_IMAGE(image),
				PACKAGE_DATA_DIR "/images/image.png");
	ewl_image_constrain_set(EWL_IMAGE(image), 32);
	ewl_container_child_append(EWL_CONTAINER(sbox), image);
	ewl_widget_show(image);

	add_fsystem(hpaned);
}

void show_normal_view(Ewl_Widget *w, void *event, void *data) {
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(ephoto_get_view_box()),
				ephoto_get_normal_vbox());
}

static Ewl_Widget *fbox_widget_constructor(unsigned int column, void *pr_data) {
 	Ewl_Widget *vbox;

	vbox = ewl_vbox_new();

	return vbox;
}

static void fbox_widget_assign(Ewl_Widget *w, void *data, unsigned int row,
					unsigned int column, void *pr_data) {
	Ewl_Widget *vbox, *image, *label;

	vbox = w;
	ewl_object_maximum_size_set(EWL_OBJECT(vbox), ephoto_get_thumb_size()+35,
						ephoto_get_thumb_size()+35);
        ewl_object_minimum_size_set(EWL_OBJECT(vbox), ephoto_get_thumb_size()+35,
                                                ephoto_get_thumb_size()+35);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_NONE);
	ewl_widget_show(vbox);

	image = ewl_image_thumbnail_new();
	ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
	ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(image), (char *)data);
	ewl_image_constrain_set(EWL_IMAGE(image), ephoto_get_thumb_size());
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
	ewl_callback_append(image, EWL_CALLBACK_CLICKED, thumb_clicked, NULL);
	ewl_container_child_append(EWL_CONTAINER(vbox), image);
	ewl_widget_name_set(image, (char *)data);
	ewl_widget_show(image);

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), basename((char *)data));
	ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(vbox), label);
	ewl_widget_show(label);
}

static void thumb_clicked(Ewl_Widget *w, void *event, void *data) {
	const char *file;
	Ewl_Event_Mouse_Down *ev;
	
	ev = (Ewl_Event_Mouse_Down *)event;
	if (ev->clicks == 2) {
		file = ewl_widget_name_get(w);
		show_single_view(NULL, NULL, (void *)file);
	}
}

static void change_size(Ewl_Widget *w, void *event, void *data) {
	ephoto_set_thumb_size(ewl_range_value_get(EWL_RANGE(w)));
	ewl_mvc_dirty_set(EWL_MVC(ephoto_get_fbox()), TRUE);
	ewl_widget_configure(ephoto_get_normal_vbox());
}
