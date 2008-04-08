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
static void set_requested_image_file(Ewl_Widget *w, void *event, void *data);

static void 
set_requested_image_file(Ewl_Widget *w, void *event, void *data)
{
	ewl_image_file_path_set(EWL_IMAGE(w), ec->requested_image);
}

Ewl_Widget *
add_single_view(Ewl_Widget *c)
{
	Ewl_Widget *vbox, *ibox, *hbox, *image, *bhbox;
	Ewl_Widget *standard, *advanced;

	em->ewin = NULL;

	em->single_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(em->single_vbox), 
					EWL_FLAG_FILL_ALL);

	hbox = add_box(em->single_vbox, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);

	standard = add_box(hbox, EWL_ORIENTATION_VERTICAL, 2);
	ewl_object_maximum_w_set(EWL_OBJECT(standard), 30);
	ewl_object_minimum_w_set(EWL_OBJECT(standard), 30);
	ewl_object_fill_policy_set(EWL_OBJECT(standard), EWL_FLAG_FILL_VFILL);

	add_standard_edit_tools(standard);

	advanced = add_image(standard, 
				PACKAGE_DATA_DIR "/images/camera-photo.png", 
					0, show_advanced, NULL);
        ewl_image_size_set(EWL_IMAGE(advanced), 25, 25);
	ewl_object_alignment_set(EWL_OBJECT(advanced), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(advanced), EWL_FLAG_FILL_SHRINK);

	vbox = add_box(hbox, EWL_ORIENTATION_VERTICAL, 0);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	
	ibox = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(ibox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), ibox);
	ewl_widget_show(ibox);

        em->simage = add_image(ibox, NULL, 0, NULL, NULL);
        ewl_object_alignment_set(EWL_OBJECT(em->simage), 
					EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(em->simage), 
					EWL_FLAG_FILL_SHRINK);
	if (ec->requested_image)
		ewl_callback_append(em->simage, EWL_CALLBACK_SHOW, 
					set_requested_image_file, NULL);
	
	bhbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_alignment_set(EWL_OBJECT(bhbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(bhbox), EWL_FLAG_FILL_SHRINK);

        image = add_image(bhbox, 
			PACKAGE_DATA_DIR "/images/media-seek-backward.png", 0, 
				previous_image, NULL);
        ewl_image_size_set(EWL_IMAGE(image), 32, 32);
        ewl_attach_tooltip_text_set(image, _("Previous Image"));

        image = add_image(bhbox, 
			PACKAGE_DATA_DIR "/images/media-seek-forward.png", 0, 
				next_image, NULL);
        ewl_image_size_set(EWL_IMAGE(image), 32, 32);
        ewl_attach_tooltip_text_set(image, _("Next Image"));

	hbox = add_box(em->single_vbox, EWL_ORIENTATION_HORIZONTAL, 5);
        ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);

        image = add_image(hbox, PACKAGE_DATA_DIR "/images/normal_view.png", 0,
                                show_normal_view, NULL);
        ewl_image_size_set(EWL_IMAGE(image), 25, 25);
        ewl_attach_tooltip_text_set(image, _("Image Thumbnail View"));
        ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_SHRINK);

	return em->single_vbox;
}

void 
show_single_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), 
						em->single_vbox);
	if (ecore_dlist_current(em->images))
	{
		ewl_image_file_path_set(EWL_IMAGE(em->simage), 
					ecore_dlist_current(em->images));
	}
	return;
}

static void 
add_standard_edit_tools(Ewl_Widget *c)
{
	Ewl_Widget *image;

	image = add_image(c, PACKAGE_DATA_DIR "/images/search.png", 0, zoom_in, 
					NULL);
        ewl_image_size_set(EWL_IMAGE(image), 25, 25);
        ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);

	image = add_image(c, PACKAGE_DATA_DIR "/images/search.png", 0, zoom_out,
					NULL);
        ewl_image_size_set(EWL_IMAGE(image), 25, 25);
        ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);

	image = add_image(c, PACKAGE_DATA_DIR "/images/undo.png", 0, 
					rotate_image_left, NULL);
	ewl_image_size_set(EWL_IMAGE(image), 25, 25);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);

        image = add_image(c, PACKAGE_DATA_DIR "/images/redo.png", 0, 
					rotate_image_right, NULL);
        ewl_image_size_set(EWL_IMAGE(image), 25, 25);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);

        image = add_image(c, PACKAGE_DATA_DIR "/images/go-next.png", 0, 
					flip_image_horizontal, NULL);
        ewl_image_size_set(EWL_IMAGE(image), 25, 25);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);

        image = add_image(c, PACKAGE_DATA_DIR "/images/go-down.png", 0, 
					flip_image_vertical, NULL);
	ewl_image_size_set(EWL_IMAGE(image), 25, 25);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);

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
	const char *file;
	Epsilon *ep;
	Ewl_Widget *grid, *image;

	ep = epsilon_new(ecore_dlist_current(em->images));
        if (!epsilon_exists(ep))
                epsilon_generate(ep);
        file = epsilon_thumb_file_get(ep);
	epsilon_free(ep);

	em->ewin = add_window("Ephoto Effects!", 375, 100, destroy, NULL);

	grid = ewl_grid_new();
	ewl_grid_dimensions_set(EWL_GRID(grid), 1, 4);
	ewl_object_alignment_set(EWL_OBJECT(grid), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(grid), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->ewin), grid);
	ewl_widget_show(grid);

	image = add_icon(grid, "Grayscale", file, 0, image_grayscale, 
					em->simage);
	ewl_icon_constrain_set(EWL_ICON(image), 81);
	ewl_callback_append(EWL_ICON(image)->image, EWL_CALLBACK_CONFIGURE, 
					image_grayscale, EWL_ICON(image)->image);
	ewl_grid_child_position_set(EWL_GRID(grid), image, 0, 0, 0, 0);
	
	image = add_icon(grid, "Sepia", file, 0, image_sepia, em->simage);
        ewl_icon_constrain_set(EWL_ICON(image), 81);
        ewl_callback_append(EWL_ICON(image)->image, EWL_CALLBACK_CONFIGURE, 
					image_sepia, EWL_ICON(image)->image);
	ewl_grid_child_position_set(EWL_GRID(grid), image, 1, 1, 0, 0);

	image = add_icon(grid, "Blur", file, 0, image_blur, em->simage);
        ewl_icon_constrain_set(EWL_ICON(image), 81);
        ewl_callback_append(EWL_ICON(image)->image, EWL_CALLBACK_CONFIGURE, 
					image_blur, EWL_ICON(image)->image);
	ewl_grid_child_position_set(EWL_GRID(grid), image, 2, 2, 0, 0);

	image = add_icon(grid, "Sharpen", file, 0, image_sharpen, em->simage);
        ewl_icon_constrain_set(EWL_ICON(image), 81);
        ewl_callback_append(EWL_ICON(image)->image, EWL_CALLBACK_CONFIGURE, 
					image_sharpen, EWL_ICON(image)->image);
	ewl_grid_child_position_set(EWL_GRID(grid), image, 3, 3, 0, 0);

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

