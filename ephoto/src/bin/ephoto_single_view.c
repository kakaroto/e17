#include "ephoto.h"
#include "ephoto_imaging.h"

static void show_effects(Ewl_Widget *w, void *event, void *data);
static void destroy_window(Ewl_Widget *w, void *event, void *data);
static void return_to_normal(Ewl_Widget *w, void *event, void *data);
static void undo_changes(Ewl_Widget *w, void *event, void *data);
static void zoom_in(Ewl_Widget *w, void *event, void *data);
static void zoom_out(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);
static void flip_horiz(Ewl_Widget *w, void *event, void *data);
static void flip_vert(Ewl_Widget *w, void *event, void *data);
static void image_grayscale(Ewl_Widget *w, void *event, void *data);
static void image_sepia(Ewl_Widget *w, void *event, void *data);
unsigned int *image_data_old = NULL;
unsigned int *image_data = NULL;

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
	ewl_button_label_set(EWL_BUTTON(button), "Back");
	ewl_button_image_set(EWL_BUTTON(button), 
				PACKAGE_DATA_DIR "/images/media-seek-backward.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, return_to_normal,
				NULL);
	ewl_widget_show(button);

        button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Zoom In");
	ewl_button_image_set(EWL_BUTTON(button),
			PACKAGE_DATA_DIR "/images/add.png",
			NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, zoom_in,
			NULL);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Zoom Out");
	ewl_button_image_set(EWL_BUTTON(button),
			PACKAGE_DATA_DIR "/images/remove.png",
			NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, zoom_out,
			NULL);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Effects");
	ewl_button_image_set(EWL_BUTTON(button),
			PACKAGE_DATA_DIR "/images/image.png",
			NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, show_effects,
			NULL);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Undo Changes");
	ewl_button_image_set(EWL_BUTTON(button),
			PACKAGE_DATA_DIR "/images/dialog-close.png",
			NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, undo_changes,
			NULL);
	ewl_widget_show(button);
}

static void destroy_window(Ewl_Widget *w, void *event, void *data) {
	ewl_widget_destroy(ephoto_get_effects_window());
	ephoto_set_effects_window(NULL);
}

static void show_effects(Ewl_Widget *w, void *event, void *data) {
	Ewl_Widget *window, *freebox, *button;

	if (ephoto_get_effects_window())
		return;

	window = ewl_window_new();
	ewl_window_dialog_set(EWL_WINDOW(window), TRUE);
	ewl_window_title_set(EWL_WINDOW(window), "Ephoto Effects");
	ewl_object_size_request(EWL_OBJECT(window), 370, 100);
	ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW,
				destroy_window, NULL);
	ewl_widget_show(window);
	ephoto_set_effects_window(window);

	freebox = ewl_hfreebox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(freebox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(window), freebox);
	ewl_widget_show(freebox);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Rotate Left");
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/undo.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_object_minimum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_object_maximum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_container_child_append(EWL_CONTAINER(freebox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, rotate_image_left,
                                NULL);
        ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Rotate Right");
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/redo.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_object_minimum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_object_maximum_size_set(EWL_OBJECT(button), 85, 20);
        ewl_container_child_append(EWL_CONTAINER(freebox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, rotate_image_right,
                                NULL);
	ewl_widget_show(button);

        button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Flip Horizontal");
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/go-next.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_object_minimum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_object_maximum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_container_child_append(EWL_CONTAINER(freebox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, flip_horiz,
                                NULL);
        ewl_widget_show(button);

        button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Flip Vertical");
        ewl_button_image_set(EWL_BUTTON(button),
				PACKAGE_DATA_DIR "/images/go-down.png",
				NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_object_minimum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_object_maximum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_container_child_append(EWL_CONTAINER(freebox), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, flip_vert,
                                NULL);
        ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Grayscale");
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_object_minimum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_object_maximum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_container_child_append(EWL_CONTAINER(freebox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, image_grayscale,
			NULL);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Sepia");
	ewl_button_image_size_set(EWL_BUTTON(button), 18, 18);
	ewl_object_minimum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_object_maximum_size_set(EWL_OBJECT(button), 85, 20);
	ewl_container_child_append(EWL_CONTAINER(freebox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, image_sepia,
			NULL);
	ewl_widget_show(button);
}

void show_single_view(Ewl_Widget *w, void *event, void *data) {
	const char *path;
	Ewl_Image *image;

	image = EWL_IMAGE(ephoto_get_single_image());

	path = (const char *)data;	
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(ephoto_get_view_box()),
				ephoto_get_single_vbox());
	ewl_image_file_set(EWL_IMAGE(ephoto_get_single_image()), path, NULL);	
}

static void return_to_normal(Ewl_Widget *w, void *event, void *data) {
	destroy_window(NULL, NULL, NULL);
	ewl_image_file_set(EWL_IMAGE(ephoto_get_single_image()),
					NULL, NULL);
	if (image_data)
		free(image_data);
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(ephoto_get_view_box()),
				ephoto_get_normal_vbox());
}

static void undo_changes(Ewl_Widget *w, void *event, void *data) {

}

static void zoom_in(Ewl_Widget *w, void *event, void *data) {
	Ewl_Widget *simage;
	int wid, h;	

	simage = ephoto_get_single_image();
	wid = ewl_object_current_w_get(EWL_OBJECT(simage));
	h = ewl_object_current_h_get(EWL_OBJECT(simage));
	ewl_image_size_set(EWL_IMAGE(simage), wid*1.5, h*1.5);
}

static void zoom_out(Ewl_Widget *w, void *event, void *data) {
	Ewl_Widget *simage;
	int wid, h;

	simage = ephoto_get_single_image();
	wid = ewl_object_current_w_get(EWL_OBJECT(simage));
	h = ewl_object_current_h_get(EWL_OBJECT(simage));
	ewl_image_size_set(EWL_IMAGE(simage), wid/1.5, h/1.5);
}

static void rotate_image_left(Ewl_Widget *w, void *event, void *data) {
	int nw, nh;
        Ewl_Image *image;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

	image_data_old = image_data;

        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nh, &nw);
        image_data = rotate_left(simage);
        image = EWL_IMAGE(simage);
        ewl_image_size_set(EWL_IMAGE(image), nw, nh);
        image->ow = nw;
        image->oh = nh;
        update_image(simage, nw, nh, image_data);
        ewl_object_preferred_inner_size_set(EWL_OBJECT(simage), nw, nh);
        ewl_widget_configure(simage->parent);

	if (image_data_old)
		free(image_data_old);

        return;
}

static void rotate_image_right(Ewl_Widget *w, void *event, void *data) {
        int nw, nh;
        Ewl_Image *image;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

	image_data_old = image_data;

        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nh, &nw);
        image_data = rotate_right(simage);
        update_image(simage, nw, nh, image_data);
        image = EWL_IMAGE(simage);
        ewl_image_size_set(EWL_IMAGE(image), nw, nh);
        image->ow = nw;
        image->oh = nh;
        ewl_object_preferred_inner_size_set(EWL_OBJECT(simage), nw, nh);
        ewl_widget_configure(simage->parent);

	if (image_data_old)
		free(image_data_old);

        return;
}

static void flip_horiz(Ewl_Widget *w, void *event, void *data) {
	int nw, nh;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();
	
	image_data_old = image_data;

        image_data = flip_horizontal(simage);
        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nw, &nh);
        update_image(simage, nw, nh, image_data);
        ewl_widget_configure(simage->parent);

	if (image_data_old)
		free(image_data_old);

        return;
}

static void flip_vert(Ewl_Widget *w, void *event, void *data) {
        int nw, nh;
	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

	image_data_old = image_data;

        image_data = flip_vertical(simage);
        evas_object_image_size_get(EWL_IMAGE(simage)->image, &nw, &nh);
        update_image(simage, nw, nh, image_data);
        ewl_widget_configure(simage->parent);

	if (image_data_old)
		free(image_data_old);

        return;
}

static void image_grayscale(Ewl_Widget *w, void *event, void *data) {
	int nw, nh;

	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

	image_data_old = image_data;

	image_data = grayscale_image(simage);
	evas_object_image_size_get(EWL_IMAGE(simage)->image, &nw, &nh);
	update_image(simage, nw, nh, image_data);
	ewl_widget_configure(simage->parent);

	if (image_data_old)
		free(image_data_old);

	return;
}

static void image_sepia(Ewl_Widget *w, void *event, void *data) {
	int nw, nh;

	Ewl_Widget *simage;

	simage = ephoto_get_single_image();

	image_data_old = image_data;

	image_data = sepia_image(simage);
	evas_object_image_size_get(EWL_IMAGE(simage)->image, &nw, &nh);
	update_image(simage, nw, nh, image_data);
	ewl_widget_configure(simage->parent);

	if (image_data_old)
		free(image_data_old);

	return;
}

