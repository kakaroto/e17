#include "ephoto.h"
#include "ephoto_imaging.h"

static void return_to_normal(Ewl_Widget *w, void *event, void *data);
static void zoom_in(Ewl_Widget *w, void *event, void *data);
static void zoom_out(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);
static void flip_horiz(Ewl_Widget *w, void *event, void *data);
static void flip_vert(Ewl_Widget *w, void *event, void *data);

void add_single_view(Ewl_Widget *c) {
	Ewl_Widget *sbox, *box, *hbox, *ibox, *image, *button;

	sbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(sbox), 2);
	ewl_object_fill_policy_set(EWL_OBJECT(sbox), 
				EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), sbox);
	ewl_widget_show(sbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), 
				sbox, "Normal");
	ephoto_set_single_vbox(sbox);

	box = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sbox),
				EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(sbox), box);
	ewl_widget_show(box);

	ibox = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(ibox), 
				EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), ibox);
	ewl_widget_show(ibox);

	image = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
	ewl_object_alignment_set(EWL_OBJECT(image), 
				EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(image),
				EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(ibox), image);
	ewl_widget_show(image);
	ephoto_set_single_image(image);

	hbox = ewl_hbox_new();
	ewl_box_homogeneous_set(EWL_BOX(hbox), TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(sbox), hbox);
	ewl_widget_show(hbox);

	button = ewl_button_new();
	ewl_button_image_set(EWL_BUTTON(button), 
				PACKAGE_DATA_DIR "/images/media-seek-backward.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, return_to_normal,
				NULL);
	ewl_widget_show(button);

	button = ewl_button_new();
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/undo.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(hbox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, rotate_image_left,
                                NULL);
        ewl_widget_show(button);

	button = ewl_button_new();
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/redo.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(hbox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, rotate_image_right,
                                NULL);
	ewl_widget_show(button);

        button = ewl_button_new();
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/go-next.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(hbox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, flip_horiz,
                                NULL);
        ewl_widget_show(button);

        button = ewl_button_new();
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/go-down.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(hbox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, flip_vert,
                                NULL);
        ewl_widget_show(button);
}

void show_single_view(Ewl_Widget *w, void *event, void *data) {
	const char *path;

	path = (const char *)data;	
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(ephoto_get_view_box()),
				ephoto_get_single_vbox());
	ewl_image_file_path_set(EWL_IMAGE(ephoto_get_single_image()), path); 
}

static void return_to_normal(Ewl_Widget *w, void *event, void *data) {
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(ephoto_get_view_box()),
				ephoto_get_normal_vbox());
}

static void zoom_in(Ewl_Widget *w, void *event, void *data) {

}

static void zoom_out(Ewl_Widget *w, void *event, void *data) {

}

static void rotate_image_left(Ewl_Widget *w, void *event, void *data) {
	unsigned int *image_data;
        int nw, nh;
        Ewl_Image *image;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nh, &nw);
        image_data = rotate_left(simage);
        image = EWL_IMAGE(simage);
        ewl_image_size_set(EWL_IMAGE(image), nw, nh);
        image->ow = nw;
        image->oh = nh;
        update_image(simage, nw, nh, image_data);
        ewl_object_preferred_inner_size_set(EWL_OBJECT(simage), nw, nh);
        ewl_widget_configure(simage->parent);

        return;
}

static void rotate_image_right(Ewl_Widget *w, void *event, void *data) {
	unsigned int *image_data;
        int nw, nh;
        Ewl_Image *image;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nh, &nw);
        image_data = rotate_right(simage);
        update_image(simage, nw, nh, image_data);
        image = EWL_IMAGE(simage);
        ewl_image_size_set(EWL_IMAGE(image), nw, nh);
        image->ow = nw;
        image->oh = nh;
        ewl_object_preferred_inner_size_set(EWL_OBJECT(simage), nw, nh);
        ewl_widget_configure(simage->parent);

        return;
}

static void flip_horiz(Ewl_Widget *w, void *event, void *data) {
	unsigned int *image_data;
	int nw, nh;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();
	
        image_data = flip_horizontal(simage);
        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nw, &nh);
        update_image(simage, nw, nh, image_data);
        ewl_widget_configure(simage->parent);

        return;
}

static void flip_vert(Ewl_Widget *w, void *event, void *data) {
	unsigned int *image_data;
        int nw, nh;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

        image_data = flip_vertical(simage);
        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nw, &nh);
        update_image(simage, nw, nh, image_data);
        ewl_widget_configure(simage->parent);

        return;
}

