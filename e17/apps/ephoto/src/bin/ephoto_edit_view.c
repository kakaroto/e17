#include "ephoto.h"

/*Ephoto Image Manipulation*/
static void add_standard_edit_tools(Ewl_Widget *c);
static void add_advanced_edit_tools(Ewl_Widget *c);
static void previous_image(Ewl_Widget *w, void *event, void *data);
static void next_image(Ewl_Widget *w, void *event, void *data);
static void zoom_in(Ewl_Widget *w, void *event, void *data);
static void zoom_out(Ewl_Widget *w, void *event, void *data);
//static void zoom_full(Ewl_Widget *w, void *event, void *data);
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data);
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);
static void image_blur(Ewl_Widget *w, void *event, void *data);
static void image_sharpen(Ewl_Widget *w, void *event, void *data);

/*Add the edit view*/
Ewl_Widget *add_edit_view(Ewl_Widget *c)
{
	Ewl_Widget *button, *vbox, *ibox, *hbox, *bhbox, *nb;
	Ewl_Widget *standard, *advanced;

	hbox = add_box(c, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);

	nb = ewl_notebook_new();
	ewl_object_fill_policy_set(EWL_OBJECT(nb), EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_container_child_append(EWL_CONTAINER(hbox), nb);
	ewl_widget_show(nb);

	standard = add_box(nb, EWL_ORIENTATION_VERTICAL, 2);
	ewl_object_maximum_w_set(EWL_OBJECT(standard), 172);
	ewl_object_minimum_w_set(EWL_OBJECT(standard), 172);
	ewl_object_fill_policy_set(EWL_OBJECT(standard), EWL_FLAG_FILL_VFILL);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(nb), standard, _("Standard Tools"));	

	add_standard_edit_tools(standard);

	advanced = add_box(nb, EWL_ORIENTATION_VERTICAL, 2);
        ewl_object_maximum_w_set(EWL_OBJECT(advanced), 172);
        ewl_object_minimum_w_set(EWL_OBJECT(advanced), 172);
        ewl_object_fill_policy_set(EWL_OBJECT(advanced), EWL_FLAG_FILL_VFILL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(nb), advanced, _("Advanced Tools"));

	add_advanced_edit_tools(advanced);

	vbox = add_box(hbox, EWL_ORIENTATION_VERTICAL, 0);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	
	ibox = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(ibox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), ibox);
	ewl_widget_show(ibox);

        em->eimage = add_image(ibox, NULL, 0, NULL, NULL);
        ewl_object_alignment_set(EWL_OBJECT(em->eimage), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(em->eimage), EWL_FLAG_FILL_SHRINK);
	
	bhbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_object_alignment_set(EWL_OBJECT(bhbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(bhbox), EWL_FLAG_FILL_SHRINK);

        button = add_button(bhbox, NULL, PACKAGE_DATA_DIR "/images/media-seek-backward.png", previous_image, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);
        ewl_attach_tooltip_text_set(button, _("Previous Image"));

        button = add_button(bhbox, NULL, PACKAGE_DATA_DIR "/images/media-seek-forward.png", next_image, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);
        ewl_attach_tooltip_text_set(button, _("Next Image"));

	return hbox;
}

/*Show the edit view*/
void show_edit_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->main_nb), em->edit_vbox);
	ewl_image_file_path_set(EWL_IMAGE(em->eimage), ecore_dlist_current(em->images));
	ewl_widget_enable(em->smi);
	return;
}

/*Add edit tools to container c*/
static void add_standard_edit_tools(Ewl_Widget *c)
{
	Ewl_Widget *button;

	button = add_button(c, "Zoom In", PACKAGE_DATA_DIR "/images/search.png", zoom_in, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

	button = add_button(c, "Zoom Out", PACKAGE_DATA_DIR "/images/search.png", zoom_out, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

	//button = add_button(c, "Zoom 1:1", PACKAGE_DATA_DIR "/images/search.png", zoom_full, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

	button = add_button(c, "Rotate Left", PACKAGE_DATA_DIR "/images/undo.png", rotate_image_left, NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

        button = add_button(c, "Rotate Right", PACKAGE_DATA_DIR "/images/redo.png", rotate_image_right, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

        button = add_button(c, "Flip Horizontally", PACKAGE_DATA_DIR "/images/go-next.png", flip_image_horizontal, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

        button = add_button(c, "Flip Vertically", PACKAGE_DATA_DIR "/images/go-down.png", flip_image_vertical, NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

	return;
}

/* Add advanced edit tools */
static void add_advanced_edit_tools(Ewl_Widget *c)
{
	Ewl_Widget *button;

	button = add_button(c, "Blur Image", NULL, image_blur, NULL);
	ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

	button = add_button(c, "Sharpen Image", NULL, image_sharpen, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 30, 30);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);

	return;
}

/*Go to the previous image*/
static void previous_image(Ewl_Widget *w, void *event, void *data)
{
        char *image;

        ecore_dlist_previous(em->images);
        image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_goto_last(em->images);
		image = ecore_dlist_current(em->images);
	}
	ewl_image_file_path_set(EWL_IMAGE(em->eimage), image);
      	ewl_widget_configure(em->eimage->parent);
	
        return;
}

/*Go to the next image*/
static void next_image(Ewl_Widget *w, void *event, void *data)
{
	char *image;

	ecore_dlist_next(em->images);
	image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_goto_first(em->images);
		image = ecore_dlist_current(em->images);
	}
	ewl_image_file_path_set(EWL_IMAGE(em->eimage), image);
	ewl_widget_configure(em->eimage->parent);
	
	return;
}

/*Zoom in on the image*/
static void zoom_in(Ewl_Widget *w, void *event, void *data)
{
	int ow, oh;
	
	ewl_object_current_size_get(EWL_OBJECT(em->eimage), &ow, &oh);

	ewl_image_size_set(EWL_IMAGE(em->eimage), ow*2, oh*2);
	ewl_widget_configure(em->eimage->parent);

	return;
}

/*Zoom out the image*/
static void zoom_out(Ewl_Widget *w, void *event, void *data)
{
        int ow, oh;

        ewl_object_current_size_get(EWL_OBJECT(em->eimage), &ow, &oh);

        ewl_image_size_set(EWL_IMAGE(em->eimage), ow/2, oh/2);
        ewl_widget_configure(em->eimage->parent);

        return;
}


/*Flip the image 180 degrees horizontally*/
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data)
{

	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;
	
	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nw, &nh);
	image_data = flip_horizontal(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	ewl_image_size_set(EWL_IMAGE(em->eimage), nh, nw);
	image = (Ewl_Image *)em->eimage;
        image->ow = nw;
        image->oh = nh;
	ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nw, nh);
	ewl_widget_configure(em->eimage->parent);

	return;
}

/*Flip the image 180 degrees vertically*/
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;

	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nw, &nh);
	image_data = flip_vertical(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	ewl_image_size_set(EWL_IMAGE(em->eimage), nh, nw);
	image = (Ewl_Image *)em->eimage;
        image->ow = nw;
	image->oh = nh;
	ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nh, nw);
	ewl_widget_configure(em->eimage->parent);

	return;
}

/*Rotate the image 90 degrees to the left*/
static void rotate_image_left(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;

	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nh, &nw);
	image_data = rotate_left(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	ewl_image_size_set(EWL_IMAGE(em->eimage), nh, nw);
	image = (Ewl_Image *)em->eimage;
        image->ow = nw;
        image->oh = nh;
        ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nw, nh);
	ewl_widget_configure(em->eimage->parent);
	
	return;
}

/*Rotate the image 90 degrees to the right*/
static void rotate_image_right(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;
	
	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nh, &nw);
	image_data = rotate_right(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	image = (Ewl_Image *)em->eimage;
	image->ow = nw;
       	image->oh = nh;
       	ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nw, nh);
	ewl_widget_configure(em->eimage->parent);

	return;
}

/* Blur the image*/
static void image_blur(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

       	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nw, &nh);
       	image_data = blur_image(em->eimage);
       	update_image(em->eimage, nw, nh, image_data);
       	ewl_widget_configure(em->eimage->parent);

        return;
}

/* Sharpen the image*/
static void image_sharpen(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

        evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nw, &nh);
        image_data = sharpen_image(em->eimage);
        update_image(em->eimage, nw, nh, image_data);
        ewl_widget_configure(em->eimage->parent);

        return;
}

