#include "ephoto.h"

static void add_standard_edit_tools(Ewl_Widget *c);
static void show_advanced(void);
static void previous_image(Ewl_Widget *w, void *event, void *data);
static void next_image(Ewl_Widget *w, void *event, void *data);
static void zoom_in(Ewl_Widget *w, void *event, void *data);
static void zoom_out(Ewl_Widget *w, void *event, void *data);
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data);
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);
static void image_blur(Ewl_Widget *w, void *event, void *data);
static void image_sharpen(Ewl_Widget *w, void *event, void *data);
static void image_grayscale(Ewl_Widget *w, void *event, void *data);
static void image_sepia(Ewl_Widget *w, void *event, void *data);
static void close_channel(Ewl_Widget *w, void *event, void *data);
static void channel_mixer(Ewl_Widget *w, void *event, void *data);
static void set_image_file(Ewl_Widget *w, void *event, void *data);

static void
set_image_file(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Selection_Idx *i;

	i = ewl_mvc_selected_get(EWL_MVC(em->fbox));

	ewl_image_file_path_set(EWL_IMAGE(w), ecore_dlist_index_goto(em->images, i->row));
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, set_image_file);	
}

Ewl_Widget *
add_single_view(Ewl_Widget *c)
{
	Ewl_Widget *ibox, *bhbox;

	em->ewin = NULL;

	em->single_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(em->single_vbox), 
					EWL_FLAG_FILL_ALL);

	ibox = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(ibox), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(em->single_vbox), ibox);
	ewl_widget_show(ibox);

        em->simage = add_image(ibox, NULL, 0, NULL, NULL);
        ewl_object_alignment_set(EWL_OBJECT(em->simage), 
					EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(em->simage), 
					EWL_FLAG_FILL_SHRINK);
	
	bhbox = add_box(em->single_vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_alignment_set(EWL_OBJECT(bhbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(bhbox), EWL_FLAG_FILL_SHRINK);

	add_standard_edit_tools(bhbox);

	return em->single_vbox;
}

void 
show_single_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), 
						em->single_vbox);
	if (ecore_dlist_current(em->images))
	{
		ewl_callback_append(em->simage, EWL_CALLBACK_CONFIGURE,
						set_image_file, NULL);
	}
	return;
}

static void 
add_standard_edit_tools(Ewl_Widget *c)
{
	Ewl_Widget *button;

	button = add_button(c, NULL, 
				PACKAGE_DATA_DIR "/images/in.png", 
				zoom_in, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
	ewl_attach_tooltip_text_set(button, _("Zoom In"));
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

	button = add_button(c, NULL, 
				PACKAGE_DATA_DIR "/images/out.png", 
				zoom_out, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
	ewl_attach_tooltip_text_set(button, _("Zoom Out"));
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

	button = add_button(c, NULL, 
				PACKAGE_DATA_DIR "/images/undo.png", 
				rotate_image_left, NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
	ewl_attach_tooltip_text_set(button, _("Rotate Left"));
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

        button = add_button(c, NULL, 
				PACKAGE_DATA_DIR "/images/redo.png", 
				rotate_image_right, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
	ewl_attach_tooltip_text_set(button, _("Rotate Right"));
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

        button = add_button(c, NULL, 
				PACKAGE_DATA_DIR "/images/go-next.png", 
				flip_image_horizontal, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
	ewl_attach_tooltip_text_set(button, _("Flip Horizontally"));
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

        button = add_button(c, NULL, 
				PACKAGE_DATA_DIR "/images/go-down.png", 
				flip_image_vertical, NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
	ewl_attach_tooltip_text_set(button, _("Flip Vertically"));
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

	button = add_button(c, NULL,
				PACKAGE_DATA_DIR "/images/camera-photo.png",
				show_advanced, NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
	ewl_attach_tooltip_text_set(button, _("Advanced Tools"));
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

	button = add_button(c, NULL,
                                PACKAGE_DATA_DIR "/images/media-seek-backward.png",
                                previous_image, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
        ewl_attach_tooltip_text_set(button, _("Previous Image"));
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

        button = add_button(c, NULL,
                                PACKAGE_DATA_DIR "/images/media-seek-forward.png",
                                next_image, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 22, 22);
        ewl_attach_tooltip_text_set(button, _("Next Image"));
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);

	return;
}

static void 
destroy(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(w);
	em->ewin = NULL;
}

static void 
show_advanced(void)
{
	Ewl_Widget *fbox, *button;

	em->ewin = add_window("Ephoto Advanced Image Tools!", 
				240, 185, destroy, NULL);

	fbox = ewl_hfreebox_new();
	ewl_object_alignment_set(EWL_OBJECT(fbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(fbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->ewin), fbox);
	ewl_widget_show(fbox);

	button = add_button(fbox, "Grayscale", NULL, image_grayscale, em->simage);
		
	button = add_button(fbox, "Sepia", NULL, image_sepia, em->simage);

	button = add_button(fbox, "Blur", NULL, image_blur, em->simage);

	button = add_button(fbox, "Sharpen", NULL, image_sharpen, em->simage);

	return;
}

static void 
previous_image(Ewl_Widget *w, void *event, void *data)
{
        char *image;

        ecore_dlist_previous(em->images);
        image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_last_goto(em->images);
		image = ecore_dlist_current(em->images);
	}
	ewl_image_file_path_set(EWL_IMAGE(em->simage), image);
      	ewl_widget_configure(em->simage->parent);
	
        return;
}

static void 
next_image(Ewl_Widget *w, void *event, void *data)
{
	char *image;

	ecore_dlist_next(em->images);
	image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_first_goto(em->images);
		image = ecore_dlist_current(em->images);
	}
	ewl_image_file_path_set(EWL_IMAGE(em->simage), image);
	ewl_widget_configure(em->simage->parent);
	
	return;
}

static void 
zoom_in(Ewl_Widget *w, void *event, void *data)
{
	int ow, oh;
	
	ewl_object_current_size_get(EWL_OBJECT(em->simage), &ow, &oh);

	ewl_image_size_set(EWL_IMAGE(em->simage), ow*2, oh*2);
	ewl_widget_configure(em->simage->parent);

	return;
}

static void 
zoom_out(Ewl_Widget *w, void *event, void *data)
{
        int ow, oh;

        ewl_object_current_size_get(EWL_OBJECT(em->simage), &ow, &oh);

        ewl_image_size_set(EWL_IMAGE(em->simage), ow/2, oh/2);
        ewl_widget_configure(em->simage->parent);

        return;
}

static void 
flip_image_horizontal(Ewl_Widget *w, void *event, void *data)
{

	unsigned int *image_data;
	int nw, nh;

	image_data = flip_horizontal(em->simage);
        evas_object_image_size_get(EWL_IMAGE(em->simage)->image, &nw, &nh);
	update_image(em->simage, nw, nh, image_data);
	ewl_widget_configure(em->simage->parent);
	
	return;
}

static void 
flip_image_vertical(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	
	image_data = flip_vertical(em->simage);
        evas_object_image_size_get(EWL_IMAGE(em->simage)->image, &nw, &nh);
	update_image(em->simage, nw, nh, image_data);	
	ewl_widget_configure(em->simage->parent);

	return;
}

static void 
rotate_image_left(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;

        evas_object_image_size_get(EWL_IMAGE(em->simage)->image, &nh, &nw);
	image_data = rotate_left(em->simage);
	image = EWL_IMAGE(em->simage);
	ewl_image_size_set(EWL_IMAGE(image), nw, nh);
        image->ow = nw;
        image->oh = nh;
        update_image(em->simage, nw, nh, image_data);
        ewl_object_preferred_inner_size_set(EWL_OBJECT(em->simage), nw, nh);
	ewl_widget_configure(em->simage->parent);
	
	return;
}

static void 
rotate_image_right(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;
	
        evas_object_image_size_get(EWL_IMAGE(em->simage)->image, &nh, &nw);
        image_data = rotate_right(em->simage);
	update_image(em->simage, nw, nh, image_data);
	image = EWL_IMAGE(em->simage);
	ewl_image_size_set(EWL_IMAGE(image), nw, nh);
        image->ow = nw;
        image->oh = nh;
	ewl_object_preferred_inner_size_set(EWL_OBJECT(em->simage), nw, nh);
	ewl_widget_configure(em->simage->parent);

	return;
}

static void 
image_blur(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Widget *image;

	image = data;

       	evas_object_image_size_get(EWL_IMAGE(image)->image, &nw, &nh);
       	image_data = blur_image(image);
       	update_image(image, nw, nh, image_data);
       	ewl_widget_configure(image->parent);

	ewl_callback_del(image, EWL_CALLBACK_CONFIGURE, image_blur);

        return;
}

static void 
image_sharpen(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Widget *image;

	image = data;

        evas_object_image_size_get(EWL_IMAGE(image)->image, &nw, &nh);
        image_data = sharpen_image(image);
        update_image(image, nw, nh, image_data);
        ewl_widget_configure(image->parent);

	ewl_callback_del(image, EWL_CALLBACK_CONFIGURE, image_sharpen);

        return;
}

static void 
image_grayscale(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Widget *image;

	image = data;

	evas_object_image_size_get(EWL_IMAGE(image)->image, &nw, &nh);
	image_data = grayscale_image(image);
	update_image(image, nw, nh, image_data);
	ewl_widget_configure(image->parent);

	ewl_callback_del(image, EWL_CALLBACK_CONFIGURE, image_grayscale);

	return;
}

static void 
image_sepia(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Widget *image;

	image = data;

        evas_object_image_size_get(EWL_IMAGE(image)->image, &nw, &nh);
        image_data = sepia_image(image);
        update_image(image, nw, nh, image_data);
        ewl_widget_configure(image->parent);

	ewl_callback_del(image, EWL_CALLBACK_CONFIGURE, image_sepia);

        return;
}

static void 
close_channel(Ewl_Widget *w, void *event, void *data)
{
/*	Ewl_Widget *win;

	win = data;

	ewl_widget_destroy(win);*/
}

static void 
channel_mixer(Ewl_Widget *w, void *event, void *data)
{
	close_channel(NULL, NULL, NULL);
/*	Ewl_Widget *window, *vbox, *seek;

	window = add_window("Channel Editor", 400, 400, NULL, NULL);
	ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, close_channel, window);

	vbox = add_box(window, EWL_ORIENTATION_VERTICAL, 1);
	
	add_label(vbox, "Hue");

	seek = ewl_hseeker_new();
        ewl_range_minimum_value_set(EWL_RANGE(seek), -100);
        ewl_range_maximum_value_set(EWL_RANGE(seek), 100);
        ewl_range_step_set(EWL_RANGE(seek), 10);
        ewl_range_value_set(EWL_RANGE(seek), 0);
        ewl_container_child_append(EWL_CONTAINER(vbox), seek);
        ewl_widget_show(seek);

	add_label(vbox, "Saturation");

	seek = ewl_hseeker_new();
        ewl_range_minimum_value_set(EWL_RANGE(seek), -100);
        ewl_range_maximum_value_set(EWL_RANGE(seek), 100);
        ewl_range_step_set(EWL_RANGE(seek), 10);
        ewl_range_value_set(EWL_RANGE(seek), 0);
        ewl_container_child_append(EWL_CONTAINER(vbox), seek);
        ewl_widget_show(seek);

	add_label(vbox, "Value");

	seek = ewl_hseeker_new();
        ewl_range_minimum_value_set(EWL_RANGE(seek), -100);
        ewl_range_maximum_value_set(EWL_RANGE(seek), 100);
        ewl_range_step_set(EWL_RANGE(seek), 10);
        ewl_range_value_set(EWL_RANGE(seek), 0);
        ewl_container_child_append(EWL_CONTAINER(vbox), seek);
        ewl_widget_show(seek);

	add_label(vbox, "Light");

        seek = ewl_hseeker_new();
        ewl_range_minimum_value_set(EWL_RANGE(seek), -100);
        ewl_range_maximum_value_set(EWL_RANGE(seek), 100);
        ewl_range_step_set(EWL_RANGE(seek), 10);
        ewl_range_value_set(EWL_RANGE(seek), 0);
        ewl_container_child_append(EWL_CONTAINER(vbox), seek);
        ewl_widget_show(seek);

	add_label(vbox, "Brightness");

        seek = ewl_hseeker_new();
        ewl_range_minimum_value_set(EWL_RANGE(seek), -100);
        ewl_range_maximum_value_set(EWL_RANGE(seek), 100);
        ewl_range_step_set(EWL_RANGE(seek), 10);
        ewl_range_value_set(EWL_RANGE(seek), 0);
        ewl_container_child_append(EWL_CONTAINER(vbox), seek);
        ewl_widget_show(seek);

        add_label(vbox, "Contrast");

        seek = ewl_hseeker_new();
        ewl_range_minimum_value_set(EWL_RANGE(seek), -100);
        ewl_range_maximum_value_set(EWL_RANGE(seek), 100);
        ewl_range_step_set(EWL_RANGE(seek), 10);
        ewl_range_value_set(EWL_RANGE(seek), 0);
        ewl_container_child_append(EWL_CONTAINER(vbox), seek);
        ewl_widget_show(seek);*/
}

